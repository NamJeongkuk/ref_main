include tools/kpit-rx/setup.mk

export TARGET:=rockhopper
export OUTPUT_DIR:=build/$(TARGET)
BOOT_LOADER_DIR:=lib/rockhopper-boot-loader
BOOT_LOADER_TARGET:=rockhopper
BOOT_LOADER_UPDATER_DIR:=$(BOOT_LOADER_DIR)/lib/boot-loader-updater
CPU:=rx100
TOOLCHAIN_VERSION:=8.3.0.202305-gdb-12.1
LINKER_SCRIPT:=$(TARGET).ld
export IMAGE_CRC

export NV_USAGE_REPORT_WRITES_PER_YEAR_INDEX:=9
export NV_USAGE_REPORT_ERD_MEMORY_TYPE_INDEX:=7
export NV_USAGE_REPORT_ERD_TYPES:=NvUserSetting NvUnitSetting NvFaultSnapshot NvCycleHistory NvUsageProfile NvRfid NvProtected
export NV_USAGE_REPORT_PRODUCT_LIFE:=20
export NV_USAGE_REPORT_MAXIMUM_ERASE_COUNT:=1000000
export NV_USAGE_REPORT_EEPROM_SIZE:=32768
export NV_USAGE_REPORT_WRITE_ALIGNMENT:=1

export SYSTEM_ERDS:=src/Application/DataSource/SystemErds.h

DEVICE:=R5F51308
SVD:=tools/kpit-rx/svd/rx130.svd
ID_CODE:=45C0C0AC1C1AC2C2AC3C3AC4C4AC5C5A

ifeq ($(PRODUCTION), Y)
PATH_TO_BUILD_PARAMETRIC:=../../
PARAMETRIC_DIRS:=../../data
else
PATH_TO_BUILD_PARAMETRIC:=./Parametric
PARAMETRIC_DIRS:=Parametric/data/Development
endif

BUILD_DEPS+=$(BOOT_LOADER_DIR)/$(BOOT_LOADER_TARGET)-boot-loader-memory.ld

ifeq ($(DEBUG), N)
else
ifeq ($(DEBUG), Y)
DEFINES+=DEBUG
#ASSEMBLY_LISTING=Y
else
$(error Please define DEBUG with Y or N.)
endif
endif

include tools/kpit-rx/defaults.mk

SRC_FILES:=\

SRC_DIRS:=\
  src \
  src/Bsp \
  src/Hardware \
  src/Hardware/Eeprom \
  src/Hardware/Flash \
  src/Hardware/PersonalityEeprom \
  src/Hardware/SoftPwm \
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
  src/Application/DataSource/ApplianceApiDataSource \
  src/Application/DataSource/BspDataSource \
  src/Application/DataSource/ConvertedDataSource \
  src/Application/Debounce \
  src/Application/Defrost \
  src/Application/Dispenser \
  src/Application/Door \
  src/Application/DutyCycle \
  src/Application/EnhancedSabbath \
  src/Application/ErdService \
  src/Application/Factory \
  src/Application/Fan \
  src/Application/Fault \
  src/Application/FaultHandlers \
  src/Application/FeaturePan \
  src/Application/Features \
  src/Application/FlowMeter \
  src/Application/Gea \
  src/Application/Mapper \
  src/Application/Motor \
  src/Application/NonHeatedCycleDefrost \
  src/Application/Grid \
  src/Application/Grid/GridFunctions \
  src/Application/Grid/GridStateTables \
  src/Application/Heater \
  src/Application/TestingFsm \
  src/Application/IceCabinet \
  src/Application/IceMaker \
  src/Application/IceMaker/IceMakerSlots \
  src/Application/IceMaker/AluminumMoldIceMaker \
  src/Application/IceMaker/TwistTrayIceMaker \
  src/Application/IsolationWaterValve \
  src/Application/Lighting \
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
  src/Application/ServiceMode \
  src/Application/Setpoints \
  src/Application/SnackMode \
  src/Application/SoundLevel \
  src/Application/StepperMotor \
  src/Application/SystemMonitor \
  src/Application/TimerModule \
  src/Application/Uassert \
  src/Application/VariableSweatHeater \
  src/Application/WaterFilter \
  src/Application/WaterVolumeUsage \
  Parametric/data/Development \
  lib/Common/Source/Gea \

INC_DIRS:=\
  src/Application/BoardDiscovery \
  src/Application/Compressor \
  src/Application/ConvertibleCompartment \
  src/Application/Cooling \
  src/Application/Damper \
  src/Application/DemandResponse \
  src/Application/Door \
  src/Application/EnhancedSabbath \
  src/Application/Setpoints \
  src/Application/StepperMotor \
  src/Application/Grid \
  src/Application/Grid/GridFunctions \
  src/Application/RequestStatusUpdater \
  src/Application/ModelInformation \
  src/Application/NonHeatedCycleDefrost \
  src/Application/Sabbath \
  src/Application/SystemMonitor \
  src/Application/ServiceDiagnostics \
  src/Application/Temperature \
  src/Application/Voting \
  src/Application/WaterFilter \
  src/Application/WaterVolumeUsage \
  lib/Common/Source/Temperature/ \

applcommon_EXTERNAL_INC_DIRS:=\
  src/Application/Uassert \
  src/Hardware/Flash \

include lib_applcommon_rx130.mk

include lib_refercommon.mk

PARAMETRIC_HASH:=$(shell cd Parametric && git rev-parse --short HEAD)

$(call add_to_package,{ from = '$(OUTPUT_DIR)/doc', to = 'doc' })
$(call add_to_package,{ from = 'doc/*.json', to = 'doc' })
$(call add_to_package,{ from = '$(OUTPUT_DIR)/$(TARGET).map', to = '' })
$(call add_to_package,{ from = '$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md', to = '' })
$(call add_to_package,{ from = '$(OUTPUT_DIR)/$(TARGET).apl', to = 'binaries/$(TARGET).apl', version = true })
$(call add_to_package,{ from = '$(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot', to = 'binaries/$(TARGET).mot', version = true })
$(call add_to_package,{ from = '$(BOOT_LOADER_UPDATER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader-updater/$(BOOT_LOADER_TARGET)-boot-loader-updater.apl', to = 'binaries/', version = true })

.PHONY: all
all: info

.PHONY: info
info: build_all
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@echo
	@cat $(OUTPUT_DIR)/$(TARGET)_nv_usage_report.md

.PHONY: build_all
build_all: target $(OUTPUT_DIR)/$(TARGET)_bootloader_app_parametric.mot
	$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

target: erd_definitions nv_usage_report erd_lock

.PHONY: package
package: build_all artifacts erd_lock
	$(eval IMAGE_CRC:=$(shell $(LUA53) $(PATH_TO_BUILD_PARAMETRIC)/lib/lua-image-header-dump/lua-image-header-dump.lua --srec $(OUTPUT_DIR)/$(PARAMETRIC_DIRS)/rockhopper.parametric.apl | grep -m1 -oE 'Calculated Image CRC: 0x([0-9A-Fa-f]+)' | cut -d' ' -f4))
	@echo Creating artifacts/$(TARGET)_v$(CRIT_VERSION_MAJOR).$(CRIT_VERSION_MINOR).$(NONCRIT_VERSION_MAJOR).$(NONCRIT_VERSION_MINOR)_$(GIT_SHORT_HASH).zip...
	@$(call create_artifacts,$(TARGET)_v$(CRIT_VERSION_MAJOR).$(CRIT_VERSION_MINOR).$(NONCRIT_VERSION_MAJOR).$(NONCRIT_VERSION_MINOR)_$(GIT_SHORT_HASH).zip)

.PHONY: $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot
$(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot:
	@OUTPUT_PREFIX="\<BootLoader\>" $(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N build

$(OUTPUT_DIR)/$(TARGET)_bootloader_app_parametric.mot: $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl $(OUTPUT_DIR)/Parametric/data/Development/rockhopper.parametric.apl
	@echo Creating $@...
	@$(LUA53) $(SREC_CONCATENATE) --input $^ --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: upload
upload: $(call upload_deps,all jlink_tools) erd_lock
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET)_bootloader_app_parametric.mot)

.PHONY: clean
clean: target_clean
	@$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N clean

.PHONY: erd_lock
erd_lock: erd_definitions
	@$(LUA53) $(LUA_ERD_LOCK_REPORT) --configuration erd_lock_config.lua --locked_definitions erd-lock.json --definitions $(OUTPUT_DIR)/doc/erd-definitions.json

.PHONY: erd_lock_update
erd_lock_update: erd_definitions
	@$(LUA53) $(LUA_ERD_LOCK_REPORT) --configuration erd_lock_config.lua --locked_definitions erd-lock.json --definitions $(OUTPUT_DIR)/doc/erd-definitions.json --generate_locked_definitions

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc toolchain
	@echo Generating ERD definitions...
	@$(CC) $(addprefix -I,$(INC_DIRS) $(SRC_DIRS)) -E -P -MMD src/Application/DataSource/SystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua

.PHONY: nv_usage_report
nv_usage_report: erd_definitions
	@echo Generating NV usage report...
	@$(LUA53) generate_nv_usage_report.lua

include tools/kpit-rx/worker.mk
