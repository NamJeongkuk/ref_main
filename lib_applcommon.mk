LIBS+=applcommon

applcommon_LIB_ROOT:=lib/applcommon

applcommon_ASFLAGS:=$(ASFLAGS)
applcommon_CPPFLAGS:=$(CPPFLAGS)
applcommon_CFLAGS:=$(CFLAGS)
applcommon_CXXFLAGS:=$(CXXFLAGS)
applcommon_DEFINES:=$(DEFINES)

INC_DIRS+=\
  lib/applcommon/ApplianceApi \
  lib/applcommon/BootLoader \
  lib/applcommon/Git \
  lib/applcommon/Image \
  lib/applcommon/ServiceMode \
  lib/applcommon/Signal \
  lib/applcommon/Utilities \
  lib/applcommon/Validator \

applcommon_INC_DIRS+=$(INC_DIRS)

applcommon_SYS_INC_DIRS+=\

applcommon_SRC_DIRS+=\
  lib/applcommon/Action \
  lib/applcommon/Alignment \
  lib/applcommon/Allocator \
  lib/applcommon/ApplianceApi \
  lib/applcommon/Audio \
  lib/applcommon/Communications/Gea2 \
  lib/applcommon/Communications/Gea2/Commands \
  lib/applcommon/Communications/Gea2/MessageLayer \
  lib/applcommon/Communications/Gea2/PacketLayer \
  lib/applcommon/Constants \
  lib/applcommon/Crc \
  lib/applcommon/CycleEngine \
  lib/applcommon/CycleEngine/Extensions \
  lib/applcommon/CycleEngine/Extensions/RemainingTime \
  lib/applcommon/DataAccess \
  lib/applcommon/DataModel \
  lib/applcommon/DataSource \
  lib/applcommon/DataSource/Packet \
  lib/applcommon/DataSource/Packet/ReadWrite \
  lib/applcommon/DataSource/Packet/Subscriptions \
  lib/applcommon/DataStructures/LinkedList \
  lib/applcommon/DataStructures/Queue \
  lib/applcommon/DataStructures/RingBuffer \
  lib/applcommon/Debounce \
  lib/applcommon/Diagnostics \
  lib/applcommon/Display \
  lib/applcommon/Endianness \
  lib/applcommon/ErdStream \
  lib/applcommon/Event \
  lib/applcommon/Filter \
  lib/applcommon/Git \
  lib/applcommon/HardwareInterfaces \
  lib/applcommon/HardwareInterfaces/Adapters \
  lib/applcommon/Image \
  lib/applcommon/KeyManager \
  lib/applcommon/NonVolatile \
  lib/applcommon/RecordStorage \
  lib/applcommon/ResourceWatchdog \
  lib/applcommon/Selector \
  lib/applcommon/Signal \
  lib/applcommon/StateMachines \
  lib/applcommon/Time \
  lib/applcommon/TimeOfDay \
  lib/applcommon/Ui \
  lib/applcommon/Utilities \
  lib/applcommon/Validator

INC_DIRS+=$(applcommon_SRC_DIRS)
