#ifndef DHAB_s125_H
#define DHAB_s125_H

#include "dwt_stm32_delay.h"
#include "time.h"
#include "defines.h"

typedef struct DHAB_sensor
{
	double current;
	double currentADC;
	double currentZero;
	double currentGain;
	double coulombCounting;

}DHAB_sensor;

float DHAB_filter(float, float);
void DHAB_read(DHAB_sensor*);
void DHAB_convert();
double DHAB_currentIntegration(DHAB_sensor*);

#endif
