#include "aligncan.h"
#include "fdcan.h"
#include "stdbool.h"
#include <string.h>
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "app_freertos.h"
#include "Transmitt.h"  // <-- Payload_t definition

// Define FDCAN handles
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;

extern QueueHandle_t Tx;

void Process_CAN_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data);
void Process_CAN3_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data);
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);

uint8_t rxData[8] = {0, 1, 2, 3, 4, 5, 6, 7};


void Process_CAN_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data) {
    uint32_t CurentTime = HAL_GetTick();

    switch (header->Identifier) {
        case 0x22: {
            const uint32_t OftenToSend_0x22 = 50;
            static uint32_t LastTime_0x22;

            if ((CurentTime - LastTime_0x22) > OftenToSend_0x22) {
                Payload_t msg1 = {
                    .id = 0xA2,
                    .length = 2,
                    .data = {data[0], data[1]}
                };
                xQueueSendToBackFromISR(Tx, &msg1, 0);

                Payload_t msg2 = {
                    .id = 0x01,
                    .length = 2,
                    .data = {data[2], data[3]}
                };
                xQueueSendToBackFromISR(Tx, &msg2, 0);

                LastTime_0x22 = CurentTime;
            }
            break;
        }

        case 0x23: {
            const uint32_t OftenToSend_0x23 = 1000;
            static uint32_t LastTime_0x23;

            if ((CurentTime - LastTime_0x23) > OftenToSend_0x23) {
                Payload_t msg1 = {
                    .id = 0xFF,
                    .length = 2,
                    .data = {data[0], data[1]}
                };
                xQueueSendToBackFromISR(Tx, &msg1, 0);

                Payload_t msg2 = {
                    .id = 0xFE,
                    .length = 2,
                    .data = {data[2], data[3]}
                };
                xQueueSendToBackFromISR(Tx, &msg2, 0);

                LastTime_0x23 = CurentTime;
            }
            break;
        }

        default:
            // Ignore other CAN messages
            break;
    }
}

void Process_CAN3_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data) {
    switch (header->Identifier) {
        case 0x300:
            // Handle 0x300
            break;
        case 0x400:
            // Handle 0x400
            break;
        default:
            // Ignore others
            break;
    }
}

int bit_get(const uint8_t *arr, int bit_index) {
    int byte = bit_index / 8;
    int bit  = 7 - (bit_index % 8);  // MSB-first
    return (arr[byte] >> bit) & 1;
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    FDCAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;

            if (hfdcan->Instance == FDCAN2) {
                Process_CAN_Message(&rxHeader, rxData);
            } else if (hfdcan->Instance == FDCAN3) {
                Process_CAN3_Message(&rxHeader, rxData);
            }

            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
