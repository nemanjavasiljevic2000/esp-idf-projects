#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "inttypes.h"

#define INTERRUPT_PIN       6u
#define LED_PIN             5u
#define INTERRUPT_THRESHOLD 3u
uint8_t dtc_counter_u8 = 0u;

static void IRAM_ATTR interrupt_handler(void* arg)
{
    dtc_counter_u8++;
    gpio_isr_handler_add(INTERRUPT_PIN, interrupt_handler, NULL);
    gpio_intr_enable(INTERRUPT_PIN);
}

void app_main(void)
{
    gpio_set_direction(INTERRUPT_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(INTERRUPT_PIN, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(INTERRUPT_PIN, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INTERRUPT_PIN, interrupt_handler, NULL);
    gpio_intr_enable(INTERRUPT_PIN);

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while(1)
    {
        if(dtc_counter_u8 >= INTERRUPT_THRESHOLD)
        {
            gpio_set_level(LED_PIN, false);
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        else
        {
            gpio_set_level(LED_PIN, true);
        }
        printf("Current counter value: %" PRIu8 "\n", dtc_counter_u8);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}