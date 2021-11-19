include tools/gcc-arm-none-eabi/setup.mk

TARGET:=micro-stm32g0
OUTPUT_DIR:=build/$(TARGET)
BOOT_LOADER_DIR=lib/boot-loaders
BOOT_LOADER_TARGET=small-stm32g0
CPU:=cortex-m0plus
TOOLCHAIN_VERSION:=7-2017-q4-major
LINKER_SCRIPT:=$(TARGET).ld

DEVICE:=STM32G070KB
HEADER_ADDRESS:=0x08003000
DEFINES+=STM32G070xx
DEFINES+=USE_FULL_LL_DRIVER

ifeq ($(DEBUG), N)
else
ifeq ($(DEBUG), Y)
DEFINES+=DEBUG
else
$(error Please define DEBUG with Y or N.)
endif
endif

include tools/gcc-arm-none-eabi/defaults.mk

SRC_FILES:=\

SRC_DIRS:=\
  src/$(TARGET) \
  src/$(TARGET)/Hardware \
  src/$(TARGET)/Plugins \
  src/MicroApplication \
  src/MicroApplication/DataSource \

INC_DIRS:=\
  src/Application \
  lib/boot-loaders/src/small-stm32g0-boot-loader/Hardware \

include lib_tiny.mk
include lib/stm32-standard-peripherals/lib_stm32.mk

PACKAGE_CONTENTS=
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
build: $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: build artifacts erd_definitions erd_lock
	@echo Creating artifacts/$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip...
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)

.PHONY: boot-loader
boot-loader:
	@OUTPUT_PREFIX="\<BootLoader\>" $(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N build

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target boot-loader
	@echo Creating $@...
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: upload
upload: all jlink_tools
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET).mot)

.PHONY: clean
clean: target_clean
	@$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N clean

.PHONY: erd_lock
erd_lock: erd_definitions
	@$(LUA53) $(LUA_ERD_LOCK_REPORT) --configuration micro_erd_lock_config.lua --locked_definitions micro-erd-lock.json --definitions $(OUTPUT_DIR)/doc/erd-definitions.json

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc toolchain
	@echo Generating ERD definitions...
	@$(CC) $(addprefix -I,$(INC_DIRS)) -E -P -MMD src/MicroApplication/DataSource/MicroSystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua

include tools/gcc-arm-none-eabi/worker.mk
