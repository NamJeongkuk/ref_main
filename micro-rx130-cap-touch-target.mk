include tools/kpit-rx/setup.mk

export TARGET:=micro-rx130-cap-touch
OUTPUT_DIR:=build/$(TARGET)
BOOT_LOADER_DIR:=lib/boot-loaders
BOOT_LOADER_TARGET:=small-rx130
CPU:=rx100
JLINK_VERSION=6.80a
TOOLCHAIN_VERSION:=8.3.0.202102
LINKER_SCRIPT=rx130.ld

DEVICE:=R5F51308
ID_CODE:=45CAFEC0FFEECAFEC0FFEECAFEC0FFEE

ifeq ($(DEBUG), N)
else
ifeq ($(DEBUG), Y)
DEFINES+=DEBUG
else
$(error Please define DEBUG with Y or N.)
endif
endif

include tools/kpit-rx/defaults.mk

SRC_FILES:=\

SRC_DIRS:=\
  src/$(TARGET) \
  src/$(TARGET)/DataSource \
  src/$(TARGET)/GeaStack \
  src/$(TARGET)/Hardware \
  src/$(TARGET)/Plugins \
  src/../lib/applcommon.tiny/src/Application/Rx130CapTouch \

INC_DIRS:=\
  src/$(TARGET)/Types \
  src/Application \

tiny_EXTERNAL_INC_DIRS:=\
  src/$(TARGET)/Hardware \

include lib_tiny_rx130.mk
include micro-rx130-cap-touch-target-qe.mk

PACKAGE_CONTENTS:=
$(call add_to_package,$(OUTPUT_DIR)/binaries,binaries)
$(call add_to_package,$(OUTPUT_DIR)/doc,doc)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: info

.PHONY: info
info: build
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: build
build: target $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

target: erd_definitions

.PHONY: package
package: build artifacts erd_lock
	@echo Creating package...
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).apl --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries --base_name $(TARGET).mot
	@$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)

.PHONY: $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot
$(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot:
	@$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N build

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot
	@echo Creating $@...
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: upload
upload: $(call upload_deps,all jlink_tools)
	$(call jlink_upload,$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot)

.PHONY: clean
clean: target_clean
	@$(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N clean

.PHONY: erd_lock
erd_lock: erd_definitions
	@$(LUA53) $(LUA_ERD_LOCK_REPORT) --configuration micro_erd_lock_config.lua --locked_definitions micro-erd-lock.json --definitions $(OUTPUT_DIR)/doc/erd-definitions.json

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc toolchain
	@echo Generating ERD definitions...
	@$(CC) $(addprefix -I,$(INC_DIRS) $(SRC_DIRS)) $(CPPFLAGS) $(CFLAGS) -E -P -MMD src/$(TARGET)/DataSource/SystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua

include tools/kpit-rx/worker.mk
