// Core/Inc/Transmitt.h
#ifndef TRANSMITT_H
#define TRANSMITT_H

#include <stdint.h>

typedef struct {
    uint8_t id;
    uint8_t length;
    uint8_t data[8];
} Payload_t;

#endif // TRANSMITT_H
