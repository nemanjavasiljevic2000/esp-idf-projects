#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#define ADC_CHANNEL ADC_CHANNEL_6
#define ADC_UNIT ADC_UNIT_1
#define ADC_ATTEN ADC_ATTEN_DB_12
//Calculate output Voltage from ADC value
#define ADC_REF_VOLTAGE 3300 // Reference voltage in mV
#define ADC_RESOLUTION 4096 // 12-bit ADC resolution
#define ADC_CALC_VOLTAGE(adc_val) ((adc_val) * ADC_REF_VOLTAGE / ADC_RESOLUTION)/1000.0 // Macro to calculate voltage from ADC value

int adc_value;
adc_oneshot_unit_handle_t adc1_handle; // Handle for ADC unit 1
adc_oneshot_unit_init_cfg_t adc1_config = { // Configuration for ADC unit 1
    .unit_id = ADC_UNIT, // ADC unit to initialize
    .ulp_mode = ADC_ULP_MODE_DISABLE// Disable ULP mode for this ADC unit
    // ULP mode definition - ADC_ULP_MODE_DISABLE means the ADC will not operate in Ultra Low Power mode
};
// Configuration for ADC channel 6 on ADC unit 1
adc_oneshot_chan_cfg_t adc1_channel_config = {
    .atten = ADC_ATTEN,
    .bitwidth = ADC_BITWIDTH_DEFAULT
};



void app_main(void)
{
    // Initialize ADC unit 1 with the specified configuration
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc1_config, &adc1_handle)); 
    // Configure ADC channel 6 on ADC unit 1 with the specified channel configuration
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &adc1_channel_config));

    uint16_t counter_u16 = 100u;
    while (counter_u16 > 0)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_value));
        printf("ADC Value: %d, Voltage: %.2f V\n", adc_value, ADC_CALC_VOLTAGE(adc_value));
        counter_u16--;
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 0.5 second
    }
    
    //Recycle ADC unit 1 handle
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
    
}