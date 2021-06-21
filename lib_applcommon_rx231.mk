include lib_applcommon.mk

$(call create_library_with_defaults,applcommon_rx231,lib/applcommon/Hardware/)

applcommon_rx231_INC_DIRS:=\
  $(applcommon_INC_DIRS) \
  $(applcommon_SRC_DIRS) \
  $(applcommon_rx231_LIB_ROOT)/Rx231 \
  $(applcommon_rx231_LIB_ROOT)/Kpit/Rx231 \

applcommon_rx231_SYS_INC_DIRS:=\

applcommon_rx231_SRC_DIRS:=\
  $(applcommon_rx231_LIB_ROOT)/Kpit/Rx \
  $(applcommon_rx231_LIB_ROOT)/Kpit/Rx/Ul \
  $(applcommon_rx231_LIB_ROOT)/Kpit/Rx/Ul/Vendor \
  $(applcommon_rx231_LIB_ROOT)/Kpit/Rx2xx \
  $(applcommon_rx231_LIB_ROOT)/Rx231 \
  $(applcommon_rx231_LIB_ROOT)/../Ul \
  $(applcommon_rx231_LIB_ROOT)/../Ul/Plugins \

INC_DIRS+=\
  $(applcommon_rx231_INC_DIRS) \
  $(applcommon_rx231_SRC_DIRS) \
