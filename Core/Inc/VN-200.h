#ifndef __VN_200_H__
#define __VN_200_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int32_t gyro_x, gyro_y, gyro_z;
    int32_t vel_n, vel_e, vel_d;
    int32_t accel_x, accel_y, accel_z;
    int32_t gps_time;
    uint8_t num_sats, gnss_fix;
    int32_t lat, lon;
} VN_Measurement;

bool vn_parse_packet(uint8_t* packet, uint16_t length, VN_Measurement* out);
void send_measurement_queue(VN_Measurement* meas);
void VNUARTTask(void *argument);

#endif
