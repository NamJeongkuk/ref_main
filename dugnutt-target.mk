include tools/kpit-rx/kpit-rx.mk

TARGET=dugnutt

DEVICE:=R5F51308
ID_CODE:=45CAFEC0FFEECAFEC0FFEECAFEC0FFEE

PROJECT_DIR=src
OUTPUT_DIR=build/$(TARGET)
APPLCOMMON_DIR=lib/applcommon
BOOT_LOADER_DIR=lib/boot-loaders

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
   $(APPLCOMMON_DIR)/Event \
   $(APPLCOMMON_DIR)/Filter \
   $(APPLCOMMON_DIR)/Git \
   $(APPLCOMMON_DIR)/Hardware/Kpit/Rx/Ul \
   $(APPLCOMMON_DIR)/Hardware/Kpit/Rx/Ul/Vendor \
   $(APPLCOMMON_DIR)/Hardware/Kpit/Rx2xx \
   $(APPLCOMMON_DIR)/Hardware/Rx130 \
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
   $(APPLCOMMON_DIR)/Ul \
   $(APPLCOMMON_DIR)/Ul/Plugins \
   $(APPLCOMMON_DIR)/Updater \
   $(APPLCOMMON_DIR)/Utilities \
   $(APPLCOMMON_DIR)/Validator \

SRC_DIRS=\
   $(PROJECT_DIR)/$(TARGET) \
   $(PROJECT_DIR)/$(TARGET)/Bsp \
   $(PROJECT_DIR)/$(TARGET)/Hardware \
   $(PROJECT_DIR)/$(TARGET)/Hardware/Flash \
   $(PROJECT_DIR)/Application \
   $(PROJECT_DIR)/Application/DataSource \
   $(PROJECT_DIR)/Application/Gea \
   $(PROJECT_DIR)/Application/Plugins \
   $(PROJECT_DIR)/Application/ServiceDiagnostics \
   $(PROJECT_DIR)/Application/TimerModule \

# Additional include directories
INC_DIRS=\
   $(APPLCOMMON_DIR)/Assert \
   $(APPLCOMMON_DIR)/ApplianceApi \
   $(APPLCOMMON_DIR)/BootLoader \
   $(APPLCOMMON_DIR)/Fingerprinter \
   $(APPLCOMMON_DIR)/Git \
   $(APPLCOMMON_DIR)/Hardware/Rx130 \
   $(APPLCOMMON_DIR)/Hardware/Kpit/Rx130 \
   $(APPLCOMMON_DIR)/Image \
   $(APPLCOMMON_DIR)/Reset \
   $(APPLCOMMON_DIR)/ServiceMode \
   $(APPLCOMMON_DIR)/Signal \
   $(APPLCOMMON_DIR)/Utilities \
   $(APPLCOMMON_DIR)/Validator \

# RX micro being used
CPU=rx100

# Set endianness as big or little
ENDIANNESS=little

# Warnings to ignore
WARNINGS_TO_IGNORE=no-array-bounds

# Files that are considered source (files that will ultimately generate an obj)
SOURCE_EXTENSIONS=.c .s

OPTIMIZE=s

TOOLCHAIN_VERSION:=4.8.4.201801

# Space delimited list, whole folders can also be included
PACKAGE_CONTENTS=
$(call add_to_package,$(OUTPUT_DIR)/binaries,binaries)
$(call add_to_package,$(OUTPUT_DIR)/doc,doc)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

include tools/kpit-rx/kpit-rx-makefile-worker.mk

.PHONY: all
all: target $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).apl --endianness little --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot --endianness little --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(BOOT_LOADER_DIR)/build/$(TARGET)-boot-loader/$(TARGET)-boot-loader.mot --endianness little --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts erd_definitions
	$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)
	@echo Archive complete

$(BOOT_LOADER_DIR)/build/$(TARGET)-boot-loader/$(TARGET)-boot-loader.mot:
	$(MAKE) -C $(BOOT_LOADER_DIR) -f $(TARGET)-boot-loader.mk RELEASE=Y DEBUG=N

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target $(BOOT_LOADER_DIR)/build/$(TARGET)-boot-loader/$(TARGET)-boot-loader.mot
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(TARGET)-boot-loader/$(TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc $(TOOLCHAIN_LOCATION)
	@echo Generating ERD definitions
	@$(CC) $(addprefix -I, $(C_FILE_LOCATIONS)) -E -P -MMD $(PROJECT_DIR)/Application/DataSource/SystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output build/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua

.PHONY: upload
upload: $(call upload_deps,all jlink_tools)
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot)

# RxMakefileWorker.mk will clean up the RX build files, use this for anything else that needs to be cleaned
.PHONY: clean
clean: target_clean
	$(MAKE) -C $(BOOT_LOADER_DIR) -f $(TARGET)-boot-loader.mk RELEASE=Y DEBUG=N clean
