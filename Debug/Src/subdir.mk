################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/freertos.c \
../Src/main.c \
../Src/stm32f1xx_hal_msp.c \
../Src/stm32f1xx_hal_timebase_tim.c \
../Src/stm32f1xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f1xx.c 

OBJS += \
./Src/freertos.o \
./Src/main.o \
./Src/stm32f1xx_hal_msp.o \
./Src/stm32f1xx_hal_timebase_tim.o \
./Src/stm32f1xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f1xx.o 

C_DEPS += \
./Src/freertos.d \
./Src/main.d \
./Src/stm32f1xx_hal_msp.d \
./Src/stm32f1xx_hal_timebase_tim.d \
./Src/stm32f1xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F103xB -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Inc" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Drivers/STM32F1xx_HAL_Driver/Inc" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"C:/Users/rbgue/OneDrive - ufmg.br/UFMG/Tesla/Baterias e Segurança/Códigos/BMS RTOS/BMS_RTOS/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


