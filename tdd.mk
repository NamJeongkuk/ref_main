PROJECT_NAME := multi-target-starter-kit

APPLCOMMON_DIR := lib/applcommon
REFERCOMMON_DIR = lib/Common
PROJECT_DIR := src
TESTING_DIR := test
TESTS_DIR := test/Tests
OUTPUT_DIR := test/Build

RUN_APPLCOMMON_TESTS := N
DISPLAY_ASCII_ART_RESULTS?=Y

CPPUTEST_CFLAGS += -DTDD_BUILD
CPPUTEST_CPPFLAGS += -DTDD_BUILD

# Specific files to include if you don't want the entire directory included
SRC_FILES := \

# List of directories that contain your code under test
SRC_DIRS := \
   $(PROJECT_DIR)/Application \
   $(PROJECT_DIR)/Application/SignOfLife \
   $(PROJECT_DIR)/Application/Auger \
   $(PROJECT_DIR)/Application/ApplianceFeatureApiUpdater \
   $(PROJECT_DIR)/Application/BoardDiscovery \
   $(PROJECT_DIR)/Application/Compressor \
   $(PROJECT_DIR)/Application/ConfigurationPlugins \
   $(PROJECT_DIR)/Application/Configurations \
   $(PROJECT_DIR)/Application/ConvertibleCompartment \
   $(PROJECT_DIR)/Application/Cooling \
   $(PROJECT_DIR)/Application/Damper \
   $(PROJECT_DIR)/Application/DataSource \
   $(PROJECT_DIR)/Application/Debounce \
   $(PROJECT_DIR)/Application/Defrost \
   $(PROJECT_DIR)/Application/DemandResponse \
   $(PROJECT_DIR)/Application/Dispenser \
   $(PROJECT_DIR)/Application/Door \
   $(PROJECT_DIR)/Application/DutyCycle \
   $(PROJECT_DIR)/Application/EnhancedSabbath \
   $(PROJECT_DIR)/Application/Evaporator \
   $(PROJECT_DIR)/Application/Factory \
   $(PROJECT_DIR)/Application/Fan \
   $(PROJECT_DIR)/Application/FlowMeter \
   $(PROJECT_DIR)/Application/Grid \
   $(PROJECT_DIR)/Application/Grid/GridFunctions \
   $(PROJECT_DIR)/Application/Grid/GridStateTables \
   $(PROJECT_DIR)/Application/Heater \
   $(PROJECT_DIR)/Application/IceMaker \
   $(PROJECT_DIR)/Application/IceMaker/AluminumMoldIceMaker \
   $(PROJECT_DIR)/Application/IceMaker/TwistTrayIceMaker \
   $(PROJECT_DIR)/Application/IsolationWaterValve \
   $(PROJECT_DIR)/Application/Lighting \
   $(PROJECT_DIR)/Application/Motor \
   $(PROJECT_DIR)/Application/ParametricData \
   $(PROJECT_DIR)/Application/Plugins \
   $(PROJECT_DIR)/Application/Pulldown \
   $(PROJECT_DIR)/Application/RecessHeater \
   $(PROJECT_DIR)/Application/RequestStatusUpdater \
   $(PROJECT_DIR)/Application/RfidCommunication \
   $(PROJECT_DIR)/Application/Sabbath \
   $(PROJECT_DIR)/Application/Sensor \
   $(PROJECT_DIR)/Application/ServiceDiagnostics \
   $(PROJECT_DIR)/Application/Setpoints \
   $(PROJECT_DIR)/Application/StepperMotor \
   $(PROJECT_DIR)/Application/SystemMonitor \
   $(PROJECT_DIR)/Application/SnackMode \
   $(PROJECT_DIR)/Application/Temperature \
   $(PROJECT_DIR)/Application/TimerModule \
   $(PROJECT_DIR)/Application/Valve \
   $(PROJECT_DIR)/Application/VariableSweatHeater \
   $(PROJECT_DIR)/Application/Voting \
   $(PROJECT_DIR)/Application/WaterFilter \
   $(PROJECT_DIR)/Application/WaterValve \
   $(PROJECT_DIR)/Hardware/SoftPwm \
   $(PROJECT_DIR)/Hardware/PersonalityEeprom \

# List of directories that contain your tests and test helpers (mocks, fakes, etc.)
TEST_SRC_DIRS := \
   $(TESTING_DIR) \
   $(TESTING_DIR)/Utils \
   $(TESTING_DIR)/Mocks \
   $(TESTING_DIR)/Mocks/Bsp \
   $(TESTING_DIR)/Mocks/DataSource \
   $(TESTING_DIR)/Mocks/ParametricTestDoubles \
   $(TESTS_DIR) \
   $(TESTS_DIR)/Application/ \
   $(TESTS_DIR)/Application/Auger \
   $(TESTS_DIR)/Application/ApplianceFeatureApiUpdater \
   $(TESTS_DIR)/Application/BoardDiscovery \
   $(TESTS_DIR)/Application/Compressor \
   $(TESTS_DIR)/Application/ConvertibleCompartment \
   $(TESTS_DIR)/Application/Cooling \
   $(TESTS_DIR)/Application/Damper \
   $(TESTS_DIR)/Application/DataSource \
   $(TESTS_DIR)/Application/Debounce \
   $(TESTS_DIR)/Application/Defrost \
   $(TESTS_DIR)/Application/Dispenser \
   $(TESTS_DIR)/Application/Door \
   $(TESTS_DIR)/Application/DutyCycle \
   $(TESTS_DIR)/Application/EnhancedSabbath \
   $(TESTS_DIR)/Application/Factory \
   $(TESTS_DIR)/Application/Fan \
   $(TESTS_DIR)/Application/FlowMeter \
   $(TESTS_DIR)/Application/Grid \
   $(TESTS_DIR)/Application/IceMaker \
   $(TESTS_DIR)/Application/IceMaker/AluminumMoldIceMaker \
   $(TESTS_DIR)/Application/Lighting \
   $(TESTS_DIR)/Application/Motor \
   $(TESTS_DIR)/Application/ParametricData \
   $(TESTS_DIR)/Application/PersonalityEeprom \
   $(TESTS_DIR)/Application/RequestStatusUpdater \
   $(TESTS_DIR)/Application/RfidCommunication \
   $(TESTS_DIR)/Application/Sabbath \
   $(TESTS_DIR)/Application/Sensor \
   $(TESTS_DIR)/Application/ServiceDiagnostics \
   $(TESTS_DIR)/Application/Setpoints \
   $(TESTS_DIR)/Application/SignOfLife \
   $(TESTS_DIR)/Application/StepperMotor \
   $(TESTS_DIR)/Application/SystemMonitor \
   $(TESTS_DIR)/Application/TimerModule \
   $(TESTS_DIR)/Application/TwistTrayIceMaker \
   $(TESTS_DIR)/Application/VariableSweatHeater \
   $(TESTS_DIR)/Bsp \
   $(TESTS_DIR)/Hardware/SoftPwm \
   $(APPLCOMMON_DIR)/test/Doubles \

ifeq ($(INTEGRATION),Y)
TEST_SRC_DIRS += \
   $(TESTS_DIR)/Application/Integration
endif

# List of include directories
INCLUDE_DIRS := \
   $(APPLCOMMON_DIR)/src/Hardware/Rx130 \
   $(APPLCOMMON_DIR)/src/Hardware/Rx2xx \
   $(APPLCOMMON_DIR)/src/Hardware/Kpit/Rx130 \
   $(APPLCOMMON_DIR)/src/Image \
   $(PROJECT_DIR)/Bsp \
   $(PROJECT_DIR)/Application/DataSource/BspDataSource \
   $(PROJECT_DIR)/Application/ModelInformation \
   $(PROJECT_DIR)/Hardware \
   $(PROJECT_DIR)/Hardware/Eeprom \
   $(REFERCOMMON_DIR)/Source/Gea \
   $(REFERCOMMON_DIR)/Source/Temperature \

include $(APPLCOMMON_DIR)/Makefile

artifacts: lcov_silent pmccabe_report
	@mkdir -p artifacts
	@mv $(OUTPUT_DIR)/Lcov Lcov
	@zip -r artifacts/lcov.zip Lcov
	@rm -rf Lcov
	@mv $(OUTPUT_DIR)/Pmccabe/pmccabe artifacts/pmccabe
