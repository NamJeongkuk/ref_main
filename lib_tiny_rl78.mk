include lib_tiny.mk

tiny_INC_DIRS+=\

tiny_SRC_DIRS+=\
  $(tiny_LIB_ROOT)/Hardware/Rl78/Kpit \
  $(tiny_LIB_ROOT)/Hardware/Rl78/Kpit/Ul \
  $(tiny_LIB_ROOT)/Hardware/Rl78/Kpit/Ul/Vendor \

INC_DIRS+=\
  $(tiny_INC_DIRS) \
  $(tiny_SRC_DIRS) \
