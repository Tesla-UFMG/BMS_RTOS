#include "tasks.h"

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
  /* USER CODE END read_cells_status */
}

/* USER CODE BEGIN Header_CAN_voltage */
/**
* @brief Function implementing the CANvoltage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CAN_voltage */
void CAN_transmission(void *argument)
{
  /* USER CODE BEGIN CAN_voltage */

  /* Infinite loop */
  for(;;)
  {
	  BMS_CAN(BMS);
	  osDelay(100);
  }
  /* USER CODE END CAN_voltage */
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
  osStatus_t status;

  //start previous_voltage array and set sum initial value
  while(index < N_AVERAGE)
  {
	  status = osMessageQueueGet(q_maxVoltagesHandle, &input_voltage, NULL, 0);
	  if(status == osOK)
	  {
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
	status = osMessageQueueGet(q_maxVoltagesHandle, &input_voltage, NULL, 0);

	if(status == osOK)
	{
		sum -= previous_voltages[index];
		sum += input_voltage;
		previous_voltages[index] = input_voltage;

		if(++index == N_AVERAGE)
			index = 0;

		BMS->v_max = sum / N_AVERAGE;
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
  osStatus_t status;

  //start previous_voltage array and set sum initial value
  while(index < N_AVERAGE)
  {
	  status = osMessageQueueGet(q_minVoltagesHandle, &input_voltage, NULL, 0);
	  if(status == osOK)
	  {
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
	status = osMessageQueueGet(q_minVoltagesHandle, &input_voltage, NULL, 0);

	if(status == osOK)
	{
		sum -= previous_voltages[index];
		sum += input_voltage;
		previous_voltages[index] = input_voltage;

		if(++index == N_AVERAGE)
			index = 0;

		BMS->v_min = sum / N_AVERAGE;
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
  osStatus_t status;

  //start previous_voltage array and set sum initial value
  while(index < N_AVERAGE)
  {
	  status = osMessageQueueGet(q_maxTemperaturesHandle, &input_temperature, NULL, 0);
	  if(status == osOK)
	  {
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
	status = osMessageQueueGet(q_maxTemperaturesHandle, &input_temperature, NULL, 0);

	if(status == osOK)
	{
		sum -= previous_temperatures[index];
		sum += input_temperature;
		previous_temperatures[index] = input_temperature;

		if(++index == N_AVERAGE)
			index = 0;

		BMS->t_max = sum / N_AVERAGE;
	}

	osDelay(100);
  }
  /* USER CODE END filter_temperature */
}

/* USER CODE BEGIN Header_error_voltage */
/**
* @brief Function implementing the errorVoltage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_error_voltage */
void error_voltage(void *argument)
{
  /* USER CODE BEGIN error_voltage */
  uint16_t errorOvervoltage = 0, errorUndervoltage = 1;
  /* Infinite loop */
  for(;;)
  {
	if(BMS->v_max >= MAX_CELL_V_DISCHARGE)
	{
		BMS->error |= ERR_UNDER_VOLTAGE;
		osMessageQueuePut(q_reportErrorHandle, &errorOvervoltage, 0, osWaitForever);
	}
	else
	{
		BMS->error &= ~ERR_UNDER_VOLTAGE;
	}

	if(BMS->v_min <= MIN_CELL_V)
	{
		BMS->error |= ERR_UNDER_VOLTAGE;
		osMessageQueuePut(q_reportErrorHandle, &errorUndervoltage, 0, osWaitForever);
	}
	else
	{
		BMS->error &= ~ERR_UNDER_VOLTAGE;
	}

    osDelay(100);
  }
  /* USER CODE END error_voltage */
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
  uint16_t errorOverTemperature = 2;
  /* Infinite loop */
  for(;;)
  {
	if(BMS->t_max >= MAX_TEMPERATURE)
	{
	  	BMS->error |= ERR_OVER_TEMPERATURE;
	  	osMessageQueuePut(q_reportErrorHandle, &errorOverTemperature, 0, osWaitForever);
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
  uint16_t errorGLV = 3;
  /* Infinite loop */
  for(;;)
  {
	if(BMS->v_GLV < MIN_GLV_V)
	{
		BMS->error |= ERR_GLV_VOLTAGE;
		osMessageQueuePut(q_reportErrorHandle, &errorGLV, 0, osWaitForever);
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

/* USER CODE BEGIN Header_nextion_loop */
/**
* @brief Function implementing the nextionLoop thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_nextion_loop */
void nextion_loop(void *argument)
{
  /* USER CODE BEGIN nextion_loop */
  /* Infinite loop */
  for(;;)
  {
	  nex_loop(BMS);
	  osDelay(1);
  }
  /* USER CODE END nextion_loop */
}
