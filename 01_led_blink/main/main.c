#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

#define LED_PIN 5

void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    static uint8_t counter = 0;
    while(counter < 10)
    {
        gpio_set_level(LED_PIN, true); //led on
        vTaskDelay(1000/portTICK_PERIOD_MS);

        gpio_set_level(LED_PIN, false); //led off
        vTaskDelay(1000/portTICK_PERIOD_MS);

        counter++;
    }
}