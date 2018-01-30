################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Definitions/CoodLocation.cpp \
../src/Definitions/EnvironmentalCurve.cpp \
../src/Definitions/EnvironmentalHadley.cpp \
../src/Definitions/GeoLocation.cpp \
../src/Definitions/IndividualOrganism.cpp \
../src/Definitions/NicheBreadth.cpp \
../src/Definitions/Scenario.cpp \
../src/Definitions/SparseMap.cpp \
../src/Definitions/SpeciesObject.cpp 

OBJS += \
./src/Definitions/CoodLocation.o \
./src/Definitions/EnvironmentalCurve.o \
./src/Definitions/EnvironmentalHadley.o \
./src/Definitions/GeoLocation.o \
./src/Definitions/IndividualOrganism.o \
./src/Definitions/NicheBreadth.o \
./src/Definitions/Scenario.o \
./src/Definitions/SparseMap.o \
./src/Definitions/SpeciesObject.o 

CPP_DEPS += \
./src/Definitions/CoodLocation.d \
./src/Definitions/EnvironmentalCurve.d \
./src/Definitions/EnvironmentalHadley.d \
./src/Definitions/GeoLocation.d \
./src/Definitions/IndividualOrganism.d \
./src/Definitions/NicheBreadth.d \
./src/Definitions/Scenario.d \
./src/Definitions/SparseMap.d \
./src/Definitions/SpeciesObject.d 


# Each subdirectory must supply rules for building sources it contributes
src/Definitions/%.o: ../src/Definitions/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


