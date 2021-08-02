include lib_applcommon.mk

applcommon_INC_DIRS+=\
  $(applcommon_LIB_ROOT)/Hardware/Rx231 \
  $(applcommon_LIB_ROOT)/Hardware/Kpit/Rx231 \

applcommon_SRC_DIRS+=\
  $(applcommon_LIB_ROOT)/Hardware/Kpit/Rx \
  $(applcommon_LIB_ROOT)/Hardware/Kpit/Rx/Ul \
  $(applcommon_LIB_ROOT)/Hardware/Kpit/Rx/Ul/Vendor \
  $(applcommon_LIB_ROOT)/Hardware/Kpit/Rx2xx \
  $(applcommon_LIB_ROOT)/Hardware/Rx231 \
  $(applcommon_LIB_ROOT)/Ul \
  $(applcommon_LIB_ROOT)/Ul/Plugins \

INC_DIRS+=\
  $(applcommon_INC_DIRS) \
  $(applcommon_SRC_DIRS) \
