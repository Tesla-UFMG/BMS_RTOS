#ifndef BMS_H
#define BMS_H

#define BMS_CONVERT_CELL 	1
#define BMS_CONVERT_GPIO	2
#define BMS_CONVERT_STAT	4
#define BMS_CONVERT_CONFIG	8


#include "stdlib.h"
#include "LTC6804.h"
#include "DHAB_s125.h"
#include "CAN.h"
#include "EEPROM.h"

typedef struct BMS_struct
{
	//General settings
	uint16_t error;							//Indicates the error
	uint8_t communication_mode;				//Set BMS communication mode
	uint8_t mode;							//Set BMS mode
	uint8_t discharging;					//
	uint8_t operating_packs;				//
	uint8_t status;							//BMS status

	LTC_sensor_t *sensor[N_OF_SLAVES];		//Number of LTC6804 used
	LTC_config_t *config;					//LTC6804's configuration
	DHAB_sensor_t *dhabSensor[N_OF_DHAB];	//Number of DHAB current sensor used

	uint16_t v_GLV;							//GLV voltage
	uint16_t v_TS;							//TS voltage
	uint16_t v_min;							//Lowest cell voltage in the bank
	uint16_t v_max;							//Highest cell voltage in the bank
	uint16_t v_delta;						//Difference between highest and lowest voltage in the bank
	uint16_t t_max;							//Highest temperature in the bank

	//Current settings
	//float current[N_OF_DHAB];				//Current real value
	//float c_adc[N_OF_DHAB];				//Current ADC value

	//Charge settings
	int32_t charge;							//BMS charge
	int32_t charge_max;						//
	int32_t charge_min;						//
	float charge_percentage;				//BMS charge percentage
	float charge_variation_percentage;		//BMS Charge variation percentage
	float discharge_percentage;				//BMS discharge percentage
	float discharge_variation_percentage;	//BMS discharge variation percentage

	//AIR settings
	uint8_t AIR;

}BMS_struct_t;

void BMS_convert(uint8_t, BMS_struct_t*);
void BMS_monitoring(BMS_struct_t*);
void BMS_set_thermistor_zeros(BMS_struct_t*);
void BMS_init(BMS_struct_t*);
void BMS_error(BMS_struct_t*);
void CAN_buf(uint8_t*, uint16_t, uint16_t, uint16_t, uint16_t);
void BMS_CAN_voltage(BMS_struct_t*);
void BMS_CAN_temperature(BMS_struct_t*);
void BMS_CAN_current(BMS_struct_t*);
void BMS_CAN_GLV(BMS_struct_t*);
void BMS_CAN_info(BMS_struct_t*);
void BMS_can(BMS_struct_t*);
void BMS_SoC(BMS_struct_t*);
void BMS_initial_SOC(BMS_struct_t*);
void BMS_charging(BMS_struct_t*);
void BMS_discharging(BMS_struct_t*);

#endif
