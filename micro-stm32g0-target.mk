# gcc-arm-none-eabi front end makefile.
include tools/gcc-arm-none-eabi/mc/makecommon.mk

# Name of the project that is being built
TARGET:=micro-stm32g0
OUTPUT_DIR:=build/$(TARGET)
APPLCOMMON_TINY_DIR:=lib/applcommon.tiny
BOOT_LOADER_DIR=lib/boot-loaders
BOOT_LOADER_TARGET=small-stm32g0

DISABLE_HAL=Y

# The STM32 CPU
DEVICE:=STM32G070KB

DEFINE_LIST+=STM32G070xx
DEFINE_LIST+=USE_FULL_LL_DRIVER

# Set custom flags here
BUILD_EMULATOR=1
BUILD_RELEASE=2

ifeq ($(DEBUG), N)
DEFINE_LIST+=CONFIG_BUILD=$(BUILD_RELEASE)
else
ifeq ($(DEBUG), Y)
DEFINE_LIST+=CONFIG_BUILD=$(BUILD_EMULATOR)
else
$(error Please define DEBUG with Y or N.)
endif
endif

SRC_FILES=\
   lib/stm32-standard-peripherals/STM32G0xx/Libraries/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.c \
   lib/stm32-standard-peripherals/STM32G0xx/Libraries/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.c \
   lib/stm32-standard-peripherals/STM32G0xx/Libraries/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_usart.c \

COMMON_LIB_DIRS=\
   $(APPLCOMMON_TINY_DIR)/src/ApplianceApi \
   $(APPLCOMMON_TINY_DIR)/src/BootLoader \
   $(APPLCOMMON_TINY_DIR)/src/Core \
   $(APPLCOMMON_TINY_DIR)/src/TinyLib \
   $(APPLCOMMON_TINY_DIR)/src/WiFi \

SRC_DIRS=\
   src/$(TARGET) \
   src/$(TARGET)/Hardware \
   src/$(TARGET)/Plugins \
   src/MicroApplication \
   src/MicroApplication/DataSource \

INC_DIRS=\
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Hal \
   src/Application/Gea \

SOURCE_EXTENSIONS:=.c .s

ARM_VERSION:=7-2017-q4-major
JLINK_VERSION:=6.80a
CPU:=cortex-m0plus
CPU_ARCHITECTURE:=armv6-m
ENDIANNESS:=little
OPTIMIZE:=s
C_STANDARD:=gnu99

WARNINGS_TO_IGNORE:=no-array-bounds no-maybe-uninitialized no-type-limits no-implicit-fallthrough

HEADER_ADDRESS:=0x08003000

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
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).mot --endianness little --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts erd_definitions
	$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)
	@echo Archive complete

.PHONY: boot-loader
boot-loader:
	@$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target boot-loader
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: upload
upload: all jlink_tools
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET).mot)

.PHONY: clean
clean: target_clean

include tools/gcc-arm-none-eabi/MakefileWorker.mk

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc $(TOOLCHAIN_LOCATION)
	@echo Generating ERD definitions
	@$(CC) $(addprefix -I, $(C_FILE_LOCATIONS)) -E -P -MMD src/MicroApplication/DataSource/MicroSystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua
