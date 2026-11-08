#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"

#define ADC_CHANNEL ADC_CHANNEL_6
#define ADC_WIDTH ADC_BITWIDTH_12
#define ADC_ATTEN ADC_ATTEN_DB_12
#define ADC_UNIT ADC_UNIT_1

const char *TAG = "ADC_CONTINUOUS";

// ADC continuous configuration for ADC1
adc_continuous_handle_t adc1_handle = NULL;
// ADC continuous configuration structure for ADC1
adc_continuous_handle_cfg_t adc1_config = {
    .max_store_buf_size = 1024,
    .conv_frame_size = 256,
};

// ADC continuous pattern: one channel on ADC1
adc_digi_pattern_config_t adc1_pattern = {
    .atten = ADC_ATTEN,
    .channel = ADC_CHANNEL,
    .unit = ADC_UNIT,
    .bit_width = ADC_WIDTH,
};

adc_continuous_config_t dig_cfg = {
    .pattern_num = 1,
    .adc_pattern = &adc1_pattern,
    .sample_freq_hz = 20 * 1000, //20kHz
    .conv_mode = ADC_CONV_SINGLE_UNIT_1,
};

void app_main(void)
{
    // Create a new ADC continuous handle for ADC1 with the specified configuration
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc1_config, &adc1_handle));
    // Configure the conversion mode, sampling rate and channel pattern (required before start)
    ESP_ERROR_CHECK(adc_continuous_config(adc1_handle, &dig_cfg));
    // Start ADC continuous conversion for ADC1
    ESP_ERROR_CHECK(adc_continuous_start(adc1_handle));

    uint16_t counter_u16 = 100u;
    while(counter_u16 > 0)
    {
        // Using the read and parse function
        adc_continuous_data_t parsed_data[64];  // User specifies maximum number of samples
        uint32_t num_samples = 0;

        esp_err_t ret = adc_continuous_read_parse(adc1_handle, parsed_data, 64, &num_samples, 1000);
        if (ret == ESP_OK) {
            for (int i = 0; i < num_samples; i++) {
                if (parsed_data[i].valid) {
                    ESP_LOGI(TAG, "ADC%d, Channel: %d, Value: %"PRIu32,
                            parsed_data[i].unit + 1,
                            parsed_data[i].channel,
                            parsed_data[i].raw_data);
                }
            }
        }
        else {
            ESP_LOGW(TAG, "Error reading ADC data");
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
        counter_u16--;
    }

    // Stop ADC continuous conversion for ADC1
    ESP_ERROR_CHECK(adc_continuous_stop(adc1_handle));
    // Deinitialize the ADC continuous handle for ADC1
    ESP_ERROR_CHECK(adc_continuous_deinit(adc1_handle));
}
