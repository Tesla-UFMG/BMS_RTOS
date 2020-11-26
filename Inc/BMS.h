#ifndef BMS_H
#define BMS_H

#include "stdlib.h"
#include "LTC6804.h"
#include "CAN.h"
#include "eeprom.h"

typedef struct BMS_struct
{
	//General settings
	uint16_t error;						//Indicates the error
	uint8_t communication_mode;			//Set BMS communication mode
	uint8_t mode;						//Set BMS mode
	uint8_t discharging;				//
	uint8_t operating_packs;			//
	uint8_t status;						//BMS status

	LTC_sensor *sensor[N_OF_SLAVES];	//Number of LTC6804 used
	LTC_config *config;					//LTC6804's configuration
	//DHAB_sensor *dhabSensor[N_OF_DHAB];	//Number of DHAB current sensor used

	uint16_t v_GLV;						//GLV voltage
	uint16_t v_TS;						//TS voltage
	uint16_t v_min;						//Lowest cell voltage in the bank
	uint16_t v_max;						//Highest cell voltage in the bank
	uint16_t v_delta;					//Difference between highest and lowest voltage in the bank
	uint16_t t_max;						//Highest temperature in the bank

	//Current settings
	float current[4];					//Current real value
	float c_adc[4];						//Current ADC value

	//Charge settings
	int32_t charge;						//BMS charge
	int32_t charge_max;					//
	int32_t charge_min;					//
	float charge_percentage;			//BMS charge percentage
	float charge_variation_percent;		//BMS Charge variation percentage
	float discharge_percent;			//BMS discharge percentage
	float discharge_variation_percent;	//BMS discharge variation percentage

	//AIR settings
	uint8_t AIR;

}BMS_struct;

void BMS_convert(uint8_t, BMS_struct*);
void BMS_init(BMS_struct*);
void BMS_error(BMS_struct*);
void BMS_can(BMS_struct*);
void BMS_SoC(BMS_struct*);
void BMS_monitoring(BMS_struct*);

#endif
