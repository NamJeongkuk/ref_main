include tools/kpit-rl78/setup.mk

TARGET:=micro-rl78g13
OUTPUT_DIR:=build/$(TARGET)
BOOT_LOADER_DIR=lib/boot-loaders
BOOT_LOADER_TARGET=small-rl78g13
LINKER_SCRIPT:=$(TARGET).ld

# RL78 micro being used (g10, g13, g14)
CPU:=g13
DEVICE:=R5F101AF
TOOLCHAIN_VERSION:=4.9.2.202002
TTY?=/dev/ttyUSB0
HEADER_ADDRESS:=0x3400

ifeq ($(DEBUG), N)
else
ifeq ($(DEBUG), Y)
DEFINES+=DEBUG
else
$(error Please define DEBUG with Y or N.)
endif
endif

include tools/kpit-rl78/defaults.mk

SRC_FILES:=\

SRC_DIRS:=\
  src/$(TARGET) \
  src/$(TARGET)/Hardware \
  src/$(TARGET)/Plugins \
  src/MicroApplication \
  src/MicroApplication/DataSource \

INC_DIRS:=\
  src/Application \

tiny_EXTERNAL_INC_DIRS:=\
  src/$(TARGET)/Hardware \

include lib_tiny_rl78.mk

# Space delimited list, whole folders can also be included
PACKAGE_CONTENTS=
$(call add_to_package,$(OUTPUT_DIR)/binaries,binaries)
$(call add_to_package,$(OUTPUT_DIR)/doc,doc)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	@$(call copy_file,$(OUTPUT_DIR)/$(TARGET).apl,$(OUTPUT_DIR)/$(TARGET).mot)
	@$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts erd_definitions erd_lock
	@echo Creating package...
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET).apl --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR)/binaries
	@$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)

.PHONY: upload
upload: $(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot
	$(call rl78flash_upload,$<,$(TTY),1000000)

.PHONY: boot-loader
boot-loader:
	@OUTPUT_PREFIX="\<BootLoader\>" $(MAKE) -C $(BOOT_LOADER_DIR) -f $(BOOT_LOADER_TARGET)-boot-loader.mk RELEASE=Y DEBUG=N

$(OUTPUT_DIR)/$(TARGET)_bootloader_app.mot: target boot-loader
	@echo Creating $@...
	@$(LUA53) $(SREC_CONCATENATE) --input $(BOOT_LOADER_DIR)/build/$(BOOT_LOADER_TARGET)-boot-loader/$(BOOT_LOADER_TARGET)-boot-loader.mot $(OUTPUT_DIR)/$(TARGET).apl --output $@

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: clean
clean: target_clean

.PHONY: erd_lock
erd_lock: erd_definitions
	@$(LUA53) $(LUA_ERD_LOCK_REPORT) --configuration micro_erd_lock_config.lua --locked_definitions micro-erd-lock.json --definitions $(OUTPUT_DIR)/doc/erd-definitions.json

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc toolchain
	@echo Generating ERD definitions...
	@$(CC) $(addprefix -I, $(SRC_DIRS) $(INC_DIRS)) -E -P -MMD src/MicroApplication/DataSource/MicroSystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua

include tools/kpit-rl78/worker.mk
