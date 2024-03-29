#ifndef DEFINES_H
#define DEFINES_H

//General defines
#define BLINK_TIME					500
#define BLINK_ERROR_TIME			200
#define N_OF_PACKS					6
#define N_OF_SLAVES					9
#define N_OF_CELLS					13
#define N_OF_THERMISTORS			4
#define N_OF_DHAB					3
#define V_ERROR_COM					65528
#define T_ERROR_COM					1
#define BALANCE_TIME				120
#define BALANCE_V_ERROR				5
#define FILTER_GAIN					255
#define N_AVERAGE					10

//BMS voltages
#define MAX_VOLTAGE					96
#define OP_VOLTAGE					92
#define ST_VOLTAGE					86
#define MAX_V_CELL					1000
#define MIN_V_CELL					1000

#define MAX_CELL_V_DISCHARGE		36000	//36500
#define MAX_CELL_V_CHARGE			36000
#define MAX_CELL_V_STORE			33000

#define MIN_CELL_V					28000	//27500
#define MIN_CELL_V_BALLANCE 		23000
#define MIN_GLV_V					13500

#define RESET_V_MAX					0
#define RESET_V_MIN					36000

//BMS temperatures
#define MAX_TEMPERATURE				500		//59000
#define RESET_T_MAX					0

//BMS operation modes
#define OPP_DEFAULT 				0
#define OPP_CHARGING 				1
#define OPP_BALANCING 				2
#define OPP_TESTING 				3

#define BMS_MONITORING 				0
#define BMS_CHARGING 				0b01
#define BMS_BALANCING 				0b10

//BMS communication modes
#define COMM_TC_ONLY 				0
#define COMM_UART_DEBUG				1
#define COMM_FULL					2

#define UART_RATE					500 	//ms
#define CAN_RATE					3000 	//ms
#define CAN_TEMPERATURE_ID  		3
#define CAN_ERROR_FLAG_ID  			4
#define CAN_CURRENT_ID				6
#define CAN_GENERAL_ID				7

//Error defines:
#define ERR_NO_ERROR 				0b000000000
#define ERR_OVER_VOLTAGE			0b000000001
#define ERR_UNDER_VOLTAGE			0b000000010
#define ERR_OVER_CURRENT			0b000000100
#define ERR_OVER_TEMPERATURE		0b000001000
#define ERR_OPEN_FUSES				0b000010000
#define ERR_COMM_ERROR				0b000100000
#define ERR_AIR_ERROR				0b001000000
#define ERR_GLV_VOLTAGE				0b010000000
#define ERR_BALANCE					0b100000000

//Logical defines
#define FALSE 	0
#define TRUE 	1

//Ports defines
#define __DLC__ 					8
#define DEBUG_Pin 					GPIO_PIN_13
#define DEBUG_GPIO_Port 			GPIOC
#define OSCI_Pin 					GPIO_PIN_0
#define OSCI_GPIO_Port 				GPIOD
#define OSCO_Pin 					GPIO_PIN_1
#define OSCO_GPIO_Port 				GPIOD
#define CURRENT2_200_Pin 			GPIO_PIN_0
#define CURRENT2_200_GPIO_Port 		GPIOA
#define CURRENT2_25_Pin 			GPIO_PIN_1
#define CURRENT2_25_GPIO_Port 		GPIOA
#define CURRENT1_200_Pin 			GPIO_PIN_2
#define CURRENT1_200_GPIO_Port 		GPIOA
#define CURRENT1_25_Pin 			GPIO_PIN_3
#define CURRENT1_25_GPIO_Port 		GPIOA
#define ISOSPI_CS_Pin 				GPIO_PIN_4
#define ISOSPI_CS_GPIO_Port 		GPIOA
#define ISOSPI_SCK_Pin 				GPIO_PIN_5
#define ISOSPI_SCK_GPIO_Port 		GPIOA
#define ISOSPI_MISO_Pin 			GPIO_PIN_6
#define ISOSPI_MISO_GPIO_Port 		GPIOA
#define ISOSPI_MOSI_Pin 			GPIO_PIN_7
#define ISOSPI_MOSI_GPIO_Port 		GPIOA
#define GLV_SAMPLE_Pin 				GPIO_PIN_15
#define GLV_SAMPLE_GPIO_Port 		GPIOA
#define MODE_SELECT_Pin 			GPIO_PIN_8
#define MODE_SELECT_GPIO_Port 		GPIOA
#define MODE_SELECT_EXTI_IRQn 		EXTI9_5_IRQn
#define FLAG_RESET_Pin 				GPIO_PIN_10
#define FLAG_RESET_GPIO_Port 		GPIOA
#define FLAG_RESET_EXTI_IRQn 		EXTI15_10_IRQn
#define AIR_ENABLE_Pin 				GPIO_PIN_3
#define AIR_ENABLE_GPIO_Port 		GPIOB
#define CHARGE_ENABLE_Pin 			GPIO_PIN_4
#define CHARGE_ENABLE_GPIO_Port 	GPIOB
#define ERR_LED_Pin 				GPIO_PIN_7
#define ERR_LED_GPIO_Port 			GPIOB
#define AIR_AUX_PLUS_Pin 			GPIO_PIN_8
#define AIR_AUX_PLUS_GPIO_Port 		GPIOB
#define AIR_AUX_MINUS_Pin 			GPIO_PIN_9
#define AIR_AUX_MINUS_GPIO_Port 	GPIOB

#endif
