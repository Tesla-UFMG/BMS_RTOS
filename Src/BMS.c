#include "BMS.h"

#define BMS_CONVERT_CELL 	1
#define BMS_CONVERT_GPIO	2
#define BMS_CONVERT_STAT	4
#define BMS_CONVERT_CONFIG	8

uint16_t flag = 0;
static int8_t UV_retries, OV_retries, OT_retries;
uint8_t NextError[5];
int16_t THERMISTOR_ZEROS[N_OF_PACKS][5];

/*******************************************************
Function void BMS_convert(uint8_t, BMS_struct*)

V1.0:
The function converts the required option that may be the
cells' voltage value, the thermistors' temperature value
or the BMS status value.

Version 1.0 - Initial release 26/11/2020 by Tesla UFMG
*******************************************************/
void BMS_convert(uint8_t BMS_CONVERT, BMS_struct* BMS)
{
	/*Convert the cells' voltage value*/
	if(BMS_CONVERT&BMS_CONVERT_CELL)
	{
		BMS->config->command->NAME = LTC_COMMAND_ADCV;
		BMS->config->command->BROADCAST = TRUE;
		LTC_send_command(BMS->config);

		for(uint8_t i = 0; i < N_OF_SLAVES; i++)
		{
			LTC_read(LTC_READ_CELL, BMS->config, BMS->sensor[i]);

			if(BMS->sensor[i]->V_MIN < BMS->v_min)
				BMS->v_min = BMS->sensor[i]->V_MIN;
			if(BMS->sensor[i]->V_MAX > BMS->v_max)
				BMS->v_max = BMS->sensor[i]->V_MAX;

			BMS->v_TS += BMS->sensor[i]->SOC;

			for(uint8_t j = 0; j < 4; j++)
			{
				if(BMS->sensor[i]->GxV[j] > BMS->t_max)
					BMS->t_max = BMS->sensor[i]->GxV[j];
			}
		}
	}

	/*Convert the thermistors' temperature value*/
	if(BMS_CONVERT&BMS_CONVERT_GPIO)
	{
		BMS->config->command->NAME = LTC_COMMAND_ADAX;
		BMS->config->command->BROADCAST = TRUE;
		LTC_send_command(BMS->config);

		for(uint8_t i = 0; i < N_OF_SLAVES; i++)
		{
			LTC_read(LTC_READ_GPIO, BMS->config, BMS->sensor[i]);

			for(uint8_t j = 0; j < 4; j++)
			{
				if(BMS->sensor[i]->GxV[j] > BMS->t_max)
					BMS->t_max = BMS->sensor[i]->GxV[j];
			}
		}
	}

	/*Convert the BMS status value*/
	if(BMS_CONVERT&BMS_CONVERT_STAT)
	{
		BMS->config->command->NAME = LTC_COMMAND_ADSTAT;
		BMS->config->command->BROADCAST = TRUE;
		LTC_send_command(BMS->config);

		for(uint8_t i = 0; i < N_OF_SLAVES; i++)
		{
			LTC_read(LTC_READ_STATUS, BMS->config, BMS->sensor[i]);

			BMS->v_TS += BMS->sensor[i]->SOC;
		}

		BMS->v_TS /= N_OF_PACKS/2; //?????
	}
}

/*******************************************************
Function void BMS_monitoring(BMS_struct*)

V1.0:
The function monitors the main aspects of the BMS, such as
the maximum and minimum voltages, the BMS charge percent and
the need of balancing the cells.

Version 1.0 - Initial release 26/11/2020 by Tesla UFMG
*******************************************************/
void BMS_monitoring(BMS_struct* BMS)
{
	BMS->v_min = 50000;
	BMS->v_max = 0;
	BMS->v_TS = 0;
	BMS->t_max = 0;
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
Function void BMS_set_thermistor_zeros(BMS_struct*)

V1.0:
The function calculates the thermistors' zeros for more
accurate readings.

Version 1.0 - Initial release 26/11/2020 by Tesla UFMG
*******************************************************/
void BMS_set_thermistor_zeros(BMS_struct* BMS)
{
	uint32_t mean = 0;

	for(int i = 0; i < N_OF_PACKS; i++)
	{
		for(int j = 0; j < 5; ++j)
			THERMISTOR_ZEROS[i][j] = 0;
	}

	BMS_convert(BMS_CONVERT_GPIO, BMS);

	for(int i = 0; i < N_OF_PACKS; i++)
	{
		for(int j = 0; j < 5; ++j)
			mean += BMS->sensor[i]->GxV[j];		//OBSERVAÇÃO 2!!!! QUANTOS TERMISTORES POR SLAVE?
	}

	mean = (uint32_t)((float)mean/(N_OF_PACKS*5));

	for(int i = 0; i < N_OF_PACKS; i++)
	{
		for(int j = 0; j < 5; ++j)
			THERMISTOR_ZEROS[i][j] = mean - BMS->sensor[i]->GxV[j];
	}
}

/*******************************************************
Function void BMS_init(BMS_struct*)

V1.0:
The function is responsible for initializing the Battery
Management System (BMS). It sets up all the configuration
needed for the BMS boot.

Version 1.0 - Initial release 24/11/2020 by Tesla UFMG
*******************************************************/
void BMS_init(BMS_struct* BMS)
{
	/*These three instructions set the initial configuration for the CAN communication and
	  start the reception process and enable reception interrupt*/
	CAN_Config_Filter();
	CAN_Config_Frames();
	CAN_Receive_IT();

	/*Memory allocation for the LTC config, command and sensor structs*/
	BMS->config = (LTC_config*) calloc(1 ,sizeof(LTC_config));
	BMS->config->command = (LTC_command*) calloc(1 ,sizeof(LTC_command));

	for(int i = 0; i < N_OF_SLAVES; i++)
	{
		BMS->sensor[i] = (LTC_sensor*) calloc(1, sizeof(LTC_sensor));
		BMS->sensor[i]->ADDR = i;
		LTC_init(BMS->config); //OBSERVAÇÃO 1
	}

	/*Set initial BMS configuration*/
	BMS->error = ERR_NO_ERROR;
	BMS->v_min = 50000;
	BMS->v_max = 0;
	BMS->mode = 0;

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

	LTC_init(BMS->config); //OBSERVAÇÃO 1

	/*Set initial SoC and the thermistors' zeros*/
	BMS_initial_SOC(BMS);
	BMS_set_thermistor_zeros(BMS);
}

/*******************************************************
Function void BMS_error(BMS_struct*)

V1.0:
The function tests a series of conditions and sets the right
error flag depending on what may happen through the BMS opera-
tion.

Version 1.0 - Initial release 25/11/2020 by Tesla UFMG
*******************************************************/
void BMS_error(BMS_struct* BMS)
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
		NextError[0] = 1;
		BMS->error |= ERR_UNDER_VOLTAGE;
	}
	else if(UV_retries == 0)
	{
		NextError[0] = 0;
		BMS->error &= ~ERR_UNDER_VOLTAGE;
	}

	/*Set true or false the over voltage error*/
	if(OV_retries == 5)
	{
		NextError[1] = 1;
		BMS->error |= ERR_OVER_VOLTAGE;
	}
	else if(OV_retries == 0)
	{
		NextError[1] = 0;
		BMS->error &= ~ERR_OVER_VOLTAGE;
	}

	/*Set true or false the over temperature error*/
	if(BMS->v_GLV < 13500)
	{
		BMS->error |= ERR_GLV_VOLTAGE;
		NextError[4] = 1;
	}
	else if(BMS->v_GLV < 13500)
	{
		BMS->error &= ~ERR_GLV_VOLTAGE;
		NextError[4] = 0;
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
