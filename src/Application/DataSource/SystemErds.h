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
#include "I_Uart.h"
#include "I_BufferedUart.h"
#include "I_ContextProtector.h"
#include "I_Gea2MessageEndpoint.h"
#include "I_GpioGroup.h"
#include "I_Interrupt.h"
#include "I_OutputGroup.h"
#include "Signal.h"
#include "Timer.h"
#include "TimerModuleDiagnostics.h"
#include "Version.h"
#include "XMacroUtils.h"
#include "AppliancePersonality.h"
#include "ApplianceRunTimeMinutes.h"
#include "ModelNumber.h"
#include "SerialNumber.h"
#include "ServiceDiagnosticsRevision3.h"
#include "FaultIds.h"
#include "FaultSnapshot.h"
#include "CycleHistoryRecord.h"
#include "I_Adc.h"
#include "KeyStreamPlugin.h"
#include "ResetCount.h"
#include "ResetReason.h"
#include "SecondsSinceLastReset.h"
#include "ProgramCounterAddress.h"
#include "ReadyToEnterBootLoaderState.h"
#include "AsyncDataSource_Eeprom.h"

// clang-format off

#define INCLUDE_RAM_Ram(_x) _x
#define INCLUDE_RAM_Virtual(_x)
#define INCLUDE_RAM_Nv(_x)
#define INCLUDE_RAM_Fault(_x)
#define INCLUDE_RAM_Bsp(_x)

#define INCLUDE_VIRTUAL_Ram(_x)
#define INCLUDE_VIRTUAL_Virtual(_x) _x
#define INCLUDE_VIRTUAL_Nv(_x)
#define INCLUDE_VIRTUAL_Fault(_x)
#define INCLUDE_VIRTUAL_Bsp(_x)

#define INCLUDE_NV_Ram(_x)
#define INCLUDE_NV_Virtual(_x)
#define INCLUDE_NV_Nv(_x) _x
#define INCLUDE_NV_Fault(_x)
#define INCLUDE_NV_Bsp(_x)

#define INCLUDE_FAULT_Ram(_x)
#define INCLUDE_FAULT_Virtual(_x)
#define INCLUDE_FAULT_Nv(_x)
#define INCLUDE_FAULT_Fault(_x) _x
#define INCLUDE_FAULT_Bsp(_x)

#define INCLUDE_BSP_Ram(_x)
#define INCLUDE_BSP_Virtual(_x)
#define INCLUDE_BSP_Nv(_x)
#define INCLUDE_BSP_Fault(_x)
#define INCLUDE_BSP_Bsp(_x) _x

#define INCLUDE_NON_RAM_Ram(_x)
#define INCLUDE_NON_RAM_Virtual(_x) _x
#define INCLUDE_NON_RAM_Nv(_x) _x
#define INCLUDE_NON_RAM_Fault(_x) _x
#define INCLUDE_NON_RAM_Bsp(_x) _x

#define INCLUDE_INPUT_OUTPUT_Io_None(_x)
#define INCLUDE_INPUT_OUTPUT_Io_O(_x)
#define INCLUDE_INPUT_OUTPUT_Io_All(_x) _x

#define INCLUDE_OUTPUT_Io_None(_x)
#define INCLUDE_OUTPUT_Io_O(_x) _x
#define INCLUDE_OUTPUT_Io_All(_x) _x

#define INCLUDE_SUBSCRIPTION_Sub_N(_x)
#define INCLUDE_SUBSCRIPTION_Sub_Y(_x) _x

enum
{
   RamErdBase = 0xFD00
};

//       Name,                                              Number, DataType,                                           Swap,   Io,      Sub,   StorageType, NvDefaultData,                         FaultId
#define ERD_TABLE(ENTRY)  \
   ENTRY(Erd_ModelNumber,                                   0x0001, ModelNumber_t,                                      Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_SerialNumber,                                  0x0002, SerialNumber_t,                                     Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ApplianceType,                                 0x0008, ApplianceType_t,                                    Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ReadyToEnterBootLoader,                        0x0030, ReadyToEnterBootLoaderState_t,                      Swap_N, Io_All,  Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_BuildNumber,                                   0x0031, uint32_t,                                           Swap_Y, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_Reset,                                         0x0032, uint8_t,                                            Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_AppliancePersonality,                          0x0035, AppliancePersonality_t,                             Swap_Y, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_SupportedImageTypes,                           0x0038, uint8_t,                                            Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_BootLoaderVersion,                             0x0039, Version_t,                                          Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationVersion,                            0x003A, Version_t,                                          Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ParametricVersion,                             0x003B, Version_t,                                          Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_AuxiliaryVersion,                              0x003C, Version_t,                                          Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_GitHash,                                       0x007F, GitHash_t,                                          Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsApiVersion,                  0x008F, ServiceDiagnosticsApiVersion_t,                     Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsRevision3Manifest,           0x0090, ServiceDiagnosticsRevision3Manifest_t,              Swap_Y, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ApplianceApiManifest,                          0x0092, ApplianceApiManifest_t,                             Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ResetCount,                                    0x0700, ResetCount_t,                                       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ResetReason,                                   0x0701, ResetReason_t,                                      Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_SecondsSinceLastReset,                         0x0702, SecondsSinceLastReset_t,                            Swap_Y, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ProgramCounterAddressAtLastUassert,            0x0703, ProgramCounterAddress_t,                            Swap_Y, Io_O,    Sub_N, Ram,       NotNv,   NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsEntityLocation,              0xE000, Erd_t,                                              Swap_Y, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsEntityManifest,              0xE100, ServiceDiagnosticsRevision3EntityManifest_t,        Swap_Y, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsEntityCycleCount,            0xE101, uint32_t,                                           Swap_Y, Io_All,  Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsRunTimeInMinutes,            0xE102, ApplianceRunTimeMinutes_t,                          Swap_Y, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableStatus,            0xE103, ServiceDiagnosticsRevision3TableStatus_t,           Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableClear,             0xE104, ServiceDiagnosticsRevision3TableClearRequest_t,     Swap_N, Io_O,    Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry0,            0xE105, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry1,            0xE106, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry2,            0xE107, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry3,            0xE108, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry4,            0xE109, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry5,            0xE10A, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry6,            0xE10B, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry7,            0xE10C, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry8,            0xE10D, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry9,            0xE10E, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot0,         0xE10F, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot1,         0xE110, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot2,         0xE111, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot3,         0xE112, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot4,         0xE113, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot5,         0xE114, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot6,         0xE115, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot7,         0xE116, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot8,         0xE117, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot9,         0xE118, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceStatus,         0xE119, ServiceDiagnosticsRevision3SequenceStatus_t,        Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceClear,          0xE11A, ServiceDiagnosticsRevision3SequenceClearRequest_t,  Swap_N, Io_O,    Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry0,         0xE11B, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry1,         0xE11C, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry2,         0xE11D, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry3,         0xE11E, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry4,         0xE11F, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry5,         0xE120, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry6,         0xE121, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry7,         0xE122, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistorySequenceStatus,  0xE123, ServiceDiagnosticsRevision3SequenceStatus_t,        Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistorySequenceClear,   0xE124, ServiceDiagnosticsRevision3SequenceClearRequest_t,  Swap_Y, Io_O,    Sub_N, Virtual,  NotNv,                                    NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord0,                0xE125, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord1,                0xE126, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord2,                0xE127, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord3,                0xE128, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord4,                0xE129, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord5,                0xE12A, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord6,                0xE12B, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord7,                0xE12C, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord8,                0xE12D, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord9,                0xE12E, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistoryAddEntry,        0xE12F, CycleHistoryRecord_t,                               Swap_Y, Io_O,    Sub_N, Virtual,  NotNv,   NotFault) \
   \
   ENTRY(Erd_OldApplicationVersion,                         0xF000, Version_t,                                          Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_TimerModuleDiagnosticsEnable,                  0xF001, bool,                                               Swap_N, Io_All,  Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_TimerModuleDiagnosticsResult,                  0xF002, TimerModuleDiagnosticsResults_t,                    Swap_Y, Io_O,    Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ResetSignal,                                   0xF003, Signal_t,                                           Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_SomeData,                                      0xF004, uint32_t,                                           Swap_Y, Io_None, Sub_N, Nv,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ApplianceRunTimeInMinutesUpdatedHourly,        0xF005, ApplianceRunTimeMinutes_t,                          Swap_Y, Io_None, Sub_N, Ram,       NotNv,                                   NotFault) \
   ENTRY(Erd_StackPercentFree,                              0xF006, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_KeyErdStream,                                  0xF007, KeyErdStream_t,                                     Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_KeyStreamSequenceNumber,                       0xF008, ErdStreamSequenceNumber_t,                          Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_KeyStreamMissedEventSignal,                    0xF009, Signal_t,                                           Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_KeyStreamSenderResetSignal,                    0xF00A, Signal_t,                                           Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_KeyStreamSenderStateRequest,                   0xF00B, ErdStreamStateRequest_t,                            Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_HeartbeatTick,                                 0xF00C, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_RemoteKeyState,                                0xF00D, bool,                                               Swap_N, Io_None, Sub_Y, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_NonVolatileDataSourceCacheSyncState,           0xF012, bool,                                               Swap_N, Io_None, Sub_N, Virtual,  NotNv,                                    NotFault) \
   \
   ENTRY(Erd_SystemTickInterrupt,                           0xF100, I_Interrupt_t *,                                    Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_TimeSource,                                    0xF101, I_TimeSource_t *,                                   Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_TimerModule,                                   0xF102, TimerModule_t *,                                    Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_InternalUart,                                  0xF103, I_Uart_t *,                                         Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ExternalUart,                                  0xF104, I_Uart_t *,                                         Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_CrcCalcTable,                                  0xF105, I_Crc16Calculator_t *,                              Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_Gea2MessageEndpoint,                           0xF106, I_Gea2MessageEndpoint_t *,                          Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_JumpToBootLoaderAction,                        0xF107, I_Action_t *,                                       Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_SystemResetAction,                             0xF108, I_Action_t *,                                       Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ExternalDataSource,                            0xF109, I_DataSource_t *,                                   Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_FaultCodeTableInputGroup,                      0xF10A, I_InputGroup_t *,                                   Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_SingleWireUart,                                0xF10B, I_Uart_t *,                                         Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   ENTRY(Erd_ContextProtector,                              0xF10C, I_ContextProtector_t *,                             Swap_N, Io_None, Sub_N, Ram,      NotNv,                                    NotFault) \
   \
   ENTRY(Erd_HeartbeatLed,                                  0xF200, bool,                                               Swap_N, Io_O,    Sub_N, Bsp,      NotNv,                                    NotFault) \
   ENTRY(Erd_OtherLed,                                      0xF201, bool,                                               Swap_N, Io_None, Sub_N, Bsp,      NotNv,                                    NotFault) \
   ENTRY(Erd_PushButtonSwitch,                              0xF202, bool,                                               Swap_N, Io_All,  Sub_Y, Bsp,      NotNv,                                    NotFault) \
   ENTRY(Erd_SomeAnalogInput,                               0xF203, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,      NotNv,                                    NotFault) \
   ENTRY(Erd_AnotherAnalogInput,                            0xF204, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,      NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData0,     0xF500, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData1,     0xF501, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData2,     0xF502, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData3,     0xF503, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData4,     0xF504, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData5,     0xF505, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData6,     0xF506, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData7,     0xF507, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData8,     0xF508, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData9,     0xF509, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, Ram,       NotNv,   NotFault) \
   \
   ENTRY(Erd_SomeFault,                                     0xF600, bool,                                               Swap_N, Io_None, Sub_N, Fault,    NotNv,                                    FaultId_SomeFault) \
   ENTRY(Erd_SomeOtherFault,                                0xF601, bool,                                               Swap_N, Io_None, Sub_N, Fault,    NotNv,                                    FaultId_SomeOtherFault) \
   \
   ENTRY(Erd_NvReserved,                                    0xFCFD, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,       NotNv,                                    NotFault) \
   ENTRY(Erd_NvMetadata,                                    0xFCFE, AsyncDataSource_EepromMetadata_t,          Swap_N, Io_None, Sub_N, Nv,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \

#define EXPAND_AS_INTERNAL_RAM_ERD_ENUM(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)(Name COMMA)

#define EXPAND_AS_NON_RAM_ERD_ENUM(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NON_RAM_, StorageType)(Name = Number COMMA)

#define EXPAND_AS_PUBLIC_ERD_ENUM(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   Public##Name = Number,

#define EXPAND_AS_FAULT_ID(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId)\
   CONCAT(INCLUDE_FAULT_, StorageType)(FaultId COMMA)

#define EXPAND_AS_FAULT_ERD_TABLE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_FAULT_, StorageType)(Name COMMA)

#define EXPAND_AS_ERD_FAULT_CODE_COUNT(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_FAULT_, StorageType)(+1)

enum
{
   ERD_TABLE(EXPAND_AS_NON_RAM_ERD_ENUM)
   Erd_RamBase = (RamErdBase - 1),
   ERD_TABLE(EXPAND_AS_INTERNAL_RAM_ERD_ENUM)
};

enum
{
   ERD_TABLE(EXPAND_AS_PUBLIC_ERD_ENUM)
};

// clang-format on

#endif
