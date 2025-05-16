/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "CAN-receive.h"
#include "queue.h"
#include "task.h"
#include "usbd_cdc_if.h"

void BlinkRLEDTask(void *argument);  // Forward declaration
void CANTask(void *argument);  // Forward declaration
void UARTSendTask(void *argument);  // Forward declaration
QueueHandle_t Tx; // Definisjon på globalt nivå

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  Tx = xQueueCreate(10, 10);  // Initialisering  
  if (Tx == NULL) {
    // Handle error
    Error_Handler();
  }


  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  const osThreadAttr_t blinkRLEDTask_attributes = {
    .name = "BlinkRLEDTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
  };
  const osThreadAttr_t CANTask_attributes = {
    .name = "CANTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
  };
  const osThreadAttr_t Tx_atributes = {
    .name = "UARTSendTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
  };
  /* Create the BlinkRLEDTask */
  osThreadId_t CANTaskHandle = osThreadNew(CANTask, NULL, &CANTask_attributes);
  osThreadId_t blinkRLEDTaskHandle = osThreadNew(BlinkRLEDTask, NULL, &blinkRLEDTask_attributes);
  osThreadId_t TxTaskHandle = osThreadNew(UARTSendTask, NULL, &Tx_atributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_Device */
  MX_USB_Device_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void CANTask(void *argument)
{
    /* Infinite loop */
    for (;;)
    {
      CAN_Read();
        
    }
}
void UARTSendTask(void *argument)
{
    /* Infinite loop */
    for (;;)
    {
      TxTask();
        
    }
}
/* USER CODE BEGIN Application */
void BlinkRLEDTask(void *argument)
{
    /* Infinite loop */
    for (;;)
    {
      vTaskDelay(pdMS_TO_TICKS(500));  // Delay for 500 ms
      HAL_GPIO_TogglePin(RLED_GPIO_Port, RLED_Pin);
        //Print("Catastrophic fault happened\n\r");
      char usb_msg[] = "Hello World\r\n";
      CDC_Transmit_FS((uint8_t *)usb_msg, strlen(usb_msg));

        
    }
}
 

/* USER CODE END Application */

