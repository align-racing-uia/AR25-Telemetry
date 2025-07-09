#include "Transmitt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "usart.h"

extern QueueHandle_t Tx;

int isSent = 1;

void TxTask(void *argument) {
    Payload_t msg;
    
    
    while (1) {
        configASSERT(Tx != NULL);
        configASSERT(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);
        

        if (xQueueReceive(Tx, &msg, portMAX_DELAY) == pdTRUE) {
            char buffer[128];
            int len = 0;

            len += sprintf(&buffer[len], "%02X ", msg.id);
            for (int i = 0; i < msg.length; i++) {
                len += sprintf(&buffer[len], "%02X ", msg.data[i]);
            }

            buffer[len++] = '\r';
            buffer[len++] = '\n';
            buffer[len] = '\0';
            //isSent = 1;



            if (isSent) {
                uint8_t tx_buf[3 + msg.length];
                tx_buf[0] = 0xFE;             // startbyte
                tx_buf[1] = msg.id;
                tx_buf[2] = msg.length;
                memcpy(&tx_buf[3], msg.data, msg.length);

    if (HAL_UART_Transmit_DMA(&huart2, tx_buf, 3 + msg.length) == HAL_OK) {
        isSent = 0;
        vTaskDelay(pdMS_TO_TICKS(5));
        
    } else {
        HAL_GPIO_WritePin(RLED_GPIO_Port, RLED_Pin, GPIO_PIN_SET);
    }
}
        }   
    }
}

__attribute__((used)) void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        isSent = 1;
    }
}


