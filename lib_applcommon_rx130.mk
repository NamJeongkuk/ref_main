include lib_applcommon.mk

$(call create_library_with_defaults,applcommon_rx130,lib/applcommon/Hardware/)

applcommon_rx130_INC_DIRS:=\
  $(applcommon_INC_DIRS) \
  $(applcommon_SRC_DIRS) \
  $(applcommon_rx130_LIB_ROOT)/Rx130 \
  $(applcommon_rx130_LIB_ROOT)/Kpit/Rx130 \

applcommon_rx130_SYS_INC_DIRS:=\

applcommon_rx130_SRC_DIRS:=\
  $(applcommon_rx130_LIB_ROOT)/Kpit/Rx \
  $(applcommon_rx130_LIB_ROOT)/Kpit/Rx/Ul \
  $(applcommon_rx130_LIB_ROOT)/Kpit/Rx/Ul/Vendor \
  $(applcommon_rx130_LIB_ROOT)/Kpit/Rx2xx \
  $(applcommon_rx130_LIB_ROOT)/Rx130 \
  $(applcommon_rx130_LIB_ROOT)/../Ul \
  $(applcommon_rx130_LIB_ROOT)/../Ul/Plugins \

INC_DIRS+=\
  $(applcommon_rx130_INC_DIRS) \
  $(applcommon_rx130_SRC_DIRS) \
