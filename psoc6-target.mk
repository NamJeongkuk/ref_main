include tools/gcc-arm-none-eabi/setup.mk

TARGET:=psoc6
OUTPUT_DIR:=build/$(TARGET)
BOOT_LOADER_DIR=lib/boot-loaders
BOOT_LOADER_TARGET:=medium-psoc6
CPU:=cortex-m4
TOOLCHAIN_VERSION:=7-2017-q4-major
LINKER_SCRIPT=$(TARGET).ld

DEVICE:=CY8C6XXA_CM4
DEFINES+=CY8C624AAZI_S2D44
HEADER_ADDRESS:=0x10008000
JLINK_SPEED:=15000
APL_FILL_VALUE:=0x00

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
   lib/cypress-psoc6-pdl/devices/COMPONENT_CAT1A/templates/COMPONENT_MBED/COMPONENT_CM4/system_psoc6_cm4.c \
   lib/cypress-psoc6-cm0p-prebuilts/COMPONENT_CM0P_SLEEP/psoc6_02_cm0p_sleep.c

SRC_DIRS:=\
   src/$(TARGET) \
   src/$(TARGET)/Bsp \
   src/$(TARGET)/Hardware \
   src/$(TARGET)/Hardware/Peripheral \
   src/$(TARGET)/Hardware/Platform \
   src/Application \
   src/Application/DataSource \
   src/Application/Gea \
   src/Application/Plugins \
   src/Application/ServiceDiagnostics \
   src/Application/TimerModule \

INC_DIRS:=\
   lib/cypress-psoc6-pdl/devices/COMPONENT_CAT1A/templates/COMPONENT_MBED

PACKAGE_CONTENTS=
$(call add_to_package,$(OUTPUT_DIR)/binaries,binaries)
$(call add_to_package,$(OUTPUT_DIR)/doc,doc)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts erd_definitions erd_lock
	@echo Creating archive...
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).apl --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)

.PHONY: boot-loader
boot-loader:
	@OUTPUT_PREFIX="\<BootLoader\>" $(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target boot-loader
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: upload
upload: all jlink_tools
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot)

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

include lib_applcommon.mk
include lib/cypress-psoc6-pdl/lib_pdl.mk
include tools/gcc-arm-none-eabi/worker.mk
