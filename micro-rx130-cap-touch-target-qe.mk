$(call create_library_with_defaults,qe,src/$(TARGET)/e2Studio/)

qe_CPPFLAGS+=-Wno-array-bounds -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-missing-braces

qe_INC_DIRS:=\
  $(qe_LIB_ROOT)/../../../lib/applcommon.tiny/src/Application/Rx130CapTouch/QeHacks/Debug \
  $(qe_LIB_ROOT)/../../../lib/applcommon.tiny/src/Application/Rx130CapTouch/QeHacks/Override \
  $(qe_LIB_ROOT)/../../../lib/applcommon.tiny/src/Application/Rx130CapTouch/QeHacks/Override/mcu/all \
  $(qe_LIB_ROOT)/src \
  $(qe_LIB_ROOT)/src/smc_gen/Config_PORT \
  $(qe_LIB_ROOT)/src/smc_gen/general \
  $(qe_LIB_ROOT)/src/smc_gen/r_bsp \
  $(qe_LIB_ROOT)/src/smc_gen/r_bsp/mcu/all \
  $(qe_LIB_ROOT)/src/smc_gen/r_config \
  $(qe_LIB_ROOT)/src/smc_gen/r_ctsu_qe \
  $(qe_LIB_ROOT)/src/smc_gen/r_ctsu_qe/src \
  $(qe_LIB_ROOT)/src/smc_gen/r_pincfg \
  $(qe_LIB_ROOT)/src/smc_gen/r_touch_qe \
  $(qe_LIB_ROOT)/src/smc_gen/r_touch_qe/src \
  $(qe_LIB_ROOT)/../../../lib/applcommon.tiny/src/Application/Rx130CapTouch/QeDiagnostics \

qe_SYS_INC_DIRS:=\

qe_SRC_DIRS:=\
  $(qe_LIB_ROOT)/../../../lib/applcommon.tiny/src/Application/Rx130CapTouch/QeHacks/Debug \
  $(qe_LIB_ROOT)/src/smc_gen/r_touch_qe/src \
  $(qe_LIB_ROOT)/src/smc_gen/r_ctsu_qe/src \
  $(qe_LIB_ROOT)/../../../lib/applcommon.tiny/src/Application/Rx130CapTouch/QeDiagnostics \

qe_SRC_FILES:=\
  $(qe_LIB_ROOT)/src/qe_common.c \
  $(qe_LIB_ROOT)/src/qe_config01.c \
  $(qe_LIB_ROOT)/src/smc_gen/r_bsp/mcu/all/r_bsp_common.c \
  $(qe_LIB_ROOT)/src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.c \
  $(qe_LIB_ROOT)/src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.c \
  $(qe_LIB_ROOT)/src/smc_gen/r_bsp/mcu/all/r_bsp_locking.c \
  $(qe_LIB_ROOT)/src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.c \
  $(qe_LIB_ROOT)/src/smc_gen/r_bsp/mcu/rx130/mcu_interrupts.c \
  $(qe_LIB_ROOT)/src/smc_gen/r_bsp/mcu/rx130/mcu_clocks.c \
  $(qe_LIB_ROOT)/src/smc_gen/r_pincfg/r_ctsu_qe_pinset.c \

INC_DIRS+=$(qe_INC_DIRS)
