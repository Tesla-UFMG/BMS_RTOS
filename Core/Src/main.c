/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

CAN_HandleTypeDef hcan;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* Definitions for t_readCellsVolts */
osThreadId_t t_readCellsVoltsHandle;
const osThreadAttr_t t_readCellsVolts_attributes = {
  .name = "t_readCellsVolts",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 128 * 4
};
/* Definitions for t_readCellsTemp */
osThreadId_t t_readCellsTempHandle;
const osThreadAttr_t t_readCellsTemp_attributes = {
  .name = "t_readCellsTemp",
  .priority = (osPriority_t) osPriorityAboveNormal1,
  .stack_size = 128 * 4
};
/* Definitions for t_readCellsStatus */
osThreadId_t t_readCellsStatusHandle;
const osThreadAttr_t t_readCellsStatus_attributes = {
  .name = "t_readCellsStatus",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 128 * 4
};
/* Definitions for t_CANtransmission */
osThreadId_t t_CANtransmissionHandle;
const osThreadAttr_t t_CANtransmission_attributes = {
  .name = "t_CANtransmission",
  .priority = (osPriority_t) osPriorityNormal4,
  .stack_size = 128 * 4
};
/* Definitions for t_chargeUpdate */
osThreadId_t t_chargeUpdateHandle;
const osThreadAttr_t t_chargeUpdate_attributes = {
  .name = "t_chargeUpdate",
  .priority = (osPriority_t) osPriorityAboveNormal2,
  .stack_size = 128 * 4
};
/* Definitions for t_filterMaxVolts */
osThreadId_t t_filterMaxVoltsHandle;
const osThreadAttr_t t_filterMaxVolts_attributes = {
  .name = "t_filterMaxVolts",
  .priority = (osPriority_t) osPriorityHigh1,
  .stack_size = 128 * 4
};
/* Definitions for t_filterMinVolts */
osThreadId_t t_filterMinVoltsHandle;
const osThreadAttr_t t_filterMinVolts_attributes = {
  .name = "t_filterMinVolts",
  .priority = (osPriority_t) osPriorityHigh1,
  .stack_size = 128 * 4
};
/* Definitions for t_filterTemp */
osThreadId_t t_filterTempHandle;
const osThreadAttr_t t_filterTemp_attributes = {
  .name = "t_filterTemp",
  .priority = (osPriority_t) osPriorityAboveNormal3,
  .stack_size = 128 * 4
};
/* Definitions for t_errorVoltage */
osThreadId_t t_errorVoltageHandle;
const osThreadAttr_t t_errorVoltage_attributes = {
  .name = "t_errorVoltage",
  .priority = (osPriority_t) osPriorityHigh4,
  .stack_size = 128 * 4
};
/* Definitions for t_errorTemp */
osThreadId_t t_errorTempHandle;
const osThreadAttr_t t_errorTemp_attributes = {
  .name = "t_errorTemp",
  .priority = (osPriority_t) osPriorityHigh3,
  .stack_size = 128 * 4
};
/* Definitions for t_errorGLV */
osThreadId_t t_errorGLVHandle;
const osThreadAttr_t t_errorGLV_attributes = {
  .name = "t_errorGLV",
  .priority = (osPriority_t) osPriorityHigh2,
  .stack_size = 128 * 4
};
/* Definitions for t_errorMonitoring */
osThreadId_t t_errorMonitoringHandle;
const osThreadAttr_t t_errorMonitoring_attributes = {
  .name = "t_errorMonitoring",
  .priority = (osPriority_t) osPriorityRealtime,
  .stack_size = 128 * 4
};
/* Definitions for t_nextionLoop */
osThreadId_t t_nextionLoopHandle;
const osThreadAttr_t t_nextionLoop_attributes = {
  .name = "t_nextionLoop",
  .priority = (osPriority_t) osPriorityBelowNormal1,
  .stack_size = 128 * 4
};
/* Definitions for q_maxVoltages */
osMessageQueueId_t q_maxVoltagesHandle;
const osMessageQueueAttr_t q_maxVoltages_attributes = {
  .name = "q_maxVoltages"
};
/* Definitions for q_minVoltages */
osMessageQueueId_t q_minVoltagesHandle;
const osMessageQueueAttr_t q_minVoltages_attributes = {
  .name = "q_minVoltages"
};
/* Definitions for q_maxTemperatures */
osMessageQueueId_t q_maxTemperaturesHandle;
const osMessageQueueAttr_t q_maxTemperatures_attributes = {
  .name = "q_maxTemperatures"
};
/* Definitions for q_reportError */
osMessageQueueId_t q_reportErrorHandle;
const osMessageQueueAttr_t q_reportError_attributes = {
  .name = "q_reportError"
};
/* Definitions for ltcSemaphore */
osSemaphoreId_t ltcSemaphoreHandle;
const osSemaphoreAttr_t ltcSemaphore_attributes = {
  .name = "ltcSemaphore"
};
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
extern void read_cells_volts(void *argument);
extern void read_cells_temp(void *argument);
extern void read_cells_status(void *argument);
extern void CAN_transmission(void *argument);
extern void charge_update(void *argument);
extern void filter_max_voltages(void *argument);
extern void filter_min_voltages(void *argument);
extern void filter_temperature(void *argument);
extern void error_voltage(void *argument);
extern void error_over_temperature(void *argument);
extern void error_GLV_undervoltage(void *argument);
extern void error_monitoring(void *argument);
extern void nextion_loop(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static const float CURRENT_GAIN[4] = {1.22, 1.51, 1.22, 1.22};
//extern DMA_HandleTypeDef hdma_usart3_rx;
uint16_t virt_add_var_tab[NumbOfVar] = {0x5555, 0x6666, 0x7777};

BMS_struct_t* BMS;
int32_t ADC_buf[5];

int initial_readings = 0;
float current_zero[N_OF_DHAB];

CAN_TxHeaderTypeDef pHeader;

float filter(float old, float new)
{
	return (FILTER_GAIN*old + new)/(FILTER_GAIN + 1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(initial_readings < 5)
	{
		for(uint8_t i = 0; i < N_OF_DHAB; i++)
		{
			current_zero[i] += ((float)ADC_buf[i] * (float)CURRENT_GAIN[i]);
			initial_readings++;

			if(initial_readings == 5)
			{
				for(uint8_t j = 0; j < N_OF_DHAB; j++)
					current_zero[j] = current_zero[j]/5;
			}
		}
	}
	else
	{
		for(uint8_t i = 0; i < N_OF_DHAB; i++)
		{
			BMS->dhabSensor[i]->currentADC = filter((float)BMS->dhabSensor[i]->currentADC, (float)ADC_buf[i]);
			BMS->dhabSensor[i]->current = filter(BMS->dhabSensor[i]->current, ((float)ADC_buf[i] * CURRENT_GAIN[i]) - current_zero[i]);			//BMS->current[i] = filter(BMS->current[i], (ADC_buf[i]));
		}
	}

	BMS->v_GLV = filter(BMS->v_GLV , ((float)(ADC_buf[4] + 400) * 4.5));
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_FLASH_Unlock();
  EE_Init();

  DWT_Delay_Init();

  HAL_ADC_Start_DMA(&hadc1, (uint32_t* )ADC_buf, 5);
  USART_DMA_Init(&huart3, &hdma_usart3_rx);

  CAN_Config_Filter();
  CAN_Config_Frames();
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_TX_MAILBOX_EMPTY);

  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim4);

  BMS = (BMS_struct_t*) calloc(1, sizeof(BMS_struct_t));
  BMS_init(BMS);

  DWT_Delay_us(50000);

  HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, 1);

  NexPageShow(1);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of ltcSemaphore */
  ltcSemaphoreHandle = osSemaphoreNew(1, 1, &ltcSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of q_maxVoltages */
  q_maxVoltagesHandle = osMessageQueueNew (16, sizeof(uint16_t), &q_maxVoltages_attributes);

  /* creation of q_minVoltages */
  q_minVoltagesHandle = osMessageQueueNew (16, sizeof(uint16_t), &q_minVoltages_attributes);

  /* creation of q_maxTemperatures */
  q_maxTemperaturesHandle = osMessageQueueNew (16, sizeof(uint16_t), &q_maxTemperatures_attributes);

  /* creation of q_reportError */
  q_reportErrorHandle = osMessageQueueNew (16, sizeof(uint16_t), &q_reportError_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of t_readCellsVolts */
  t_readCellsVoltsHandle = osThreadNew(read_cells_volts, NULL, &t_readCellsVolts_attributes);

  /* creation of t_readCellsTemp */
  t_readCellsTempHandle = osThreadNew(read_cells_temp, NULL, &t_readCellsTemp_attributes);

  /* creation of t_readCellsStatus */
  t_readCellsStatusHandle = osThreadNew(read_cells_status, NULL, &t_readCellsStatus_attributes);

  /* creation of t_CANtransmission */
  t_CANtransmissionHandle = osThreadNew(CAN_transmission, NULL, &t_CANtransmission_attributes);

  /* creation of t_chargeUpdate */
  t_chargeUpdateHandle = osThreadNew(charge_update, NULL, &t_chargeUpdate_attributes);

  /* creation of t_filterMaxVolts */
  t_filterMaxVoltsHandle = osThreadNew(filter_max_voltages, NULL, &t_filterMaxVolts_attributes);

  /* creation of t_filterMinVolts */
  t_filterMinVoltsHandle = osThreadNew(filter_min_voltages, NULL, &t_filterMinVolts_attributes);

  /* creation of t_filterTemp */
  t_filterTempHandle = osThreadNew(filter_temperature, NULL, &t_filterTemp_attributes);

  /* creation of t_errorVoltage */
  t_errorVoltageHandle = osThreadNew(error_voltage, NULL, &t_errorVoltage_attributes);

  /* creation of t_errorTemp */
  t_errorTempHandle = osThreadNew(error_over_temperature, NULL, &t_errorTemp_attributes);

  /* creation of t_errorGLV */
  t_errorGLVHandle = osThreadNew(error_GLV_undervoltage, NULL, &t_errorGLV_attributes);

  /* creation of t_errorMonitoring */
  t_errorMonitoringHandle = osThreadNew(error_monitoring, NULL, &t_errorMonitoring_attributes);

  /* creation of t_nextionLoop */
  t_nextionLoopHandle = osThreadNew(nextion_loop, NULL, &t_nextionLoop_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
