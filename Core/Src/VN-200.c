#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "usart.h"
#include "VN-200.h"
#include "queue.h"

extern QueueHandle_t Tx;
typedef struct {
    uint8_t id;
    uint8_t length;
    uint8_t data[16];  // Supports up to 8 bytes of payload (e.g., float, int32, etc.)
} Payload_t;

void VNUARTTask(void *argument)
{
    (void)argument;

    uint8_t byte;
    char line[64];
    uint16_t pos = 0;

    for (;;) {
        //osDelay(100); // Yield to other tasks
        if (HAL_UART_Receive(&huart1, &byte, 1, 1000) == HAL_OK) {
            if (byte == '*' || pos >= sizeof(line) - 2) {
                line[pos] = '\0';

                char *start = strstr(line, "$VNINS,");
                if (!start) {
                    for (uint16_t i = 0; i < pos; i++) {
                        if (strncmp(&line[i], "$VNINS,", 7) == 0) {
                            start = &line[i];
                            break;
                        }
                    }
                    if (!start) {
                        pos = 0;
                        continue;
                    }
                }

                // Truncate at '*', '\r' or '\n'
                char *end = strpbrk(start, "*\r\n");
                if (end) *end = '\0';

                // Parse values
                char *token = strtok(start + 7, ","); // skip "$VNINS,"
                float values[9];
                int i = 0;

                while (token && i < 9) {
                    values[i++] = strtof(token, NULL);
                    token = strtok(NULL, ",");
                }

                if (i == 9) {
                    #define SEND(ID, VAL) do { \
                        Payload_t msg; \
                        msg.id = (ID); \
                        msg.length = 4; \
                        memcpy(msg.data, &(VAL), 4); \
                        if (xQueueSendToBack(Tx, &msg, pdMS_TO_TICKS(10)) != pdTRUE) { \
                            HAL_GPIO_WritePin(RLED_GPIO_Port, RLED_Pin, GPIO_PIN_SET); \
                        } \
                    } while(0)
                    

                    SEND(0xA1, values[0]); // lat
                    SEND(0xA2, values[1]); // lon
                    SEND(0xA3, values[2]); // alt
                    SEND(0xB1, values[3]); // gx
                    SEND(0xB2, values[4]); // gy
                    SEND(0xB3, values[5]); // gz
                    SEND(0xC1, values[6]); // ax
                    SEND(0xC2, values[7]); // ay
                    SEND(0xC3, values[8]); // az

                    #undef SEND
                }

                pos = 0;
            } else {
                // Build clean string
                if (byte == '$') pos = 0;
                if (isprint(byte)) {
                    line[pos++] = byte;
                }
            }
        }
    }
}
