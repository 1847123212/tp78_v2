################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/MCU.c \
../HAL/RTC.c \
../HAL/SLEEP.c 

OBJS += \
./HAL/MCU.o \
./HAL/RTC.o \
./HAL/SLEEP.o 

C_DEPS += \
./HAL/MCU.d \
./HAL/RTC.d \
./HAL/SLEEP.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DDEBUG=1 -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Startup" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\HAL\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\APP\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Profile\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\StdPeriphDriver\inc" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Ld" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\LIB" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
