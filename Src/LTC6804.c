#include "LTC6804.h"

#define BYTESWAP(word) ((word >> 8) + (word << 8))

uint16_t pecTable[256];

/*******************************************************
 Function void LTC_init_pecTable()

V1.0:
The function will initialize a PEC15 table array called
pecTable[], which will be used in all future PEC calcula-
tions. This function should only be called once when the
microcontroller starts. For more informations, read the
"Improved PEC Calculation" section on page 63 of LTC6804's
datasheet.

 Version 1.0 - Initial release 21/10/2020 by Tesla UFMG
*******************************************************/
void LTC_init_pecTable()
{
	for(int i = 0; i < 256; i++)
	{
		uint16_t remainder = i << 7;
		for(int bit = 8; bit > 0; --bit)
		{
			if(remainder & 0x4000)
			{
				remainder = remainder << 1;
				remainder = remainder ^ 0x4599;
			}
			else
				remainder = remainder << 1;
		}
		pecTable[i] = remainder&0xFFFF;
	}
}

/*******************************************************
 Function void LTC_pec(uint16_t*, uint8_t)

V1.0:
The function calculates the PEC and returns the correct
15 bit PEC for byte arrays of any given length. For more
informations, read the "Improved PEC Calculation" section
on page 63 of LTC6804's datasheet.

 Version 1.0 - Initial release 21/10/2020 by Tesla UFMG
*******************************************************/
uint16_t LTC_pec(uint16_t *data, uint8_t len)
{
	uint16_t remainder, address;

	//PEC seed
	remainder = 16;
	for(int i = 0; i < len; i++)
	{
		//Calculate PEC table address
		address = ((remainder >> 7) ^ data[i]) & 0xFF;
		remainder = (remainder << 8) ^ pecTable[address];
	}
	//The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
	return (remainder*2);
}

/*******************************************************
 Function uint16_t LTC_make_command(LTC_command*)

V1.0:
The function is responsible for returning the bits of a
command divided into different parts of bits depending
on the command about to use and its bits of interest.
See Table 34 for clear understanding.

 Version 1.0 - Initial release 20/10/2020 by Tesla UFMG
*******************************************************/
uint16_t LTC_make_command(LTC_command *command)
{
	switch(command->NAME)
	{
		case LTC_COMMAND_ADCV:
			return command->NAME | command->MD | command->DCP | command->CH;
			break;

		case LTC_COMMAND_ADOW:
			return command->NAME | command->MD | command->PUP | command->DCP | command->CH;
			break;

		case LTC_COMMAND_CVST:
		case LTC_COMMAND_AXST:
		case LTC_COMMAND_STATST:
			return command->NAME | command->MD | command->ST;
			break;

		case LTC_COMMAND_ADAX:
			return command->NAME | command->MD | command->CHG;
			break;

		case LTC_COMMAND_ADSTAT:
			return command->NAME | command->MD | command->CHST;
			break;

		case LTC_COMMAND_ADCVAX:
			return command->NAME | command->MD | command->CHG;
			break;

		default:
			return command->NAME;
			break;
	}
}

/*******************************************************
 Function void LTC_CS(uint8_t)

V1.0:
The function is used as an auxiliary function for writing
a bit in the CS GPIO port and waiting for 10us.

 Version 1.0 - Initial release 21/10/2020 by Tesla UFMG
*******************************************************/
void LTC_CS(uint8_t level)
{
	HAL_GPIO_WritePin(ISOSPI_CS_GPIO_Port, ISOSPI_CS_Pin , level);
	DWT_Delay_us(10);
}

/*******************************************************
 Function void LTC_spi(uint16_t)

V1.0:
The function is responsible for making the SPI communi-
cation with the LTC6804 IC.

 Version 1.0 - Initial release 21/10/2020 by Tesla UFMG
*******************************************************/
uint16_t LTC_spi(uint16_t Tx_data)
{
	uint16_t Rx_data = 0;
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&Tx_data, (uint8_t *)&Rx_data, 1, 50);

	return(BYTESWAP(Rx_data));
}

/*******************************************************
 Function void LTC_transmit_recieve (uint16_t, uint16_t*, uint16_t*)

V1.0:
The function is responsible for doing the transmit/receive messages'
routine, following the steps described in the LTC6804 datasheet.

 Version 1.0 - Initial release 20/10/2020 by Tesla UFMG
*******************************************************/
void LTC_transmit_receive (uint16_t command, uint16_t* tx_data, uint16_t* rx_data)
{
	uint16_t pec = LTC_pec(&command, 1);
	uint16_t buffer[4] = {command, pec, 0, 0}; 	//Tx buffer

	//Wake-up routine
	LTC_CS(0);
	LTC_spi(0);
	LTC_CS(1);

	//Send command routine
	LTC_CS(0);
	LTC_spi(buffer[0]);
	LTC_spi(buffer[1]);

	//Transmit/Receive data routine
	for(uint8_t i = 0; i < 4; ++i)
		buffer[i] = tx_data[i];

	if((command & 0x07FF) == LTC_COMMAND_WRCFG)
	{
		pec = LTC_pec(tx_data, 3);
		buffer[3] = pec;
	}

	if((buffer[0] & 0x07FF) < LTC_COMMAND_ADCV)
	{
		for (uint8_t i = 0; i < 4; ++i)
			rx_data[i] = LTC_spi(buffer[i]);
	}

	LTC_CS(1);
}

/*******************************************************
 Function void LTC_send_command(LTC_config*, ...)

V1.0:
The function sends the command made in the LTC_make_command
function and do the tasks accordingly to the command sent.

 Version 1.0 - Initial release 08/10/2020 by Tesla UFMG
*******************************************************/
void LTC_sendCommand(LTC_config *config, ...)
{
	LTC_sensor *sensor;

	uint16_t tx_data[3] = {0, 0, 0};
	uint16_t rx_data[3] = {0, 0, 0};

	if(!config->command->BROADCAST)
	{
		va_list list;
		va_start(list, 1);

		sensor = va_arg(list, LTC_sensor*);
		config->command->NAME  |= ((sensor->ADDR & (0x1111 * ~config->command->BROADCAST)) | ~config->command->BROADCAST << 4) << 11;
		tx_data[2] |= ((config->DCC & 0xff) << 8) | ((config->DCC & 0xf00) >> 8);
		va_end(list);
	}

	if((config->command->NAME & 0x07FF) == LTC_COMMAND_WRCFG)
	{
		tx_data[0] = (config->ADCOPT << 8) | (config->SWTRD << 9) | (config->REFON << 10) | (config->GPIO << 11) | (config->VUV);
		tx_data[1] = (config->VUV >> 8) | (config->VOV << 4);
		tx_data[2] |= ((config->DCTO & 0xf) << 4);
	}

	LTC_transmit_receive(LTC_make_command(config->command), tx_data, rx_data);

	switch(config->command->NAME & 0x07FF)
	{
	case LTC_COMMAND_RDCFG:
		break;

	case LTC_COMMAND_RDCVA:
		sensor->CxV[0] = rx_data[0];
		sensor->CxV[1] = rx_data[1];
		sensor->CxV[2] = rx_data[2];
		break;

	case LTC_COMMAND_RDCVB:
		sensor->CxV[3] = rx_data[0];
		if(sensor->ADDR >= 6)
			sensor->CxV[4] = rx_data[1];
		//sensor->CxV[5] = rx_data[2];
		break;

	case LTC_COMMAND_RDCVC:
		sensor->CxV[6] = rx_data[0];
		sensor->CxV[7] = rx_data[1];
		sensor->CxV[8] = rx_data[2];
		break;

	case LTC_COMMAND_RDCVD:
		sensor->CxV[9]  = rx_data[0];
		if(sensor->ADDR >= 6)
			sensor->CxV[10] = rx_data[1];
		//sensor->CxV[11] = rx_data[2];
		break;

	case LTC_COMMAND_RDAUXA:
		sensor->GxV[0] = rx_data[0];
		sensor->GxV[1] = rx_data[1];
		sensor->GxV[2] = rx_data[2];
		break;

	case LTC_COMMAND_RDAUXB:
		sensor->GxV[3] = rx_data[0];
		sensor->GxV[4] = rx_data[1];
		sensor->REF =    rx_data[2];
		break;

	case LTC_COMMAND_RDSTATA:
		sensor->SOC =  rx_data[0] * 0.2;
		sensor->ITMP = rx_data[1] * 7.5;
		sensor->VA =   rx_data[2];
		break;

	case LTC_COMMAND_RDSTATB:
		sensor->VD = rx_data[0];
		uint32_t flag = rx_data[1] | (uint32_t)rx_data[2] << 16;
		for (int j = 0; j < 12; j++)
			sensor->V_ERROR[j] = (flag >> (j * 2)) & 0x3;
		break;

	case LTC_COMMAND_ADCV:
	case LTC_COMMAND_ADOW:
	case LTC_COMMAND_CVST:
	case LTC_COMMAND_ADAX:
	case LTC_COMMAND_AXST:
	case LTC_COMMAND_ADSTAT:
	case LTC_COMMAND_STATST:
	case LTC_COMMAND_ADCVAX:
		config->ADC_READY = FALSE;
		break;

	case LTC_COMMAND_PLADC:
		if(rx_data[0] == 0 || rx_data[1] == 0 || rx_data[2] == 0)
			config->ADC_READY = FALSE;
		else
			config->ADC_READY = TRUE;
		break;

	case LTC_COMMAND_DIAGN:
		break;

	case LTC_COMMAND_WRCOMM:
		break;

	case LTC_COMMAND_RDCOMM:
		break;

	case LTC_COMMAND_STCOMM:
		break;

	default:
		break;
	}
}

/*******************************************************
 Function void LTC_init(LTC_config*)

V1.0:
The function initializes the LTC IC by assigning its initial
configuration.

 Version 1.0 - Initial release 02/10/2020 by Tesla UFMG
*******************************************************/
void LTC_init(LTC_config *config)
{
	config->GPIO = 0x1F;
	config->REFON = 0;
	config->SWTRD = 0;
	config->ADCOPT = 0;
	config->VUV = 0;
	config->VOV = 0;
	config->DCC = 0;
	config->DCTO = 0;

	config->command->MD = MD_FILTERED;
	config->command->BROADCAST = TRUE;
	LTC_sendCommand(config);
}
