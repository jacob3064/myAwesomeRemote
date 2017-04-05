# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./src/init-default.c \
./src/main.c \
./src/uart.c \
./src/gpio.c \
./src/i2c.c \
./src/interrupt.c \
./src/timer.c \
./src/du.c \
./src/dma.c	\
./src/cache.c \
./src/sys_api.c \
./src/vpu_api.c \
./src/vpu_test.c \

S_UPPER_SRCS += \
./src/crt0.S 

C_DEPS += \
./src/init-default.d \
./src/main.d \
./src/uart.d \
./src/gpio.d \
./src/i2c.d \
./src/interrupt.d \
./src/timer.d \
./src/du.d \
./src/dma.d	\
./src/cache.d \
./src/sys_api.d \
./src/vpu_api.d \
./src/vpu_test.d \

S_UPPER_DEPS += \
./src/crt0.d 

INC_DIRS += \
./include

OBJS += \
$(S_UPPER_SRCS:.S=.o) $(C_SRCS:.c=.o)

# Each subdirectory must supply rules for building sources it contributes
src/%.o: src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -I$(INC_DIRS) -mcmodel=medium -g3 -Wall -mcpu=d1088-spu -c -fmessage-length=0 -fsingle-precision-constant -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -I$(INC_DIRS) -mcmodel=medium -g3 -Wall -mcpu=d1088-spu -c -fmessage-length=0 -fsingle-precision-constant -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


