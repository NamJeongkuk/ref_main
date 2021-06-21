include lib_tiny.mk

$(call create_library_with_defaults,tiny_psoc4100,lib/applcommon.tiny/src/Hardware/)

tiny_psoc4100_INC_DIRS:=\
  $(tiny_INC_DIRS) \
  $(tiny_SRC_DIRS) \

tiny_psoc4100_SYS_INC_DIRS:=\
  $(tiny_SYS_INC_DIRS) \

tiny_psoc4100_SRC_DIRS:=\
  $(tiny_psoc4100_LIB_ROOT)/Psoc4100 \
  $(tiny_psoc4100_LIB_ROOT)/Psoc4100/Ul \
  $(tiny_psoc4100_LIB_ROOT)/Psoc4100/Ul/Vendor \
  $(tiny_psoc4100_LIB_ROOT)/../Application/PsocCapTouch \

INC_DIRS+=\
  $(tiny_psoc4100_INC_DIRS) \
  $(tiny_psoc4100_SRC_DIRS) \
