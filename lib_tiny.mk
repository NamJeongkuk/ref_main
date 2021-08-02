$(call create_library_with_defaults,tiny,lib/applcommon.tiny/src/)

tiny_INC_DIRS+=\
  $(tiny_EXTERNAL_INC_DIRS) \
  $(tiny_LIB_ROOT)/Hardware/Hal \

tiny_SYS_INC_DIRS+=\
  $(tiny_EXTERNAL_SYS_INC_DIRS) \

tiny_SRC_DIRS+=\
  $(tiny_LIB_ROOT)/ApplianceApi \
  $(tiny_LIB_ROOT)/BootLoader \
  $(tiny_LIB_ROOT)/Core \
  $(tiny_LIB_ROOT)/TinyLib \
  $(tiny_LIB_ROOT)/WiFi \

INC_DIRS+=\
  $(tiny_INC_DIRS) \
  $(tiny_SRC_DIRS) \
