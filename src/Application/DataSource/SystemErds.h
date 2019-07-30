/*!
 * @file
 * @brief System-wide ERD definitions.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SYSTEMERDS_H
#define SYSTEMERDS_H

#include "NonVolatileDataSourceDefaultData.h"
#include "ApplianceApiManifest.h"
#include "AsyncDataSource_FlashBlockGroup.h"
#include "Crc16Calculator_Table.h"
#include "GitHash.h"
#include "I_BufferedUart.h"
#include "I_Gea2MessageEndpoint.h"
#include "I_GpioGroup.h"
#include "I_Interrupt.h"
#include "I_OutputGroup.h"
#include "Signal.h"
#include "Timer.h"
#include "TimerModuleDiagnostics.h"
#include "Version.h"
#include "XMacroUtils.h"

#define Swap_N 0
#define Swap_Y 1

#define Io_None 0
#define Io_O 1
#define Io_All 2

#define O_N 0
#define O_Y 1

#define NotNv 0

#define INCLUDE_RAM_Ram(_x) _x
#define INCLUDE_RAM_Virtual(_x)
#define INCLUDE_RAM_NvPublic(_x)
#define INCLUDE_RAM_NvPrivate(_x)

#define INCLUDE_VIRTUAL_Ram(_x)
#define INCLUDE_VIRTUAL_Virtual(_x) _x
#define INCLUDE_VIRTUAL_NvPublic(_x)
#define INCLUDE_VIRTUAL_NvPrivate(_x)

#define INCLUDE_NV_PUBLIC_Ram(_x)
#define INCLUDE_NV_PUBLIC_Virtual(_x)
#define INCLUDE_NV_PUBLIC_NvPublic(_x) _x
#define INCLUDE_NV_PUBLIC_NvPrivate(_x)

#define INCLUDE_NV_PRIVATE_Ram(_x)
#define INCLUDE_NV_PRIVATE_Virtual(_x)
#define INCLUDE_NV_PRIVATE_NvPublic(_x)
#define INCLUDE_NV_PRIVATE_NvPrivate(_x) _x

#define INCLUDE_NV_Ram(_x)
#define INCLUDE_NV_Virtual(_x)
#define INCLUDE_NV_NvPublic(_x) _x
#define INCLUDE_NV_NvPrivate(_x) _x

#define INCLUDE_INPUT_OUTPUT_0(_x)
#define INCLUDE_INPUT_OUTPUT_1(_x)
#define INCLUDE_INPUT_OUTPUT_2(_x) _x

#define INCLUDE_OUTPUT_0(_x)
#define INCLUDE_OUTPUT_1(_x) _x
#define INCLUDE_OUTPUT_2(_x) _x

enum
{
   RamErdBaseId = 0xF000
};

// ErdEnum, ID, DataType, EndiannessAware, InOutConfig, StorageLoc, NonVolatileDefaultData)
#define ERD_TABLE(ENTRY)  \
   ENTRY(Erd_ReadyToEnterBootLoader,           0x0030, bool,                                       Swap_N, Io_All,  Ram,         NotNv) \
   ENTRY(Erd_BuildNumber,                      0x0031, uint32_t,                                   Swap_Y, Io_None, Ram,         NotNv) \
   ENTRY(Erd_Reset,                            0x0032, uint8_t,                                    Swap_N, Io_None, Virtual,     NotNv) \
   ENTRY(Erd_SupportedImageTypes,              0x0038, uint8_t,                                    Swap_N, Io_None, Virtual,     NotNv) \
   ENTRY(Erd_BootLoaderVersion,                0x0039, Version_t,                                  Swap_N, Io_None, Virtual,     NotNv) \
   ENTRY(Erd_ApplicationVersion,               0x003A, Version_t,                                  Swap_N, Io_None, Virtual,     NotNv) \
   ENTRY(Erd_ParametricVersion,                0x003B, Version_t,                                  Swap_N, Io_None, Virtual,     NotNv) \
   ENTRY(Erd_AuxiliaryVersion,                 0x003C, Version_t,                                  Swap_N, Io_None, Virtual,     NotNv) \
   ENTRY(Erd_GitHash,                          0x007F, GitHash_t,                                  Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_ApplianceApiManifest,             0x0092, ApplianceApiManifest_t,                     Swap_Y, Io_None, Virtual,     NotNv) \
   ENTRY(Erd_OldApplicationVersion,            0xF000, Version_t,                                  Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_SystemTickInterrupt,              0xF003, I_Interrupt_t *,                            Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_TimeSource,                       0xF004, I_TimeSource_t *,                           Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_TimerModule,                      0xF005, TimerModule_t *,                            Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_GpioGroup,                        0xF006, I_GpioGroup_t *,                            Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_InternalBufferedUart,             0xF007, I_BufferedUart_t *,                         Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_ExternalBufferedUart,             0xF008, I_BufferedUart_t *,                         Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_CrcCalcTable,                     0xF009, I_Crc16Calculator_t *,                      Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_Gea2MessageEndpoint,              0xF00A, I_Gea2MessageEndpoint_t *,                  Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_JumpToBootLoaderAction,           0xF00B, I_Action_t *,                               Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_SystemResetAction,                0xF00C, I_Action_t *,                               Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_TimerModuleDiagnosticsEnable,     0xF00D, bool,                                       Swap_N, Io_All,  Ram,         NotNv) \
   ENTRY(Erd_TimerModuleDiagnosticsResult,     0xF00E, TimerModuleDiagnosticsResults_t,            Swap_Y, Io_O,    Ram,         NotNv) \
   ENTRY(Erd_ResetSignal,                      0xF00F, Signal_t,                                   Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_ExternalDataSource,               0xF010, I_DataSource_t *,                           Swap_N, Io_None, Ram,         NotNv) \
   ENTRY(Erd_NvReserved,                       0xFFFD, uint8_t,                                    Swap_N, Io_None, NvPublic,    NotNv) \
   ENTRY(Erd_NvMetadata,                       0xFFFE, AsyncDataSource_FlashBlockGroupMetadata_t,  Swap_N, Io_None, NvPrivate,   NonVolatileDataSourceDefaultData_Zeros) \

#define EXPAND_AS_NV_PUBLIC_ERD_ENUM(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData) \
   CONCAT(INCLUDE_NV_PUBLIC_, StorageType)(Name = Number COMMA)

#define EXPAND_AS_VIRTUAL_ERD_ENUM(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData) \
   CONCAT(INCLUDE_VIRTUAL_, StorageType)(Name = Number COMMA)

#define EXPAND_AS_INTERNAL_RAM_ERD_ENUM(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData) \
   CONCAT(INCLUDE_RAM_, StorageType)(Name COMMA)

#define EXPAND_AS_NV_PRIVATE_ERD_ENUM(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData) \
   CONCAT(INCLUDE_NV_PRIVATE_, StorageType)(Name = Number COMMA)

#define EXPAND_AS_PUBLIC_ERD_ENUM(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData) \
   Public##Name = Number,

enum
{
   ERD_TABLE(EXPAND_AS_NV_PUBLIC_ERD_ENUM)
   ERD_TABLE(EXPAND_AS_VIRTUAL_ERD_ENUM)
   Erd_Base = (RamErdBaseId - 1),
   ERD_TABLE(EXPAND_AS_INTERNAL_RAM_ERD_ENUM)
   ERD_TABLE(EXPAND_AS_NV_PRIVATE_ERD_ENUM)
};

enum
{
   ERD_TABLE(EXPAND_AS_PUBLIC_ERD_ENUM)
};

#endif
