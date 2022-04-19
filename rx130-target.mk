include tools/kpit-rx/setup.mk

export TARGET:=rx130
OUTPUT_DIR:=build/$(TARGET)
BOOT_LOADER_DIR:=lib/rockhopper-boot-loader
BOOT_LOADER_TARGET:=rockhopper
BOOT_LOADER_UPDATER_DIR:=$(BOOT_LOADER_DIR)/lib/boot-loader-updater
CPU:=rx100
TOOLCHAIN_VERSION:=8.3.0.202102
LINKER_SCRIPT:=$(TARGET).ld

DEVICE:=R5F51308
ID_CODE:=45CAFEC0FFEECAFEC0FFEECAFEC0FFEE

PATH_TO_BUILD_PARAMETRIC:=./Parametric
PARAMETRIC_DIRS:=Parametric/data/

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
  src/$(TARGET) \
  src/$(TARGET)/Bsp \
  src/$(TARGET)/Hardware \
  src/$(TARGET)/Hardware/Eeprom \
  src/$(TARGET)/Hardware/Flash \
  src/Application \
  src/Application/DataSource \
  src/Application/Defrost \
  src/Application/Door \
  src/Application/Evaporator \
  src/Application/Grid \
  src/Application/Grid/GridFunctions \
  src/Application/Gea \
  src/Application/ParametricData \
  src/Application/Plugins \
  src/Application/Sensor \
  src/Application/Sabbath \
  src/Application/ServiceDiagnostics \
  src/Application/Sensor \
  src/Application/SystemMonitor \
  src/Application/TimerModule \
  Parametric/data/ \

INC_DIRS:=\
  src/Application/Compressor \
  src/Application/DemandResponse \
  src/Application/Door \
  src/Application/Fan \
  src/Application/Grid \
  src/Application/Grid/GridFunctions \
  src/Application/Sabbath \
  src/Application/SystemMonitor \
  src/Application/Temperature \
  src/Application/Valve \
  lib/Common/Source/Temperature/ \

applcommon_EXTERNAL_INC_DIRS:=\
  src/Application \
  src/$(TARGET)/Hardware/Flash \

include lib_applcommon_rx130.mk

include lib_refercommon.mk

PACKAGE_CONTENTS:=
$(call add_to_package,$(OUTPUT_DIR)/binaries,binaries)
$(call add_to_package,$(OUTPUT_DIR)/doc,doc)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: info

.PHONY: info
info: build
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: build
build: target $(OUTPUT_DIR)/$(TARGET)_bootloader_app_parametric.mot
	$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

target: erd_definitions

.PHONY: package
package: build artifacts erd_lock
	@echo Creating artifacts/$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip...
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).apl --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app_parametric.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(BOOT_LOADER_UPDATER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader-updater/$(BOOT_LOADER_TARGET)-boot-loader-updater.apl --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)

$(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot:
	@OUTPUT_PREFIX="\<BootLoader\>" $(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N build

$(OUTPUT_DIR)/$(TARGET)_bootloader_app_parametric.mot: target $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot build_parametric
	@echo Creating $@...
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl $(OUTPUT_DIR)/Parametric/data/rockhopper.parametric.apl --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: upload
upload: $(call upload_deps,all jlink_tools)
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET)_bootloader_app_parametric.mot)

.PHONY: clean
clean: target_clean
	@$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N clean

.PHONY: erd_lock
erd_lock: erd_definitions
	@$(LUA53) $(LUA_ERD_LOCK_REPORT) --configuration erd_lock_config.lua --locked_definitions erd-lock.json --definitions $(OUTPUT_DIR)/doc/erd-definitions.json

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc toolchain
	@echo Generating ERD definitions...
	@$(CC) $(addprefix -I,$(INC_DIRS) $(SRC_DIRS)) -E -P -MMD src/Application/DataSource/SystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua

include tools/kpit-rx/worker.mk
