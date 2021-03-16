/*
 * nextion_functions.h
 *
 *  Created on: 13 de ago de 2019
 *      Author: Rodolfo Lessa
 */

#ifndef NEXTION_FUNCTIONS_H_
#define NEXTION_FUNCTIONS_H_

#include "BMS.h"
#include "nextion.h"

//Nextion pages defines
#define N_PAGE0 0
#define N_PAGE1 1
#define N_PAGE2 2
#define N_PAGE3 3
#define N_PAGE4 4
#define N_PAGE5 5
#define N_PAGE6 6
#define N_PAGE7 7

uint8_t actual_page;

void uart3_message_received(BMS_struct_t*);
uint8_t nexSetPageError(BMS_struct_t*);
void nex_loop(BMS_struct_t*);

#endif /* NEXTION_FUNCTIONS_H_ */
