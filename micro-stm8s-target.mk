include tools/sdcc-stm8/sdcc-stm8.mk

TARGET:=micro-stm8s
OUTPUT_DIR:=build/$(TARGET)
APPLCOMMON_TINY_DIR=lib/applcommon-tiny
APPLCOMMON_DIR=$(APPLCOMMON_TINY_DIR)/lib/applcommon
PROJECT_DIR=src
BOOT_LOADER_DIR=lib/boot-loaders

# This needs to be the address _after_ the header
# The application header is at 0xA000 and is 64 (0x40) bytes
HEX_LINKER_OPTIONS=--code-loc 0xA040

# Configures the STM8S headers for your part
DEVICE_TYPE:=STM8S105

OPENOCD_TARGET:=target/stm8s.cfg
STM8FLASH_PART:=stm8s105c6
STM8FLASH_PROGRAMMER:=stlinkv2
ENDIANNESS:=big

SOURCE_EXTENSIONS:=.c .s

TOOLCHAIN_VERSION:=3.9.0
OPTIMIZE:=--opt-code-size
IGNORE_WARNINGS=24 94

# Update value to tweak optimization
# In general, a higher number means longer build times but smaller code
# OPTIMIZE_MAX_ALLOCS_PER_NODE:=50000

ifeq ($(DEBUG), N)
else
ifeq ($(DEBUG), Y)
   DEFINE_LIST+=DISABLE_UL_CHECKS
else
$(error Please define DEBUG with Y or N.)
endif
endif

MAIN:=$(PROJECT_DIR)/$(TARGET)/main.c

SRC_FILES:=\

SRC_DIRS:=\
   $(PROJECT_DIR)/$(TARGET) \

COMMON_LIB_DIRS:=\
   $(APPLCOMMON_TINY_DIR)/src/Core \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8 \
   $(APPLCOMMON_TINY_DIR)/src/TinyLib \

#  $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8/Ul \

INC_DIRS:=\
   $(APPLCOMMON_DIR)/Utilities \
   $(APPLCOMMON_TINY_DIR)/src/BootLoader \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Hal \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8 \
   $(BOOT_LOADER_DIR)/src/$(TARGET)-boot-loader \
   $(PROJECT_DIR)/Application/Gea \

PACKAGE_CONTENTS:=
$(call add_to_package,$(OUTPUT_DIR)/binaries,binaries)
$(call add_to_package,$(OUTPUT_DIR)/doc,doc)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).apl --endianness little --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot --endianness little --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(BOOT_LOADER_DIR)/build/$(TARGET)-boot-loader/$(TARGET)-boot-loader.mot --endianness little --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts erd_definitions
	$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)
	@echo Archive complete

.PHONY: target
target: $(OUTPUT_DIR)/$(TARGET).apl

.PHONY: boot-loader
boot-loader:
	$(MAKE) -C $(BOOT_LOADER_DIR) -f $(TARGET)-boot-loader.mk RELEASE=Y DEBUG=N

# Overlap is allowed during concatenate because the boot loader is not stripped
$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target boot-loader
	@$(LUA53) $(SREC_CONCATENATE) --allow_overlap --input $(BOOT_LOADER_DIR)/build/$(TARGET)-boot-loader/$(TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot.hex: $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	@$(LUA53) $(LUA_SREC_TO_INTEL_HEX) --input $< --output $@

.PHONY: upload
upload: $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot.hex stm8flash
	$(call stm8flash_upload,$(STM8FLASH_PROGRAMMER),$(STM8FLASH_PART),$^)

.PHONY: clean
clean: target_clean
	$(MAKE) -C $(BOOT_LOADER_DIR) -f $(TARGET)-boot-loader.mk RELEASE=Y DEBUG=N clean

include tools/sdcc-stm8/sdcc-stm8-makefile-worker.mk

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc $(TOOLCHAIN_LOCATION)
	@echo Generating ERD definitions
	@$(CC) $(addprefix -I, $(C_FILE_LOCATIONS)) -E -P -MMD $(PROJECT_DIR)/Application/DataSource/SystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output build/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua
