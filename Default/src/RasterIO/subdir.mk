################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/RasterIO/RasterController.cpp \
../src/RasterIO/RasterObject.cpp 

OBJS += \
./src/RasterIO/RasterController.o \
./src/RasterIO/RasterObject.o 

CPP_DEPS += \
./src/RasterIO/RasterController.d \
./src/RasterIO/RasterObject.d 


# Each subdirectory must supply rules for building sources it contributes
src/RasterIO/%.o: ../src/RasterIO/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


