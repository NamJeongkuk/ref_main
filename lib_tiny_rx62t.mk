include lib_tiny.mk

tiny_INC_DIRS+=\
  $(tiny_LIB_ROOT)/Hardware/Hal \
  $(tiny_LIB_ROOT)/Hardware/Rx/Kpit \
  $(tiny_LIB_ROOT)/Hardware/Rx62t \
  $(tiny_LIB_ROOT)/Hardware/Rx62t/Kpit \

tiny_SRC_DIRS+=\
  $(tiny_LIB_ROOT)/Hardware/Rx62t/Kpit \

tiny_SRC_FILES+=\
  $(tiny_LIB_ROOT)/Hardware/Rx/Kpit/Interrupts.c \

INC_DIRS+=\
  $(tiny_INC_DIRS) \
  $(tiny_SRC_DIRS) \
