#include "fdcan.h"
#include "cmsis_os.h"
#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;

void StartFDCANTask(void *argument)
{
    FDCAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    // Start FDCAN
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
        // Error handler
    }

    // Activate notification for new RX messages
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    for(;;) {
        // Wait until there's something in FIFO0
        if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) > 0) {
            if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK) {
                // Process rxHeader and rxData here
            }
        }

        osDelay(1); // Let other tasks run
    }
}
