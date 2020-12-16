PROJECT_NAME := multi-target-starter-kit

APPLCOMMON_DIR := lib/applcommon
PROJECT_DIR := src
TESTING_DIR := test
TESTS_DIR := test/Tests
OUTPUT_DIR := test

RUN_APPLCOMMON_TESTS := N

# Specific files to include if you don't want the entire directory included
SRC_FILES := \

# List of directories that contain your code under test
SRC_DIRS := \
   $(PROJECT_DIR)/Application/DataSource \

# List of directories that contain your tests and test helpers (mocks, fakes, etc.)
TEST_SRC_DIRS := \
   $(TESTING_DIR) \
   $(TESTING_DIR)/Doubles \
   $(TESTING_DIR)/Mocks \
   $(TESTING_DIR)/Mocks/Bsp \
   $(TESTS_DIR) \
   $(TESTS_DIR)/Application/DataSource \

# List of include directories
INCLUDE_DIRS := \
   $(APPLCOMMON_DIR)/Hardware/Rx130 \
   $(APPLCOMMON_DIR)/Image \
   $(PROJECT_DIR)/Application/Plugins \
   $(PROJECT_DIR)/Application/ServiceDiagnostics \
   $(PROJECT_DIR)/rx130/Bsp \
   $(PROJECT_DIR)/rx130/Hardware \

include $(APPLCOMMON_DIR)/Makefile

artifacts: lcov_silent pmccabe_report
	@mkdir -p artifacts
	@mv $(OUTPUT_DIR)/Lcov Lcov
	@zip -r artifacts/lcov.zip Lcov
	@rm -rf Lcov
	@mv $(OUTPUT_DIR)/Pmccabe/pmccabe artifacts/pmccabe
