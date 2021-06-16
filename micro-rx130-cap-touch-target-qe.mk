LIBS+=qe

qe_LIB_ROOT:=src/$(TARGET)/e2Studio

qe_ASFLAGS:=$(ASFLAGS)
qe_CPPFLAGS:=$(CPPFLAGS)
qe_CFLAGS:=$(CFLAGS)
qe_CXXFLAGS:=$(CXXFLAGS)
qe_DEFINES:=$(DEFINES)

qe_CPPFLAGS+=-Wno-array-bounds -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-missing-braces

INC_DIRS+=\
  src/$(TARGET)/e2Studio/src \
  src/$(TARGET)/e2Studio/src/smc_gen/Config_PORT \
  src/$(TARGET)/e2Studio/src/smc_gen/general \
  src/$(TARGET)/e2Studio/src/smc_gen/r_bsp \
  src/$(TARGET)/e2Studio/src/smc_gen/r_bsp/mcu/all \
  src/$(TARGET)/e2Studio/src/smc_gen/r_config \
  src/$(TARGET)/e2Studio/src/smc_gen/r_ctsu_qe \
  src/$(TARGET)/e2Studio/src/smc_gen/r_ctsu_qe/src \
  src/$(TARGET)/e2Studio/src/smc_gen/r_pincfg \
  src/$(TARGET)/e2Studio/src/smc_gen/r_touch_qe \
  src/$(TARGET)/e2Studio/src/smc_gen/r_touch_qe/src \

qe_INC_DIRS+=$(INC_DIRS)

qe_SYS_INC_DIRS+=\

qe_SRC_DIRS+=\
  src/$(TARGET)/e2Studio/src/smc_gen/r_touch_qe/src \
  src/$(TARGET)/e2Studio/src/smc_gen/r_ctsu_qe/src \

qe_SRC_FILES+=\
  src/$(TARGET)/e2Studio/src/qe_common.c \
  src/$(TARGET)/e2Studio/src/qe_config01.c \
  src/$(TARGET)/e2Studio/src/smc_gen/r_bsp/mcu/all/r_bsp_common.c \
  src/$(TARGET)/e2Studio/src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.c \
  src/$(TARGET)/e2Studio/src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.c \
  src/$(TARGET)/e2Studio/src/smc_gen/r_bsp/mcu/all/r_bsp_locking.c \
  src/$(TARGET)/e2Studio/src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.c \
  src/$(TARGET)/e2Studio/src/smc_gen/r_bsp/mcu/rx130/mcu_interrupts.c \
  src/$(TARGET)/e2Studio/src/smc_gen/r_bsp/mcu/rx130/mcu_clocks.c \
  src/$(TARGET)/e2Studio/src/smc_gen/r_pincfg/r_ctsu_qe_pinset.c \

INC_DIRS+=$(qe_SRC_DIRS)
