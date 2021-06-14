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

/* Definitions for readCellsVolts */
osThreadId_t readCellsVoltsHandle;
const osThreadAttr_t readCellsVolts_attributes = {
  .name = "readCellsVolts",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for readCellsTemp */
osThreadId_t readCellsTempHandle;
const osThreadAttr_t readCellsTemp_attributes = {
  .name = "readCellsTemp",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for readCellsStatus */
osThreadId_t readCellsStatusHandle;
const osThreadAttr_t readCellsStatus_attributes = {
  .name = "readCellsStatus",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for CANvoltage */
osThreadId_t CANvoltageHandle;
const osThreadAttr_t CANvoltage_attributes = {
  .name = "CANvoltage",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for CANtemperature */
osThreadId_t CANtemperatureHandle;
const osThreadAttr_t CANtemperature_attributes = {
  .name = "CANtemperature",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for CANcurrent */
osThreadId_t CANcurrentHandle;
const osThreadAttr_t CANcurrent_attributes = {
  .name = "CANcurrent",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for CANglv */
osThreadId_t CANglvHandle;
const osThreadAttr_t CANglv_attributes = {
  .name = "CANglv",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for CANinfo */
osThreadId_t CANinfoHandle;
const osThreadAttr_t CANinfo_attributes = {
  .name = "CANinfo",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for balanceCheck */
osThreadId_t balanceCheckHandle;
const osThreadAttr_t balanceCheck_attributes = {
  .name = "balanceCheck",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for chargeUpdate */
osThreadId_t chargeUpdateHandle;
const osThreadAttr_t chargeUpdate_attributes = {
  .name = "chargeUpdate",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for dataUpdate */
osThreadId_t dataUpdateHandle;
const osThreadAttr_t dataUpdate_attributes = {
  .name = "dataUpdate",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for filterMaxVolts */
osThreadId_t filterMaxVoltsHandle;
const osThreadAttr_t filterMaxVolts_attributes = {
  .name = "filterMaxVolts",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 128 * 4
};
/* Definitions for filterMinVolts */
osThreadId_t filterMinVoltsHandle;
const osThreadAttr_t filterMinVolts_attributes = {
  .name = "filterMinVolts",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for filterTemp */
osThreadId_t filterTempHandle;
const osThreadAttr_t filterTemp_attributes = {
  .name = "filterTemp",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for errorOvervolt */
osThreadId_t errorOvervoltHandle;
const osThreadAttr_t errorOvervolt_attributes = {
  .name = "errorOvervolt",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for errorUndervolt */
osThreadId_t errorUndervoltHandle;
const osThreadAttr_t errorUndervolt_attributes = {
  .name = "errorUndervolt",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for errorOverTemp */
osThreadId_t errorOverTempHandle;
const osThreadAttr_t errorOverTemp_attributes = {
  .name = "errorOverTemp",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for errorGLV */
osThreadId_t errorGLVHandle;
const osThreadAttr_t errorGLV_attributes = {
  .name = "errorGLV",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for errorMonitoring */
osThreadId_t errorMonitoringHandle;
const osThreadAttr_t errorMonitoring_attributes = {
  .name = "errorMonitoring",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for nextion_page */
osThreadId_t nextion_pageHandle;
const osThreadAttr_t nextion_page_attributes = {
  .name = "nextion_page",
  .priority = (osPriority_t) osPriorityLow,
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
/* Definitions for commSemaphore */
osSemaphoreId_t commSemaphoreHandle;
const osSemaphoreAttr_t commSemaphore_attributes = {
  .name = "commSemaphore"
};
/* Definitions for ltcSemaphore */
osSemaphoreId_t ltcSemaphoreHandle;
const osSemaphoreAttr_t ltcSemaphore_attributes = {
  .name = "ltcSemaphore"
};
/* USER CODE BEGIN PV */

// Definitions for q_maxVoltages
osMessageQDef(q_maxVoltages, 16, uint16_t);
q_maxVoltagesHandle = osMessageCreate(osMessageQ(q_maxVoltages), NULL);

// Definitions for q_minVoltages
osMessageQDef(q_minVoltages, 16, uint16_t);
q_minVoltagesHandle = osMessageCreate(osMessageQ(q_minVoltages), NULL);

// Definitions for q_maxTemperatures
osMessageQDef(q_maxTemperatures, 16, uint16_t);
q_maxTemperaturesHandle = osMessageCreate(osMessageQ(q_maxTemperatures), NULL);

// Definitions for q_reportError
osMessageQDef(q_reportError, 16, uint16_t);
q_reportErrorHandle = osMessageCreate(osMessageQ(q_reportError), NULL);

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
void read_cells_volts(void *argument);
void read_cells_temp(void *argument);
void read_cells_status(void *argument);
void CAN_voltage(void *argument);
void CAN_temperature(void *argument);
void CAN_current(void *argument);
void CAN_GLV(void *argument);
void CAN_info(void *argument);
void balance_check(void *argument);
void charge_update(void *argument);
void data_update(void *argument);
void filter_max_voltages(void *argument);
void filter_min_voltages(void *argument);
void filter_temperature(void *argument);
void error_overvoltage(void *argument);
void error_undervoltage(void *argument);
void error_over_temperature(void *argument);
void error_GLV_undervoltage(void *argument);
void error_monitoring(void *argument);
void nextionPage(void *argument);

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
  /* creation of commSemaphore */
  commSemaphoreHandle = osSemaphoreNew(1, 1, &commSemaphore_attributes);

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
  /* creation of readCellsVolts */
  readCellsVoltsHandle = osThreadNew(read_cells_volts, NULL, &readCellsVolts_attributes);

  /* creation of readCellsTemp */
  readCellsTempHandle = osThreadNew(read_cells_temp, NULL, &readCellsTemp_attributes);

  /* creation of readCellsStatus */
  readCellsStatusHandle = osThreadNew(read_cells_status, NULL, &readCellsStatus_attributes);

  /* creation of CANvoltage */
  CANvoltageHandle = osThreadNew(CAN_voltage, NULL, &CANvoltage_attributes);

  /* creation of CANtemperature */
  CANtemperatureHandle = osThreadNew(CAN_temperature, NULL, &CANtemperature_attributes);

  /* creation of CANcurrent */
  CANcurrentHandle = osThreadNew(CAN_current, NULL, &CANcurrent_attributes);

  /* creation of CANglv */
  CANglvHandle = osThreadNew(CAN_GLV, NULL, &CANglv_attributes);

  /* creation of CANinfo */
  CANinfoHandle = osThreadNew(CAN_info, NULL, &CANinfo_attributes);

  /* creation of balanceCheck */
  balanceCheckHandle = osThreadNew(balance_check, NULL, &balanceCheck_attributes);

  /* creation of chargeUpdate */
  chargeUpdateHandle = osThreadNew(charge_update, NULL, &chargeUpdate_attributes);

  /* creation of dataUpdate */
  dataUpdateHandle = osThreadNew(data_update, NULL, &dataUpdate_attributes);

  /* creation of filterMaxVolts */
  filterMaxVoltsHandle = osThreadNew(filter_max_voltages, NULL, &filterMaxVolts_attributes);

  /* creation of filterMinVolts */
  filterMinVoltsHandle = osThreadNew(filter_min_voltages, NULL, &filterMinVolts_attributes);

  /* creation of filterTemp */
  filterTempHandle = osThreadNew(filter_temperature, NULL, &filterTemp_attributes);

  /* creation of errorOvervolt */
  errorOvervoltHandle = osThreadNew(error_overvoltage, NULL, &errorOvervolt_attributes);

  /* creation of errorUndervolt */
  errorUndervoltHandle = osThreadNew(error_undervoltage, NULL, &errorUndervolt_attributes);

  /* creation of errorOverTemp */
  errorOverTempHandle = osThreadNew(error_over_temperature, NULL, &errorOverTemp_attributes);

  /* creation of errorGLV */
  errorGLVHandle = osThreadNew(error_GLV_undervoltage, NULL, &errorGLV_attributes);

  /* creation of errorMonitoring */
  errorMonitoringHandle = osThreadNew(error_monitoring, NULL, &errorMonitoring_attributes);

  /* creation of nextion_page */
  nextion_pageHandle = osThreadNew(nextionPage, NULL, &nextion_page_attributes);

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

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 5;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 9;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_6TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7200;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 5000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 720;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 100;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ISOSPI_CS_GPIO_Port, ISOSPI_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, AIR_ENABLE_Pin|CHARGE_ENABLE_Pin|ERR_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : DEBUG_Pin */
  GPIO_InitStruct.Pin = DEBUG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DEBUG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CURRENT2_200_Pin CURRENT2_25_Pin */
  GPIO_InitStruct.Pin = CURRENT2_200_Pin|CURRENT2_25_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ISOSPI_CS_Pin */
  GPIO_InitStruct.Pin = ISOSPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ISOSPI_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MODE_SELECT_Pin FLAG_RESET_Pin */
  GPIO_InitStruct.Pin = MODE_SELECT_Pin|FLAG_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : AIR_ENABLE_Pin CHARGE_ENABLE_Pin ERR_LED_Pin */
  GPIO_InitStruct.Pin = AIR_ENABLE_Pin|CHARGE_ENABLE_Pin|ERR_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : AIR_AUX_PLUS_Pin AIR_AUX_MINUS_Pin */
  GPIO_InitStruct.Pin = AIR_AUX_PLUS_Pin|AIR_AUX_MINUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_read_cells_volts */
/**
  * @brief  Function implementing the readCellsVolts thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_read_cells_volts */
void read_cells_volts(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  BMS_convert(BMS_CONVERT_CELL, BMS);
	  osMessagePut(q_maxVoltagesHandle, BMS->v_max, 0);
	  osMessagePut(q_minVoltagesHandle, BMS->v_min, 0);

	  osDelay(100);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_read_cells_temp */
/**
  * @brief  Function implementing the readCellsTemp thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_read_cells_temp */
void read_cells_temp(void *argument)
{
  /* USER CODE BEGIN read_cells_temp */
  /* Infinite loop */
  for(;;)
  {
	  BMS_convert(BMS_CONVERT_GPIO, BMS);
	  osMessagePut(q_maxTemperaturesHandle, BMS->t_max, 0);

	  osDelay(100);
  }
  /* USER CODE END read_cells_temp */
}

/* USER CODE BEGIN Header_read_cells_status */
/**
* @brief Function implementing the readCellsStatus thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_read_cells_status */
void read_cells_status(void *argument)
{
  /* USER CODE BEGIN read_cells_status */
  /* Infinite loop */
  for(;;)
  {
	  BMS_convert(BMS_CONVERT_STAT, BMS);
	  osDelay(100);
  }
  /* USER CODE END read_cells_status */
}

/* USER CODE BEGIN Header_CAN_voltage */
/**
* @brief Function implementing the CANvoltage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CAN_voltage */
void CAN_voltage(void *argument)
{
  /* USER CODE BEGIN CAN_voltage */

  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(commSemaphoreHandle, osWaitForever);
	  BMS_CAN_voltage(BMS);
	  osSemaphoreRelease(commSemaphoreHandle);
	  osDelay(100);
  }
  /* USER CODE END CAN_voltage */
}

/* USER CODE BEGIN Header_CAN_temperature */
/**
* @brief Function implementing the CANtemperature thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CAN_temperature */
void CAN_temperature(void *argument)
{
  /* USER CODE BEGIN CAN_temperature */
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(commSemaphoreHandle, osWaitForever);
	  BMS_CAN_temperature(BMS);
	  osSemaphoreRelease(commSemaphoreHandle);
	  osDelay(100);
  }
  /* USER CODE END CAN_temperature */
}

/* USER CODE BEGIN Header_CAN_current */
/**
* @brief Function implementing the CANcurrent thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CAN_current */
void CAN_current(void *argument)
{
  /* USER CODE BEGIN CAN_current */
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(commSemaphoreHandle, osWaitForever);
	  BMS_CAN_current(BMS);
	  osSemaphoreRelease(commSemaphoreHandle);
	  osDelay(100);
  }
  /* USER CODE END CAN_current */
}

/* USER CODE BEGIN Header_CAN_GLV */
/**
* @brief Function implementing the CANglv thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CAN_GLV */
void CAN_GLV(void *argument)
{
  /* USER CODE BEGIN CAN_GLV */
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(commSemaphoreHandle, osWaitForever);
	  BMS_CAN_GLV(BMS);
	  osSemaphoreRelease(commSemaphoreHandle);
	  osDelay(100);
  }
  /* USER CODE END CAN_GLV */
}

/* USER CODE BEGIN Header_CAN_info */
/**
* @brief Function implementing the CANinfo thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CAN_info */
void CAN_info(void *argument)
{
  /* USER CODE BEGIN CAN_info */
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(commSemaphoreHandle, osWaitForever);
	  BMS_CAN_info(BMS);
	  osSemaphoreRelease(commSemaphoreHandle);
	  osDelay(100);
  }
  /* USER CODE END CAN_info */
}

/* USER CODE BEGIN Header_balance_check */
/**
* @brief Function implementing the balanceCheck thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_balance_check */
void balance_check(void *argument)
{
  /* USER CODE BEGIN balance_check */
  /* Infinite loop */
  for(;;)
  {
	  for(uint8_t i = 0; i < N_OF_PACKS; i++)
	  {
		  if(BMS->mode & BMS_BALANCING)
	  		LTC_set_balance_flag(BMS->config, BMS->sensor[i]);
		  else
	  		LTC_reset_balance_flag(BMS->config, BMS->sensor[i]);

		  LTC_balance(BMS->config, BMS->sensor[i]);
	  }

	  osDelay(100);
  }
  /* USER CODE END balance_check */
}

/* USER CODE BEGIN Header_charge_update */
/**
* @brief Function implementing the chargeUpdate thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_charge_update */
void charge_update(void *argument)
{
  /* USER CODE BEGIN charge_update */
  /* Infinite loop */
  for(;;)
  {
	  if(BMS->mode & BMS_MONITORING)
		  BMS_discharging(BMS);
	  else if(BMS->mode & BMS_CHARGING)
		  BMS_charging(BMS);

	  if(BMS->charge < BMS->charge_min)
	  	BMS->charge_min = BMS->charge;
	  if(BMS->charge > BMS->charge_max)
	  	BMS->charge_max = BMS->charge;

	  EE_WriteVariable(0x0, (uint16_t) (BMS->charge >> 16));
	  EE_WriteVariable(0x1, (uint16_t) BMS->charge);

	  EE_WriteVariable(0x2, (uint16_t) (BMS->charge_min >> 16));
	  EE_WriteVariable(0x3, (uint16_t) BMS->charge_min);

	  EE_WriteVariable(0x4, (uint16_t) (BMS->charge_max >> 16));
	  EE_WriteVariable(0x5, (uint16_t) BMS->charge_max);

	  osDelay(100);
  }
  /* USER CODE END charge_update */
}

/* USER CODE BEGIN Header_data_update */
/**
* @brief Function implementing the dataUpdate thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_data_update */
void data_update(void *argument)
{
  /* USER CODE BEGIN data_update */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END data_update */
}

/* USER CODE BEGIN Header_filter_max_voltages */
/**
* @brief Function implementing the filterMaxVolts thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_filter_max_voltages */
void filter_max_voltages(void *argument)
{
  /* USER CODE BEGIN filter_max_voltages */
  uint16_t sum = 0, input_voltage, previous_voltages[N_AVERAGE];
  uint8_t index = 0;
  osEvent event;

  //start previous_voltage array and set sum initial value
  while(index < N_AVERAGE)
  {
	  event = osMessageGet(q_maxVoltagesHandle, 0);
	  if(event == osEventMessage)
	  {
		  input_voltage = event->value;
		  sum += input_voltage;
		  previous_voltages[index] = input_voltage;

		  index++;
	  }
	  else
		  osDelay(10);
  }

  index = 0;
  /* Infinite loop */
  for(;;)
  {
	event = osMessageGet(q_maxVoltagesHandle, 0);

	if(event == osEventMessage)
	{
		input_voltage = event->value;

		sum -= previous_voltages[index];
		sum += input_voltage;
		previous_voltages[index] = input_voltage;

		if(++index == N_AVERAGE)
			index = 0;

		BMS->v_max_filtered = sum / N_AVERAGE;
	}

	osDelay(100);
  }
  /* USER CODE END filter_max_voltages */
}

/* USER CODE BEGIN Header_filter_min_voltages */
/**
* @brief Function implementing the filterMinVolts thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_filter_min_voltages */
void filter_min_voltages(void *argument)
{
  /* USER CODE BEGIN filter_min_voltages */
  uint16_t sum = 0, input_voltage, previous_voltages[N_AVERAGE];
  uint8_t index = 0;
  osEvent event;

  //start previous_voltage array and set sum initial value
  while(index < N_AVERAGE)
  {
	  event = osMessageGet(q_minVoltagesHandle, 0);
	  if(event == osEventMessage)
	  {
		  input_voltage = event->value;
		  sum += input_voltage;
		  previous_voltages[index] = input_voltage;

		  index++;
	  }
	  else
		  osDelay(10);
  }

  index = 0;
  /* Infinite loop */
  for(;;)
  {
	event = osMessageGet(q_minVoltagesHandle, 0);

	if(event == osEventMessage)
	{
		input_voltage = event->value;

		sum -= previous_voltages[index];
		sum += input_voltage;
		previous_voltages[index] = input_voltage;

		if(++index == N_AVERAGE)
			index = 0;

		BMS->v_min_filtered = sum / N_AVERAGE;
	}

    osDelay(100);
  }
  /* USER CODE END filter_min_voltages */
}

/* USER CODE BEGIN Header_filter_temperature */
/**
* @brief Function implementing the filterTemp thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_filter_temperature */
void filter_temperature(void *argument)
{
  /* USER CODE BEGIN filter_temperature */
  uint16_t sum = 0, input_temperature, previous_temperatures[N_AVERAGE];
  uint8_t index = 0;
  osEvent event;

  //start previous_voltage array and set sum initial value
  while(index < N_AVERAGE)
  {
	  event = osMessageGet(q_maxTemperaturesHandle, 0);
	  if(event == osEventMessage)
	  {
		  input_temperature = event->value;
		  sum += input_temperature;
		  previous_temperatures[index] = input_temperature;

		  index++;
	  }
	  else
		  osDelay(10);
  }

  index = 0;
  /* Infinite loop */
  for(;;)
  {
	event = osMessageGet(q_maxTemperaturesHandle, 0);

	if(event == osEventMessage)
	{
		input_temperature = event->value;

		sum -= previous_temperatures[index];
		sum += input_temperature;
		previous_temperatures[index] = input_temperature;

		if(++index == N_AVERAGE)
			index = 0;

		BMS->t_max_filtered = sum / N_AVERAGE;
	}

	osDelay(100);
  }
  /* USER CODE END filter_temperature */
}

/* USER CODE BEGIN Header_error_overvoltage */
/**
* @brief Function implementing the errorOvervolt thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_error_overvoltage */
void error_overvoltage(void *argument)
{
  /* USER CODE BEGIN error_overvoltage */
  /* Infinite loop */
  for(;;)
  {
	if(BMS->v_max_filtered >= MAX_CELL_V_DISCHARGE)
	{
		BMS->error |= ERR_OVER_VOLTAGE;
		osMessagePut(q_reportErrorHandle, info, 0);
	}
	else
	{
		BMS->error &= ~ERR_OVER_VOLTAGE;
	}

    osDelay(100);
  }
  /* USER CODE END error_overvoltage */
}

/* USER CODE BEGIN Header_error_undervoltage */
/**
* @brief Function implementing the errorUndervolt thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_error_undervoltage */
void error_undervoltage(void *argument)
{
  /* USER CODE BEGIN error_undervoltage */
  /* Infinite loop */
  for(;;)
  {
	if(BMS->v_min_filtered <= MIN_CELL_V)
	{
		BMS->error |= ERR_UNDER_VOLTAGE;
		osMessagePut(q_reportErrorHandle, info, 0);
	}
	else
	{
		BMS->error &= ~ERR_UNDER_VOLTAGE;
	}

    osDelay(100);
  }
  /* USER CODE END error_undervoltage */
}

/* USER CODE BEGIN Header_error_over_temperature */
/**
* @brief Function implementing the errorOverTemp thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_error_over_temperature */
void error_over_temperature(void *argument)
{
  /* USER CODE BEGIN error_over_temperature */
  /* Infinite loop */
  for(;;)
  {
	if(BMS->t_max_filtered >= MAX_TEMPERATURE)
	{
	  	BMS->error |= ERR_OVER_TEMPERATURE;
	  	osMessagePut(q_reportErrorHandle, info, 0);
	}
	else
	{
	  	BMS->error &= ~ERR_OVER_TEMPERATURE;
	}

    osDelay(100);
  }
  /* USER CODE END error_over_temperature */
}

/* USER CODE BEGIN Header_error_GLV_undervoltage */
/**
* @brief Function implementing the errorGLV thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_error_GLV_undervoltage */
void error_GLV_undervoltage(void *argument)
{
  /* USER CODE BEGIN error_GLV_undervoltage */
  /* Infinite loop */
  for(;;)
  {
	if(BMS->v_GLV < MIN_GLV_V)
	{
		BMS->error |= ERR_GLV_VOLTAGE;
		osMessagePut(q_reportErrorHandle, info, 0);
	}
	else
	{
		BMS->error &= ~ERR_GLV_VOLTAGE;
	}

    osDelay(100);
  }
  /* USER CODE END error_GLV_undervoltage */
}

/* USER CODE BEGIN Header_error_monitoring */
/**
* @brief Function implementing the errorMonitoring thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_error_monitoring */
void error_monitoring(void *argument)
{
  /* USER CODE BEGIN error_monitoring */
  /* Infinite loop */
  for(;;)
  {
	if(BMS->error != ERR_NO_ERROR)
	{
		HAL_GPIO_WritePin(AIR_ENABLE_GPIO_Port, AIR_ENABLE_Pin, RESET);
		HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, SET);
	}
	else
	{
		HAL_GPIO_WritePin(AIR_ENABLE_GPIO_Port, AIR_ENABLE_Pin, SET);
		HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, RESET);
	}

    osDelay(100);
  }
  /* USER CODE END error_monitoring */
}

/* USER CODE BEGIN Header_nextionPage */
/**
* @brief Function implementing the nextion_page thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_nextionPage */
void nextionPage(void *argument)
{
  /* USER CODE BEGIN nextionPage */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END nextionPage */
}

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
