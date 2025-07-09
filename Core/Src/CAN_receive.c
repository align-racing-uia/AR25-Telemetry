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
BaseType_t xHigherPriorityTaskWoken = 0;
void Process_CAN_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data);
void Process_CAN3_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data);
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);

uint8_t rxData[8] = {0, 1, 2, 3, 4, 5, 6, 7};


void Process_CAN_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data) {
    uint32_t CurentTime = HAL_GetTick();

    switch (header->Identifier) {
        // ERPM + HV voltage (Packet ID 0x20)
    case 0x241E: 
    {
        const uint32_t OftenToSend_0x241E = 100;
        static uint32_t LastTime_0x241E;

        if ((CurentTime - LastTime_0x241E) > OftenToSend_0x241E) 
        {
            Payload_t erpmMsg = {
                .id = 0x01,
                .length = 4,
                .data = { data[0], data[1], data[2], data[3] }
            };
            xQueueSendToBackFromISR(Tx, &erpmMsg, &xHigherPriorityTaskWoken);

            Payload_t voltageMsg = {
                .id = 0x02,
                .length = 2,
                .data = { data[6], data[7] }
            };
            xQueueSendToBackFromISR(Tx, &voltageMsg, &xHigherPriorityTaskWoken);

            LastTime_0x241E = CurentTime;
        }
    break;
}

// Inverter + Motor Temp (Packet ID 0x22)
case 0x221E: {
    const uint32_t OftenToSend_0x221E = 100;
    static uint32_t LastTime_0x221E;

    if ((CurentTime - LastTime_0x221E) > OftenToSend_0x221E) {
        Payload_t ErrorCode = {
            .id = 0x22,
            .length = 1,
            .data = { data[0]}
        };
        xQueueSendToBackFromISR(Tx, &ErrorCode, &xHigherPriorityTaskWoken);

        Payload_t motorTempMsg = {
            .id = 0x04,
            .length = 2,
            .data = { data[2], data[3] }
        };
        xQueueSendToBackFromISR(Tx, &motorTempMsg, &xHigherPriorityTaskWoken);

        LastTime_0x221E = CurentTime;

        Payload_t inverterTempMsg = {
            .id = 0x03,
            .length = 2,
            .data = { data[0], data[1] }
        };

        xQueueSendToBackFromISR(Tx, &inverterTempMsg, &xHigherPriorityTaskWoken);
    }
    break;
}

case 0x30A: {
    const uint32_t OftenToSend_0x30A = 2000;
    static uint32_t LastTime_0x30A;

    if ((CurentTime - LastTime_0x30A) > OftenToSend_0x30A) {
        Payload_t fan = {
            .id = 0x11,
            .length = 1,
            .data = { data[0]}
        };
        xQueueSendToBackFromISR(Tx, &fan, &xHigherPriorityTaskWoken);

        Payload_t pump = {
            .id = 0x06,
            .length = 1,
            .data = {data[5] }
        };
        xQueueSendToBackFromISR(Tx, &pump, &xHigherPriorityTaskWoken);

        LastTime_0x30A = CurentTime;
    }
    break;
}
// Drive Enable (Packet ID 0x24)
case 0x31A: {
    const uint32_t OftenToSend_0x24 = 100;
    static uint32_t LastTime_0x24;

    if ((CurentTime - LastTime_0x24) > OftenToSend_0x24) {
        Payload_t driveEnableMsg = {
            .id = 0x05,
            .length = 1,
            .data = { data[3] }  // send full byte, you can mask bit 0 if needed elsewhere
        };
        xQueueSendToBackFromISR(Tx, &driveEnableMsg, &xHigherPriorityTaskWoken);

        LastTime_0x24 = CurentTime;
    }
    break;
}

case 0x25: {
    const uint32_t OftenToSend_0x25 = 100;
    static uint32_t LastTime_0x25;

    if ((CurentTime - LastTime_0x25) > OftenToSend_0x25) {
        Payload_t VCUError = {
            .id = 0x05,
            .length = 1,
            .data = { data[3] }  // send full byte, you can mask bit 0 if needed elsewhere
        };
        xQueueSendToBackFromISR(Tx, &VCUError, &xHigherPriorityTaskWoken);

        LastTime_0x25 = CurentTime;
    }
    break;
}
// Motor Position (Packet ID 0x26)              
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
