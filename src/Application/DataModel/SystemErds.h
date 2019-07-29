/*!
 * @file
 * @brief System-wide ERD definitions.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SYSTEMERDS_H
#define SYSTEMERDS_H

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

#define Swap_Y 1
#define Swap_N 0

#define Io_Y 1
#define Io_N 0

#define O_Y 1
#define O_N 0

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

enum
{
   RamErdBaseId = 0xF000
};

// ENTRY(Name, Number, DataType, Type, Swap, Io, O)
#define ERD_TABLE(ENTRY)  \
   ENTRY(Erd_ReadyToEnterBootLoader,           0x0030, bool,                                      Ram,       Swap_N, Io_Y, O_N) \
   ENTRY(Erd_BuildNumber,                      0x0031, uint32_t,                                  Ram,       Swap_Y, Io_N, O_N) \
   ENTRY(Erd_Reset,                            0x0032, uint8_t,                                   Virtual,   Swap_N, Io_N, O_N) \
   ENTRY(Erd_SupportedImageTypes,              0x0038, uint8_t,                                   Virtual,   Swap_N, Io_N, O_N) \
   ENTRY(Erd_BootLoaderVersion,                0x0039, Version_t,                                 Virtual,   Swap_N, Io_N, O_N) \
   ENTRY(Erd_ApplicationVersion,               0x003A, Version_t,                                 Virtual,   Swap_N, Io_N, O_N) \
   ENTRY(Erd_ParametricVersion,                0x003B, Version_t,                                 Virtual,   Swap_N, Io_N, O_N) \
   ENTRY(Erd_AuxiliaryVersion,                 0x003C, Version_t,                                 Virtual,   Swap_N, Io_N, O_N) \
   ENTRY(Erd_GitHash,                          0x007F, GitHash_t,                                 Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_ApplianceApiManifest,             0x0092, ApplianceApiManifest_t,                    Virtual,   Swap_Y, Io_N, O_N) \
   ENTRY(Erd_OldApplicationVersion,            0xF000, Version_t,                                 Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_SystemTickInterrupt,              0xF003, I_Interrupt_t *,                           Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_TimeSource,                       0xF004, I_TimeSource_t *,                          Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_TimerModule,                      0xF005, TimerModule_t *,                           Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_GpioGroup,                        0xF006, I_GpioGroup_t *,                           Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_InternalBufferedUart,             0xF007, I_BufferedUart_t *,                        Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_ExternalBufferedUart,             0xF008, I_BufferedUart_t *,                        Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_CrcCalcTable,                     0xF009, I_Crc16Calculator_t *,                     Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_Gea2MessageEndpoint,              0xF00A, I_Gea2MessageEndpoint_t *,                 Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_JumpToBootLoaderAction,           0xF00B, I_Action_t *,                              Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_SystemResetAction,                0xF00C, I_Action_t *,                              Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_TimerModuleDiagnosticsEnable,     0xF00D, bool,                                      Ram,       Swap_N, Io_Y, O_Y) \
   ENTRY(Erd_TimerModuleDiagnosticsResult,     0xF00E, TimerModuleDiagnosticsResults_t,           Ram,       Swap_Y, Io_N, O_Y) \
   ENTRY(Erd_ResetSignal,                      0xF00F, Signal_t,                                  Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_ExternalDataSource,               0xF010, I_DataSource_t *,                          Ram,       Swap_N, Io_N, O_N) \
   ENTRY(Erd_NvU8,                             0xFFFD, uint8_t,                                   NvPublic,  Swap_N, Io_N, O_N) \
   ENTRY(Erd_NvMetadata,                       0xFFFE, AsyncDataSource_FlashBlockGroupMetadata_t, NvPrivate, Swap_N, Io_N, O_N) \

#define EXPAND_AS_NV_PUBLIC_ERD_ENUM(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_NV_PUBLIC_, Type)(Name = Number COMMA)

#define EXPAND_AS_VIRTUAL_ERD_ENUM(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_VIRTUAL_, Type)(Name = Number COMMA)

#define EXPAND_AS_INTERNAL_RAM_ERD_ENUM(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_RAM_, Type)(Name COMMA)

#define EXPAND_AS_NV_PRIVATE_ERD_ENUM(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_NV_PRIVATE_, Type)(Name = Number COMMA)

#define EXPAND_AS_PUBLIC_ERD_ENUM(Name, Number, DataType, Type, Swap, Io, O) \
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
