include lib/applcommon/setup.mk

TARGET:=rockhopper
OUTPUT_DIR:=build/test
DISPLAY_ASCII_ART_RESULTS?=Y
TOOLCHAIN_VERSION:=zig-llvm-0.11.0

$(call require_bool_flag,DEBUG,default_N append_to_output_dir)
$(call require_bool_flag,INTEGRATION,default_N append_to_output_dir)

include lib/applcommon/defaults.mk

ifneq ($(DEBUG),Y)
  CPPFLAGS+=-O2 -Wno-macro-redefined
endif

CXXFLAGS+=-Wno-unneeded-internal-declaration -Wno-overloaded-virtual

DEFINES+=TDD_BUILD

SRC_DIRS:=\
  src/Application \
  src/Application/Auger \
  src/Application/BoardDiscovery \
  src/Application/BroadcastReset \
  src/Application/Compressor \
  src/Application/ConfigurationPlugins \
  src/Application/Configurations \
  src/Application/ConvertibleCompartment \
  src/Application/Cooling \
  src/Application/Damper \
  src/Application/DataSource \
  src/Application/DataSource/BspDataSource \
  src/Application/DataSource/ConvertedDataSource \
  src/Application/Debounce \
  src/Application/Defrost \
  src/Application/DemandResponse \
  src/Application/Dispenser \
  src/Application/Door \
  src/Application/DutyCycle \
  src/Application/EnhancedSabbath \
  src/Application/ErdService \
  src/Application/Factory \
  src/Application/FaultHandlers \
  src/Application/Fan \
  src/Application/Fault \
  src/Application/FeaturePan \
  src/Application/Features \
  src/Application/FlowMeter \
  src/Application/Grid \
  src/Application/Grid/GridFunctions \
  src/Application/Grid/GridStateTables \
  src/Application/Heater \
  src/Application/IceCabinet \
  src/Application/IceMaker \
  src/Application/IceMaker/IceMakerSlots \
  src/Application/IceMaker/AluminumMoldIceMaker \
  src/Application/IceMaker/IceMakerSlots \
  src/Application/IceMaker/TwistTrayIceMaker \
  src/Application/IceMaker/IceMakerSlots \
  src/Application/IsolationWaterValve \
  src/Application/Lighting \
  src/Application/Mapper \
  src/Application/Motor \
  src/Application/NonHeatedCycleDefrost \
  src/Application/Notifications \
  src/Application/ParametricData \
  src/Application/Plugins \
  src/Application/Pulldown \
  src/Application/Platform \
  src/Application/RecessHeater \
  src/Application/RequestStatusUpdater \
  src/Application/RfidCommunication \
  src/Application/Sabbath \
  src/Application/SealedSystemValve \
  src/Application/Sensor \
  src/Application/ServiceDiagnostics \
  src/Application/Setpoints \
  src/Application/ServiceMode \
  src/Application/SoundLevel \
  src/Application/StepperMotor \
  src/Application/SystemMonitor \
  src/Application/SnackMode \
  src/Application/Temperature \
  src/Application/TimerModule \
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
  test/Tests/Application/Fan \
  test/Tests/Application/Fault \
  test/Tests/Application/FaultHandlers \
  test/Tests/Application/FeaturePan \
  test/Tests/Application/Features \
  test/Tests/Application/FlowMeter \
  test/Tests/Application/Grid \
  test/Tests/Application/IceCabinet \
  test/Tests/Application/IceMaker \
  test/Tests/Application/IceMaker/AluminumMoldIceMaker \
  test/Tests/Application/IceMaker/TwistTrayIceMaker \
  test/Tests/Application/Lighting \
  test/Tests/Application/Mapper \
  test/Tests/Application/Motor \
  test/Tests/Application/NonHeatedCycleDefrost \
  test/Tests/Application/Notifications \
  test/Tests/Application/ParametricData \
  test/Tests/Application/PersonalityEeprom \
  test/Tests/Application/RequestStatusUpdater \
  test/Tests/Application/RfidCommunication \
  test/Tests/Application/Sabbath \
  test/Tests/Application/SealedSystemValve \
  test/Tests/Application/Sensor \
  test/Tests/Application/ServiceMode \
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

SRC_FILES+=\
  src/Bsp/DataSource_Bsp.c \

INC_DIRS:=\
  lib/applcommon/src/Hardware/Rx130 \
  lib/applcommon/src/Hardware/Rx2xx \
  lib/applcommon/src/Hardware/Kpit/Rx130 \
  src/Bsp \
  src/Application/DataSource/ApplianceApiDataSource \
  src/Application/ModelInformation \
  src/Application/NonHeatedCycleDefrost \
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
