PROJECT_NAME = bonzalez

APPLCOMMON_DIR = lib/applcommon
PROJECT_DIR = src/$(PROJECT_NAME)
TESTING_DIR = Testing
TESTS_DIR = Testing/Tests/$(PROJECT_NAME)

RUN_APPLCOMMON_TESTS = Y

# Specific files to include if you don't want the entire directory included
SRC_FILES += \

# List of directories that contain your code under test
SRC_DIRS = \

# List of directories that contain your tests and test helpers (mocks, fakes, etc.)
TEST_SRC_DIRS = \
   $(TESTING_DIR) \
   $(TESTING_DIR)/Doubles \
   $(TESTING_DIR)/Mocks \
   $(TESTS_DIR)

# List of include directories
INCLUDE_DIRS = \

include $(APPLCOMMON_DIR)/Makefile

artifacts: lcov_silent
	@mkdir -p artifacts
	@mv $(OUTPUT_DIR)/Lcov Lcov
	@zip -r artifacts/lcov.zip Lcov
	@rm -rf Lcov
