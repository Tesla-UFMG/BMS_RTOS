#ifndef TASKS_H
#define TASKS_H

#include "BMS.h"
#include "nextion_functions.h"
#include "DMA_USART.h"
#include "EEPROM.h"
#include "cmsis_os.h"

extern BMS_struct_t* BMS;
extern CAN_HandleTypeDef hcan;
extern CAN_TxHeaderTypeDef pHeader;
extern osSemaphoreId_t ltcSemaphoreHandle;
extern osSemaphoreId_t commSemaphoreHandle;
extern osMessageQueueId_t q_maxVoltagesHandle;
extern osMessageQueueId_t q_minVoltagesHandle;
extern osMessageQueueId_t q_maxTemperaturesHandle;
extern osMessageQueueId_t q_reportErrorHandle;

#endif
