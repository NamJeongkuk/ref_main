include tools/sdcc-stm8/sdcc-stm8.mk

TARGET:=nano-stm8s-cap-touch
OUTPUT_DIR:=build/$(TARGET)
APPLCOMMON_TINY_DIR=lib/applcommon.tiny
APPLCOMMON_DIR=$(APPLCOMMON_TINY_DIR)/lib/applcommon
BOOT_LOADER_DIR=lib/stm8-boot-loader
BOOT_LOADER:=$(BOOT_LOADER_DIR)/build/boot-loader.hex

# Configures the STM8S headers for your part
DEVICE_TYPE:=STM8S105

OPENOCD_TARGET:=target/stm8s105.cfg
STM8FLASH_PART:=stm8s105?6
STM8FLASH_PROGRAMMER:=stlinkv2

MAIN:=src/$(TARGET)/main.c

SRC_FILES:=\

SRC_DIRS:=\
   src/$(TARGET) \
   src/$(TARGET)/Hardware \

COMMON_LIB_DIRS:=\
   $(APPLCOMMON_TINY_DIR)/src/ApplianceApi \
   $(APPLCOMMON_TINY_DIR)/src/Application/Stm8CapTouch \
   $(APPLCOMMON_TINY_DIR)/src/BootLoader \
   $(APPLCOMMON_TINY_DIR)/src/Core \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8 \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8/TouchSense/src \
   $(APPLCOMMON_TINY_DIR)/src/TinyLib \

INC_DIRS:=\
   $(APPLCOMMON_DIR)/Constants \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Hal \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8 \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8/TouchSense/inc \
   src/Application/Gea \

SOURCE_EXTENSIONS:=.c

TOOLCHAIN_VERSION:=4.0.0
OPTIMIZE:=--opt-code-size
IGNORE_WARNINGS=24
HEX_LINKER_OPTIONS=--code-loc 0x8440

ifeq ($(DEBUG), Y)
   DEFINE_LIST+=DISABLE_UL_CHECKS
endif

PACKAGE_CONTENTS:=
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_combined.hex,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: $(OUTPUT_DIR)/$(TARGET)_combined.hex $(OUTPUT_DIR)/$(TARGET).tiny
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: target
target: $(OUTPUT_DIR)/$(TARGET).htiny

.PHONY: $(BOOT_LOADER)
$(BOOT_LOADER):
	@$(MAKE) -C $(BOOT_LOADER_DIR) -f target.mk RELEASE=Y TC=$(TC)

# The ordering of the boot loader and htiny is important
# The vector table of the app (htiny) must overwrite the vector table of the boot loader
$(OUTPUT_DIR)/$(TARGET)_combined.hex: $(BOOT_LOADER) $(OUTPUT_DIR)/$(TARGET).htiny
	@$(LUA53) $(LUA_INTEL_HEX_CONCATENATE) --allow_overlap --input $^ --output $@

.PHONY: upload
upload: $(OUTPUT_DIR)/$(TARGET)_combined.hex stm8flash
	$(call stm8flash_upload,$(STM8FLASH_PROGRAMMER),$(STM8FLASH_PART),$^)

.PHONY: upload_boot_loader
upload_boot_loader: $(BOOT_LOADER) stm8flash
	$(call stm8flash_upload,$(STM8FLASH_PROGRAMMER),$(STM8FLASH_PART),$^)

.PHONY: clean
clean: target_clean
	$(MAKE) -C $(BOOT_LOADER_DIR) -f target.mk RELEASE=Y DEBUG=N clean

.PHONY: package
package: all artifacts
	@$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)

include tools/sdcc-stm8/sdcc-stm8-makefile-worker.mk
