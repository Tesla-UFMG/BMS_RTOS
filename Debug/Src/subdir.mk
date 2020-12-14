################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/ADC.c \
../Src/BMS.c \
../Src/CAN.c \
../Src/DHAB_s125.c \
../Src/DMA.c \
../Src/DMA_USART.c \
../Src/EEPROM.c \
../Src/GPIO.c \
../Src/LTC6804.c \
../Src/SPI.c \
../Src/USART.c \
../Src/dwt_stm32_delay.c \
../Src/flash.c \
../Src/freertos.c \
../Src/main.c \
../Src/stm32f1xx_hal_msp.c \
../Src/stm32f1xx_hal_timebase_tim.c \
../Src/stm32f1xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f1xx.c \
../Src/tim.c 

OBJS += \
./Src/ADC.o \
./Src/BMS.o \
./Src/CAN.o \
./Src/DHAB_s125.o \
./Src/DMA.o \
./Src/DMA_USART.o \
./Src/EEPROM.o \
./Src/GPIO.o \
./Src/LTC6804.o \
./Src/SPI.o \
./Src/USART.o \
./Src/dwt_stm32_delay.o \
./Src/flash.o \
./Src/freertos.o \
./Src/main.o \
./Src/stm32f1xx_hal_msp.o \
./Src/stm32f1xx_hal_timebase_tim.o \
./Src/stm32f1xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f1xx.o \
./Src/tim.o 

C_DEPS += \
./Src/ADC.d \
./Src/BMS.d \
./Src/CAN.d \
./Src/DHAB_s125.d \
./Src/DMA.d \
./Src/DMA_USART.d \
./Src/EEPROM.d \
./Src/GPIO.d \
./Src/LTC6804.d \
./Src/SPI.d \
./Src/USART.d \
./Src/dwt_stm32_delay.d \
./Src/flash.d \
./Src/freertos.d \
./Src/main.d \
./Src/stm32f1xx_hal_msp.d \
./Src/stm32f1xx_hal_timebase_tim.d \
./Src/stm32f1xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f1xx.d \
./Src/tim.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F103xB -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Inc" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Drivers/STM32F1xx_HAL_Driver/Inc" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


