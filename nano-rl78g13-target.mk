export BUILD_TOOLS?=kpit-rl78

include tools/$(BUILD_TOOLS)/setup.mk

TARGET:=nano-rl78g13
OUTPUT_DIR:=build/$(TARGET)
LINKER_SCRIPT:=$(TARGET).ld

# RL78 micro being used (g10, g13, g14)
CPU:=g13
DEVICE:=R5F100LE
ifeq ($(BUILD_TOOLS),llvm-rl78)
TOOLCHAIN_VERSION:=10.0.0.202110
else
TOOLCHAIN_VERSION:=4.9.2.202002
endif
TTY?=/dev/ttyUSB0

ifeq ($(DEBUG), N)
else
ifeq ($(DEBUG), Y)
DEFINES+=DEBUG
else
$(error Please define DEBUG with Y or N.)
endif
endif

include tools/$(BUILD_TOOLS)/defaults.mk

SRC_FILES:=\

SRC_DIRS:=\
  src/$(TARGET) \
  src/$(TARGET)/Hardware \
  src/NanoApplication \
  src/NanoApplication/DataSource \
  src/NanoApplication/Plugins \

INC_DIRS:=\
  src/Application \

tiny_EXTERNAL_INC_DIRS:=\
  src/$(TARGET)/Hardware \

tiny_INCLUDE_HARDWARE:=Y
tiny_INCLUDED_APPLICATIONS:=StackAnalysis
include lib/applcommon.tiny/lib_tiny.mk

PACKAGE_CONTENTS=
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
build: $(OUTPUT_DIR)/$(TARGET).napl
	@$(call make_directory,$(OUTPUT_DIR)/binaries)
	@$(call copy_file,$(OUTPUT_DIR)/$(TARGET).napl,$(OUTPUT_DIR)/binaries/$(TARGET).mot)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: build artifacts erd_definitions
	@echo Creating artifacts/$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip...
	@$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)

.PHONY: upload
upload: $(OUTPUT_DIR)/$(TARGET).napl
	@$(call rl78flash_upload,$<,$(TTY),1000000)

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: clean
clean: target_clean

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc toolchain
	@echo Generating ERD definitions...
	@$(CC) $(addprefix -I, $(SRC_DIRS) $(INC_DIRS)) -E -P -MMD src/NanoApplication/DataSource/NanoSystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@sed -i '/typedef __size_t size_t/d' $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua

include tools/$(BUILD_TOOLS)/worker.mk
