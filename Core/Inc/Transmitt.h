#ifndef TRANSMITT_H
#define TRANSMITT_H

#include "cmsis_os.h"
#include "queue.h"
#include "stdint.h"

// Define the message structure used in the Tx queue
typedef struct {
    uint8_t *data;
    uint16_t length;
} TxMessage_t;

// Externally defined queue handle
extern QueueHandle_t Tx;

// UART transmit task function prototype
void TxTask(void *argument);

#endif // TRANSMITT_H
