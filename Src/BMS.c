#include "BMS.h"

uint16_t flag = 0;
static int8_t UV_retries, OV_retries, OT_retries;
uint8_t NextError[5];

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
	BMS->charge = ((uint32_t)aux) << 16; 	//load upper bytes
	EE_ReadVariable(0x1, &aux);
	BMS->charge += aux;		//load lower bytes

	EE_ReadVariable(0x2, &aux);
	BMS->charge_min = ((uint32_t)aux) << 16; 	//load upper bytes
	EE_ReadVariable(0x3, &aux);
	BMS->charge_min += aux;		//load lower bytes

	EE_ReadVariable(0x4, &aux);
	BMS->charge_max = ((uint32_t)aux) << 16; 	//load upper bytes
	EE_ReadVariable(0x5, &aux);
	BMS->charge_max += aux;		//load lower bytes

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
