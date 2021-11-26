/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "number_imgs.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"


/*********************
 *      DEFINES
 *********************/
#define LV_TICK_PERIOD_MS 1

static const char* TAG = "main";



/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
// static void hello_world(void);
static void display_image(void);

lv_obj_t * num_col_0;
lv_obj_t * num_col_1;
lv_obj_t * num_col_2;
int counter = 0;
/**********************
 *   APPLICATION MAIN
 **********************/
void app_main() {

    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    xTaskCreatePinnedToCore(guiTask, "gui", 4096*2, NULL, 0, NULL, 1);
}

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;

static void guiTask(void *pvParameter) {

    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t* buf = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf != NULL);

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE*8;

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf, NULL, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Create the demo application */
    display_image();

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        counter%=1000;
        ESP_LOGI(TAG,"Counter: %d",counter);
        lv_img_set_src(num_col_2, digits[counter%10]);
        if(counter > 9){
          lv_img_set_src(num_col_1, digits[(counter%100)/10]);
        }else{
          lv_img_set_src(num_col_1, digits[0]);
        }
        if(counter > 99){
          lv_img_set_src(num_col_0, digits[counter/100]);
        }else{
          lv_img_set_src(num_col_0, digits[0]);
        }
        counter++;
        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
       }
    }

    free(buf);
    vTaskDelete(NULL);
}

// static void hello_world(void){
//     lv_obj_t * scr = lv_disp_get_scr_act(NULL);

//     lv_obj_t * label1 =  lv_label_create(scr, NULL);

//     lv_label_set_text(label1, "Hello\nwod");
//     lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
// }

static void display_image(void){
    LV_IMG_DECLARE(zero);
    LV_IMG_DECLARE(two);

    lv_obj_t * scr = lv_disp_get_scr_act(NULL);
    num_col_0 = lv_img_create(scr,NULL);
    num_col_1 = lv_img_create(scr,NULL);
    num_col_2 = lv_img_create(scr,NULL);

    lv_img_set_src(num_col_0, digits[0]);
    lv_img_set_src(num_col_1, digits[0]);
    lv_img_set_src(num_col_2, digits[0]);

    lv_obj_align(num_col_0, NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_1_X_OFFSET,DIGIT_1_Y_OFFSET);
    lv_obj_align(num_col_1, NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_1_X_OFFSET, DIGIT_2_Y_OFFSET);
    lv_obj_align(num_col_2, NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_1_X_OFFSET, DIGIT_3_Y_OFFSET);
}

static void lv_tick_task(void *arg) {
    (void) arg;


    lv_tick_inc(LV_TICK_PERIOD_MS);
}
