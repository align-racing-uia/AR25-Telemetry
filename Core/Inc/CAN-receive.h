#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include "aligncan.h"
#include "fdcan.h"
#include "stdbool.h"
#include "FreeRTOS.h"
#include "queue.h"

// External FDCAN handles
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;

// External queue handle
extern QueueHandle_t Tx;

// Public functions
void CAN_Setup(void);
void CAN_Read(void);
void Process_CAN_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data);
void Process_CAN3_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data);
int bit_get(const uint8_t *arr, int bit_index);

#endif // CAN_HANDLER_H
