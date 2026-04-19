################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/src/app.c \
../app/src/driver_uart.c \
../app/src/linked_list.c \
../app/src/protocol.c \
../app/src/ringbuffer.c \
../app/src/task_inbound.c \
../app/src/task_outbound.c \
../app/src/task_process.c \
../app/src/task_tick.c 

OBJS += \
./app/src/app.o \
./app/src/driver_uart.o \
./app/src/linked_list.o \
./app/src/protocol.o \
./app/src/ringbuffer.o \
./app/src/task_inbound.o \
./app/src/task_outbound.o \
./app/src/task_process.o \
./app/src/task_tick.o 

C_DEPS += \
./app/src/app.d \
./app/src/driver_uart.d \
./app/src/linked_list.d \
./app/src/protocol.d \
./app/src/ringbuffer.d \
./app/src/task_inbound.d \
./app/src/task_outbound.d \
./app/src/task_process.d \
./app/src/task_tick.d 


# Each subdirectory must supply rules for building sources it contributes
app/src/%.o app/src/%.su app/src/%.cyclo: ../app/src/%.c app/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../app/inc -I../app/src -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-src

clean-app-2f-src:
	-$(RM) ./app/src/app.cyclo ./app/src/app.d ./app/src/app.o ./app/src/app.su ./app/src/driver_uart.cyclo ./app/src/driver_uart.d ./app/src/driver_uart.o ./app/src/driver_uart.su ./app/src/linked_list.cyclo ./app/src/linked_list.d ./app/src/linked_list.o ./app/src/linked_list.su ./app/src/protocol.cyclo ./app/src/protocol.d ./app/src/protocol.o ./app/src/protocol.su ./app/src/ringbuffer.cyclo ./app/src/ringbuffer.d ./app/src/ringbuffer.o ./app/src/ringbuffer.su ./app/src/task_inbound.cyclo ./app/src/task_inbound.d ./app/src/task_inbound.o ./app/src/task_inbound.su ./app/src/task_outbound.cyclo ./app/src/task_outbound.d ./app/src/task_outbound.o ./app/src/task_outbound.su ./app/src/task_process.cyclo ./app/src/task_process.d ./app/src/task_process.o ./app/src/task_process.su ./app/src/task_tick.cyclo ./app/src/task_tick.d ./app/src/task_tick.o ./app/src/task_tick.su

.PHONY: clean-app-2f-src

