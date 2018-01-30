################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/JsonPaster/json_reader.cpp \
../src/JsonPaster/json_value.cpp \
../src/JsonPaster/json_writer.cpp 

OBJS += \
./src/JsonPaster/json_reader.o \
./src/JsonPaster/json_value.o \
./src/JsonPaster/json_writer.o 

CPP_DEPS += \
./src/JsonPaster/json_reader.d \
./src/JsonPaster/json_value.d \
./src/JsonPaster/json_writer.d 


# Each subdirectory must supply rules for building sources it contributes
src/JsonPaster/%.o: ../src/JsonPaster/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


