################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RVMSIS/core_riscv.c 

OBJS += \
./RVMSIS/core_riscv.o 

C_DEPS += \
./RVMSIS/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
RVMSIS/%.o: ../RVMSIS/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Startup" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\HAL\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\APP\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Profile\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\StdPeriphDriver\inc" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Ld" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\LIB" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

