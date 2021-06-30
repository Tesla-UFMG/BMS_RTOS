#include "DHAB_s125.h"

/*float DHAB_filter(float old, float new)
{
	return(FILTER_GAIN*old + new)/(FILTER_GAIN + 1);
}

void DHAB_read(DHAB_sensor* sensor)
{
	if(initialReadings < 5)
	{
		for(uint8_t i = 0; i < N_OF_DHAB; i++)
		{

			sensor[i].currentZero += ((float)ADC_BUF[i] * sensor[i].currentGain);	//ADC_BUF??? COMO LER VALOR ADC
			initialReadings++;

			if(initialReadings == 5)
			{
				for(uint8_t j = 0; j < N_OF_DHAB; j++)
					sensor[j].currentZero = sensor[j].currentZero/5;
			}
		}
	}
	else
	{
		for(uint8_t i = 0; i < N_OF_DHAB; i++)
		{
			sensor[i].currentADC = DHAB_filter((float)sensor[i].currentADC, (float)ADC_BUF[i]);
			sensor[i].current = DHAB_filter(sensor[i].current, ((float)ADC_BUF[i] * sensor[i].currentGain) - sensor[i].currentZero);
		}
	}
}*/

/*******************************************************
Function double DHAB_currentIntegration()

V1.0:
The function takes the read values from the current sensors
and calculates the amount of charge drawn or loaded by the
battery integrating the current in time after the conversion
is done.

Version 1.0 - Initial release 07/12/2020 by Tesla UFMG
*******************************************************/
double DHAB_current_integration(DHAB_sensor_t* sensor)
{
	double total_coulomb_counting = 0;
	clock_t running_time = clock();

	if(ADC_FLAG_EOC && ADC_FLAG_JEOC)
	{
		running_time = clock() - running_time;
		double timeSpent = (double)running_time/CLOCKS_PER_SEC;

		sensor->coulombCounting= sensor->current*timeSpent;
		total_coulomb_counting = total_coulomb_counting + sensor->coulombCounting;
	}

	return total_coulomb_counting;
}
