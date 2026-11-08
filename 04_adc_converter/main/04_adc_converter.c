#include "esp_adc/adc_oneshot.h" 
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define EXAMPLE_ADC_UNIT ADC_UNIT_1
#define EXAMPLE_ADC_CHANNEL ADC_CHANNEL_6 // needs to be looked up in pinout which channel is connected to the desired GPIO pin
#define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_12

static const char *TAG = "ADC_VOLTAGE";

// Helper function to initialize calibration
static bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle) {
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    ESP_LOGI(TAG, "Using Curve Fitting calibration scheme");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);    

    *out_handle = handle;
    if (ret == ESP_OK) {
        calibrated = true;
        ESP_LOGI(TAG, "Calibration initialized successfully");
    } else {
        ESP_LOGE(TAG, "Calibration initialization failed (%s)", esp_err_to_name(ret));
    }
    return calibrated;
}

void app_main(void) {

    // 1. Initialize ADC Unit
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = EXAMPLE_ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    // 2. Configure ADC Channel
    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = EXAMPLE_ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC_CHANNEL, &channel_config));

    // 3. Initialize Calibration
    adc_cali_handle_t cali_handle = NULL;
    bool do_cali = adc_calibration_init(EXAMPLE_ADC_UNIT, EXAMPLE_ADC_ATTEN, &cali_handle);

    // 4. Read loop
    while (1) {
        int raw_val = 0;
        int voltage_mv = 0;
        
        // Get raw data
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC_CHANNEL, &raw_val));
        
        if (do_cali) {
            // Convert raw counts to millivolts
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, raw_val, &voltage_mv));
            ESP_LOGI(TAG, "Raw: %d | Voltage: %d mV (%.2f V)", raw_val, voltage_mv, (float)voltage_mv / 1000.0);
        } else {
            ESP_LOGI(TAG, "Raw: %d (No calibration available)", raw_val);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // 5. Clean up
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
    if (do_cali) 
        ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(cali_handle));
    
}