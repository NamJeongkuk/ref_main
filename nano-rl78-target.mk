# kpit-rl78 front end makefile.
include tools/kpit-rl78/mc/makecommon.mk

# Name of the project that is being built
TARGET:=nano-rl78
OUTPUT_DIR:=build/$(TARGET)
APPLCOMMON_TINY_DIR:=lib/applcommon.tiny
BOOT_LOADER_DIR=lib/boot-loaders
BOOT_LOADER_TARGET=small-rl78
DEVICE:=R5F100LE

ifeq ($(DEBUG), N)
else
ifeq ($(DEBUG), Y)
DEFINE_LIST+=DEBUG
else
$(error Please define DEBUG with Y or N.)
endif
endif

TTY?=/dev/ttyUSB0

SRC_FILES=\

COMMON_LIB_DIRS=\
   $(APPLCOMMON_TINY_DIR)/src/ApplianceApi \
   $(APPLCOMMON_TINY_DIR)/src/BootLoader \
   $(APPLCOMMON_TINY_DIR)/src/Core \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Rl78/Kpit \
   $(APPLCOMMON_TINY_DIR)/src/TinyLib \

SRC_DIRS=\
   src/$(TARGET) \
   src/$(TARGET)/Hardware \
   src/NanoApplication \
   src/NanoApplication/DataSource \
   src/NanoApplication/Plugins \

INC_DIRS=\
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Hal \
   src/Application/Gea \

# RL78 micro being used (g10, g13, g14)
CPU=g13
SOURCE_EXTENSIONS:=.c .s
TOOLCHAIN_VERSION:=4.9.2.202002
OPTIMIZE:=s
ENDIANNESS:=little

WARNINGS_TO_IGNORE:=

# Space delimited list, whole folders can also be included
PACKAGE_CONTENTS=
$(call add_to_package,$(OUTPUT_DIR)/binaries,binaries)
$(call add_to_package,$(OUTPUT_DIR)/doc,doc)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).map,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md,)

.PHONY: all
all: $(OUTPUT_DIR)/$(TARGET).napl
	$(call make_directory,$(OUTPUT_DIR)/binaries)
	$(call copy_file,$(OUTPUT_DIR)/$(TARGET).napl,$(OUTPUT_DIR)/binaries/$(TARGET).mot)
	@$(LUA53) $(LUA_MEMORY_USAGE_REPORT) --configuration $(TARGET)_memory_report_config.lua --output $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md
	@cat $(OUTPUT_DIR)/$(TARGET)_memory_usage_report.md

.PHONY: package
package: all artifacts erd_definitions
	@$(call create_artifacts,$(TARGET)_$(GIT_SHORT_HASH)_BN_$(BUILD_NUMBER).zip)
	@echo Archive complete

.PHONY: upload
upload: $(OUTPUT_DIR)/$(TARGET).napl
	$(call rl78flash_upload,$<,$(TTY),1000000)

$(OUTPUT_DIR)/doc:
	@mkdir -p $(OUTPUT_DIR)/doc

.PHONY: clean
clean: target_clean

include tools/kpit-rl78/kpit-rl78-makefile-worker.mk

.PHONY: erd_definitions
erd_definitions: $(OUTPUT_DIR)/doc $(TOOLCHAIN_LOCATION)
	@echo Generating ERD definitions
	@$(CC) $(addprefix -I, $(C_FILE_LOCATIONS)) -E -P -MMD src/NanoApplication/DataSource/NanoSystemErds.h -o $(OUTPUT_DIR)/temporary.h
	@$(LUA53) $(LUA_C_DATA_TYPE_GENERATOR) --header $(OUTPUT_DIR)/temporary.h --configuration types_configuration.lua --output $(OUTPUT_DIR)/GeneratedTypes.lua
	@$(LUA53) $(TARGET)_generate_erd_definitions.lua
