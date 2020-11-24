#include "BMS.h"

void BMS_init(BMS_struct* BMS)
{
	/*These three instructions set the initial configuration for the CAN communication and
	  start the reception process and enable reception interrupt*/
	CAN_Config_Filter();
	CAN_Config_Frames();
	CAN_Receive_IT();

	BMS->config = (LTC_config*) calloc(1 ,sizeof(LTC_config));
	BMS->config->command = (LTC_command*) calloc(1 ,sizeof(LTC_command));

	for(int i = 0; i < N_OF_SLAVES; i++)
	{
		BMS->sensor[i] = (LTC_sensor*) calloc(1, sizeof(LTC_sensor));
		BMS->sensor[i]->ADDR = i;
		LTC_init(BMS->config); //OBSERVAÇÃO 1
	}

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

	BMS_initial_SOC(BMS);
	BMS_set_thermistor_zeros(BMS);
}
