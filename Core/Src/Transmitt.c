#include "Transmitt.h"
#include "usart.h"     // For huart1
#include "usb_device.h"
#include "usbd_cdc_if.h"
// UART transmit task

void TxTask(void *argument) {
    char *msg;

    for (;;) {
        if (xQueueReceive(Tx, &msg, portMAX_DELAY) == pdTRUE) {
            
            char usb_msg [strlen(msg) + 3]; // +2 for \r\n and +1 for null terminator

            sprintf(usb_msg,"%s\r\n", msg);

            CDC_Transmit_FS((uint8_t *) usb_msg, strlen(usb_msg));

            //HAL_UART_Transmit(&huart1, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
        }
    }
}
