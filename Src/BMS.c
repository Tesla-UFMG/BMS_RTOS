#include "BMS.h"

uint16_t flag = 0;
uint8_t next_error[5];
int16_t thermistor_zeros[N_OF_PACKS][5];

static int8_t UV_retries, OV_retries, OT_retries;

extern CAN_HandleTypeDef hcan;
extern CAN_TxHeaderTypeDef pHeader;
extern osSemaphoreId_t ltcSemaphoreHandle;

extern void Error_Handler();

uint32_t CAN_ID_TABLE[8][5] = {

		{
				256,		//PACK 2 V_GROUP 0 		(cells 0, 1, 2, 3)
				257,		//PACK 2 V_GROUP 1		(cells 4, 5, 6, 7)
				258,		//PACK 2 V_GROUP 2      (cells 8, 9, 10, 11)
				259,		//PACK 2 T_GROUP        (sensor 0, 1, 2, 3)
		},
		{
				260,		//PACK 3 V_GROUP 0 		(cells 0, 1, 2, 3)
				261,		//PACK 3 V_GROUP 1      (cells 4, 5, 6, 7)
				262,		//PACK 3 V_GROUP 2      (cells 8, 9, 10, 11)
				263,		//PACK 3 T_GROUP        (sensor 0, 1, 2, 3)
		},
		{
				264,		//PACK 4 V_GROUP 0 		(cells 0, 1, 2, 3)
				265,		//PACK 4 V_GROUP 1      (cells 4, 5, 6, 7)
				266,		//PACK 4 V_GROUP 2      (cells 8, 9, 10, 11)
				267,		//PACK 4 T_GROUP        (sensor 0, 1, 2, 3)
		},
		{
				268,		//PACK 5 V_GROUP 0      (cells 0, 1, 2, 3)
				269,		//PACK 5 V_GROUP 1      (cells 4, 5, 6, 7)
				270,		//PACK 5 V_GROUP 2      (cells 8, 9, 10, 11)
				271,		//PACK 5 T_GROUP        (sensor 0, 1, 2, 3)
		},
		{
				272,		//PACK 0 V_GROUP 0 		(cells 0, 1, 2, 3)
				273,		//PACK 0 V_GROUP 1		(cells 4, 5, 6, 7)
				274,		//PACK 0 V_GROUP 2      (cells 8, 9, 10, 11)
				275,		//PACK 0 T_GROUP        (sensor 0, 1, 2, 3)
		},

		{
				276,		//PACK 1 V_GROUP 0 		(cells 0, 1, 2, 3)
				277,		//PACK 1 V_GROUP 1		(cells 4, 5, 6, 7)
				278,		//PACK 1 V_GROUP 2      (cells 8, 9, 10, 11)
				279,		//PACK 1 T_GROUP        (sensor 0, 1, 2, 3)
		},
		{
				50,			//CHARGING CURRENT (bytes 0, 1, 2, 3) AND DISCHARGING CURRENT(bytes 4, 5, 6, 7)
		},
		{
				51,			//BMS MODE(bytes 0),COMM MODE(bytes 1), AIR STATUS(bytes 2 & 3), BATTERY %(bytes 4 & 5), GLOBAL ERROR FLAG(byte 6)
				52,
				53
		},
};

/*******************************************************
Function void BMS_convert(uint8_t, BMS_struct_t*)

V1.0:
The function converts the required option that may be the
cells' voltage value, the thermistors' temperature value
or the BMS status value.

Version 1.0 - Initial release 26/11/2020 by Tesla UFMG
*******************************************************/
void BMS_convert(uint8_t BMS_CONVERT, BMS_struct_t* BMS)
{
	/*Convert the cells' voltage value*/
	if(BMS_CONVERT&BMS_CONVERT_CELL)
	{
		osSemaphoreAcquire(ltcSemaphoreHandle, osWaitForever);
		BMS->config->command->NAME = LTC_COMMAND_ADCV;
		BMS->config->command->BROADCAST = TRUE;
		LTC_send_command(BMS->config);

		BMS->v_max	 = RESET_V_MAX;
		BMS->v_min = RESET_V_MIN;

		for(uint8_t i = 0; i < N_OF_SLAVES; i++)
		{
			LTC_read(LTC_READ_CELL, BMS->config, BMS->sensor[i]);
			osSemaphoreRelease(ltcSemaphoreHandle);

			if(BMS->sensor[i]->V_MAX > BMS->v_max)
				BMS->v_max = BMS->sensor[i]->V_MAX;

			if(BMS->sensor[i]->V_MIN < BMS->v_min)
				BMS->v_min = BMS->sensor[i]->V_MIN;
		}
	}

	/*Convert the thermistors' temperature value*/
	if(BMS_CONVERT&BMS_CONVERT_GPIO)
	{
		osSemaphoreAcquire(ltcSemaphoreHandle, osWaitForever);
		BMS->config->command->NAME = LTC_COMMAND_ADAX;
		BMS->config->command->BROADCAST = TRUE;
		LTC_send_command(BMS->config);

		BMS->t_max = RESET_T_MAX;

		for(uint8_t i = 0; i < N_OF_SLAVES; i++)
		{
			LTC_read(LTC_READ_GPIO, BMS->config, BMS->sensor[i]);
			osSemaphoreRelease(ltcSemaphoreHandle);

			for(uint8_t j = 0; j < N_OF_THERMISTORS; j++)
			{
				if(BMS->sensor[i]->GxV[j] > BMS->t_max)
					BMS->t_max = BMS->sensor[i]->GxV[j];
			}
		}
	}

	/*Convert the BMS status value*/
	if(BMS_CONVERT&BMS_CONVERT_STAT)
	{
		osSemaphoreAcquire(ltcSemaphoreHandle, osWaitForever);
		BMS->config->command->NAME = LTC_COMMAND_ADSTAT;
		BMS->config->command->BROADCAST = TRUE;
		LTC_send_command(BMS->config);

		BMS->v_TS = 0;

		for(uint8_t i = 0; i < N_OF_SLAVES; i++)
		{
			LTC_read(LTC_READ_STATUS, BMS->config, BMS->sensor[i]);
			osSemaphoreRelease(ltcSemaphoreHandle);

			BMS->v_TS += BMS->sensor[i]->SOC;
		}

		BMS->v_TS /= N_OF_PACKS/2;
	}
}

/*******************************************************
Function void BMS_monitoring(BMS_struct_t*)

V1.0:
The function monitors the main aspects of the BMS, such as
the maximum and minimum voltages, the BMS charge percent and
the need of balancing the cells.

Version 1.0 - Initial release 26/11/2020 by Tesla UFMG
*******************************************************/
void BMS_monitoring(BMS_struct_t* BMS)
{
	BMS->v_max = RESET_V_MAX;
	BMS->v_min = RESET_V_MIN;
	BMS->t_max = RESET_T_MAX;
	BMS->v_TS = 0;
	BMS->charge_percentage = 0;

	BMS_convert(BMS_CONVERT_CELL|BMS_CONVERT_GPIO|BMS_CONVERT_STAT, BMS);

	for(uint8_t i = 0; i < N_OF_PACKS; i++)
	{
		if(BMS->mode & BMS_BALANCING)
			LTC_set_balance_flag(BMS->config, BMS->sensor[i]);
		else
			LTC_reset_balance_flag(BMS->config, BMS->sensor[i]);

		LTC_balance(BMS->config, BMS->sensor[i]);
		BMS->charge_percentage += BMS->sensor[i]->TOTAL_CHARGE;
	}

	BMS->charge_percentage /= N_OF_PACKS;

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
}

/*******************************************************
Function void BMS_set_thermistor_zeros(BMS_struct_t*)

V1.0:
The function calculates the thermistors' zeros for more
accurate readings.

Version 1.0 - Initial release 26/11/2020 by Tesla UFMG
*******************************************************/
void BMS_set_thermistor_zeros(BMS_struct_t* BMS)
{
	uint32_t mean = 0;

	for(int i = 0; i < N_OF_PACKS; i++)
	{
		for(int j = 0; j < N_OF_THERMISTORS; ++j)
			thermistor_zeros[i][j] = 0;
	}

	BMS_convert(BMS_CONVERT_GPIO, BMS);

	for(int i = 0; i < N_OF_PACKS; i++)
	{
		for(int j = 0; j < N_OF_THERMISTORS; ++j)
			mean += BMS->sensor[i]->GxV[j];		//OBSERVA��O 2!!!! QUANTOS TERMISTORES POR SLAVE?
	}

	mean = (uint32_t)((float)mean/(N_OF_PACKS*5));

	for(int i = 0; i < N_OF_PACKS; i++)
	{
		for(int j = 0; j < N_OF_THERMISTORS; ++j)
			thermistor_zeros[i][j] = mean - BMS->sensor[i]->GxV[j];
	}
}

/*******************************************************
Function void BMS_init(BMS_struct_t*)

V1.0:
The function is responsible for initializing the Battery
Management System (BMS). It sets up all the configuration
needed for the BMS boot.

Version 1.0 - Initial release 24/11/2020 by Tesla UFMG
*******************************************************/
void BMS_init(BMS_struct_t* BMS)
{
	/*These three instructions set the initial configuration for the CAN communication and
	  start the reception process and enable reception interrupt*/
	//CAN_Config_Filter();
	//CAN_Config_Frames();

	/*Memory allocation for the LTC config, command and sensor structs*/
	BMS->config = (LTC_config_t*) calloc(1 ,sizeof(LTC_config_t));
	BMS->config->command = (LTC_command_t*) calloc(1 ,sizeof(LTC_command_t));

	for(int i = 0; i < N_OF_SLAVES; i++)
	{
		BMS->sensor[i] = (LTC_sensor_t*) calloc(1, sizeof(LTC_sensor_t));
		BMS->sensor[i]->ADDR = i;
		LTC_init(BMS->config); //OBSERVA��O 1
	}

	for(int i = 0; i < N_OF_DHAB; i++)
		BMS->dhabSensor[i] = (DHAB_sensor_t*) calloc(1, sizeof(DHAB_sensor_t));

	/*Set initial BMS configuration*/
	BMS->error = ERR_NO_ERROR;
	BMS->v_max = RESET_V_MAX;
	BMS->v_min = RESET_V_MIN;
	BMS->mode = BMS_MONITORING;

	uint16_t aux;
	EE_ReadVariable(0x0, &aux);
	BMS->charge = ((uint32_t)aux) << 16; 		//Load upper bytes
	EE_ReadVariable(0x1, &aux);
	BMS->charge += aux;							//Load lower bytes

	EE_ReadVariable(0x2, &aux);
	BMS->charge_min = ((uint32_t)aux) << 16; 	//Load upper bytes
	EE_ReadVariable(0x3, &aux);
	BMS->charge_min += aux;						//Load lower bytes

	EE_ReadVariable(0x4, &aux);
	BMS->charge_max = ((uint32_t)aux) << 16; 	//Load upper bytes
	EE_ReadVariable(0x5, &aux);
	BMS->charge_max += aux;						//Load lower bytes

	LTC_init(BMS->config); //OBSERVA��O 1

	/*Set initial SoC and the thermistors' zeros*/
	BMS_initial_SOC(BMS);
	BMS_set_thermistor_zeros(BMS);
}

/*******************************************************
Function void BMS_error(BMS_struct_t*)

V1.0:
The function tests a series of conditions and sets the right
error flag depending on what may happen through the BMS opera-
tion.

Version 1.0 - Initial release 25/11/2020 by Tesla UFMG
*******************************************************/
void BMS_error(BMS_struct_t* BMS)
{
	/*Under voltage error*/
	if(BMS->v_min <= 28000)
		flag |= ERR_UNDER_VOLTAGE;

	/*Over voltage error*/
	if(BMS->v_max >= 36000)
		flag |= ERR_OVER_VOLTAGE;

	/*Over temperature error*/
	if(BMS->t_max >= 500)
		flag |= ERR_OVER_TEMPERATURE;

	/*Used for checking if the under voltage flag is false due to hardware problems*/
	if((flag & ERR_UNDER_VOLTAGE) == ERR_UNDER_VOLTAGE)
		UV_retries++;
	else
		UV_retries--;

	/*Used for checking if the over voltage flag is false due to hardware problems*/
	if((flag & ERR_OVER_VOLTAGE) == ERR_OVER_VOLTAGE)
		OV_retries++;
	else
		OV_retries--;

	/*Used for checking if the over temperature flag is false due to hardware problems*/
	if((flag & ERR_OVER_TEMPERATURE) == ERR_OVER_TEMPERATURE)
		OT_retries++;
	else
		OT_retries--;

	if(UV_retries > 5) UV_retries = 5;
	if(OV_retries > 5) OV_retries = 5;
	if(OT_retries > 5) OT_retries = 5;
	if(UV_retries < 0) UV_retries = 0;
	if(OV_retries < 0) OV_retries = 0;
	if(OT_retries < 0) OT_retries = 0;

	/*Set true or false the under voltage error*/
	if(UV_retries == 5)
	{
		next_error[0] = 1;
		BMS->error |= ERR_UNDER_VOLTAGE;
	}
	else if(UV_retries == 0)
	{
		next_error[0] = 0;
		BMS->error &= ~ERR_UNDER_VOLTAGE;
	}

	/*Set true or false the over voltage error*/
	if(OV_retries == 5)
	{
		next_error[1] = 1;
		BMS->error |= ERR_OVER_VOLTAGE;
	}
	else if(OV_retries == 0)
	{
		next_error[1] = 0;
		BMS->error &= ~ERR_OVER_VOLTAGE;
	}

	/*Set true or false the GLV undervoltage error*/
	if(BMS->v_GLV < 13500)
	{
		BMS->error |= ERR_GLV_VOLTAGE;
		next_error[4] = 1;
	}
	else if(BMS->v_GLV > 13500)
	{
		BMS->error &= ~ERR_GLV_VOLTAGE;
		next_error[4] = 0;
	}

	/*In case of error, the error LED is lit and the AIR must be shut off*/
	if(BMS->error != ERR_NO_ERROR)
	{
		HAL_GPIO_WritePin(AIR_ENABLE_GPIO_Port, AIR_ENABLE_Pin, RESET);
		HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, SET);
	}
	else
	{
		flag &= ERR_NO_ERROR;
		HAL_GPIO_WritePin(AIR_ENABLE_GPIO_Port, AIR_ENABLE_Pin, SET);
		HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, RESET);
	}
}

/*******************************************************
Function void can_buf(uint8_t*, uint16_t, uint16_t,
					  uint16_t, uint16_t)

V1.0:
The function fills the CAN buffer with four 16 bits words
so the data can be sent through CAN communication.

Version 1.0 - Initial release 02/12/2020 by Tesla UFMG
*******************************************************/
void CAN_buf(uint8_t buffer[8], uint16_t word1, uint16_t word2, uint16_t word3, uint16_t word4)
{
	buffer[0] = word1;
	buffer[1] = word1 >> 8;
	buffer[2] = word2;
	buffer[3] = word2 >> 8;
	buffer[4] = word3;
	buffer[5] = word3 >> 8;
	buffer[6] = word4;
	buffer[7] = word4 >> 8;
}

void BMS_CAN_voltage(BMS_struct_t* BMS)
{
	uint8_t CAN_buffer[8];

	for(uint8_t i = 0; i < N_OF_PACKS; i++)
	{
		for (uint8_t j = 0; j < 3; j++)
		{
			CAN_buffer[0] = BMS->sensor[i]->CxV[4 * j + 0];
			CAN_buffer[1] = BMS->sensor[i]->CxV[4 * j + 0] >> 8;
			CAN_buffer[2] = BMS->sensor[i]->CxV[4 * j + 1];
			CAN_buffer[3] = BMS->sensor[i]->CxV[4 * j + 1] >> 8;
			CAN_buffer[4] = BMS->sensor[i]->CxV[4 * j + 2];
			CAN_buffer[5] = BMS->sensor[i]->CxV[4 * j + 2] >> 8;
			CAN_buffer[6] = BMS->sensor[i]->CxV[4 * j + 3];
			CAN_buffer[7] = BMS->sensor[i]->CxV[4 * j + 3] >> 8;

			if(HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, (uint32_t*)CAN_ID_TABLE[i][j]) != HAL_OK)
				Error_Handler();

		}
	}
}

void BMS_CAN_temperature(BMS_struct_t* BMS)
{
	uint8_t CAN_buffer[8];

	for(uint8_t i = 0; i < N_OF_PACKS; i++)
	{
		CAN_buffer[0] = BMS->sensor[i]->GxV[0];
		CAN_buffer[1] = BMS->sensor[i]->GxV[0] >> 8;
		CAN_buffer[2] = BMS->sensor[i]->GxV[1];
		CAN_buffer[3] = BMS->sensor[i]->GxV[1] >> 8;
		CAN_buffer[4] = BMS->sensor[i]->GxV[2];
		CAN_buffer[5] = BMS->sensor[i]->GxV[2] >> 8;
		CAN_buffer[6] = BMS->sensor[i]->GxV[3];
		CAN_buffer[7] = BMS->sensor[i]->GxV[3] >> 8;

		if(HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, (uint32_t*)CAN_ID_TABLE[i][CAN_TEMPERATURE_ID]) != HAL_OK)
			Error_Handler();
	}
}

void BMS_CAN_current(BMS_struct_t* BMS)
{
	uint8_t CAN_buffer[8];

	CAN_buffer[0] = ((int16_t)BMS->dhabSensor[0]->current);
	CAN_buffer[1] = ((int16_t)BMS->dhabSensor[0]->current) >> 8;
	CAN_buffer[2] = ((int16_t)BMS->dhabSensor[1]->current);
	CAN_buffer[3] = ((int16_t)BMS->dhabSensor[1]->current) >> 8;
	CAN_buffer[4] = ((int16_t)BMS->dhabSensor[2]->current);
	CAN_buffer[5] = ((int16_t)BMS->dhabSensor[2]->current) >> 8;
	CAN_buffer[6] = ((int16_t)BMS->dhabSensor[3]->current);
	CAN_buffer[7] = ((int16_t)BMS->dhabSensor[3]->current) >> 8;

	if(HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, (uint32_t*)50) != HAL_OK)
		Error_Handler();
}

void BMS_CAN_GLV(BMS_struct_t* BMS)
{
	uint8_t CAN_buffer[8];

	CAN_buf(CAN_buffer, BMS->v_GLV, (uint16_t)(BMS->charge_percentage/10), 0, BMS->AIR);

	if(HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, (uint32_t*)51) != HAL_OK)
		Error_Handler();
}

void BMS_CAN_info(BMS_struct_t* BMS)
{
	uint8_t CAN_buffer[8];

	CAN_buffer[0] = 0;
	CAN_buffer[1] = 0;
	CAN_buffer[2] = BMS->v_TS;
	CAN_buffer[3] = BMS->v_TS;
	CAN_buffer[4] = 0;
	CAN_buffer[5] = 0;
	CAN_buffer[6] = BMS->t_max;
	CAN_buffer[7] = BMS->t_max >> 8;

	if(HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, (uint32_t*)52) != HAL_OK)
		Error_Handler();

	CAN_buf(CAN_buffer, BMS->v_min, BMS->v_max, 0, 0);

	if(HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, (uint32_t*)53) != HAL_OK)
		Error_Handler();
}

/*******************************************************
Function void BMS_can(BMS_struct_t*)

V1.0:
The function sends essential informations about the BMS such
as the cells' voltages, temperature, charge percent, maximum
and minimum voltages through CAN communication.

Version 1.0 - Initial release 02/12/2020 by Tesla UFMG
*******************************************************/
//void BMS_can(BMS_struct_t* BMS)
//{
//	uint8_t CAN_buffer[8];
//
//	for(uint8_t i = 0; i < N_OF_PACKS; i++)
//	{
//		for (uint8_t j = 0; j < 3; j++)
//		{
//			CAN_buffer[0] = BMS->sensor[i]->CxV[4 * j + 0];
//			CAN_buffer[1] = BMS->sensor[i]->CxV[4 * j + 0] >> 8;
//			CAN_buffer[2] = BMS->sensor[i]->CxV[4 * j + 1];
//			CAN_buffer[3] = BMS->sensor[i]->CxV[4 * j + 1] >> 8;
//			CAN_buffer[4] = BMS->sensor[i]->CxV[4 * j + 2];
//			CAN_buffer[5] = BMS->sensor[i]->CxV[4 * j + 2] >> 8;
//			CAN_buffer[6] = BMS->sensor[i]->CxV[4 * j + 3];
//			CAN_buffer[7] = BMS->sensor[i]->CxV[4 * j + 3] >> 8;
//
//			//CAN_Transmit(CAN_buffer, CAN_ID_TABLE[i][j]);
//			HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, CAN_ID_TABLE[i][j]);
//		}
//
//		CAN_buffer[0] = BMS->sensor[i]->GxV[0];
//		CAN_buffer[1] = BMS->sensor[i]->GxV[0] >> 8;
//		CAN_buffer[2] = BMS->sensor[i]->GxV[1];
//		CAN_buffer[3] = BMS->sensor[i]->GxV[1] >> 8;
//		CAN_buffer[4] = BMS->sensor[i]->GxV[2];
//		CAN_buffer[5] = BMS->sensor[i]->GxV[2] >> 8;
//		CAN_buffer[6] = BMS->sensor[i]->GxV[3];
//		CAN_buffer[7] = BMS->sensor[i]->GxV[3] >> 8;
//
//		//CAN_Transmit(CAN_buffer, CAN_ID_TABLE[i][CAN_TEMPERATURE_ID]);
//		HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, CAN_ID_TABLE[i][CAN_TEMPERATURE_ID]);
//	}
//
//	CAN_buffer[0] = ((int16_t)BMS->dhabSensor[0]->current);
//	CAN_buffer[1] = ((int16_t)BMS->dhabSensor[0]->current) >> 8;
//	CAN_buffer[2] = ((int16_t)BMS->dhabSensor[1]->current);
//	CAN_buffer[3] = ((int16_t)BMS->dhabSensor[1]->current) >> 8;
//	CAN_buffer[4] = ((int16_t)BMS->dhabSensor[2]->current);
//	CAN_buffer[5] = ((int16_t)BMS->dhabSensor[2]->current) >> 8;
//	CAN_buffer[6] = ((int16_t)BMS->dhabSensor[3]->current);
//	CAN_buffer[7] = ((int16_t)BMS->dhabSensor[3]->current) >> 8;
//
//	HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, 50);
//
//	CAN_buf(CAN_buffer, BMS->v_GLV, (uint16_t)(BMS->charge_percentage/10), 0, BMS->AIR);
//
//	HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, 51);
//
//	CAN_buffer[0] = 0;
//	CAN_buffer[1] = 0;
//	CAN_buffer[2] = BMS->v_TS;
//	CAN_buffer[3] = BMS->v_TS;
//	CAN_buffer[4] = 0;
//	CAN_buffer[5] = 0;
//	CAN_buffer[6] = BMS->t_max;
//	CAN_buffer[7] = BMS->t_max >> 8;
//
//	HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, 52);
//
//	CAN_buf(CAN_buffer, BMS->v_min, BMS->v_max, 0, 0);
//
//	HAL_CAN_AddTxMessage(&hcan, &pHeader, CAN_buffer, 53);
//}

/*******************************************************
Function void BMS_initial_SOC(LTC_sensor_t)

V1.0:
The function reads all cells' voltage and takes the least
charged cell as the Open Circuit Voltage (OCV). Depending
on the value taken as the OCV, the SoC will be determined
by a linear equation.

Version 1.0 - Initial release 04/12/2020 by Tesla UFMG
*******************************************************/
void BMS_initial_SOC(BMS_struct_t* BMS)
{
	for(uint8_t i = 0; i < N_OF_PACKS; i++)
		LTC_read(LTC_READ_CELL, BMS->config, BMS->sensor[i]);

	if(BMS->v_min < 33680 && BMS->v_min >= 33400)						//98,00% -- 96,00%
		BMS->charge_percentage = 71.43*(BMS->v_min/10000) - 142.57;
	else if(BMS->v_min < 33400 && BMS->v_min >= 33310)					//96,61% -- 76,16%
		BMS->charge_percentage = 2297.96*(BMS->v_min/10000) - 7578.34;
	else if(BMS->v_min < 33310 && BMS->v_min >= 32990)					//75,50% -- 68,61%
		BMS->charge_percentage = 216.09*(BMS->v_min/10000) - 644.27;
	else if(BMS->v_min < 32990 && BMS->v_min >= 32880)					//71,30% -- 40,39%
		BMS->charge_percentage = 2835.23*(BMS->v_min/10000) - 9281.84;
	else if(BMS->v_min < 32880 && BMS->v_min >= 31980)					//38,38% -- 10,01%
		BMS->charge_percentage = 315.54*(BMS->v_min/10000) - 999.08;
	else if(BMS->v_min < 31980 && BMS->v_min >= 31290)					//9,53% -- 5,86%
		BMS->charge_percentage = 53.23*(BMS->v_min/10000) - 160.69;
	else
		BMS->charge_percentage = 0;

	if(BMS->charge_percentage > 100)
		BMS->charge_percentage = 100;
	if(BMS->charge_percentage < 0)
		BMS->charge_percentage = 0;
}

/*******************************************************
Function void BMS_charging(BMS_struct_t)

V1.0:
The function updates the battery's charge percent with the
current integration function, determining the battery's SoC
along with the BMS_Initial_SOC function.

Version 1.0 - Initial release 04/12/2020 by Tesla UFMG
*******************************************************/
void BMS_charging(BMS_struct_t* BMS)
{
	for(int i = 0; i < N_OF_DHAB; i++)
		BMS->charge = BMS->charge + DHAB_current_integration(BMS->dhabSensor[i]);

	BMS->charge_variation_percentage = (BMS->charge/BMS->charge_max)*100;
	BMS->charge_percentage = BMS->charge_percentage + BMS->charge_variation_percentage;
	BMS->discharge_percentage = 100 - BMS->charge_percentage;

	if(BMS->error == ERR_OVER_VOLTAGE)
		BMS->charge_max = BMS->charge;
}

/*******************************************************
Function void BMS_discharging(BMS_struct_t)

V1.0:
The function updates the battery's discharge percent with
the current integration function. Finally, it calculates
the battery's charge percent with the relation
charge = 100 - discharge

Version 1.0 - Initial release 04/12/2020 by Tesla UFMG
*******************************************************/
void BMS_discharging(BMS_struct_t* BMS)
{
	for(int i = 0; i < N_OF_DHAB; i++)
		BMS->charge = BMS->charge + DHAB_current_integration(BMS->dhabSensor[i]);

	BMS->discharge_variation_percentage = (BMS->charge/BMS->charge_max)*100;
	BMS->discharge_percentage = BMS->discharge_percentage + BMS->discharge_variation_percentage;
	BMS->charge_percentage = 100 - BMS->discharge_percentage;

	if(BMS->error == ERR_UNDER_VOLTAGE)
		BMS->charge_min = BMS->charge;
}
