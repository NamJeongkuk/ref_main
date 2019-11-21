ifeq ($(TC), IAR)
   STM8_MAKEFILE_LOCATION:=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))
   include tools/iar-stm8-wine/iar-stm8-wine-tools.mk
else
   include tools/sdcc-stm8/sdcc-stm8.mk
endif

TARGET:=bonzalez
OUTPUT_DIR:=build/$(TARGET)
APPLCOMMON_TINY_DIR=lib/applcommon-tiny
APPLCOMMON_DIR=$(APPLCOMMON_TINY_DIR)/lib/applcommon
PROJECT_DIR=src
BOOT_LOADER_DIR=lib/stm8-boot-loader
BOOT_LOADER:=$(BOOT_LOADER_DIR)/build/boot-loader.hex

# Configures the STM8S headers for your part
DEVICE_TYPE:=STM8S003

OPENOCD_TARGET:=target/stm8s003.cfg
STM8FLASH_PART:=stm8s103f3
STM8FLASH_PROGRAMMER:=stlinkv2

MAIN:=$(PROJECT_DIR)/$(TARGET)/main.c

SRC_FILES:=\

SRC_DIRS:=\
   $(PROJECT_DIR)/$(TARGET) \
   $(PROJECT_DIR)/$(TARGET)/Hardware \
   $(PROJECT_DIR)/TinyApplication \
   $(PROJECT_DIR)/TinyApplication/DataSource \
   $(PROJECT_DIR)/TinyApplication/Gea \
   $(PROJECT_DIR)/TinyApplication/Plugins \

COMMON_LIB_DIRS:=\
   $(APPLCOMMON_TINY_DIR)/src/Core \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8 \
   $(APPLCOMMON_TINY_DIR)/src/TinyLib \

INC_DIRS:=\
   $(APPLCOMMON_DIR)/Constants \
   $(APPLCOMMON_DIR)/Image \
   $(APPLCOMMON_DIR)/Utilities \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Hal \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8 \
   $(PROJECT_DIR)/Application/Gea \

SOURCE_EXTENSIONS:=.c

ifeq ($(TC), IAR)
   TOOLCHAIN_VERSION=1.04
   OPTIMIZE:=hz
   WARNINGS_TO_IGNORE:=Pe618,Pe236,Pa050
   ENDIANNESS:=big

   # Compiler optimization level (n - none, hs - High, favoring speed, hz - High, favoring size)
   OPTIMIZE:=hz

   # Code and data model size (small|medium|large)
   CODE_MODEL:=small
   DATA_MODEL:=medium

   # Define how many virtual registers to use
   VREGS:=16
else
   TOOLCHAIN_VERSION:=3.9.0
   OPTIMIZE:=--opt-code-size
   IGNORE_WARNINGS=24
   HEX_LINKER_OPTIONS=--code-loc 0x8440
endif

PACKAGE_CONTENTS:=
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).hex,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: $(OUTPUT_DIR)/$(TARGET)_combined.hex $(OUTPUT_DIR)/$(TARGET).tiny
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@echo Build complete

.PHONY: target
target: $(OUTPUT_DIR)/$(TARGET).htiny

.PHONY: $(BOOT_LOADER)
$(BOOT_LOADER):
	@$(MAKE) -C $(BOOT_LOADER_DIR) -f target.mk RELEASE=Y

# The ordering of the boot loader and htiny is important
# The vector table of the app (htiny) must overwrite the vector table of the boot loader
$(OUTPUT_DIR)/$(TARGET)_combined.hex: $(BOOT_LOADER) $(OUTPUT_DIR)/$(TARGET).htiny
	@$(LUA53) $(LUA_INTEL_HEX_CONCATENATE) --allow_overlap --input $^ --output $@

upload: $(OUTPUT_DIR)/$(TARGET)_combined.hex stm8flash
	$(call stm8flash_upload,$(STM8FLASH_PROGRAMMER),$(STM8FLASH_PART),$^)

upload_boot_loader: $(BOOT_LOADER) stm8flash
	$(call stm8flash_upload,$(STM8FLASH_PROGRAMMER),$(STM8FLASH_PART),$^)

.PHONY: clean
clean: target_clean
	$(MAKE) -C $(BOOT_LOADER_DIR) -f target.mk RELEASE=Y DEBUG=N clean

.PHONY: package
package: all
	@$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)

ifeq ($(TC), IAR)
   include tools/iar-stm8-wine/iar-stm8-wine-tools-makefile-worker.mk
else
   include tools/sdcc-stm8/sdcc-stm8-makefile-worker.mk
endif
