################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ExpressionParser/error.cpp \
../src/ExpressionParser/functions.cpp \
../src/ExpressionParser/parser.cpp \
../src/ExpressionParser/variablelist.cpp 

OBJS += \
./src/ExpressionParser/error.o \
./src/ExpressionParser/functions.o \
./src/ExpressionParser/parser.o \
./src/ExpressionParser/variablelist.o 

CPP_DEPS += \
./src/ExpressionParser/error.d \
./src/ExpressionParser/functions.d \
./src/ExpressionParser/parser.d \
./src/ExpressionParser/variablelist.d 


# Each subdirectory must supply rules for building sources it contributes
src/ExpressionParser/%.o: ../src/ExpressionParser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/include/gdal/ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


