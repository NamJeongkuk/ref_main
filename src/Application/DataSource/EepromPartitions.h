/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EEPROMPARTITIONS_H
#define EEPROMPARTITIONS_H

// clang-format off

// ENTRY(name, startAddress, size, nvType)
#define EEPROM_PARTITIONS(ENTRY)                                    \
   ENTRY(Protected,              0,      48,      NvProtected)      \
   ENTRY(UnitSetting,            48,     1024,    NvUnitSetting)   \
   ENTRY(UserSetting,            1072,   1024,    NvUserSetting)   \
   ENTRY(Rfid,                   2096,   256,     NvRfid)           \
   ENTRY(UsageProfile,           2352,   1024,    NvUsageProfile)   \
   ENTRY(FaultSnapshot,          3376,   1024,    NvFaultSnapshot)  \
   ENTRY(CycleHistory,           4400,   1024,    NvCycleHistory)

// clang-format on

#endif
