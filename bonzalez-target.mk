include tools/sdcc-stm8/sdcc-stm8.mk

TARGET:=bonzalez

OUTPUT_DIR:=build/bonzalez
PROJECT_DIR=src
APPLCOMMON_TINY_DIR=lib/applcommon-tiny
APPLCOMMON_DIR=$(APPLCOMMON_TINY_DIR)/lib/applcommon

# Configures the STM8S headers for your part
DEVICE_TYPE:=STM8S003

OPENOCD_TARGET:=target/stm8s003.cfg
STM8FLASH_PART:=stm8s103f3
STM8FLASH_PROGRAMMER:=stlinkv2

MAIN:=$(PROJECT_DIR)/$(TARGET)/main.c

FORCE_INCLUDE:=\

SRC_FILES:=\

SRC_DIRS:=\
   $(PROJECT_DIR)/$(TARGET) \
   $(PROJECT_DIR)/$(TARGET)/Hardware \

COMMON_LIB_DIRS:=\
   $(APPLCOMMON_TINY_DIR)/src \
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Stm8 \

INC_DIRS:=\
   $(APPLCOMMON_TINY_DIR)/src/Hardware/Hal \
   $(APPLCOMMON_DIR)/Utilities \

TOOLCHAIN_VERSION:=3.9.0

SOURCE_EXTENSIONS:=.c

# Options are --opt-code-size, --opt-code-speed or nothing
OPTIMIZE:=--opt-code-size

# Update value to tweak optimization
# In general, a higher number means longer build times but smaller code
# OPTIMIZE_MAX_ALLOCS_PER_NODE:=50000

PACKAGE_CONTENTS:=
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).debug.elf,)
$(call add_to_package,$(OUTPUT_DIR)/$(TARGET).hex,)

include tools/sdcc-stm8/sdcc-stm8-makefile-worker.mk

.PHONY: all
all: target $(TOOLCHAIN_LOCATION)
	@echo Build complete

.PHONY: clean
clean: target_clean

.PHONY: package
package: all
	@$(call create_artifacts,$(TARGET).zip)
