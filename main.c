#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#define GPIO_OUTPUT_IO 4
#define GPIO_OUTPUT_PIN_SEL (1ULL<<GPIO_OUTPUT_IO)
#define LEVEL_ON 1
#define LEVEL_OFF 0
#define ONE_SEC 1000 / portTICK_PERIOD_MS
#define HALF_SEC 500 / portTICK_PERIOD_MS
#define QUARTER_SEC 250 / portTICK_PERIOD_MS
#define QUARTER_SEC_75 750 / portTICK_PERIOD_MS

void app_main() {
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    int cnt = 0;
    while(1) {
        printf("cnt: %d\n", cnt++);
        // task2
        // Ton1=1 sec
        vTaskDelay(ONE_SEC);
        gpio_set_level(GPIO_OUTPUT_IO, LEVEL_ON);

        // Toff1=0,5 sec
        vTaskDelay(HALF_SEC);
        gpio_set_level(GPIO_OUTPUT_IO, LEVEL_OFF);

        // Ton2=0,25 sec
        vTaskDelay(QUARTER_SEC);
        gpio_set_level(GPIO_OUTPUT_IO, LEVEL_ON);

        // Toff2=0,75 sec
        vTaskDelay(QUARTER_SEC_75);
        gpio_set_level(GPIO_OUTPUT_IO, LEVEL_OFF);
    }
}