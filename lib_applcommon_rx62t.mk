include lib_applcommon.mk

applcommon_CFLAGS+=-Wno-unused-const-variable

applcommon_INC_DIRS+=\

applcommon_SRC_DIRS+=\
  $(applcommon_LIB_ROOT)/Ul \
  $(applcommon_LIB_ROOT)/Ul/Plugins \
  $(applcommon_LIB_ROOT)/Hardware/Kpit/Rx62t \
  $(applcommon_LIB_ROOT)/Hardware/Kpit/Rx62t/DataFlash \
  $(applcommon_LIB_ROOT)/Hardware/Kpit/Rx62t/Ul \
  $(applcommon_LIB_ROOT)/Hardware/Kpit/Rx62t/Ul/Vendor \

INC_DIRS+=\
  $(applcommon_INC_DIRS) \
  $(applcommon_SRC_DIRS) \
