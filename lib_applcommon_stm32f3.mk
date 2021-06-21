include lib_applcommon.mk

$(call create_library_with_defaults,applcommon_stm32f3,lib/applcommon/Hardware/)

applcommon_stm32f3_INC_DIRS+=\
  $(applcommon_INC_DIRS) \
  $(applcommon_SRC_DIRS) \
  lib/applcommon/HardwareInterfaces \
  lib/applcommon/Utilities \

applcommon_stm32f3_SYS_INC_DIRS+=\

applcommon_stm32f3_SRC_DIRS+=\
  $(applcommon_stm32f3_LIB_ROOT)/Stm32F3xx \

INC_DIRS+=\
  $(applcommon_stm32f3_INC_DIRS) \
  $(applcommon_stm32f3_SRC_DIRS) \
