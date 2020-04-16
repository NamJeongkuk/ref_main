include tools/gcc-arm-none-eabi/mc/makecommon.mk

TARGET:=micro-psoc4100s-cap-touch
OUTPUT_DIR:=build/$(TARGET)
APPLCOMMON_TINY_DIR=lib/applcommon.tiny
APPLCOMMON_DIR=$(APPLCOMMON_TINY_DIR)/lib/applcommon
PROJECT_DIR=src
# fixme dustice => $(TARGET)
PSOC_CREATOR_DIR = src/$(TARGET)/PsocCreator/dustice.cydsn/Generated_Source/PSoC4

# Set custom flags here
BUILD_RELEASE=2
BUILD_EMULATOR=1

ifeq ($(DEBUG), N)
DEFINE_LIST+=CONFIG_BUILD=$(BUILD_RELEASE) NDEBUG
else
ifeq ($(DEBUG), Y)
DEFINE_LIST+=CONFIG_BUILD=$(BUILD_EMULATOR) NDEBUG
else
$(error Please define DEBUG with Y or N.)
endif
endif

SRC_FILES:=\
   $(PSOC_CREATOR_DIR)/CapSense_Control.c \
   $(PSOC_CREATOR_DIR)/CapSense_Filter.c \
   $(PSOC_CREATOR_DIR)/CapSense_Processing.c \
   $(PSOC_CREATOR_DIR)/CapSense_SelfTest.c \
   $(PSOC_CREATOR_DIR)/CapSense_Sensing.c \
   $(PSOC_CREATOR_DIR)/CapSense_SensingCSX_LL.c \
   $(PSOC_CREATOR_DIR)/CapSense_Structure.c \
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
   $(PROJECT_DIR)/$(TARGET)/Hardware \
   $(PROJECT_DIR)/$(TARGET)/Header \
   $(PROJECT_DIR)/$(TARGET)/PsocShim \
   $(PROJECT_DIR)/$(TARGET)/Plugins \
   $(PROJECT_DIR)/MicroApplication \
   $(PROJECT_DIR)/MicroApplication/DataSource \

COMMON_LIB_DIRS:=\
   $(APPLCOMMON_TINY_DIR)/src/ApplianceApi \
   $(APPLCOMMON_TINY_DIR)/src/Application/Stm8CapTouch \
   $(APPLCOMMON_TINY_DIR)/src/BootLoader \
   $(APPLCOMMON_TINY_DIR)/src/Core \
   $(APPLCOMMON_TINY_DIR)/src/TinyLib \

INC_DIRS:=\
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Hal \
   $(PSOC_CREATOR_DIR) \
   $(PROJECT_DIR)/Application/Gea \

SOURCE_EXTENSIONS:=.c .s

ARM_VERSION:=5-4-2016q3
DEVICE:=CY8C4025XXX-SXXX
DEVICE_PART_NUMBER:=CY8C4025LQI-S411
CPU:=cortex-m0plus
CPU_ARCHITECTURE:=armv6-m
ENDIANNESS:=little
OPTIMIZE:=s
C_STANDARD:=gnu99

HEADER_ADDRESS = 0x00000200

WARNINGS_TO_IGNORE:=no-sign-compare

PACKAGE_CONTENTS:=
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).elf,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: target
	@$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@echo
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts
	$(call create_artifacts,$(TARGET)_BN_$(BUILD_NUMBER).zip)

.PHONY: upload
upload: all jlink_tools
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET).mot)

.PHONY: clean
clean: target_clean
	@echo Clean Complete.

include tools/gcc-arm-none-eabi/MakefileWorker.mk
