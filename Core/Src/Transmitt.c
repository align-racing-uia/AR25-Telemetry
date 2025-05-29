#include "Transmitt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os2.h"

#include "usbd_cdc_if.h"
#include "usb_device.h"
#include "usart.h"
#include "usbd_def.h"  // Needed for USBD_STATE_CONFIGURED
#include "usbd_cdc_if.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
extern QueueHandle_t Tx;
int isSent = 1;


void TxTask(void *argument) {
    Payload_t msg;
    usb_tx_ready = 1;

    while (1) {
        configASSERT(Tx != NULL);
        configASSERT(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);

        if (xQueueReceive(Tx, &msg, portMAX_DELAY) == pdTRUE) {
            char buffer[64];
            int len = 0;

            len += sprintf(&buffer[len], "%02X ", msg.id);
            for (int i = 0; i < msg.length; i++) {
                len += sprintf(&buffer[len], "%02X ", msg.data[i]);
            }

            buffer[len++] = '\r';
            buffer[len++] = '\n';
            buffer[len] = '\0';

            if (isSent) {
                if (HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer, len) == HAL_OK) {
                    isSent = 0;
                }
            }
        }
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    isSent = 1;
   
}
