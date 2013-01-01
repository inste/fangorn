################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gl.c \
../main.c \
../parser.c \
../rewrite.c \
../stack.c 

OBJS += \
./gl.o \
./main.o \
./parser.o \
./rewrite.o \
./stack.o 

C_DEPS += \
./gl.d \
./main.d \
./parser.d \
./rewrite.d \
./stack.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


