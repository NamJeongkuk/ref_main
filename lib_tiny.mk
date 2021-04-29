LIBS+=tiny

tiny_LIB_ROOT:=lib/applcommon.tiny

tiny_ASFLAGS:=$(ASFLAGS)
tiny_CPPFLAGS:=$(CPPFLAGS)
tiny_CFLAGS:=$(CFLAGS)
tiny_CXXFLAGS:=$(CXXFLAGS)
tiny_DEFINES:=$(DEFINES)

INC_DIRS+=\
   lib/applcommon.tiny/src/Hardware/Hal \

tiny_INC_DIRS+=$(INC_DIRS)

tiny_SYS_INC_DIRS+=\

tiny_SRC_DIRS+=\
   lib/applcommon.tiny/src/ApplianceApi \
   lib/applcommon.tiny/src/BootLoader \
   lib/applcommon.tiny/src/Core \
   lib/applcommon.tiny/src/TinyLib \
   lib/applcommon.tiny/src/WiFi \

INC_DIRS+=$(tiny_SRC_DIRS)
