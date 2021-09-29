include lib_tiny.mk

tiny_INC_DIRS+=\

ifdef __llvm_rl78_tools_setup_included
tiny_SRC_DIRS+=\
  $(tiny_LIB_ROOT)/Hardware/Rl78/Llvm \
  $(tiny_LIB_ROOT)/Hardware/Rl78/Llvm/Ul \
  $(tiny_LIB_ROOT)/Hardware/Rl78/Llvm/Ul/Vendor
endif

ifdef __kpit_rl78_tools_setup_included
tiny_SRC_DIRS+=\
  $(tiny_LIB_ROOT)/Hardware/Rl78/Kpit \
  $(tiny_LIB_ROOT)/Hardware/Rl78/Kpit/Ul \
  $(tiny_LIB_ROOT)/Hardware/Rl78/Kpit/Ul/Vendor
endif

INC_DIRS+=\
  $(tiny_INC_DIRS) \
  $(tiny_SRC_DIRS) \
