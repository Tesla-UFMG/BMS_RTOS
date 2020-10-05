#include "LTC6804.h"
#include "defines.h"

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
