#include "tasks.h"

void read_cells_volts(void *argument)
{
  for(;;)
  {
	  BMS_convert(BMS_CONVERT_CELL, BMS);
	  osDelay(100);
  }
}

void read_cells_temp(void *argument)
{
  for(;;)
  {
	  BMS_convert(BMS_CONVERT_GPIO, BMS);
	  osDelay(100);
  }
}

void read_cells_status(void *argument)
{
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
}

void CAN_transmission(void *argument)
{
  for(;;)
  {
	  BMS_CAN(BMS);
	  osDelay(100);
  }
}

void charge_update(void *argument)
{
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
}

void filter_max_voltages(void *argument)
{
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
}

void filter_min_voltages(void *argument)
{
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
}

void filter_temperature(void *argument)
{
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
}

void error_voltage(void *argument)
{
  uint16_t errorOvervoltage = ERR_OVER_VOLTAGE, errorUndervoltage = ERR_UNDER_VOLTAGE;
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
}

void error_over_temperature(void *argument)
{
  uint16_t errorOverTemperature = ERR_OVER_TEMPERATURE;
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
}

void error_GLV_undervoltage(void *argument)
{
  uint16_t errorGLV = ERR_GLV_VOLTAGE;
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
}

void error_monitoring(void *argument)
{
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
}

void nextion_loop(void *argument)
{
  for(;;)
  {
	  nex_loop(BMS);
	  osDelay(1);
  }
}
