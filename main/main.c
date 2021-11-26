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
#define BUM_PIXEL_WIDTH 18

static const char* TAG = "main";


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif
#ifndef LV_ATTRIBUTE_IMG_ZERO
#define LV_ATTRIBUTE_IMG_ZERO
#endif
const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_IMG_ZERO uint8_t zero_map[] = {
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0xff, 	/*Color of index 1*/

  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x01, 0xff, 0xfe, 0x00, 
  0x07, 0xff, 0xff, 0x80, 
  0x1f, 0xff, 0xff, 0xe0, 
  0x3f, 0x80, 0x07, 0xf0, 
  0x3c, 0x00, 0x00, 0xf0, 
  0x70, 0x00, 0x00, 0x38, 
  0x70, 0x00, 0x00, 0x38, 
  0xe0, 0x00, 0x00, 0x1c, 
  0xe0, 0x00, 0x00, 0x1c, 
  0xe0, 0x00, 0x00, 0x1c, 
  0xe0, 0x00, 0x00, 0x1c, 
  0x60, 0x00, 0x00, 0x18, 
  0x70, 0x00, 0x00, 0x38, 
  0x78, 0x00, 0x00, 0x78, 
  0x3e, 0x00, 0x01, 0xf0, 
  0x1f, 0xf0, 0x1f, 0xe0, 
  0x0f, 0xff, 0xff, 0xc0, 
  0x03, 0xff, 0xff, 0x00, 
  0x00, 0x7f, 0xf8, 0x00, 
};

const lv_img_dsc_t zero = {
  .header.always_zero = 0,
  .header.w = 30,
  .header.h = 24,
  .data_size = 105,
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .data = zero_map,
};

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif
#ifndef LV_ATTRIBUTE_IMG_ONE
#define LV_ATTRIBUTE_IMG_ONE
#endif
const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_IMG_ONE uint8_t one_map[] = {
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0xff, 	/*Color of index 1*/

  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1c, 
  0x00, 0x00, 0x00, 0x1c, 
  0x00, 0x00, 0x00, 0x1c, 
  0x00, 0x00, 0x00, 0x1c, 
  0x00, 0x00, 0x00, 0x1c, 
  0x00, 0x00, 0x00, 0x1c, 
  0x00, 0x00, 0x00, 0x1c, 
  0xff, 0xff, 0xff, 0xfc, 
  0xff, 0xff, 0xff, 0xfc, 
  0xff, 0xff, 0xff, 0xfc, 
  0xe0, 0x00, 0x00, 0x1c, 
  0xe0, 0x00, 0x00, 0x1c, 
  0x60, 0x00, 0x00, 0x1c, 
  0x60, 0x00, 0x00, 0x1c, 
  0x60, 0x00, 0x00, 0x1c, 
  0x60, 0x00, 0x00, 0x1c, 
  0x70, 0x00, 0x00, 0x1c, 
  0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t one = {
  .header.always_zero = 0,
  .header.w = 30,
  .header.h = 24,
  .data_size = 105,
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .data = one_map,
};

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif
#ifndef LV_ATTRIBUTE_IMG_TWO
#define LV_ATTRIBUTE_IMG_TWO
#endif
const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_IMG_TWO uint8_t two_map[] = {
  0xf9, 0xf9, 0xf9, 0xff, 	/*Color of index 0*/
  0x11, 0x11, 0x11, 0xff, 	/*Color of index 1*/

  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 
  0x03, 0xf0, 0x00, 0x1c, 
  0x0f, 0xfc, 0x00, 0x1c, 
  0x3f, 0xfe, 0x00, 0x1c, 
  0x3e, 0xbf, 0x00, 0x1c, 
  0x78, 0x0f, 0x80, 0x1c, 
  0x70, 0x03, 0xe0, 0x1c, 
  0x70, 0x01, 0xe0, 0x1c, 
  0xe0, 0x00, 0xf8, 0x1c, 
  0xe0, 0x00, 0x7c, 0x1c, 
  0xe0, 0x00, 0x3e, 0x1c, 
  0xe0, 0x00, 0x1f, 0x1c, 
  0xe0, 0x00, 0x0f, 0x9c, 
  0x60, 0x00, 0x07, 0xdc, 
  0x70, 0x00, 0x01, 0xfc, 
  0x70, 0x00, 0x01, 0xfc, 
  0x70, 0x00, 0x00, 0x7c, 
  0x38, 0x00, 0x00, 0x3c, 
  0x38, 0x00, 0x00, 0x1c, 
};

const lv_img_dsc_t two = {
  .header.always_zero = 0,
  .header.w = 30,
  .header.h = 24,
  .data_size = 105,
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .data = two_map,
};
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
// static void hello_world(void);
static void display_image(void);

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
    lv_obj_t * img_zero = lv_img_create(scr,NULL);
    lv_obj_t * img_one = lv_img_create(scr,NULL);
    lv_obj_t * img_two = lv_img_create(scr,NULL);

    lv_img_set_src(img_zero, &zero);
    lv_img_set_src(img_one, &one);
    lv_img_set_src(img_two, &two);

    lv_obj_align(img_zero, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 2, 5);
    lv_obj_align(img_one, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 22, 5);
    lv_obj_align(img_two, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 42, 5);
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
