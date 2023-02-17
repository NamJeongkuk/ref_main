$(call create_library_with_defaults,applcommon,lib/applcommon/src)

applcommon_INC_DIRS:=\
  $(applcommon_EXTERNAL_INC_DIRS) \
  $(applcommon_LIB_ROOT)/ApplianceApi \
  $(applcommon_LIB_ROOT)/BootLoader \
  $(applcommon_LIB_ROOT)/Git \
  $(applcommon_LIB_ROOT)/Image \
  $(applcommon_LIB_ROOT)/ServiceMode \
  $(applcommon_LIB_ROOT)/Signal \
  $(applcommon_LIB_ROOT)/Utilities \
  $(applcommon_LIB_ROOT)/Validator \

applcommon_SYS_INC_DIRS:=\
  $(applcommon_EXTERNAL_SYS_INC_DIRS) \

applcommon_SRC_DIRS:=\
  $(applcommon_LIB_ROOT)/Action \
  $(applcommon_LIB_ROOT)/Alignment \
  $(applcommon_LIB_ROOT)/Allocator \
  $(applcommon_LIB_ROOT)/ApplianceApi \
  $(applcommon_LIB_ROOT)/Audio \
  $(applcommon_LIB_ROOT)/BootLoopDefender \
  $(applcommon_LIB_ROOT)/Communications/Gea2 \
  $(applcommon_LIB_ROOT)/Communications/Gea2/Commands \
  $(applcommon_LIB_ROOT)/Communications/Gea2/MessageLayer \
  $(applcommon_LIB_ROOT)/Communications/Gea2/PacketLayer \
  $(applcommon_LIB_ROOT)/Constants \
  $(applcommon_LIB_ROOT)/Crc \
  $(applcommon_LIB_ROOT)/CycleEngine \
  $(applcommon_LIB_ROOT)/CycleEngine/Extensions \
  $(applcommon_LIB_ROOT)/CycleEngine/Extensions/RemainingTime \
  $(applcommon_LIB_ROOT)/DataAccess \
  $(applcommon_LIB_ROOT)/DataModel \
  $(applcommon_LIB_ROOT)/DataSource \
  $(applcommon_LIB_ROOT)/DataSource/Packet \
  $(applcommon_LIB_ROOT)/DataSource/Packet/ReadWrite \
  $(applcommon_LIB_ROOT)/DataSource/Packet/Subscriptions \
  $(applcommon_LIB_ROOT)/DataStructures/LinkedList \
  $(applcommon_LIB_ROOT)/DataStructures/Queue \
  $(applcommon_LIB_ROOT)/DataStructures/RingBuffer \
  $(applcommon_LIB_ROOT)/Debounce \
  $(applcommon_LIB_ROOT)/Diagnostics \
  $(applcommon_LIB_ROOT)/Display \
  $(applcommon_LIB_ROOT)/Endianness \
  $(applcommon_LIB_ROOT)/ErdStream \
  $(applcommon_LIB_ROOT)/Event \
  $(applcommon_LIB_ROOT)/Filter \
  $(applcommon_LIB_ROOT)/Git \
  $(applcommon_LIB_ROOT)/HardwareInterfaces \
  $(applcommon_LIB_ROOT)/HardwareInterfaces/Adapters \
  $(applcommon_LIB_ROOT)/HardwareInterfaces/Relays \
  $(applcommon_LIB_ROOT)/Image \
  $(applcommon_LIB_ROOT)/KeyManager \
  $(applcommon_LIB_ROOT)/Mapper \
  $(applcommon_LIB_ROOT)/NonVolatile \
  $(applcommon_LIB_ROOT)/OverrideArbiter \
  $(applcommon_LIB_ROOT)/RecordStorage \
  $(applcommon_LIB_ROOT)/ResourceWatchdog \
  $(applcommon_LIB_ROOT)/Selector \
  $(applcommon_LIB_ROOT)/Sha256/ \
  $(applcommon_LIB_ROOT)/Signal \
  $(applcommon_LIB_ROOT)/StateMachines \
  $(applcommon_LIB_ROOT)/Time \
  $(applcommon_LIB_ROOT)/TimeOfDay \
  $(applcommon_LIB_ROOT)/Ui \
  $(applcommon_LIB_ROOT)/Utilities \
  $(applcommon_LIB_ROOT)/Validator

INC_DIRS+=\
  $(applcommon_INC_DIRS) \
  $(applcommon_SRC_DIRS) \
