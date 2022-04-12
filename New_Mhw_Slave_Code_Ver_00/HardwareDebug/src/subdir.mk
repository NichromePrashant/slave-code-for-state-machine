################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/New_Mhw_Slave_Code_Ver_00.c \
../src/adc_cs5532_code.c \
../src/calibration_code.c \
../src/can_code.c \
../src/delay_code.c \
../src/manual_code.c \
../src/run_code.c \
../src/stepper_code.c \
../src/vibrator_code.c 

COMPILER_OBJS += \
src/New_Mhw_Slave_Code_Ver_00.obj \
src/adc_cs5532_code.obj \
src/calibration_code.obj \
src/can_code.obj \
src/delay_code.obj \
src/manual_code.obj \
src/run_code.obj \
src/stepper_code.obj \
src/vibrator_code.obj 

C_DEPS += \
src/New_Mhw_Slave_Code_Ver_00.d \
src/adc_cs5532_code.d \
src/calibration_code.d \
src/can_code.d \
src/delay_code.d \
src/manual_code.d \
src/run_code.d \
src/stepper_code.d \
src/vibrator_code.d 

# Each subdirectory must supply rules for building sources it contributes
src/%.obj: ../src/%.c 
	@echo 'Scanning and building file: $<'
	@echo 'Invoking: Scanner and Compiler'
	@echo src\cDepSubCommand.tmp=
	@sed -e "s/^/ /" "src\cDepSubCommand.tmp"
	ccrx -subcommand="src\cDepSubCommand.tmp" -output=dep="$(@:%.obj=%.d)"  -MT="$(@:%.d=%.obj)"  -MT="$(@:%.obj=%.d)" "$<"
	@echo src\cSubCommand.tmp=
	@sed -e "s/^/ /" "src\cSubCommand.tmp"
	ccrx -subcommand="src\cSubCommand.tmp" "$<"
	@echo 'Finished Scanning and building: $<'
	@echo.

