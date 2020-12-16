# gcc-arm-none-eabi front end makefile.
include tools/gcc-arm-none-eabi/mc/makecommon.mk

# Name of the project that is being built
TARGET:=mini-stm32f3
BASE_TARGET:=stm32f3
BOOT_LOADER_TARGET:=small-stm32f3

OUTPUT_DIR:=build/$(TARGET)
APPLCOMMON_DIR:=lib/applcommon
BOOT_LOADER_DIR=lib/boot-loaders
LINKER_SCRIPT=$(BASE_TARGET)

# The STM32 CPU
DEVICE:=STM32F303RB

DEFINE_LIST+=STM32F303xC
DEFINE_LIST+=USE_HAL_DRIVER
DEFINE_LIST+=HSI_VALUE=8000000

# Set custom flags here
BUILD_EMULATOR=1
BUILD_RELEASE=2

ifeq ($(DEBUG), N)
else
ifeq ($(DEBUG), Y)
DEFINE_LIST+=DEBUG
else
$(error Please define DEBUG with Y or N.)
endif
endif

# Specific files to include if you don't want the entire directory included
SRC_FILES=\

COMMON_LIB_DIRS=\
   $(APPLCOMMON_DIR)/Action \
   $(APPLCOMMON_DIR)/Alignment \
   $(APPLCOMMON_DIR)/Allocator \
   $(APPLCOMMON_DIR)/ApplianceApi \
   $(APPLCOMMON_DIR)/Audio \
   $(APPLCOMMON_DIR)/Communications/Gea2 \
   $(APPLCOMMON_DIR)/Communications/Gea2/Commands \
   $(APPLCOMMON_DIR)/Communications/Gea2/MessageLayer \
   $(APPLCOMMON_DIR)/Communications/Gea2/PacketLayer \
   $(APPLCOMMON_DIR)/Constants \
   $(APPLCOMMON_DIR)/Crc \
   $(APPLCOMMON_DIR)/CycleEngine \
   $(APPLCOMMON_DIR)/CycleEngine/Extensions \
   $(APPLCOMMON_DIR)/CycleEngine/Extensions/RemainingTime \
   $(APPLCOMMON_DIR)/DataAccess \
   $(APPLCOMMON_DIR)/DataModel \
   $(APPLCOMMON_DIR)/DataSource \
   $(APPLCOMMON_DIR)/DataSource/Packet \
   $(APPLCOMMON_DIR)/DataSource/Packet/ReadWrite \
   $(APPLCOMMON_DIR)/DataSource/Packet/Subscriptions \
   $(APPLCOMMON_DIR)/DataStructures/LinkedList \
   $(APPLCOMMON_DIR)/DataStructures/Queue \
   $(APPLCOMMON_DIR)/DataStructures/RingBuffer \
   $(APPLCOMMON_DIR)/Debounce \
   $(APPLCOMMON_DIR)/Diagnostics \
   $(APPLCOMMON_DIR)/Display \
   $(APPLCOMMON_DIR)/Endianness \
   $(APPLCOMMON_DIR)/ErdStream \
   $(APPLCOMMON_DIR)/Event \
   $(APPLCOMMON_DIR)/Filter \
   $(APPLCOMMON_DIR)/Git \
   $(APPLCOMMON_DIR)/Hardware/Stm32F3xx \
   $(APPLCOMMON_DIR)/HardwareInterfaces \
   $(APPLCOMMON_DIR)/HardwareInterfaces/Adapters \
   $(APPLCOMMON_DIR)/Image \
   $(APPLCOMMON_DIR)/KeyManager \
   $(APPLCOMMON_DIR)/NonVolatile \
   $(APPLCOMMON_DIR)/RecordStorage \
   $(APPLCOMMON_DIR)/ResourceWatchdog \
   $(APPLCOMMON_DIR)/Selector \
   $(APPLCOMMON_DIR)/Signal \
   $(APPLCOMMON_DIR)/StateMachines \
   $(APPLCOMMON_DIR)/Time \
   $(APPLCOMMON_DIR)/TimeOfDay \
   $(APPLCOMMON_DIR)/Ui \
   $(APPLCOMMON_DIR)/Utilities \
   $(APPLCOMMON_DIR)/Validator

SRC_DIRS=\
   src/$(BASE_TARGET) \
   src/$(BASE_TARGET)/Bsp \
   src/$(BASE_TARGET)/Hardware \
   src/$(BASE_TARGET)/Hardware/Peripheral \
   src/$(BASE_TARGET)/Hardware/Platform \
   src/Application \
   src/Application/DataSource \
   src/Application/Gea \
   src/Application/Plugins \
   src/Application/ServiceDiagnostics \
   src/Application/TimerModule \

# Additional include directories
INC_DIRS=\
   $(APPLCOMMON_DIR)/ApplianceApi \
   $(APPLCOMMON_DIR)/BootLoader \
   $(APPLCOMMON_DIR)/Git \
   $(APPLCOMMON_DIR)/Image \
   $(APPLCOMMON_DIR)/ServiceMode \
   $(APPLCOMMON_DIR)/Signal \
   $(APPLCOMMON_DIR)/Utilities \
   $(APPLCOMMON_DIR)/Validator \

SOURCE_EXTENSIONS:=.c .s

ARM_VERSION:=7-2017-q4-major
CPU:=cortex-m4
CPU_ARCHITECTURE:=armv7e-m
ENDIANNESS:=little
OPTIMIZE:=s
C_STANDARD:=gnu99

WARNINGS_TO_IGNORE:=\
  no-array-bounds \
  no-maybe-uninitialized \
  no-type-limits \
  no-implicit-fallthrough \
  no-unused-parameter \
  no-strict-aliasing \

HEADER_ADDRESS:=0x08006000

# Space delimited list, whole folders can also be included
PACKAGE_CONTENTS=
$(call add_to_package,$(OUTPUT_DIR)/binaries,binaries)
$(call add_to_package,$(OUTPUT_DIR)/doc,doc)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

include lib/stm32-standard-peripherals/stm32-standard-peripherals.mk

.PHONY: all
all: $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts erd_definitions
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).apl --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)
	@echo Archive complete

.PHONY: boot-loader
boot-loader:
	$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target boot-loader
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: upload
upload: all jlink_tools
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot)

.PHONY: clean
clean: target_clean
	$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N clean

include tools/gcc-arm-none-eabi/MakefileWorker.mk

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc $(TOOLCHAIN_LOCATION)
	@echo Generating ERD definitions
	@$(CC) $(addprefix -I, $(C_FILE_LOCATIONS)) -E -P -MMD src/Application/DataSource/SystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua
