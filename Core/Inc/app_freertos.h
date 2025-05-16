#ifndef APP_FREERTOS_H
#define APP_FREERTOS_H

#include "FreeRTOS.h"
#include "queue.h"

// Extern for queue
extern QueueHandle_t Tx;

// Task prototypes
void BlinkRLEDTask(void *argument);
void CANTask(void *argument);
void TxTask(void *argument);
void MX_FREERTOS_Init(void);

#endif /* APP_FREERTOS_H */
