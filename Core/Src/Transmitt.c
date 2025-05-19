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



void TxTask(void *argument) {
    Payload_t msg;
    usb_tx_ready = 1;

    while (1) {
        configASSERT(Tx != NULL);
        configASSERT(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);
        // Wait for a message to be available in the queue
        if (xQueueReceive(Tx, &msg, portMAX_DELAY) == pdTRUE) {
            char buffer[64];
            int len = 0;

            // Format: ID + each data byte
            len += sprintf(&buffer[len], "%02X ", msg.id);
            for (int i = 0; i < msg.length && i < sizeof(msg.data); i++) {
                len += sprintf(&buffer[len], "%02X ", msg.data[i]);
            }

            buffer[len++] = '\r';
            buffer[len++] = '\n';
            buffer[len] = '\0';

            if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
                while (!usb_tx_ready) {
                    //vTaskDelay(pdMS_TO_TICKS(500));
                    
                }
                usb_tx_ready = 0;
                CDC_Transmit_FS((uint8_t *)buffer, len);
            }
            
        }
    }
}
