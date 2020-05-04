include tools/gcc-arm-none-eabi/mc/makecommon.mk

TARGET:=micro-psoc4100s-cap-touch
OUTPUT_DIR:=build/$(TARGET)
APPLCOMMON_TINY_DIR=lib/applcommon.tiny
APPLCOMMON_DIR=$(APPLCOMMON_TINY_DIR)/lib/applcommon
BOOT_LOADER_DIR=lib/boot-loaders
BOOT_LOADER_TARGET=micro-psoc4100s
PROJECT_DIR=src
PSOC_CREATOR_DIR = src/$(TARGET)/PsocCreator/$(TARGET).cydsn/Generated_Source/PSoC4

# Set custom flags here
BUILD_RELEASE=2
BUILD_EMULATOR=1

ifeq ($(DEBUG), N)
DEFINE_LIST+=CONFIG_BUILD=$(BUILD_RELEASE)
else
ifeq ($(DEBUG), Y)
DEFINE_LIST+=CONFIG_BUILD=$(BUILD_EMULATOR) DEBUG
else
$(error Please define DEBUG with Y or N.)
endif
endif

SRC_FILES:=\
   $(PSOC_CREATOR_DIR)/CapTouch_Control.c \
   $(PSOC_CREATOR_DIR)/CapTouch_Filter.c \
   $(PSOC_CREATOR_DIR)/CapTouch_Processing.c \
   $(PSOC_CREATOR_DIR)/CapTouch_SelfTest.c \
   $(PSOC_CREATOR_DIR)/CapTouch_Sensing.c \
   $(PSOC_CREATOR_DIR)/CapTouch_SensingCSX_LL.c \
   $(PSOC_CREATOR_DIR)/CapTouch_Structure.c \
   $(PSOC_CREATOR_DIR)/CyBootAsmGnu.s \
   $(PSOC_CREATOR_DIR)/cyfitter_cfg.c \
   $(PSOC_CREATOR_DIR)/CyLFClk.c \
   $(PSOC_CREATOR_DIR)/CyLib.c \
   $(PSOC_CREATOR_DIR)/CyFlash.c \
   $(PSOC_CREATOR_DIR)/cymetadata.c \
   $(PSOC_CREATOR_DIR)/cyutils.c \
   $(PSOC_CREATOR_DIR)/Gea3_SPI_UART.c \
   $(PSOC_CREATOR_DIR)/Gea3_UART.c \
   $(PSOC_CREATOR_DIR)/Gea3.c \
   $(PSOC_CREATOR_DIR)/Gea3_SPI_UART_INT.c \

SRC_DIRS:=\
   $(PROJECT_DIR) \
   $(PROJECT_DIR)/$(TARGET) \
   $(PROJECT_DIR)/$(TARGET)/DataSource \
   $(PROJECT_DIR)/$(TARGET)/GeaStack \
   $(PROJECT_DIR)/$(TARGET)/Hardware \
   $(PROJECT_DIR)/$(TARGET)/Plugins \
   $(PROJECT_DIR)/$(TARGET)/PsocShim \

COMMON_LIB_DIRS:=\
   $(APPLCOMMON_TINY_DIR)/src/ApplianceApi \
   $(APPLCOMMON_TINY_DIR)/src/Application/PsocCapTouch \
   $(APPLCOMMON_TINY_DIR)/src/BootLoader \
   $(APPLCOMMON_TINY_DIR)/src/Core \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Psoc4100 \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Psoc4100/Ul \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Psoc4100/Ul/Vendor \
   $(APPLCOMMON_TINY_DIR)/src/TinyLib \
   $(APPLCOMMON_TINY_DIR)/src/WiFi \

INC_DIRS:=\
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Hal \
   $(PSOC_CREATOR_DIR) \
   $(PROJECT_DIR)/Application/Gea \

SOURCE_EXTENSIONS:=.c .s

ARM_VERSION:=5-4-2016q3
DEVICE:=CY8C4147XXX-SXXX
DEVICE_PART_NUMBER:=CY8C4147AZI-S455
CPU:=cortex-m0plus
CPU_ARCHITECTURE:=armv6-m
ENDIANNESS:=little
OPTIMIZE:=s
C_STANDARD:=gnu99

HEADER_ADDRESS = 0x00004000

WARNINGS_TO_IGNORE:=no-sign-compare

PACKAGE_CONTENTS:=
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).elf,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: target $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	@$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).apl --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts
	$(call create_artifacts,$(TARGET)_BN_$(BUILD_NUMBER).zip)

.PHONY: $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot
$(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot:
	$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

.PHONY: upload
upload: all jlink_tools
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot)

.PHONY: clean
clean: target_clean
	$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N clean

include tools/gcc-arm-none-eabi/MakefileWorker.mk
