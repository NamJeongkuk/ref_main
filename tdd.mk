include lib/applcommon/setup.mk

TARGET:=rockhopper
OUTPUT_DIR:=build/test
DISPLAY_ASCII_ART_RESULTS?=Y

include lib/applcommon/defaults.mk

DEFINES+=TDD_BUILD

SRC_DIRS:=\
  src/Application \
  src/Application/Auger \
  src/Application/ApplianceFeatureApiUpdater \
  src/Application/BoardDiscovery \
  src/Application/BroadcastReset \
  src/Application/Compressor \
  src/Application/ConfigurationPlugins \
  src/Application/Configurations \
  src/Application/ConvertibleCompartment \
  src/Application/Cooling \
  src/Application/Damper \
  src/Application/DataSource \
  src/Application/DataSource/ConvertedDataSource \
  src/Application/Debounce \
  src/Application/Defrost \
  src/Application/DemandResponse \
  src/Application/Dispenser \
  src/Application/Door \
  src/Application/DutyCycle \
  src/Application/EnhancedSabbath \
  src/Application/ErdService \
  src/Application/Evaporator \
  src/Application/Factory \
  src/Application/FaultHandlers \
  src/Application/Fan \
  src/Application/Fault \
  src/Application/Features \
  src/Application/FlowMeter \
  src/Application/Grid \
  src/Application/Grid/GridFunctions \
  src/Application/Grid/GridStateTables \
  src/Application/Heater \
  src/Application/IceMaker \
  src/Application/IceMaker/AluminumMoldIceMaker \
  src/Application/IceMaker/TwistTrayIceMaker \
  src/Application/IsolationWaterValve \
  src/Application/Lighting \
  src/Application/Mapper \
  src/Application/Motor \
  src/Application/Notifications \
  src/Application/ParametricData \
  src/Application/Plugins \
  src/Application/Pulldown \
  src/Application/RecessHeater \
  src/Application/RequestStatusUpdater \
  src/Application/RfidCommunication \
  src/Application/Sabbath \
  src/Application/Sensor \
  src/Application/ServiceDiagnostics \
  src/Application/Setpoints \
  src/Application/SoundLevel \
  src/Application/StepperMotor \
  src/Application/SystemMonitor \
  src/Application/SnackMode \
  src/Application/Temperature \
  src/Application/TimerModule \
  src/Application/Valve \
  src/Application/VariableSweatHeater \
  src/Application/Voting \
  src/Application/WaterFilter \
  src/Application/WaterVolumeUsage \
  src/Hardware/SoftPwm \
  src/Hardware/PersonalityEeprom \
  test \
  test/Utils \
  test/Mocks \
  test/Mocks/Bsp \
  test/Mocks/DataSource \
  test/Mocks/Gea2MessageEndpoint \
  test/Mocks/ParametricTestDoubles \
  test/Mocks/ServiceDiagnostics \
  test/Tests \
  test/Tests/Application/ \
  test/Tests/Application/Auger \
  test/Tests/Application/ApplianceFeatureApiUpdater \
  test/Tests/Application/BoardDiscovery \
  test/Tests/Application/BroadcastReset \
  test/Tests/Application/Compressor \
  test/Tests/Application/ConvertibleCompartment \
  test/Tests/Application/Cooling \
  test/Tests/Application/Damper \
  test/Tests/Application/DataSource \
  test/Tests/Application/DataSource/ConvertedDataSource \
  test/Tests/Application/Debounce \
  test/Tests/Application/Defrost \
  test/Tests/Application/Dispenser \
  test/Tests/Application/Door \
  test/Tests/Application/DutyCycle \
  test/Tests/Application/EnhancedSabbath \
  test/Tests/Application/ErdService \
  test/Tests/Application/Factory \
  test/Tests/Application/FaultHandlers \
  test/Tests/Application/Fan \
  test/Tests/Application/Fault \
  test/Tests/Application/Features \
  test/Tests/Application/FlowMeter \
  test/Tests/Application/Grid \
  test/Tests/Application/IceMaker \
  test/Tests/Application/IceMaker/AluminumMoldIceMaker \
  test/Tests/Application/IceMaker/TwistTrayIceMaker \
  test/Tests/Application/Lighting \
  test/Tests/Application/Mapper \
  test/Tests/Application/Motor \
  test/Tests/Application/Notifications \
  test/Tests/Application/ParametricData \
  test/Tests/Application/PersonalityEeprom \
  test/Tests/Application/RequestStatusUpdater \
  test/Tests/Application/RfidCommunication \
  test/Tests/Application/Sabbath \
  test/Tests/Application/Sensor \
  test/Tests/Application/Setpoints \
  test/Tests/Application/SoundLevel \
  test/Tests/Application/StepperMotor \
  test/Tests/Application/SystemMonitor \
  test/Tests/Application/TimerModule \
  test/Tests/Application/VariableSweatHeater \
  test/Tests/Application/WaterFilter \
  test/Tests/Application/WaterVolumeUsage \
  test/Tests/Bsp \
  test/Tests/Hardware/SoftPwm \

ifeq ($(INTEGRATION),Y)
SRC_DIRS += \
  test/Tests/Application/Integration
endif

INC_DIRS:=\
  lib/applcommon/src/Hardware/Rx130 \
  lib/applcommon/src/Hardware/Rx2xx \
  lib/applcommon/src/Hardware/Kpit/Rx130 \
  src/Bsp \
  src/Application/DataSource/ApplianceApiDataSource \
  src/Application/DataSource/BspDataSource \
  src/Application/ModelInformation \
  src/Hardware \
  src/Hardware/Eeprom \
  lib/Common/Source/Gea \
  lib/Common/Source/Temperature \

.PHONY: all
all: run_tests

artifacts: lcov_silent pmccabe_report
	@mkdir -p artifacts
	@mv $(OUTPUT_DIR)/Lcov Lcov
	@zip -r artifacts/lcov.zip Lcov
	@rm -rf Lcov
	@mv $(OUTPUT_DIR)/Pmccabe/pmccabe artifacts/pmccabe

include lib/applcommon/lib_uassert_test.mk
include lib/applcommon/lib_applcommon.mk
include lib/applcommon/lib_applcommon_lua.mk
include lib/applcommon/lib_applcommon_test_tools.mk
include lib/applcommon/lib_applcommon_test_runner.mk

include lib/applcommon/worker.mk
