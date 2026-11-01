#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#define led_output   5u
#define button_input 6u

void reset_gpio(void)
{
    gpio_reset_pin(led_output);
    gpio_reset_pin(button_input);
}

void set_inputs(void)
{
    gpio_set_pull_mode(button_input, GPIO_PULLUP_ONLY);
    gpio_set_direction(button_input, GPIO_MODE_INPUT);
}

void set_outputs(void)
{
    gpio_set_direction(led_output, GPIO_MODE_OUTPUT);
}

void app_main(void)
{
    reset_gpio();
    set_inputs();
    set_outputs();

    while(1)
    {
        if(!gpio_get_level(button_input))
        {
            gpio_set_level(led_output, true);
        }
        else
        {
            gpio_set_level(led_output,false);
        }
    }

}