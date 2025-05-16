#include "aligncan.h"
#include "fdcan.h"
#include "stdbool.h"
#include <string.h>
#include "cmsis_os.h" // Eller "FreeRTOS.h" og "queue.h" hvis du bruker de direkte.
#include "FreeRTOS.h"
#include "queue.h"
#include "app_freertos.h"

// Define FDCAN handles
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;
void CANtest();
uint8_t rxData[8];

extern QueueHandle_t Tx;



void Process_CAN_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data){
    switch(header->Identifier){

        case 0x22:

            uint32_t CurentTime = HAL_GetTick();
            
            const uint32_t OftenToSend = 1000; // how often to send the message in ms
            static uint32_t LastTime;           // Last time the message was sent
            
            if ((CurentTime - LastTime) > OftenToSend){
                
                static uint8_t ITemp[4]= {0}; // Initialize ITemp array. 1 id byte, 2 data bytes, 1 null terminator
                static uint8_t MTemp[4]= {0};

                ITemp[0] = 0xAA;  //sensor id 
                MTemp[0] = 0xAA;

                ITemp[3] = '\0';
                MTemp[3] = '\0';
                
                LastTime = CurentTime;

                memcpy(ITemp[1], &data[0], 2);
                memcpy(MTemp[1], &data[2], 2);


                xQueueSendToBack(Tx, &ITemp, 0);
                xQueueSendToBack(Tx, &MTemp, 0);

                return;
            }
            break;

        case 0x200:
            // Handle specific ID 0x200 for CAN2
            break;
        default:
            // Default case or ignore
            break;
    }
}

void Process_CAN3_Message(FDCAN_RxHeaderTypeDef *header, uint8_t *data){
    switch(header->Identifier){
        case 0x300:
            // Handle specific ID 0x300 for CAN3
            break;
        case 0x400:
            // Handle specific ID 0x400 for CAN3
            break;
        default:
            // Default case or ignore
            break;
    }
}

void CAN_Read(){
    FDCAN_RxHeaderTypeDef rxHeader;
    CANtest();

    // Check CAN2 messages
    while(Align_CAN_Receive(&hfdcan2, &rxHeader, rxData)){
        Process_CAN_Message(&rxHeader, rxData);
    }

    // Check CAN3 messages
    while(Align_CAN_Receive(&hfdcan3, &rxHeader, rxData)){
        Process_CAN_Message(&rxHeader, rxData);
    }
}

int bit_get(const uint8_t *arr, int bit_index) {
    int byte = bit_index / 8;
    int bit  = 7 - (bit_index % 8);  // MSB-first

    return (arr[byte] >> bit) & 1;
}
void CANtest() {
    typedef struct {
        uint32_t id;
        uint8_t data[8];
    } TestMessage;

    TestMessage testMsgs[] = {
        { 0x100, {1, 2, 3, 4, 5, 6, 7, 8} },
        { 0x200, {10, 20, 30, 40, 50, 60, 70, 80} },
        { 0x300, {0xAA, 0xBB, 0xCC, 0xDD, 0, 0, 0, 0} },
    };

    FDCAN_RxHeaderTypeDef rxHeader;

    for (int i = 0; i < sizeof(testMsgs)/sizeof(testMsgs[0]); i++) {
        rxHeader.Identifier = testMsgs[i].id;
        rxHeader.DataLength = FDCAN_DLC_BYTES_8;
        Process_CAN_Message(&rxHeader, testMsgs[i].data);
    }
}




