#ifndef LTC_2_H
#define LTC_2_H

#include "stdarg.h"
#include "math.h"
#include "defines.h"
#include "stm32f1xx_hal.h"
#include "dwt_stm32_delay.h"

typedef struct LTC_command
{
	uint16_t NAME;			//Command name
	uint8_t BROADCAST;		//Broadcast or not

	//Command settings (bit description) (Table 35)
	uint16_t MD;			//ADC mode
	uint16_t DCP;			//Discharge permitted
	uint16_t CH;			//Cell selection for ADC conversion
	uint16_t PUP;			//Pull-up/Pull-down current for open-wire conversions
	uint16_t ST;			//Self-test mode selection
	uint16_t CHG;			//GPIO selection for ADC conversion
	uint16_t CHST;			//Status group selection

}LTC_command_t;

typedef struct LTC_config
{
	LTC_command_t *command;

	uint8_t ORDER:1;		//Set printing order (0 -> normal; 1 -> lowest to highest)

	//Configuration Register Group (Tables 36 and 46)
	uint8_t GPIO:5;			//GPIOx pin control
	uint8_t REFON:1;		//Reference powered up
	uint8_t SWTRD:1;		//SWTEN pin status
	uint8_t ADCOPT:1;		//ADC mode option
	uint16_t VUV:12;		//Undervoltage comparison voltage
	uint16_t VOV:12;		//Overvoltage comparison voltage
	uint8_t DCTO:4;			//Discharge time out value
	uint8_t ADC_READY;		//ADC is ready or not

}LTC_config_t;

typedef struct LTC_sensor
{
	//Sensor's info
	uint8_t ADDR;			//Sensor address
	uint8_t V_ERROR[12];	//Cell voltage error
	uint8_t T_ERROR[5];		//Temperature error
	uint16_t V_MAX;			//Maximum measured voltage
	uint16_t V_MIN;			//Minimum measured voltage
	uint16_t T_MAX;			//Maximum measured temperature
	uint16_t V_DELTA;		//Difference between maximum and minimum measured voltages
	uint16_t TOTAL_CHARGE;	//Stack total charge
	uint16_t DCC;			//Discharge cell x (DCC[x])


	//Cell voltage register group A to D (Tables 37, 38, 39 and 40)
	uint16_t CxV[12];		//12 voltage measures of 16 bits each

	//Auxiliary register group A to B (Tables 41 and 42)
	uint16_t GxV[5];		//5 GPIO measures of 16 bits each
	uint16_t REF;			//2nd reference voltage

	//Status register group A to B (Tables 43 and 44)
	uint16_t SOC;			//Sum of cells measurement
	uint16_t ITMP;			//Internal die temperature
	uint16_t VA;			//Analog power supply voltage
	uint16_t VD;			//Digital power supply voltage

}LTC_sensor_t;

//Command codes definitions (Table 34)
#define LTC_COMMAND_WRCFG 	0b00000000001		//Write configuration register group
#define LTC_COMMAND_RDCFG 	0b00000000010		//Read configuration register group
#define LTC_COMMAND_RDCVA 	0b00000000100		//Read cell voltage register group A
#define LTC_COMMAND_RDCVB 	0b00000000110		//Read cell voltage register group B
#define LTC_COMMAND_RDCVC 	0b00000001000		//Read cell voltage register group C
#define LTC_COMMAND_RDCVD 	0b00000001010		//Read cell voltage register group D
#define LTC_COMMAND_RDAUXA 	0b00000001100		//Read auxiliary register group A
#define LTC_COMMAND_RDAUXB 	0b00000001110		//Read auxiliary register group B
#define LTC_COMMAND_RDSTATA 0b00000010000		//Read status register group A
#define LTC_COMMAND_RDSTATB 0b00000010010		//Read status register group B
#define LTC_COMMAND_ADCV 	0b01001100000		//Start cell voltage ADC conversion and poll status
#define LTC_COMMAND_ADOW 	0b01000101000		//Start open wire ADC conversion and poll status
#define LTC_COMMAND_CVST 	0b01000000111		//Start self-test cell voltage conversion and poll status
#define LTC_COMMAND_ADAX 	0b10001100000		//Start GPIOs ADC conversion and poll status
#define LTC_COMMAND_AXST 	0b10000000111		//Start self-test GPIOs conversion and poll status
#define LTC_COMMAND_ADSTAT 	0b10001101000		//Start status group ADC conversion and poll status
#define LTC_COMMAND_STATST 	0b10000001111		//Start self-test status groups conversion and poll status
#define LTC_COMMAND_ADCVAX 	0b10001101111		//Start combined cell voltage and GPIO1, GPIO2 conversion and poll status
#define LTC_COMMAND_CLRCELL 0b11100010001		//Clear cell voltage register group
#define LTC_COMMAND_CLRAUX 	0b11100010010		//Clear auxiliary register group
#define LTC_COMMAND_CLRSTAT 0b11100010011		//Clear status register group
#define LTC_COMMAND_PLADC 	0b11100010100		//Poll ADC conversion status
#define LTC_COMMAND_DIAGN 	0b11100010101		//Diagnose MUX and poll status			*NOT IMPLEMENTED
#define LTC_COMMAND_WRCOMM 	0b11100100001		//Write COMM register group				*NOT IMPLEMENTED
#define LTC_COMMAND_RDCOMM 	0b11100100010		//Read COMM register group				*NOT IMPLEMENTED
#define LTC_COMMAND_STCOMM 	0b11100100011		//Start I�C/SPI communication			*NOT IMPLEMENTED

typedef enum
{

	LTC_READ_CELL 	= 0b0001,
	LTC_READ_GPIO  	= 0b0010,
	LTC_READ_STATUS = 0b0100,
	LTC_READ_CONFIG = 0b1000,

}LTC_READ;

typedef enum
{
	MD_FAST 	= 0b0010000000,		//27kHz (fast) or 14kHz
	MD_NORMAL 	= 0b0100000000,		//7kHz (normal) or  3kHz
	MD_FILTERED = 0b0110000000,		//26Hz (filtered) or  2kHz

}LTC_MD;

void LTC_init_pec_table();
uint16_t LTC_pec(uint16_t*, uint8_t);
uint16_t LTC_make_command(LTC_command_t*);
void LTC_CS(uint8_t);
uint16_t LTC_SPI(uint16_t);
void LTC_transmit_receive (uint16_t, uint16_t*, uint16_t*);
void LTC_send_command(LTC_config_t*, ...);
void LTC_init(LTC_config_t*);
static void LTC_temperature_convert(LTC_sensor_t*);
void LTC_wait(LTC_config_t*, LTC_sensor_t*);
void LTC_read(uint8_t, LTC_config_t*, LTC_sensor_t*);

#endif
