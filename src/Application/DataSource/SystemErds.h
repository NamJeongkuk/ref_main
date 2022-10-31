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
#include "ResetCount.h"
#include "ResetReason.h"
#include "SecondsSinceLastReset.h"
#include "ProgramCounterAddress.h"
#include "ReadyToEnterBootLoaderState.h"
#include "PersonalityParametricData.h"
#include "AsyncDataSource_Eeprom.h"
#include "CompressorVotedSpeed.h"
#include "FanState.h"
#include "FanSpeed.h"
#include "GridArea.h"
#include "GridBlockNumber.h"
#include "TemperatureDegFx100.h"
#include "CalculatedGridLines.h"
#include "ValvePosition.h"
#include "Setpoint.h"
#include "DefrostState.h"
#include "DefrostHsmState.h"
#include "DefrostRequest.h"
#include "HeaterVotedState.h"
#include "EnergyDemandLevel.h"
#include "I_FaultWrapper.h"
#include "ConvertibleCompartmentStateType.h"
#include "CompressorState.h"
#include "CoolingMode.h"
#include "CoolingSpeed.h"
#include "PwmDutyCycle.h"
#include "InputCaptureType.h"
#include "DefrostCompressorOnTimeCounterFsmState.h"
#include "DoorAccelerationCounterFsmState.h"
#include "CompressorSpeedFrequencyInHz.h"
#include "StepperPositionRequest.h"
#include "SetpointZone.h"
#include "SetpointZoneTemperatureBounds.h"
#include "HeaterVotedDutyCycle.h"

// clang-format off

#define INCLUDE_RAM_Ram(_x) _x
#define INCLUDE_RAM_Virtual(_x)
#define INCLUDE_RAM_Nv(_x)
#define INCLUDE_RAM_NvProtected(_x)
#define INCLUDE_RAM_NvUnitSetting(_x)
#define INCLUDE_RAM_NvUserSetting(_x)
#define INCLUDE_RAM_NvRfid(_x)
#define INCLUDE_RAM_NvFaultSnapshot(_x)
#define INCLUDE_RAM_NvCycleHistory(_x)
#define INCLUDE_RAM_Fault(_x)
#define INCLUDE_RAM_Bsp(_x)
#define INCLUDE_RAM_MappedBsp(_x)

#define INCLUDE_VIRTUAL_Ram(_x)
#define INCLUDE_VIRTUAL_Virtual(_x) _x
#define INCLUDE_VIRTUAL_Nv(_x)
#define INCLUDE_VIRTUAL_NvProtected(_x)
#define INCLUDE_VIRTUAL_NvUnitSetting(_x)
#define INCLUDE_VIRTUAL_NvUserSetting(_x)
#define INCLUDE_VIRTUAL_NvRfid(_x) _x
#define INCLUDE_VIRTUAL_NvFaultSnapshot(_x)
#define INCLUDE_VIRTUAL_NvCycleHistory(_x)
#define INCLUDE_VIRTUAL_Fault(_x)
#define INCLUDE_VIRTUAL_Bsp(_x)
#define INCLUDE_VIRTUAL_MappedBsp(_x)

#define INCLUDE_NV_Ram(_x)
#define INCLUDE_NV_Virtual(_x)
#define INCLUDE_NV_Nv(_x) _x
#define INCLUDE_NV_Fault(_x)
#define INCLUDE_NV_Bsp(_x)
#define INCLUDE_NV_MappedBsp(_x)

#define INCLUDE_FAULT_Ram(_x)
#define INCLUDE_FAULT_Virtual(_x)
#define INCLUDE_FAULT_Nv(_x)
#define INCLUDE_FAULT_NvProtected(_x)
#define INCLUDE_FAULT_NvUnitSetting(_x)
#define INCLUDE_FAULT_NvUserSetting(_x)
#define INCLUDE_FAULT_NvRfid(_x)
#define INCLUDE_FAULT_NvFaultSnapshot(_x)
#define INCLUDE_FAULT_NvCycleHistory(_x)
#define INCLUDE_FAULT_Fault(_x) _x
#define INCLUDE_FAULT_Bsp(_x)
#define INCLUDE_FAULT_MappedBsp(_x)

#define INCLUDE_BSP_Ram(_x)
#define INCLUDE_BSP_Virtual(_x)
#define INCLUDE_BSP_Nv(_x)
#define INCLUDE_BSP_NvProtected(_x)
#define INCLUDE_BSP_NvUnitSetting(_x)
#define INCLUDE_BSP_NvUserSetting(_x)
#define INCLUDE_BSP_NvRfid(_x)
#define INCLUDE_BSP_NvFaultSnapshot(_x)
#define INCLUDE_BSP_NvCycleHistory(_x)
#define INCLUDE_BSP_Fault(_x)
#define INCLUDE_BSP_Bsp(_x) _x
#define INCLUDE_BSP_MappedBsp(_x)

#define INCLUDE_NVONLY_Ram(_x)
#define INCLUDE_NVONLY_Virtual(_x)
#define INCLUDE_NVONLY_Nv(_x) _x
#define INCLUDE_NVONLY_NvProtected(_x)
#define INCLUDE_NVONLY_NvUnitSetting(_x)
#define INCLUDE_NVONLY_NvUserSetting(_x)
#define INCLUDE_NVONLY_NvRfid(_x)
#define INCLUDE_NVONLY_NvFaultSnapshot(_x)
#define INCLUDE_NVONLY_NvCycleHistory(_x)
#define INCLUDE_NVONLY_Fault(_x)
#define INCLUDE_NVONLY_Bsp(_x)
#define INCLUDE_NVONLY_MappedBsp(_x)

#define INCLUDE_NVALL_Ram(_x)
#define INCLUDE_NVALL_Virtual(_x)
#define INCLUDE_NVALL_Nv(_x) _x
#define INCLUDE_NVALL_NvProtected(_x) _x
#define INCLUDE_NVALL_NvUnitSetting(_x) _x
#define INCLUDE_NVALL_NvUserSetting(_x) _x
#define INCLUDE_NVALL_NvRfid(_x) _x
#define INCLUDE_NVALL_NvFaultSnapshot(_x) _x
#define INCLUDE_NVALL_NvCycleHistory(_x) _x
#define INCLUDE_NVALL_Fault(_x)
#define INCLUDE_NVALL_Bsp(_x)
#define INCLUDE_NVALL_MappedBsp(_x)

#define INCLUDE_NVPROTECTED_Ram(_x)
#define INCLUDE_NVPROTECTED_Virtual(_x)
#define INCLUDE_NVPROTECTED_Nv(_x) _x
#define INCLUDE_NVPROTECTED_NvProtected(_x) _x
#define INCLUDE_NVPROTECTED_NvUnitSetting(_x)
#define INCLUDE_NVPROTECTED_NvUserSetting(_x)
#define INCLUDE_NVPROTECTED_NvRfid(_x)
#define INCLUDE_NVPROTECTED_NvFaultSnapshot(_x)
#define INCLUDE_NVPROTECTED_NvCycleHistory(_x)
#define INCLUDE_NVPROTECTED_Fault(_x)
#define INCLUDE_NVPROTECTED_Bsp(_x)
#define INCLUDE_NVPROTECTED_MappedBsp(_x)

#define INCLUDE_NVUNITSETTING_Ram(_x)
#define INCLUDE_NVUNITSETTING_Virtual(_x)
#define INCLUDE_NVUNITSETTING_Nv(_x) _x
#define INCLUDE_NVUNITSETTING_NvProtected(_x)
#define INCLUDE_NVUNITSETTING_NvUnitSetting(_x) _x
#define INCLUDE_NVUNITSETTING_NvUserSetting(_x)
#define INCLUDE_NVUNITSETTING_NvRfid(_x)
#define INCLUDE_NVUNITSETTING_NvFaultSnapshot(_x)
#define INCLUDE_NVUNITSETTING_NvCycleHistory(_x)
#define INCLUDE_NVUNITSETTING_Fault(_x)
#define INCLUDE_NVUNITSETTING_Bsp(_x)
#define INCLUDE_NVUNITSETTING_MappedBsp(_x)

#define INCLUDE_NVUSERSETTING_Ram(_x)
#define INCLUDE_NVUSERSETTING_Virtual(_x)
#define INCLUDE_NVUSERSETTING_Nv(_x) _x
#define INCLUDE_NVUSERSETTING_NvProtected(_x)
#define INCLUDE_NVUSERSETTING_NvUnitSetting(_x)
#define INCLUDE_NVUSERSETTING_NvUserSetting(_x) _x
#define INCLUDE_NVUSERSETTING_NvRfid(_x)
#define INCLUDE_NVUSERSETTING_NvFaultSnapshot(_x)
#define INCLUDE_NVUSERSETTING_NvCycleHistory(_x)
#define INCLUDE_NVUSERSETTING_Fault(_x)
#define INCLUDE_NVUSERSETTING_Bsp(_x)
#define INCLUDE_NVUSERSETTING_MappedBsp(_x)

#define INCLUDE_NVRFID_Ram(_x)
#define INCLUDE_NVRFID_Virtual(_x)
#define INCLUDE_NVRFID_Nv(_x) _x
#define INCLUDE_NVRFID_NvProtected(_x)
#define INCLUDE_NVRFID_NvUnitSetting(_x)
#define INCLUDE_NVRFID_NvUserSetting(_x)
#define INCLUDE_NVRFID_NvRfid(_x) _x
#define INCLUDE_NVRFID_NvFaultSnapshot(_x)
#define INCLUDE_NVRFID_NvCycleHistory(_x)
#define INCLUDE_NVRFID_Fault(_x)
#define INCLUDE_NVRFID_Bsp(_x)
#define INCLUDE_NVRFID_MappedBsp(_x)

#define INCLUDE_NVFAULTSNAPSHOT_Ram(_x)
#define INCLUDE_NVFAULTSNAPSHOT_Virtual(_x)
#define INCLUDE_NVFAULTSNAPSHOT_Nv(_x) _x
#define INCLUDE_NVFAULTSNAPSHOT_NvProtected(_x)
#define INCLUDE_NVFAULTSNAPSHOT_NvUnitSetting(_x)
#define INCLUDE_NVFAULTSNAPSHOT_NvUserSetting(_x)
#define INCLUDE_NVFAULTSNAPSHOT_NvRfid(_x)
#define INCLUDE_NVFAULTSNAPSHOT_NvFaultSnapshot(_x) _x
#define INCLUDE_NVFAULTSNAPSHOT_NvCycleHistory(_x)
#define INCLUDE_NVFAULTSNAPSHOT_Fault(_x)
#define INCLUDE_NVFAULTSNAPSHOT_Bsp(_x)
#define INCLUDE_NVFAULTSNAPSHOT_MappedBsp(_x)

#define INCLUDE_NVCYCLEHISTORY_Ram(_x)
#define INCLUDE_NVCYCLEHISTORY_Virtual(_x)
#define INCLUDE_NVCYCLEHISTORY_Nv(_x) _x
#define INCLUDE_NVCYCLEHISTORY_NvProtected(_x)
#define INCLUDE_NVCYCLEHISTORY_NvUnitSetting(_x)
#define INCLUDE_NVCYCLEHISTORY_NvUserSetting(_x)
#define INCLUDE_NVCYCLEHISTORY_NvRfid(_x)
#define INCLUDE_NVCYCLEHISTORY_NvFaultSnapshot(_x)
#define INCLUDE_NVCYCLEHISTORY_NvCycleHistory(_x)  _x
#define INCLUDE_NVCYCLEHISTORY_Fault(_x)
#define INCLUDE_NVCYCLEHISTORY_Bsp(_x)
#define INCLUDE_NVCYCLEHISTORY_MappedBsp(_x)

#define INCLUDE_NON_RAM_Ram(_x)
#define INCLUDE_NON_RAM_Virtual(_x) _x
#define INCLUDE_NON_RAM_Nv(_x) _x
#define INCLUDE_NON_RAM_NvProtected(_x) _x
#define INCLUDE_NON_RAM_NvUnitSetting(_x) _x
#define INCLUDE_NON_RAM_NvUserSetting(_x) _x
#define INCLUDE_NON_RAM_NvRfid(_x) _x
#define INCLUDE_NON_RAM_NvFaultSnapshot(_x) _x
#define INCLUDE_NON_RAM_NvCycleHistory(_x) _x
#define INCLUDE_NON_RAM_Fault(_x) _x
#define INCLUDE_NON_RAM_Bsp(_x) _x
#define INCLUDE_NON_RAM_MappedBsp(_x) _x

#define INCLUDE_MAPPED_BSP_Ram(_x)
#define INCLUDE_MAPPED_BSP_Virtual(_x)
#define INCLUDE_MAPPED_BSP_Nv(_x)
#define INCLUDE_MAPPED_BSP_NvProtected(_x)
#define INCLUDE_MAPPED_BSP_NvUnitSetting(_x)
#define INCLUDE_MAPPED_BSP_NvUserSetting(_x)
#define INCLUDE_MAPPED_BSP_NvRfid(_x)
#define INCLUDE_MAPPED_BSP_NvFaultSnapshot(_x)
#define INCLUDE_MAPPED_BSP_NvCycleHistory(_x)
#define INCLUDE_MAPPED_BSP_Fault(_x)
#define INCLUDE_MAPPED_BSP_Bsp(_x)
#define INCLUDE_MAPPED_BSP_MappedBsp(_x) _x

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
   ENTRY(Erd_NvMetadata,                                    0x0000, AsyncDataSource_EepromMetadata_t,                   Swap_N, Io_None, Sub_N, Nv,                     NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ModelNumber,                                   0x0001, ModelNumber_t,                                      Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_SerialNumber,                                  0x0002, SerialNumber_t,                                     Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ApplianceType,                                 0x0008, ApplianceType_t,                                    Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_SabbathMode,                                   0x0009, bool,                                               Swap_N, Io_All,  Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ReadyToEnterBootLoader,                        0x0030, ReadyToEnterBootLoaderState_t,                      Swap_N, Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_BuildNumber,                                   0x0031, uint32_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Reset,                                         0x0032, uint8_t,                                            Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_AppliancePersonality,                          0x0035, AppliancePersonality_t,                             Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SupportedImageTypes,                           0x0038, uint8_t,                                            Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_BootLoaderVersion,                             0x0039, Version_t,                                          Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationVersion,                            0x003A, Version_t,                                          Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ParametricVersion,                             0x003B, Version_t,                                          Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_AuxiliaryVersion,                              0x003C, Version_t,                                          Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_GitHash,                                       0x007F, GitHash_t,                                          Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsApiVersion,                  0x008F, ServiceDiagnosticsApiVersion_t,                     Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsRevision3Manifest,           0x0090, ServiceDiagnosticsRevision3Manifest_t,              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ApplianceApiManifest,                          0x0092, ApplianceApiManifest_t,                             Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ResetCount,                                    0x0700, ResetCount_t,                                       Swap_Y, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ResetReason,                                   0x0701, ResetReason_t,                                      Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SecondsSinceLastReset,                         0x0702, SecondsSinceLastReset_t,                            Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ProgramCounterAddressAtLastUassert,            0x0703, ProgramCounterAddress_t,                            Swap_Y, Io_O,    Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_FreshFoodSetpointRequest,                      0x1200, Setpoint_t,                                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                      NotFault) \
   ENTRY(Erd_FreshFoodSetpointRangeData,                    0x1201, UserSetpointRangeData_t,                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                      NotFault) \
   ENTRY(Erd_FreshFoodSetpointStatus,                       0x1202, Setpoint_t,                                         Swap_N, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Int8Max,   NotFault) \
   ENTRY(Erd_FreezerSetpointRequest,                        0x1210, Setpoint_t,                                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                      NotFault) \
   ENTRY(Erd_FreezerSetpointRangeData,                      0x1211, UserSetpointRangeData_t,                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                      NotFault) \
   ENTRY(Erd_FreezerSetpointStatus,                         0x1212, Setpoint_t,                                         Swap_N, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Int8Max,   NotFault) \
   \
   ENTRY(Erd_DemandResponseLevel,                           0xD006, EnergyDemandLevel_t,                                Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsEntityLocation,              0xE000, Erd_t,                                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsEntityManifest,              0xE100, ServiceDiagnosticsRevision3EntityManifest_t,        Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsEntityCycleCount,            0xE101, uint32_t,                                           Swap_Y, Io_All,  Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsRunTimeInMinutes,            0xE102, ApplianceRunTimeMinutes_t,                          Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableStatus,            0xE103, ServiceDiagnosticsRevision3TableStatus_t,           Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableClear,             0xE104, ServiceDiagnosticsRevision3TableClearRequest_t,     Swap_N, Io_O,    Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry0,            0xE105, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry1,            0xE106, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry2,            0xE107, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry3,            0xE108, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry4,            0xE109, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry5,            0xE10A, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry6,            0xE10B, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry7,            0xE10C, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry8,            0xE10D, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry9,            0xE10E, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot0,         0xE10F, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot1,         0xE110, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot2,         0xE111, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot3,         0xE112, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot4,         0xE113, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot5,         0xE114, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot6,         0xE115, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot7,         0xE116, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot8,         0xE117, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot9,         0xE118, FaultSnapshot_t,                                    Swap_Y, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceStatus,         0xE119, ServiceDiagnosticsRevision3SequenceStatus_t,        Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceClear,          0xE11A, ServiceDiagnosticsRevision3SequenceClearRequest_t,  Swap_N, Io_O,    Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry0,         0xE11B, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry1,         0xE11C, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry2,         0xE11D, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry3,         0xE11E, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry4,         0xE11F, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry5,         0xE120, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry6,         0xE121, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry7,         0xE122, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistorySequenceStatus,  0xE123, ServiceDiagnosticsRevision3SequenceStatus_t,        Swap_Y, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistorySequenceClear,   0xE124, ServiceDiagnosticsRevision3SequenceClearRequest_t,  Swap_Y, Io_O,    Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord0,                0xE125, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord1,                0xE126, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord2,                0xE127, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord3,                0xE128, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord4,                0xE129, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord5,                0xE12A, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord6,                0xE12B, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord7,                0xE12C, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord8,                0xE12D, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord9,                0xE12E, CycleHistoryRecord_t,                               Swap_Y, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistoryAddEntry,        0xE12F, CycleHistoryRecord_t,                               Swap_Y, Io_O,    Sub_N, Virtual,                NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_OldApplicationVersion,                         0xF000, Version_t,                                          Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimerModuleDiagnosticsEnable,                  0xF001, bool,                                               Swap_N, Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimerModuleDiagnosticsResult,                  0xF002, TimerModuleDiagnosticsResults_t,                    Swap_Y, Io_O,    Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ParametricDataImageCrc,                        0xF003, uint16_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SomeData,                                      0xF004, uint32_t,                                           Swap_Y, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Code,    NotFault) \
   ENTRY(Erd_ApplianceRunTimeInMinutesUpdatedHourly,        0xF005, ApplianceRunTimeMinutes_t,                          Swap_Y, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_StackPercentFree,                              0xF006, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_HeartbeatTick,                                 0xF00C, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PersonalityParametricData,                     0xF00E, PersonalityParametricData_t *,                      Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_NonVolatileDataSourceCacheSyncState,           0xF012, bool,                                               Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorIsSingleSpeed,                       0xF013, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DelayConvertibleCompartmentCooling,            0xF014, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CoolConvertibleCompartmentBeforeOff,           0xF015, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DelayedConvertibleCompartmentCoolingLowSpeed,  0xF016, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMakersEnabled,                              0xF017, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodPulldownOffsetEnabled,                0xF018, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_MaxValveTimeInPosAEnabled,                     0xF019, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentGridBlockNumber,         0xF01A, GridBlockNumber_t,                                  Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorIsOn,                                0xF01B, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CoolingSpeed,                                  0xF01C, CoolingSpeed_t,                                     Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SingleEvaporatorPulldownActive,                0xF01D, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorTripMitigationActive,                0xF01E, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_SensorsReadyToBeRead,                          0xF020, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SetpointResolverReady,                         0xF021, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentStateResolverReady,      0xF022, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_OverrideArbiterReady,                          0xF023, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridPluginReady,                               0xF024, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PeriodicNvUpdaterReady,                        0xF025, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SabbathPluginReady,                            0xF026, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridLineCalculatorIsReady,                     0xF027, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostParameterSelectorReady,                 0xF028, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, 0xF029, bool,                                             Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ActivelyWaitingForDefrostOnCompareMatchReady,  0xF02A, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostCompressorOnTimeCounterReady,           0xF02B, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DoorAccelerationCounterReady,                  0xF02C, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AdjustedSetpointPluginReady,                   0xF02D, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_PostDwellCompletionSignal,                     0xF030, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_LongTermAverageInDegFx100,           0xF031, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_LongTermAverageInDegFx100,             0xF032, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartment_LongTermAverageInDegFx100, 0xF033, TemperatureDegFx100_t,                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceBox_LongTermAverageInDegFx100,              0xF034, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliPan_LongTermAverageInDegFx100,             0xF035, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_ResetThermalShiftOffsetSignal,       0xF036, Signal_t,                                           Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_ResetThermalShiftOffsetSignal,         0xF037, Signal_t,                                           Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartment_ResetThermalShiftOffsetSignal, 0xF038, Signal_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceBox_ResetThermalShiftOffsetSignal,          0xF039, Signal_t,                                           Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliPan_ResetThermalShiftOffsetSignal,         0xF03A, Signal_t,                                           Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_WifiBufferedUart,                              0xF040, I_BufferedUart_t *,                                 Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FactoryBufferedUart,                           0xF041, I_BufferedUart_t *,                                 Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DoorBufferedUart,                              0xF042, I_BufferedUart_t *,                                 Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CaseBufferedUart,                              0xF043, I_BufferedUart_t *,                                 Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gea2Uart,                                      0xF044, I_Uart_t *,                                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_SystemTickInterrupt,                           0xF100, I_Interrupt_t *,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimeSource,                                    0xF101, I_TimeSource_t *,                                   Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimerModule,                                   0xF102, TimerModule_t *,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_InternalUart,                                  0xF103, I_Uart_t *,                                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ExternalUart,                                  0xF104, I_Uart_t *,                                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CrcCalcTable,                                  0xF105, I_Crc16Calculator_t *,                              Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gea2MessageEndpoint,                           0xF106, I_Gea2MessageEndpoint_t *,                          Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_JumpToBootLoaderAction,                        0xF107, I_Action_t *,                                       Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SystemResetAction,                             0xF108, I_Action_t *,                                       Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ExternalDataSource,                            0xF109, I_DataSource_t *,                                   Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FaultCodeTableInputGroup,                      0xF10A, I_InputGroup_t *,                                   Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SingleWireUart,                                0xF10B, I_Uart_t *,                                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ContextProtector,                              0xF10C, I_ContextProtector_t *,                             Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DefrostState,                                  0xF10D, DefrostState_t,                                     Swap_N, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DefrostHsmState,                               0xF10E, DefrostHsmState_t,                                  Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostRequest,                                0xF10F, DefrostRequest_t,                                   Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeaterOnTimeInMinutes,           0xF110, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostWasAbnormal,                     0xF112, bool,                                               Swap_N, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_FreezerAbnormalDefrostCycleCount,              0xF113, uint16_t,                                           Swap_Y, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_FreezerDefrostCycleCount,                      0xF114, uint16_t,                                           Swap_Y, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NumberOfFreezerAbnormalDefrostCycles,          0xF115, uint16_t,                                           Swap_Y, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DefrostFreezerScaledDoorAccelerationInSeconds, 0xF116, uint32_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds, 0xF117, uint32_t,                                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds, 0xF118, uint32_t,                            Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_MaxTimeBetweenDefrostsInMinutes,               0xF119, uint16_t,                                           Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostWasAbnormal,                   0xF11A, bool,                                               Swap_N, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,0xF11B, uint8_t,                                            Swap_N, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ExtendDefrostSignal,                           0xF11C, Signal_t,                                           Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostMaxHoldoffMet,                          0xF11D, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_MaxPrechillTimeInMinutes,                      0xF11E, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PostDwellExitTimeInMinutes,                    0xF11F, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostPrechillRunCounterInMinutes,            0xF120, uint16_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,      0xF121, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,        0xF122, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes, 0xF123, uint8_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PrechillTimeMet,                               0xF124, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_NoFreezeLimitIsActive,                         0xF125, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ActivelyWaitingForNextDefrost,                 0xF126, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostCompressorOnTimeInSeconds,              0xF127, uint32_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostCompressorOnTimeCounterFsmState,        0xF128, DefrostCompressorOnTimeCounterFsmState_t,           Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DoorAccelerationCounterFsmState,               0xF129, DoorAccelerationCounterFsmState_t,                  Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostWasAbnormal,      0xF12A, bool,                                               Swap_N, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_TimeInMinutesUntilReadyToDefrost,              0xF12B, uint16_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ReadyToDefrost,                                0xF12C, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimeThatPrechillConditionsAreMetInMinutes,     0xF12D, uint16_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,    0xF12E, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PulldownInMediumCompressorSpeedEnabled,        0xF12F, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_LeftHandFreshFoodDoorIsOpen,                   0xF130, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_RightHandFreshFoodDoorIsOpen,                  0xF131, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DoorInDoorIsOpen,                              0xF132, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDoorIsOpen,                             0xF133, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDoorIsOpen,              0xF134, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostIsFreshFoodOnly,                        0xF136, bool,                                               Swap_N, Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   ENTRY(Erd_AllFreshFoodDoorsAreClosed,                    0xF138, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodThermistor_AdcCount,                  0xF13A, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerThermistor_AdcCount,                    0xF13B, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_AdcCount,              0xF13C, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_AdcCount,                0xF13D, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_AdcCount, 0xF13E, AdcCounts_t,                                     Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientThermistor_AdcCount,                    0xF13F, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_AdcCount, 0xF140, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_UnfilteredTemperature,               0xF141, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_UnfilteredTemperature,                 0xF142, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvap_UnfilteredTemperature,           0xF143, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvap_UnfilteredTemperature,             0xF144, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartment_UnfilteredTemperature,  0xF145, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_UnfilteredTemperature,                 0xF146, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvap_UnfilteredTemperature, 0xF147, TemperatureDegFx100_t,                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_FilteredTemperature,                 0xF148, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperature,                   0xF149, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvap_FilteredTemperature,             0xF14A, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvap_FilteredTemperature,               0xF14B, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartment_FilteredTemperature,    0xF14C, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_FilteredTemperature,                   0xF14D, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvap_FilteredTemperature,0xF14E, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceBox_FilteredTemperature,                    0xF14F, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliPan_FilteredTemperature,                   0xF150, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentState,                   0xF155, ConvertibleCompartmentStateType_t,                  Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_ResolvedVote,   0xF156, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_WinningVoteErd, 0xF157, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_FactoryVote,    0xF158, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_UserVote,       0xF159, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_TimeInMinutesInValvePositionB,                 0xF160, uint16_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureOverrideRequest,0xF161, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureOverrideValue,  0xF162, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureResolved,       0xF163, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperatureOverrideRequest,    0xF164, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperatureOverrideValue,      0xF165, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperatureResolved,           0xF166, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_FilteredTemperatureOverrideRequest,  0xF167, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_FilteredTemperatureOverrideValue,    0xF168, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_FilteredTemperatureResolved,         0xF169, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CoolingMode,                                   0xF16A, CoolingMode_t,                                      Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpointZone,                           0xF16B, SetpointZone_t,                                     Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpointZone,                         0xF16C, SetpointZone_t,                                     Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpointZone,            0xF16D, SetpointZone_t,                                     Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_IceRateTriggerSignal,                          0xF170, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CalculatedCondenserFanControl,                 0xF171, FanControl_t,                                       Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedIceCabinetFanControl,                0xF173, FanControl_t,                                       Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedConvertibleCompartmentFanControl,    0xF174, FanControl_t,                                       Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedDeliFanControl,                      0xF175, FanControl_t,                                       Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedFreezerEvapFanControl,               0xF176, FanControl_t,                                       Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedFreshFoodEvapFanControl,             0xF177, FanControl_t,                                       Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerSetpoint_TemperatureBounds,             0xF178, SetpointZoneTemperatureBounds_t,                    Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_TemperatureBounds,           0xF179, SetpointZoneTemperatureBounds_t,                    Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_TemperatureBounds, 0xF17A, SetpointZoneTemperatureBounds_t,                 Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerEvaporatorThermistorIsValid,            0xF180, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvaporatorThermistorIsValid,          0xF181, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvaporatorThermistorIsValid, 0xF182, bool,                                           Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_ThermistorIsValid,                     0xF183, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_ThermistorIsValid,                   0xF184, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_ThermistorIsValid,                     0xF185, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartment_ThermistorIsValid,      0xF186, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodThermistor_IsValidOverrideValue,                      0xF190, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerThermistor_IsValidOverrideValue,                        0xF191, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_IsValidOverrideValue,                  0xF192, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_IsValidOverrideValue,                    0xF193, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideValue,  0xF194, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientThermistor_IsValidOverrideValue,                        0xF195, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideValue,     0xF196, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodThermistor_IsValidResolved,                           0xF197, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerThermistor_IsValidResolved,                             0xF198, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_IsValidResolved,                       0xF199, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_IsValidResolved,                         0xF19A, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved,       0xF19B, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientThermistor_IsValidResolved,                             0xF19C, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,          0xF19D, bool,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodThermistor_IsValidOverrideRequest,                     0xF19E, bool,                              Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerThermistor_IsValidOverrideRequest,                       0xF19F, bool,                              Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_IsValidOverrideRequest,                 0xF1A0, bool,                              Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_IsValidOverrideRequest,                   0xF1A1, bool,                              Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideRequest, 0xF1A2, bool,                              Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientThermistor_IsValidOverrideRequest,                       0xF1A3, bool,                              Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideRequest,    0xF1A4, bool,                              Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_HeartbeatLed,                                  0xF200, bool,                                               Swap_N, Io_O,    Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_RelayWatchdog,                                 0xF201, bool,                                               Swap_N, Io_O,    Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorInverterDriver,                      0xF206, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorRelay,                               0xF207, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater,                                0xF208, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeaterRelay,                     0xF209, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DisableMinimumCompressorTimes,                 0xF20E, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CompressorState,                               0xF20F, CompressorState_t,                                  Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CompressorControllerSpeedRequest,              0xF211, CompressorSpeed_t,                                  Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_ResolvedVote,                  0xF212, CompressorVotedSpeed_t,                             Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_WinningVoteErd,                0xF213, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_FactoryVote,                   0xF214, CompressorVotedSpeed_t,                             Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_DefrostVote,                   0xF215, CompressorVotedSpeed_t,                             Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_GridVote,                      0xF216, CompressorVotedSpeed_t,                             Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CondenserFanSpeed_ResolvedVote,                0xF230, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_WinningVoteErd,              0xF231, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_FactoryVote,                 0xF232, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_DefrostVote,                 0xF233, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_CompressorStartUpVote,       0xF234, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_GridVote,                    0xF235, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_IceCabinetFanSpeed_ResolvedVote,               0xF240, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_WinningVoteErd,             0xF241, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_FactoryVote,                0xF242, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_DefrostVote,                0xF243, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ValvePosition_ResolvedVote,                    0xF250, ValveVotedPosition_t,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ValvePosition_WinningVoteErd,                  0xF251, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ValvePosition_FactoryVote,                     0xF252, ValveVotedPosition_t,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ValvePosition_DefrostVote,                     0xF253, ValveVotedPosition_t,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ValvePosition_GridVote,                        0xF25E, ValveVotedPosition_t,                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodEvapFanSpeed_ResolvedVote,            0xF260, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_WinningVoteErd,          0xF261, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_FactoryVote,             0xF262, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_DefrostVote,             0xF263, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote,   0xF264, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_GridVote,                0xF265, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerEvapFanSpeed_ResolvedVote,              0xF270, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_WinningVoteErd,            0xF271, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_FactoryVote,               0xF272, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_DefrostVote,               0xF273, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_CompressorStartUpVote,     0xF274, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_IceRateVote,               0xF275, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_GridVote,                  0xF276, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Grid_BlockNumber,                              0xF2A0, GridBlockNumber_t,                                  Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Grid_PreviousBlocks,                           0xF2A1, PreviousGridBlockNumbers_t,                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Grid_CalculatedGridLines,                      0xF2A2, CalculatedGridLines_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridLoopSingleEvapRun_Test,                    0xF2A3, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridLoopDualEvapRun_Test,                      0xF2A4, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridLoopTripleEvapRun_Test,                    0xF2A5, uint8_t,                                            Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridArea,                                      0xF2A6, GridArea_t,                                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridOverrideSignal,                            0xF2A7, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridOverrideEnable,                            0xF2A8, bool,                                               Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_AdjustedSetpointInDegFx100,          0xF2B0, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_AdjustedSetpointInDegFx100,            0xF2B1, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartment_AdjustedSetpointInDegFx100, 0xF2B2, TemperatureDegFx100_t,                          Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceBox_AdjustedSetpointInDegFx100,             0xF2B3, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliPan_AdjustedSetpointInDegFx100,            0xF2B4, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerSetpoint_ResolvedVote,                  0xF2C0, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_WinningVoteErd,                0xF2C1, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_FactoryVote,                   0xF2C2, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_EnhancedSabbathVote,           0xF2C3, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_TurboFreezeVote,               0xF2C4, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_IceInDoorVote,                 0xF2C5, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_FreezerIceMakerVote,           0xF2C6, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_IceRateVote,                   0xF2C7, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_UserVote,                      0xF2C8, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodSetpoint_ResolvedVote,                0xF2D0, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_WinningVoteErd,              0xF2D1, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_FactoryVote,                 0xF2D2, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_EnhancedSabbathVote,         0xF2D3, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_TurboCoolVote,               0xF2D4, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_UserVote,                    0xF2D5, SetpointVotedTemperature_t,                         Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodDefrostHeater_ResolvedVote,           0xF2E0, HeaterVotedState_t,                                 Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_WinningVoteErd,         0xF2E1, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_FactoryVote,            0xF2E2, HeaterVotedState_t,                                 Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_DefrostVote,            0xF2E3, HeaterVotedState_t,                                 Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerDefrostHeater_ResolvedVote,             0xF2F0, HeaterVotedState_t,                                 Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_WinningVoteErd,           0xF2F1, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_FactoryVote,              0xF2F2, HeaterVotedState_t,                                 Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_DefrostVote,              0xF2F3, HeaterVotedState_t,                                 Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Fault_EepromReadFailure,                       0xF300, bool,                                               Swap_N, Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Fault_EepromWriteFailure,                      0xF301, bool,                                               Swap_N, Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Fault_EepromEraseFailure,                      0xF302, bool,                                               Swap_N, Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PersonalityIdOutOfRangeFlag,                   0xF303, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FaultWrapperInterfaceArray,                    0xF310, I_FaultWrapper_t *,                                 Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GpioGroupInterface,                            0xF311, I_GpioGroup_t *,                                    Swap_N, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_CoolingStatesGridVotesConstArrayMapInterface,  0xF312, I_ConstArrayMap_t *,                                Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_TestFaultWrapperCountSetRequest,               0xF313, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TestFaultWrapperCountClearRequest,             0xF314, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TestFaultWrapperCountResetRequest,             0xF315, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TestFaultWrapperConsecutiveSetRequest,         0xF316, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TestFaultWrapperConsecutiveClearRequest,       0xF317, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TestFaultWrapperConsecutiveResetRequest,       0xF318, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_ResolvedVote,   0xF320, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_WinningVoteErd, 0xF321, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_FactoryVote,    0xF322, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_DefrostVote,    0xF323, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_GridVote,       0xF324, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DeliFanSpeed_ResolvedVote,                     0xF350, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFanSpeed_WinningVoteErd,                   0xF351, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFanSpeed_FactoryVote,                      0xF352, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFanSpeed_DefrostVote,                      0xF353, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFanSpeed_GridVote,                         0xF354, FanVotedSpeed_t,                                    Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CondenserFan_Pwm,                              0xF361, PwmDutyCycle_t,                                     Swap_Y, Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFan_Pwm,                             0xF363, PwmDutyCycle_t,                                     Swap_Y, Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFan_Pwm,                 0xF364, PwmDutyCycle_t,                                     Swap_Y, Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFan_Pwm,                                   0xF365, PwmDutyCycle_t,                                     Swap_Y, Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFan_Pwm,                            0xF366, PwmDutyCycle_t,                                     Swap_Y, Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFan_Pwm,                          0xF367, PwmDutyCycle_t,                                     Swap_Y, Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CondenserFan_ActualRpm,                        0xF369, FanRpm_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFan_ActualRpm,                       0xF36B, FanRpm_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFan_ActualRpm,           0xF36C, FanRpm_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFan_ActualRpm,                             0xF36D, FanRpm_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFan_ActualRpm,                      0xF36E, FanRpm_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFan_ActualRpm,                    0xF36F, FanRpm_t,                                           Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CondenserFan_InputCaptureTime,                 0xF371, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFan_InputCaptureTime,                0xF373, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFan_InputCaptureTime,    0xF374, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFan_InputCaptureTime,                      0xF375, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFan_InputCaptureTime,               0xF376, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFan_InputCaptureTime,             0xF377, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodDamperPosition_ResolvedVote,               0xF380, DamperVotedPosition_t,                         Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_WinningVoteErd,             0xF381, WinningVoteErd_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_FactoryVote,                0xF382, DamperVotedPosition_t,                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_DefrostVote,                0xF383, DamperVotedPosition_t,                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_DamperFreezePreventionVote, 0xF384, DamperVotedPosition_t,                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_MaxOpenTimeVote,            0xF385, DamperVotedPosition_t,                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_GridVote,                   0xF386, DamperVotedPosition_t,                         Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FillTubeHeater_ResolvedVote,                   0xF390, HeaterVotedDutyCycle_t,                             Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater_WinningVoteErd,                 0xF391, WinningVoteErd_t,                                   Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater_FactoryVote,                    0xF392, HeaterVotedDutyCycle_t,                             Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater_IceMakerHarvestVote,            0xF393, HeaterVotedDutyCycle_t,                             Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater_ContinuousVote,                 0xF394, HeaterVotedDutyCycle_t,                             Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_ResolvedSetpointInDegFx100,          0xF3A0, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_CabinetOffsetInDegFx100,             0xF3A1, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_SetpointOffsetInDegFx100,            0xF3A2, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_CrossAmbientOffsetInDegFx100,        0xF3A3, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_PulldownOffsetInDegFx100,            0xF3A4, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_ThermalShiftInDegFx100,              0xF3A5, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Freezer_ResolvedSetpointInDegFx100,            0xF3A6, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_CabinetOffsetInDegFx100,               0xF3A7, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_CrossAmbientOffsetInDegFx100,          0xF3A8, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_ThermalShiftInDegFx100,                0xF3A9, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartment_ResolvedSetpointInDegFx100,     0xF3AA, TemperatureDegFx100_t,                      Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartment_SetpointOffsetInDegFx100,       0xF3AB, TemperatureDegFx100_t,                      Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartment_CrossAmbientOffsetInDegFx100,   0xF3AC, TemperatureDegFx100_t,                      Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartment_ThermalShiftInDegFx100, 0xF3AD, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_IceBox_ResolvedSetpointInDegFx100,             0xF3AE, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceBox_CabinetOffsetInDegFx100,                0xF3AF, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceBox_IceFormationOffsetInDegFx100,           0xF3B0, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceBox_ThermalShiftInDegFx100,                 0xF3B1, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DeliPan_ResolvedSetpointInDegFx100,            0xF3B2, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliPan_SetpointOffsetInDegFx100,              0xF3B3, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliPan_ThermalShiftInDegFx100,                0xF3B4, TemperatureDegFx100_t,                              Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodDamperStepperMotorPositionRequest,    0xF3C0, StepperPositionRequest_t,                           Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHomingRequest,                  0xF3C1, bool,                                               Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperCurrentPosition,                0xF3C2, DamperPosition_t,                                   Swap_N, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodDamperHeater_ResolvedVote,               0xF3D0, HeaterVotedDutyCycle_t,                          Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater_WinningVoteErd,             0xF3D1, WinningVoteErd_t,                                Swap_Y, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater_FactoryVote,                0xF3D2, HeaterVotedDutyCycle_t,                          Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater_DamperFreezePreventionVote, 0xF3D3, HeaterVotedDutyCycle_t,                          Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,      0xF3D4, HeaterVotedDutyCycle_t,                          Swap_N, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FillTubeHeaterPwmDutyCycle,                    0xF3E0, PwmDutyCycle_t,                                     Swap_Y, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ApplicationToBspPwm_PWM_0,                     0xF400, PwmDutyCycle_t,                                     Swap_Y, Io_O, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationToBspPwm_PWM_1,                     0xF401, PwmDutyCycle_t,                                     Swap_Y, Io_O, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationToBspPwm_PWM_2,                     0xF402, PwmDutyCycle_t,                                     Swap_Y, Io_O, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationToBspPwm_PWM_4,                     0xF403, PwmDutyCycle_t,                                     Swap_Y, Io_O, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationToBspPwm_PWM_5,                     0xF404, PwmDutyCycle_t,                                     Swap_Y, Io_O, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ApplicationToBspInputCapture_CAPT_0,           0xF410, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationToBspInputCapture_CAPT_1,           0xF411, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationToBspInputCapture_CAPT_2,           0xF412, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationToBspInputCapture_CAPT_4,           0xF413, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationToBspInputCapture_CAPT_5,           0xF414, InputCaptureMicroSeconds_t,                         Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Adc_ADC_0,                                     0xF420, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ADC_1,                                     0xF421, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_475K_TH_00,                         0xF422, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_00,                          0xF423, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_01,                          0xF424, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_02,                          0xF425, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_03,                          0xF426, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_04,                          0xF427, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_05,                          0xF428, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_06,                          0xF429, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_07,                          0xF42A, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_08,                          0xF42B, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_LOAD_CURRENT,                              0xF42C, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_HW_PERSONALITY_00,                         0xF42D, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_HW_PERSONALITY_01,                         0xF42E, AdcCounts_t,                                        Swap_Y, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData0,     0xF500, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData1,     0xF501, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData2,     0xF502, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData3,     0xF503, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData4,     0xF504, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData5,     0xF505, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData6,     0xF506, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData7,     0xF507, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData8,     0xF508, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData9,     0xF509, FaultSnapshotData_t,                                Swap_Y, Io_None, Sub_N, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_SomeFault,                                     0xF600, bool,                                               Swap_N, Io_None, Sub_N, Fault,                 NotNv,                                    FaultId_SomeFault) \
   ENTRY(Erd_SomeOtherFault,                                0xF601, bool,                                               Swap_N, Io_None, Sub_N, Fault,                 NotNv,                                    FaultId_SomeOtherFault) \
   \
   ENTRY(Erd_CacheSyncState,                                0xF7FF, bool,                                               Swap_N, Io_None, Sub_N, Virtual,               NotNv,                                    NotFault) \
   \
   ENTRY(Erd_EnhancedSabbathMode,                           0xF800, bool,                                               Swap_N, Io_None, Sub_Y, NvUserSetting,         NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   \
   ENTRY(Erd_EepromEraseRequestSignal,                      0xFAFF, Signal_t,                                           Swap_N, Io_None, Sub_Y, Ram,                   NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ProtectedSomeData,                             0xFB00, uint32_t,                                           Swap_N, Io_None, Sub_Y, NvProtected,           NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_UnitSettingSomeData,                           0xFB01, uint32_t,                                           Swap_N, Io_None, Sub_Y, NvUnitSetting,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_UserSettingSomeData,                           0xFB02, uint32_t,                                           Swap_N, Io_None, Sub_Y, NvUserSetting,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_RfidSomeData,                                  0xFB03, uint32_t,                                           Swap_N, Io_None, Sub_Y, NvRfid,                NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_FaultSnapshotSomeData,                         0xFB04, uint32_t,                                           Swap_N, Io_None, Sub_Y, NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_CycleHistorySomeData,                          0xFB05, uint32_t,                                           Swap_N, Io_None, Sub_Y, NvCycleHistory,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_DefrostCompressorOnTimeInSeconds,       0xFB06, uint32_t,                                           Swap_Y, Io_None, Sub_N, NvUnitSetting,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_DefrostFreezerScaledDoorAccelerationInSeconds, 0xFB07, uint32_t,                                    Swap_Y, Io_None, Sub_N, NvUnitSetting,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_DefrostFreshFoodScaledDoorAccelerationInSeconds, 0xFB08, uint32_t,                                  Swap_Y, Io_None, Sub_N, NvUnitSetting,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds, 0xFB09, uint32_t,                     Swap_Y, Io_None, Sub_N, NvUnitSetting,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_FreezerDefrostHeaterOnTimeInMinutes,    0xFB0A, uint8_t,                                            Swap_N, Io_None, Sub_N, NvUnitSetting,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_NvReservedProtected,                           0xFCFA, uint8_t,                                            Swap_N, Io_None, Sub_N,  NvProtected,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedUnitSetting,                         0xFCFB, uint8_t,                                            Swap_N, Io_None, Sub_N,  NvUnitSetting,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedUserSetting,                         0xFCFC, uint8_t,                                            Swap_N, Io_None, Sub_N,  NvUserSetting,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedRfid,                                0xFCFD, uint8_t,                                            Swap_N, Io_None, Sub_N,  NvRfid,               NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedFaultSnapshot,                       0xFCFE, uint8_t,                                            Swap_N, Io_None, Sub_N,  NvFaultSnapshot,      NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedCycleHistory,                        0xFCFF, uint8_t,                                            Swap_N, Io_None, Sub_N,  NvCycleHistory,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \

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

/*
 * In order to avoid circular dependencies, this file must be included as the last thing
 * in this file.
 */

#include "WinningVoteErd.h"

#endif
