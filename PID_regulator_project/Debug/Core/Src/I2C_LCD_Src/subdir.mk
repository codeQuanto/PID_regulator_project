################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/I2C_LCD_Src/I2C_LCD.c \
../Core/Src/I2C_LCD_Src/I2C_LCD_cfg.c \
../Core/Src/I2C_LCD_Src/Util.c 

OBJS += \
./Core/Src/I2C_LCD_Src/I2C_LCD.o \
./Core/Src/I2C_LCD_Src/I2C_LCD_cfg.o \
./Core/Src/I2C_LCD_Src/Util.o 

C_DEPS += \
./Core/Src/I2C_LCD_Src/I2C_LCD.d \
./Core/Src/I2C_LCD_Src/I2C_LCD_cfg.d \
./Core/Src/I2C_LCD_Src/Util.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/I2C_LCD_Src/%.o Core/Src/I2C_LCD_Src/%.su Core/Src/I2C_LCD_Src/%.cyclo: ../Core/Src/I2C_LCD_Src/%.c Core/Src/I2C_LCD_Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-I2C_LCD_Src

clean-Core-2f-Src-2f-I2C_LCD_Src:
	-$(RM) ./Core/Src/I2C_LCD_Src/I2C_LCD.cyclo ./Core/Src/I2C_LCD_Src/I2C_LCD.d ./Core/Src/I2C_LCD_Src/I2C_LCD.o ./Core/Src/I2C_LCD_Src/I2C_LCD.su ./Core/Src/I2C_LCD_Src/I2C_LCD_cfg.cyclo ./Core/Src/I2C_LCD_Src/I2C_LCD_cfg.d ./Core/Src/I2C_LCD_Src/I2C_LCD_cfg.o ./Core/Src/I2C_LCD_Src/I2C_LCD_cfg.su ./Core/Src/I2C_LCD_Src/Util.cyclo ./Core/Src/I2C_LCD_Src/Util.d ./Core/Src/I2C_LCD_Src/Util.o ./Core/Src/I2C_LCD_Src/Util.su

.PHONY: clean-Core-2f-Src-2f-I2C_LCD_Src

