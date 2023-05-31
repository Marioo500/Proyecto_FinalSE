#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "uart.h"

static const char *TAG = "RECIBIDO";

static void delayMs(uint32_t ms)
{
    vTaskDelay(ms/portTICK_PERIOD_MS);
}

void app_main(void)
{
    char cadToRec[20];
    const char s[2] = ":";
    char *token;
    uart_init(UART2_PORT, UART2_RX_PIN, UART2_TX_PIN);
    while (1)
    {
        memset(cadToRec,0,20);
        if(uartKbhit(UART2_PORT))
        {
            int len = uart_read_bytes(UART2_PORT, cadToRec, (READ_BUF_SIZE - 1), 0);
            if (len)
            {
                token = strtok(cadToRec, s);
                while( token != NULL ) 
                {
                    ESP_LOGI(TAG,"%s", token);
                    token = strtok(NULL, s);
                }
            }
        }
        delayMs(100);
    }
}