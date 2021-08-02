include tools/gcc-arm-none-eabi/setup.mk

TARGET:=micro-psoc4100s-cap-touch
OUTPUT_DIR:=build/$(TARGET)
BOOT_LOADER_DIR=lib/boot-loaders
BOOT_LOADER_TARGET=small-psoc4100s
CPU:=cortex-m0plus
TOOLCHAIN_VERSION:=7-2017-q4-major
LINKER_SCRIPT:=$(TARGET).ld

DEVICE:=CY8C4127XXX-SXXX
DEVICE_PART_NUMBER:=CY8C4127AZI-S455
HEADER_ADDRESS:=0x00004000

ifeq ($(DEBUG), N)
else
ifeq ($(DEBUG), Y)
DEFINES+=DEBUG
else
$(error Please define DEBUG with Y or N.)
endif
endif

include tools/gcc-arm-none-eabi/defaults.mk

CFLAGS+=-Wno-sign-compare

PSOC_CREATOR_DIR=src/$(TARGET)/PsocCreator/psoc.cydsn

SRC_FILES:=\
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CapTouch_Control.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CapTouch_Filter.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CapTouch_ISR.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CapTouch_Processing.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CapTouch_SelfTest.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CapTouch_Sensing.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CapTouch_SensingCSX_LL.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CapTouch_Structure.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CyBootAsmGnu.s \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/cyfitter_cfg.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CyLFClk.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CyLib.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/CyFlash.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/cymetadata.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/cyutils.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/Gea3_SPI_UART.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/Gea3_UART.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/Gea3.c \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4/Gea3_SPI_UART_INT.c \

SRC_DIRS:=\
  src \
  src/$(TARGET) \
  src/$(TARGET)/DataSource \
  src/$(TARGET)/GeaStack \
  src/$(TARGET)/Hardware \
  src/$(TARGET)/Plugins \
  src/../lib/applcommon.tiny/src/Application/PsocCapTouch

INC_DIRS:=\
  src/Application \
  src/$(TARGET) \
  src/$(TARGET)/DataSource \
  $(PSOC_CREATOR_DIR) \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4 \
  lib/applcommon.tiny/src/Hardware/Psoc4100 \

SYS_INC_DIRS:=\
  $(SRC_DIRS) \
  $(PSOC_CREATOR_DIR) \
  $(PSOC_CREATOR_DIR)/Generated_Source/PSoC4 \

tiny_EXTERNAL_INC_DIRS:=\
  src/$(TARGET) \

tiny_EXTERNAL_SYS_INC_DIRS:=\
  $(SYS_INC_DIRS) \

include lib_tiny_psoc4100.mk

LUA_FINALIZE_PSOC4_APL:=tools/lua-finalize-psoc4-apl/lua-finalize-psoc4-apl.lua

PACKAGE_CONTENTS:=
$(call add_to_package,$(OUTPUT_DIR)/binaries,binaries)
$(call add_to_package,$(OUTPUT_DIR)/doc,doc)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: target $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	@$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_FINALIZE_PSOC4_APL) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot --output $(OUTPUT_DIR)/binaries/$(TARGET).hex --device $(DEVICE_PART_NUMBER)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts erd_definitions erd_lock
	@echo Creating package...
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).apl --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(call create_artifacts,$(TARGET)_BN_$(BUILD_NUMBER).zip)

.PHONY: $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot
$(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot:
	@OUTPUT_PREFIX="\<BootLoader\>" $(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot
	@echo Creating $@...
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

.PHONY: upload
upload: all jlink_tools
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot)

.PHONY: clean
clean: target_clean
	@$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N clean

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: erd_lock
erd_lock: erd_definitions
	@$(LUA53) $(LUA_ERD_LOCK_REPORT) --configuration $(TARGET)_erd_lock_config.lua --locked_definitions micro-erd-lock.json --definitions $(OUTPUT_DIR)/doc/erd-definitions.json

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc toolchain
	@echo Generating ERD definitions...
	@$(CC) $(addprefix -I,$(INC_DIRS) $(SRC_DIRS)) -E -P -MMD src/$(TARGET)/DataSource/SystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua

include tools/gcc-arm-none-eabi/worker.mk
