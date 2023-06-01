#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "uart.h"
#include "dht11.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"


#define DHT11_PIN                   GPIO_NUM_5
#define ADC1_EXAMPLE_CHAN0          ADC1_CHANNEL_6
#define ADC_EXAMPLE_ATTEN           ADC_ATTEN_DB_11
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_VREF


static const char *TAG = "MANDADO";
static int adc_raw[10];
static esp_adc_cal_characteristics_t adc1_chars;

static void delayMs(uint32_t ms)
{
    vTaskDelay(ms/portTICK_PERIOD_MS);
}

static bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_EXAMPLE_ATTEN, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}

void app_main(void)
{
    DHT11_init(DHT11_PIN);
    uint32_t voltage = 0;
    bool cali_enable = adc_calibration_init();
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));
    char cadToSend[16];
    char cadToRec[20];
    uart_init(UART2_PORT, UART2_RX_PIN, UART2_TX_PIN);
    while (1)
    {
        adc_raw[0] = adc1_get_raw(ADC1_EXAMPLE_CHAN0);
        if (cali_enable) {
            voltage = esp_adc_cal_raw_to_voltage(adc_raw[0], &adc1_chars);
        }
        sprintf(cadToSend, "%d:%d", DHT11_read().humidity, voltage);
        uart_write_bytes(UART2_PORT, (const char*)cadToSend, sizeof(cadToSend));
        ESP_LOGI(TAG, "%s", cadToSend);
        delayMs(2500);
    }
}