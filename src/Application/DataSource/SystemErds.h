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
#include "SetpointVotedTemperature.h"
#include "DefrostState.h"
#include "DefrostHsmState.h"
#include "DefrostTestRequest.h"
#include "DefrostTestStateRequestMessage.h"
#include "HeaterVotedState.h"
#include "EnergyDemandLevel.h"
#include "I_FaultWrapper.h"
#include "ConvertibleCompartmentStateType.h"
#include "CompressorState.h"
#include "CoolingMode.h"
#include "CoolingSpeed.h"
#include "PwmDutyCycle.h"
#include "InputCaptureType.h"
#include "CompressorSpeedFrequencyInHz.h"
#include "StepperPositionRequest.h"
#include "SetpointZone.h"
#include "SetpointZoneTemperatureBounds.h"
#include "DamperFreezePreventionFsmState.h"
#include "DefrostType.h"
#include "AluminumMoldIceMakerHsmState.h"
#include "AluminumMoldIceMakerMotorVotedState.h"
#include "WaterValveVotedState.h"
#include "FeelerArmPosition.h"
#include "GridOffsetAdder.h"
#include "RakePosition.h"
#include "IceMakerMoldHeaterControlRequest.h"
#include "PercentageDutyCycleVote.h"
#include "DispenseSelection.h"
#include "ReadyToDefrostHsmState.h"
#include "PwmFrequency.h"
#include "TwistTrayIceMakerMotorActionResult.h"
#include "TwistTrayIceMakerMotorAction.h"
#include "TwistTrayIceMakerMotorDoAction.h"
#include "TwistTrayIceMakerMotorErrorReason.h"
#include "TwistTrayIceMakerMotorOperationState.h"
#include "TwistTrayIceMakerOperationState.h"
#include "TwistTrayIceMakerHighLevelState.h"
#include "DispensingRequest.h"
#include "DispensingRequestHandler.h"
#include "DispenseStatus.h"
#include "AugerMotorVotedIceType.h"
#include "RelativeHumidityPercentx100.h"
#include "IceMakerTestRequest.h"
#include "AugerMotorVotedIceType.h"
#include "AugerMotorControllerFsmState.h"
#include "PwmVotedDutyCycle.h"
#include "FlowMeterMonitoringRequest.h"
#include "IceMakerWaterFillMonitoringRequest.h"
#include "ShortGitHash.h"
#include "ApplianceFeatureApi.h"
#include "WaterFilterState.h"
#include "WaterFilterType.h"
#include "IceMakerTypeInformation.h"
#include "AndroidUiSignOfLifeFsmState.h"
#include "I_OneWire.h"
#include "DispensingInhibitedBitmap.h"
#include "ControlLockStatus.h"
#include "BooleanVotedState.h"
#include "RampingPwmDutyCycle.h"
#include "IceMakerEnableRequest.h"
#include "TurboModeRequest.h"
#include "CoolingSystemRequest.h"
#include "DoorAlarmEnableRequest.h"
#include "RampingPwmDutyCyclePercentageVote.h"
#include "EnhancedSabbathModeHsmState.h"
#include "DimmableLightConfiguration.h"
#include "TemperatureDisplayUnit.h"
#include "RefrigeratorModel.h"
#include "SnackModeEnableRequest.h"
#include "PresenceSensingEnableRequest.h"
#include "PresenceActivatesRecessLightEnableRequest.h"
#include "RfidTypes.h"

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
#define INCLUDE_RAM_PersonalityEeprom(_x)

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
#define INCLUDE_VIRTUAL_PersonalityEeprom(_x)

#define INCLUDE_NV_Ram(_x)
#define INCLUDE_NV_Virtual(_x)
#define INCLUDE_NV_Nv(_x) _x
#define INCLUDE_NV_Fault(_x)
#define INCLUDE_NV_Bsp(_x)
#define INCLUDE_NV_MappedBsp(_x)
#define INCLUDE_NV_PersonalityEeprom(_x)

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
#define INCLUDE_FAULT_PersonalityEeprom(_x)

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
#define INCLUDE_BSP_PersonalityEeprom(_x)

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
#define INCLUDE_NVONLY_PersonalityEeprom(_x)

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
#define INCLUDE_NVALL_PersonalityEeprom(_x) _x

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
#define INCLUDE_NVPROTECTED_PersonalityEeprom(_x)

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
#define INCLUDE_NVUNITSETTING_PersonalityEeprom(_x)

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
#define INCLUDE_NVUSERSETTING_PersonalityEeprom(_x)

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
#define INCLUDE_NVRFID_PersonalityEeprom(_x)

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
#define INCLUDE_NVFAULTSNAPSHOT_PersonalityEeprom(_x)

#define INCLUDE_PERSONALITYEEPROM_Ram(_x)
#define INCLUDE_PERSONALITYEEPROM_Virtual(_x)
#define INCLUDE_PERSONALITYEEPROM_Nv(_x) _x
#define INCLUDE_PERSONALITYEEPROM_NvProtected(_x)
#define INCLUDE_PERSONALITYEEPROM_NvUnitSetting(_x)
#define INCLUDE_PERSONALITYEEPROM_NvUserSetting(_x)
#define INCLUDE_PERSONALITYEEPROM_NvRfid(_x)
#define INCLUDE_PERSONALITYEEPROM_NvFaultSnapshot(_x)
#define INCLUDE_PERSONALITYEEPROM_NvCycleHistory(_x)
#define INCLUDE_PERSONALITYEEPROM_Fault(_x)
#define INCLUDE_PERSONALITYEEPROM_Bsp(_x)
#define INCLUDE_PERSONALITYEEPROM_MappedBsp(_x)
#define INCLUDE_PERSONALITYEEPROM_PersonalityEeprom(_x) _x

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
#define INCLUDE_NVCYCLEHISTORY_PersonalityEeprom(_x)

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
#define INCLUDE_NON_RAM_PersonalityEeprom(_x) _x

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
#define INCLUDE_MAPPED_BSP_PersonalityEeprom(_x)

#define INCLUDE_BSP_ALL_Ram(_x)
#define INCLUDE_BSP_ALL_Virtual(_x)
#define INCLUDE_BSP_ALL_Nv(_x)
#define INCLUDE_BSP_ALL_NvProtected(_x)
#define INCLUDE_BSP_ALL_NvUnitSetting(_x)
#define INCLUDE_BSP_ALL_NvUserSetting(_x)
#define INCLUDE_BSP_ALL_NvRfid(_x)
#define INCLUDE_BSP_ALL_NvFaultSnapshot(_x)
#define INCLUDE_BSP_ALL_NvCycleHistory(_x)
#define INCLUDE_BSP_ALL_Fault(_x)
#define INCLUDE_BSP_ALL_Bsp(_x) _x
#define INCLUDE_BSP_ALL_MappedBsp(_x) _x
#define INCLUDE_BSP_ALL_PersonalityEeprom(_x)

#define INCLUDE_INPUT_OUTPUT_Io_None(_x)
#define INCLUDE_INPUT_OUTPUT_Io_O(_x)
#define INCLUDE_INPUT_OUTPUT_Io_All(_x) _x

#define INCLUDE_OUTPUT_Io_None(_x)
#define INCLUDE_OUTPUT_Io_O(_x) _x
#define INCLUDE_OUTPUT_Io_All(_x) _x

#define INCLUDE_SUBSCRIPTION_Sub_N(_x)
#define INCLUDE_SUBSCRIPTION_Sub_Y(_x) _x

#define INCLUDE_SWAP_Swap_No(_x)
#define INCLUDE_SWAP_Swap_Yes(_x) _x
#define INCLUDE_SWAP_Swap_Range(_x)
#define INCLUDE_SWAP_Swap_Range_S(_x) _x
#define INCLUDE_SWAP_Swap_Range_E(_x)

#define INCLUDE_SWAP_RANGE_START_Swap_No(_x)
#define INCLUDE_SWAP_RANGE_START_Swap_Yes(_x)
#define INCLUDE_SWAP_RANGE_START_Swap_Range(_x)
#define INCLUDE_SWAP_RANGE_START_Swap_Range_S(_x) _x
#define INCLUDE_SWAP_RANGE_START_Swap_Range_E(_x)

#define INCLUDE_SWAP_RANGE_END_Swap_No(_x)
#define INCLUDE_SWAP_RANGE_END_Swap_Yes(_x)
#define INCLUDE_SWAP_RANGE_END_Swap_Range(_x)
#define INCLUDE_SWAP_RANGE_END_Swap_Range_S(_x)
#define INCLUDE_SWAP_RANGE_END_Swap_Range_E(_x) _x

enum
{
   RamErdBase = 0xFD00
};

//       Name,                                              Number, DataType,                                           Swap,   Io,      Sub,   StorageType, NvDefaultData,                         FaultId
#define ERD_TABLE(ENTRY)  \
   ENTRY(Erd_NvMetadata,                                    0x0000, AsyncDataSource_EepromMetadata_t,                   Swap_No,  Io_None, Sub_N, Nv,                     NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ModelNumber,                                   0x0001, ModelNumber_t,                                      Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_SerialNumber,                                  0x0002, SerialNumber_t,                                     Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ControlLockStatus,                             0x0004, ControlLockStatus_t,                                Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_UnlockedControlLockStatus, NotFault) \
   ENTRY(Erd_TemperatureDisplayUnits,                       0x0007, TemperatureDisplayUnit_t,                           Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ApplianceType,                                 0x0008, ApplianceType_t,                                    Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_SabbathMode,                                   0x0009, bool,                                               Swap_No,  Io_All,  Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ReadyToEnterBootLoader,                        0x0030, ReadyToEnterBootLoaderState_t,                      Swap_No,  Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_BuildNumber,                                   0x0031, uint32_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Reset,                                         0x0032, uint8_t,                                            Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_AppliancePersonality,                          0x0035, AppliancePersonality_t,                             Swap_Yes, Io_None, Sub_N, PersonalityEeprom,      NonVolatileDataSourceDefaultData_DefaultPersonality, NotFault) \
   ENTRY(Erd_SupportedImageTypes,                           0x0038, uint8_t,                                            Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_BootLoaderVersion,                             0x0039, Version_t,                                          Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ApplicationVersion,                            0x003A, Version_t,                                          Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ParametricVersion,                             0x003B, Version_t,                                          Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_AuxiliaryVersion,                              0x003C, Version_t,                                          Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_EnhancedSabbathModeSupported,                  0x004E, bool,                                               Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_EnhancedSabbathModeStatus,                     0x004F, bool,                                               Swap_No,  Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   ENTRY(Erd_GitHash,                                       0x007F, GitHash_t,                                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsApiVersion,                  0x008F, ServiceDiagnosticsApiVersion_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsRevision3Manifest,           0x0090, ServiceDiagnosticsRevision3Manifest_t,              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ApplianceApiManifest,                          0x0092, ApplianceApiManifest_t,                             Swap_Yes, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ApplianceApiApplianceTypeManifest1,            0x0093, ApplianceFeatureApiManifest_t,                      Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ApplianceApiApplianceTypeManifest2,            0x0094, ApplianceFeatureApiManifest_t,                      Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ResetCount,                                    0x0700, ResetCount_t,                                       Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ResetReason,                                   0x0701, ResetReason_t,                                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SecondsSinceLastReset,                         0x0702, SecondsSinceLastReset_t,                            Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ProgramCounterAddressAtLastUassert,            0x0703, ProgramCounterAddress_t,                            Swap_Yes, Io_O,    Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_PrimaryUiSignOfLife,                           0x0900, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_SoundLevelStatus,                              0x0903, uint8_t,                                            Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NumberOfSoundLevelSettings,                    0x0904, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_WaterFilterType,                               0x1019, WaterFilterType_t,                                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_RefrigeratorModel,                             0x101D, RefrigeratorModel_t,                                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CumulativeWaterDispensedOunces,                0x1060, uint32_t,                                           Swap_Yes, Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,  NotFault) \
   \
   ENTRY(Erd_FreshFoodSetpointRequest,                      0x1200, Setpoint_t,                                         Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpointRangeData,                    0x1201, UserSetpointRangeData_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpointStatus,                       0x1202, Setpoint_t,                                         Swap_No,  Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_Int8Max, NotFault) \
   ENTRY(Erd_FreezerSetpointRequest,                        0x1203, Setpoint_t,                                         Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpointRangeData,                      0x1204, UserSetpointRangeData_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpointStatus,                         0x1205, Setpoint_t,                                         Swap_No,  Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_Int8Max, NotFault) \
   ENTRY(Erd_DispenseSelectionRequest,                      0x1206, DispenseSelection_t,                                Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DispenseSelectionStatus,                       0x1207, DispenseSelection_t,                                Swap_No,  Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_DispenseSelection, NotFault) \
   ENTRY(Erd_DispensingRequest,                             0x1208, DispensingRequest_t,                                Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DispensingRequestStatus,                       0x1209, DispensingRequestStatus_t,                          Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CoolingOffRequest,                             0x120A, CoolingSystemRequest_t,                             Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CoolingOffStatus,                              0x120B, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_TurboCoolOnOffRequest,                         0x120C, TurboModeRequest_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TurboCoolOnOffStatus,                          0x120D, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_RightSideFreshFoodDoorStatusResolved,          0x120E, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_LeftSideFreezerDoorStatusResolved,             0x120F, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMaker0EnableRequest,                        0x1210, IceMakerEnableRequest_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMaker0EnableStatus,                         0x1211, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,       NotFault) \
   ENTRY(Erd_IceMaker0FullStatus,                           0x1212, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMaker0TypeInformation,                      0x1213, IceMakerTypeInformation_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_WaterFilterState,                              0x1215, WaterFilterState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMaker1EnableRequest,                        0x1216, IceMakerEnableRequest_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMaker1EnableStatus,                         0x1217, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,       NotFault) \
   ENTRY(Erd_IceMaker1FullStatus,                           0x1218, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMaker1TypeInformation,                      0x1219, IceMakerTypeInformation_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TurboFreezeOnOffRequest,                       0x121A, TurboModeRequest_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TurboFreezeOnOffStatus,                        0x121B, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DimmableLight0PercentLevelRequest,             0x121E, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DimmableLight0PercentLevelStatus,              0x121F, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_MaxLightBrightness, NotFault) \
   ENTRY(Erd_DimmableLight0Configuration,                   0x1220, UserAllowableInteriorLightingBitmap_t,              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_PresenceSensingEnableRequest,                  0x1222, PresenceSensingEnableRequest_t,                     Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PresenceSensingEnableStatus,                   0x1223, bool,                                               Swap_No,  Io_None, Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,       NotFault) \
   ENTRY(Erd_PresenceSensedActivatesRecessLightRequest,     0x1224, PresenceActivatesRecessLightEnableRequest_t,        Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PresenceSensedActivatesRecessLightStatus,      0x1225, bool,                                               Swap_No,  Io_None, Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,       NotFault) \
   \
   ENTRY(Erd_PresenceDetected,                              0x1226, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                              NotFault) \
   ENTRY(Erd_DoorAlarmEnableRequest,                        0x1227, DoorAlarmEnableRequest_t,                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                              NotFault) \
   ENTRY(Erd_DoorAlarmEnableStatus,                         0x1228, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,       NotFault) \
   ENTRY(Erd_NightTimeSnackModeEnableRequest,               0x1229, SnackModeEnableRequest_t,                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                              NotFault) \
   ENTRY(Erd_NightTimeSnackModeEnableStatus,                0x122A, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,      NotFault) \
   \
   ENTRY(Erd_DispenserRecessHeaterDutyCyclePercent,         0x122C, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SecondarySignOfLife,                           0x122D, uint8_t,                                            Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DispensingInhibited,                           0x122F, DispensingInhibitedBitmap_t,                        Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMaker2EnableRequest,                        0x1230, IceMakerEnableRequest_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMaker2EnableStatus,                         0x1231, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,       NotFault) \
   ENTRY(Erd_IceMaker2FullStatus,                           0x1232, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMaker2TypeInformation,                      0x1233, IceMakerTypeInformation_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsEntityLocation,              0xE000, Erd_t,                                              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsEntityManifest,              0xE100, ServiceDiagnosticsRevision3EntityManifest_t,        Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsEntityCycleCount,            0xE101, uint32_t,                                           Swap_Yes,   Io_All,  Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsRunTimeInMinutes,            0xE102, ApplianceRunTimeMinutes_t,                          Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableStatus,            0xE103, ServiceDiagnosticsRevision3TableStatus_t,           Swap_Yes,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableClear,             0xE104, ServiceDiagnosticsRevision3TableClearRequest_t,     Swap_No,    Io_O,    Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry0,            0xE105, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range_S, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry1,            0xE106, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry2,            0xE107, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry3,            0xE108, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry4,            0xE109, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry5,            0xE10A, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry6,            0xE10B, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry7,            0xE10C, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry8,            0xE10D, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry9,            0xE10E, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range_E, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot0,         0xE10F, FaultSnapshot_t,                                    Swap_Range_S, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot1,         0xE110, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot2,         0xE111, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot3,         0xE112, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot4,         0xE113, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot5,         0xE114, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot6,         0xE115, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot7,         0xE116, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot8,         0xE117, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot9,         0xE118, FaultSnapshot_t,                                    Swap_Range_E, Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceStatus,         0xE119, ServiceDiagnosticsRevision3SequenceStatus_t,        Swap_Yes,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceClear,          0xE11A, ServiceDiagnosticsRevision3SequenceClearRequest_t,  Swap_No,    Io_O,    Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry0,         0xE11B, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range_S, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry1,         0xE11C, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry2,         0xE11D, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry3,         0xE11E, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry4,         0xE11F, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry5,         0xE120, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry6,         0xE121, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry7,         0xE122, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range_E, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistorySequenceStatus,  0xE123, ServiceDiagnosticsRevision3SequenceStatus_t,        Swap_Yes,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistorySequenceClear,   0xE124, ServiceDiagnosticsRevision3SequenceClearRequest_t,  Swap_No,    Io_O,    Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord0,                0xE125, CycleHistoryRecord_t,                               Swap_Range_S, Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord1,                0xE126, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord2,                0xE127, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord3,                0xE128, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord4,                0xE129, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord5,                0xE12A, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord6,                0xE12B, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord7,                0xE12C, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord8,                0xE12D, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord9,                0xE12E, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistoryAddEntry,        0xE12F, CycleHistoryRecord_t,                               Swap_Range_E, Io_O,    Sub_N, Virtual,                NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_OldApplicationVersion,                         0xF000, Version_t,                                          Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimerModuleDiagnosticsEnable,                  0xF001, bool,                                               Swap_No,    Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimerModuleDiagnosticsResult,                  0xF002, TimerModuleDiagnosticsResults_t,                    Swap_Yes,   Io_O,    Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ParametricDataImageCrc,                        0xF003, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SomeData,                                      0xF004, uint32_t,                                           Swap_Yes,   Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Code,    NotFault) \
   ENTRY(Erd_ApplianceRunTimeInMinutesUpdatedHourly,        0xF005, ApplianceRunTimeMinutes_t,                          Swap_Yes,   Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_StackPercentFree,                              0xF006, uint8_t,                                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ParametricShortGitHash,                        0xF007, ShortGitHash_t,                                     Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_HeartbeatTick,                                 0xF008, uint8_t,                                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PersonalityParametricData,                     0xF009, PersonalityParametricData_t *,                      Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_NonVolatileDataSourceCacheSyncState,           0xF00A, bool,                                               Swap_No,    Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   ENTRY(Erd_TimeAcceleration_Enable,                       0xF00B, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimeAcceleration_Ticks,                        0xF00C, uint32_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimeAcceleration_CompleteSignal,               0xF00D, Signal_t,                                           Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_SystemTickInterrupt,                           0xF010, I_Interrupt_t *,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimeSource,                                    0xF011, I_TimeSource_t *,                                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimerModule,                                   0xF012, TimerModule_t *,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CrcCalcTable,                                  0xF013, I_Crc16Calculator_t *,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gea2MessageEndpoint,                           0xF014, I_Gea2MessageEndpoint_t *,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_JumpToBootLoaderAction,                        0xF015, I_Action_t *,                                       Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SystemResetAction,                             0xF016, I_Action_t *,                                       Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ExternalDataSource,                            0xF017, I_DataSource_t *,                                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FaultCodeTableInputGroup,                      0xF018, I_InputGroup_t *,                                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_WifiBufferedUart,                              0xF019, I_BufferedUart_t *,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FactoryBufferedUart,                           0xF01A, I_BufferedUart_t *,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DoorBufferedUart,                              0xF01B, I_BufferedUart_t *,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CaseBufferedUart,                              0xF01C, I_BufferedUart_t *,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gea2Uart,                                      0xF01D, I_Uart_t *,                                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Grid_BlockNumber,                              0xF030, GridBlockNumber_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Grid_PreviousBlocks,                           0xF031, PreviousGridBlockNumbers_t,                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Grid_CalculatedGridLines,                      0xF032, CalculatedGridLines_t,                              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridArea,                                      0xF034, GridArea_t,                                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridOverrideSignal,                            0xF035, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridOverrideEnable,                            0xF036, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridBlockNumberOverrideRequest,                0xF037, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridBlockNumberOverrideValue,                  0xF038, GridBlockNumber_t,                                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridBlockNumberOverrideResolved,               0xF039, GridBlockNumber_t,                                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentGridBlockNumber,         0xF03A, GridBlockNumber_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SingleEvaporatorPulldownActive,                0xF03B, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorTripMitigationActive,                0xF03C, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanAntiSweatBehaviorEnabledByGrid,    0xF03D, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanAntiSweatBehaviorEnabledByGrid,  0xF03E, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DelayConvertibleCompartmentCooling,            0xF03F, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CoolConvertibleCompartmentBeforeOff,           0xF040, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DelayedConvertibleCompartmentCoolingLowSpeed,  0xF041, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodPulldownOffsetEnabled,                0xF042, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_MaxValveTimeInPosAEnabled,                     0xF043, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CoolingSpeed,                                  0xF044, CoolingSpeed_t,                                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CoolingMode,                                   0xF045, CoolingMode_t,                                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PulldownInMediumCompressorSpeedEnabled,        0xF046, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimeInMinutesInValvePositionB,                 0xF047, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CrossAmbientHysteresisAdjustmentInDegFx100,    0xF048, TemperatureDegFx100_t,                              Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_EnhancedSabbathModeHsmState,                   0xF049, EnhancedSabbathModeHsmState_t,                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DamperFreezePreventionFsmState,                0xF04A, DamperFreezePreventionFsmState_t,                   Swap_No,  Io_None, Sub_N, Ram,                   NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentState,                   0xF04B, ConvertibleCompartmentStateType_t,                  Swap_No,  Io_None, Sub_Y, Ram,                   NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorControllerSpeedRequest,              0xF04C, CompressorSpeed_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                   NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorState,                               0xF04D, CompressorState_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                   NotNv,                                    NotFault) \
   ENTRY(Erd_HasConvertibleCompartment,                     0xF04E, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                   NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DefrostState,                                  0xF060, DefrostState_t,                                     Swap_No,    Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_DefrostHsmState,                               0xF061, DefrostHsmState_t,                                  Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_WaitingToDefrost,                              0xF062, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Defrosting,                                    0xF063, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CurrentDefrostType,                            0xF064, DefrostType_t,                                      Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_NextDefrostType,                               0xF065, DefrostType_t,                                      Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,0xF066, uint8_t,                                            Swap_No,    Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_FreezerDefrostCycleCount,                      0xF067, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_FreshFoodDefrostCount,                         0xF068, uint8_t,                                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimeBetweenDefrostsInMinutes,                  0xF069, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_WaitingForDefrostTimeInSeconds,                0xF06B, uint32_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostCompressorOnTimeInSeconds,              0xF06C, uint32_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds,  0xF06D, uint32_t,                                       Swap_Range_S, Io_None, Sub_N, Ram,                  NotNv,                                    NotFault) \
   ENTRY(Erd_RightSideFreshFoodScaledDoorAccelerationInSeconds, 0xF06E, uint32_t,                                       Swap_Range,   Io_None, Sub_N, Ram,                  NotNv,                                    NotFault) \
   ENTRY(Erd_DoorInDoorScaledDoorAccelerationInSeconds,         0xF06F, uint32_t,                                       Swap_Range,   Io_None, Sub_N, Ram,                  NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerScaledDoorAccelerationInSeconds,            0xF070, uint32_t,                                       Swap_Range,   Io_None, Sub_N, Ram,                  NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds, 0xF071, uint32_t,                        Swap_Range,   Io_None, Sub_N, Ram,                  NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds,   0xF072, uint32_t,                        Swap_Range_E, Io_None, Sub_N, Ram,                  NotNv,                                    NotFault) \
   ENTRY(Erd_ReadyToDefrost,                                0xF073, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ReadyToDefrostHsmState,                        0xF074, ReadyToDefrostHsmState_t,                           Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TimeThatPrechillConditionsAreMetInMinutes,     0xF075, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_MaxPrechillTimeInMinutes,                      0xF076, uint8_t,                                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeaterOnTimeInMinutes,           0xF077, uint8_t,                                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,        0xF078, uint8_t,                                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,      0xF079, uint8_t,                                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes, 0xF07A, uint8_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostWasAbnormal,                     0xF07B, bool,                                               Swap_No,    Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NumberOfFreezerAbnormalDefrostCycles,          0xF07C, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_FreshFoodDefrostWasAbnormal,                   0xF07D, bool,                                               Swap_No,    Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostWasAbnormal,      0xF07E, bool,                                               Swap_No,    Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_PostDwellExitTimeInMinutes,                    0xF07F, uint8_t,                                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PostDwellCompletionSignal,                     0xF080, Signal_t,                                           Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_InvalidFreezerEvaporatorThermistorDuringDefrost,0xF081,bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,    0xF082, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorIsOn,                                0xF084, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DisableDefrost,                                0xF085, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostTestRequest,                            0xF086, DefrostTestRequest_t,                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostTestStateRequest,                       0xF087, DefrostTestStateRequestMessage_t,                   Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostTestRequestStatus,                      0xF088, DefrostTestRequest_t,                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DontSkipDefrostPrechill,                       0xF089, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_UseMinimumReadyToDefrostTimeAndResetDefrostCounts, 0xF08A, bool,                                           Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SabbathIsReadyToDefrost,                       0xF08B, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SabbathTimeBetweenDefrostsInMinutes,           0xF08C, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SabbathWaitingForDefrostTimeInMinutes,         0xF08D, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_LeftSideFreshFoodDoorIsOpen,                   0xF0A0, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DoorInDoorIsOpen,                              0xF0A1, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDoorIsOpen,              0xF0A2, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentAsFreshFoodDoorIsOpen,   0xF0A3, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentAsFreezerDoorIsOpen,     0xF0A4, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AllFreshFoodDoorsAreClosed,                    0xF0A5, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_LeftSideFreshFoodDoorIsOpenResolved,           0xF0A6, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DoorInDoorIsOpenResolved,                      0xF0A7, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_RightSideFreshFoodDoorStatus,                  0xF0A8, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_LeftSideFreezerDoorStatus,                     0xF0A9, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   \
   ENTRY(Erd_TwistTrayIceMakerThermistor_AdcCount,          0xF0B0, AdcCounts_t,                                        Swap_Range_S, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerMoldThermistor_AdcCount,   0xF0B1, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_IceMakerBoxThermistor_AdcCount,                0xF0B2, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodThermistor_AdcCount,                  0xF0B3, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerThermistor_AdcCount,                    0xF0B4, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_AdcCount,              0xF0B5, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_AdcCount,                0xF0B6, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_AdcCount, 0xF0B7, AdcCounts_t,                                     Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientThermistor_AdcCount,                    0xF0B8, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientHumidity_AdcCount,                      0xF0B9, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_AdcCount, 0xF0BA, AdcCounts_t,                                        Swap_Range_E, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_UnfilteredTemperatureInDegFx100,                  0xF0C0, TemperatureDegFx100_t,                 Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_UnfilteredTemperatureInDegFx100,                    0xF0C1, TemperatureDegFx100_t,                 Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvap_UnfilteredTemperatureInDegFx100,                0xF0C2, TemperatureDegFx100_t,                 Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_UnfilteredTemperatureInDegFx100,                    0xF0C3, TemperatureDegFx100_t,                 Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerMold_UnfilteredTemperatureInDegFx100,   0xF0C4, TemperatureDegFx100_t,                 Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_UnfilteredTemperatureInDegFx100,          0xF0C5, TemperatureDegFx100_t,                 Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientHumidity_UnfilteredRelativeHumidityPercentx100,      0xF0C6, RelativeHumidityPercentx100_t,         Swap_Yes,     Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_FilteredTemperatureInDegFx100,                    0xF0D0, TemperatureDegFx100_t,                 Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperatureInDegFx100,                      0xF0D1, TemperatureDegFx100_t,                 Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureInDegFx100,                  0xF0D2, TemperatureDegFx100_t,                 Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalTemperatureInDegFx100,              0xF0D3, TemperatureDegFx100_t,                 Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerMold_FilteredTemperatureInDegFx100,     0xF0D4, TemperatureDegFx100_t,                 Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100,            0xF0D5, TemperatureDegFx100_t,                 Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientHumidity_FilteredInternalRelativeHumidityPercentx100, 0xF0D6, RelativeHumidityPercentx100_t,        Swap_Yes,     Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Ambient_WindowAveragedTemperatureInDegFx100,   0xF0E0, TemperatureDegFx100_t,                              Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_LongTermAverageInDegFx100,           0xF0E1, TemperatureDegFx100_t,                              Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_LongTermAverageInDegFx100,             0xF0E2, TemperatureDegFx100_t,                              Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_ResetThermalShiftOffsetSignal,       0xF0E7, Signal_t,                                           Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_ResetThermalShiftOffsetSignal,         0xF0E8, Signal_t,                                           Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerSetpoint_TemperatureBounds,             0xF0ED, SetpointZoneTemperatureBounds_t,                    Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_TemperatureBounds,           0xF0EE, SetpointZoneTemperatureBounds_t,                    Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_TemperatureBounds, 0xF0EF, SetpointZoneTemperatureBounds_t,                 Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerEvaporatorThermistorIsValid,            0xF0F0, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvaporatorThermistorIsValid,          0xF0F1, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvaporatorThermistorIsValid, 0xF0F2, bool,                                           Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_ThermistorIsValid,                     0xF0F3, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_ThermistorIsValid,                   0xF0F4, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_ThermistorIsValid,                     0xF0F5, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerMoldThermistorIsValid,     0xF0F6, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_ThermistorIsValid,           0xF0F7, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_HumiditySensorIsValid,                 0xF0F8, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_AdjustedSetpointInDegFx100,             0xF0FF, TemperatureDegFx100_t,                           Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_AdjustedSetpointInDegFx100,               0xF100, TemperatureDegFx100_t,                           Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100, 0xF101, TemperatureDegFx100_t,                           Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_AdjustedSetpointWithoutShiftInDegFx100,   0xF102, TemperatureDegFx100_t,                           Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_ResolvedSetpointInDegFx100,          0xF105, TemperatureDegFx100_t,                              Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_CabinetOffsetInDegFx100,             0xF106, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_SetpointOffsetInDegFx100,            0xF107, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_CrossAmbientOffsetInDegFx100,        0xF108, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_PulldownOffsetInDegFx100,            0xF109, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_ThermalShiftInDegFx100,              0xF10A, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100, 0xF10B, TemperatureDegFx100_t,                          Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_HighAmbientOffsetInDegFx100,         0xF10C, TemperatureDegFx100_t,                              Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Freezer_ResolvedSetpointInDegFx100,            0xF110, TemperatureDegFx100_t,                              Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_CabinetOffsetInDegFx100,               0xF111, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_CrossAmbientOffsetInDegFx100,          0xF112, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_ThermalShiftInDegFx100,                0xF113, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100, 0xF114, TemperatureDegFx100_t,                            Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_HighAmbientOffsetInDegFx100,           0xF115, TemperatureDegFx100_t,                              Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerSetpointZone,                           0xF120, SetpointZone_t,                                     Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpointZone,                         0xF121, SetpointZone_t,                                     Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TurboCoolOnTimeInMinutes,                      0xF122, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TurboFreezeOnTimeInMinutes,                    0xF123, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Freezer_IceRateIsActive,                       0xF12A, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerIceRateTriggerSignal,                   0xF12B, Signal_t,                                           Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_IceMakerEnabledByGrid,                         0xF12D, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMakerEnabledResolved,                       0xF12E, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMakerEnabledOverrideRequest,                0xF12F, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMakerEnabledOverrideValue,                  0xF130, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMakerEnabledEnhancedSabbathOverrideRequest, 0xF131, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceMakerEnabledEnhancedSabbathOverrideValue,   0xF132, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FeelerArmMonitoringRequest,                    0xF133, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FeelerArmIsReadyToEnterHarvest,                0xF134, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100, 0xF135, TemperatureDegFx100_t,       Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_AveragedTemperatureOverrideRequest,                         0xF137, bool,                        Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Freezer_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100,   0xF138, TemperatureDegFx100_t,       Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_AveragedTemperatureOverrideRequest,                           0xF139, bool,                        Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_TwistTrayIceMaker_MotorActionResult,                      0xF140, TwistTrayIceMakerMotorActionResult_t,    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_OperationState,                         0xF141, TwistTrayIceMakerOperationState_t,       Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_HighLevelState,                         0xF142, TwistTrayIceMakerHighLevelState_t,       Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_FreezeIntegrationCount,                 0xF143, uint32_t,                                Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_MinimumFreezeTimeCounterInMinutes,      0xF144, uint8_t,                                 Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_MotorFaultActive,                       0xF145, bool,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerTestRequest,                             0xF146, IceMakerTestRequest_t,                   Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_MotorOperationState,                    0xF147, TwistTrayIceMakerMotorOperationState_t,  Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_MotorErrorReason,                       0xF148, TwistTrayIceMakerMotorErrorReason_t,     Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerWaterFillMonitoringRequest,              0xF149, IceMakerWaterFillMonitoringRequest_t,    Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerStopFillSignal,                          0xF14A, Signal_t,                                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerFlowMeterWaterDispensedOzX100,           0xF14B, uint32_t,                                Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerFlowMeterMonitoringRequest,              0xF14C, FlowMeterMonitoringRequest_t,            Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_HarvestCountCalculationRequest,         0xF14D, bool,                                    Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_HarvestCountIsReadyToHarvest,           0xF14E, bool,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_MotorDoAction,                          0xF14F, TwistTrayIceMakerMotorDoAction_t,        Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AluminumMoldIceMakerHsmState,                  0xF150, AluminumMoldIceMakerHsmState_t,                     Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerFeelerArmPosition,         0xF151, FeelerArmPosition_t,                                Swap_No,    Io_None, Sub_Y, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerMoldHeaterControlRequest,  0xF152, IceMakerMoldHeaterControlRequest_t,                 Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerRakeCompletedRevolution,   0xF153, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerSkipFillRequest,           0xF154, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerRakeControlRequest,        0xF155, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerRakePosition,              0xF156, RakePosition_t,                                     Swap_No,    Io_None, Sub_Y, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerRakeHasNotBeenHome,        0xF157, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerFeelerArmHasBeenBucketFull,0xF158, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerWaterFillMonitoringRequest,0xF159, IceMakerWaterFillMonitoringRequest_t,               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerStopFillSignal,            0xF15A, Signal_t,                                           Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldFreezeIntegrationCount,            0xF15B, uint32_t,                                           Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerMinimumFreezeTimeCounterInMinutes,  0xF15C, uint8_t,                                   Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerTestRequest,               0xF15D, IceMakerTestRequest_t,                              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMaker_HarvestCountCalculationRequest, 0xF15E, bool,                                         Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMaker_HarvestCountIsReadyToHarvest,   0xF15F, bool,                                         Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FlowMeterWaterDispensedOzX100,                 0xF160, uint32_t,                                           Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FlowMeterMonitoringRequest,                    0xF161, FlowMeterMonitoringRequest_t,                       Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AugerMotorControllerFsmState,                  0xF162, AugerMotorControllerFsmState_t,                     Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PrivateDispensingRequest,                      0xF163, DispensingRequest_t,                                Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PrivateDispensingResultStatus,                 0xF164, DispenseStatus_t,                                   Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AutofillSensorError,                           0xF168, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DispensingDisabled,                            0xF169, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_RfidFilterReadWriteResult_RfidBoard,           0xF181, RfidFilterReadWriteResult_t,                        Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterUid_RfidBoard,                       0xF182, RfidUid_t,                                          Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterIdentifier_RfidBoard,                0xF183, RfidFilterIdentifier_t,                             Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterWaterVolumeUsageInOunces_RfidBoard,  0xF184, VolumeInOunces_t,                                   Swap_Yes, Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterCalendarUsageInSeconds_RfidBoard,    0xF185, CalendarUsageInSeconds_t,                           Swap_Yes, Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterLeakDetectedTimeInSeconds_RfidBoard, 0xF186, LeakDetectedInSeconds_t,                            Swap_Yes, Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterNumberOfWriteOperations_RfidBoard,   0xF187, NumberOfWriteOperations_t,                          Swap_Yes, Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterStatus_RfidBoard,                    0xF188, FilterStatusRfidBoard_t,                            Swap_Yes, Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterInitializationErrorBitfield_RfidBoard,       0xF189, RfidFilterInitializationErrorBitfield_t,    Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterLockByte_RfidBoard,                          0xF18A, uint8_t,                                    Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_RfidBoard,  0xF18B, RfidFilterNumberOfUnitsFilterHasBeenOn_t,   Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterUnitSerialNumber_RfidBoard,                  0xF18C, UnitSerialNumber_t,                         Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterPreviousUnitSerialNumber_RfidBoard,          0xF18D, UnitSerialNumber_t,                         Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_RfidBoard, 0xF18E, RfidTwelveMonthsGallonUsage_t,          Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   \
   ENTRY(Erd_RfidFilterUid_Mainboard,                       0xF18F, RfidUid_t,                                          Swap_No,  Io_None, Sub_Y, NvUnitSetting,            NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_RfidFilterWaterVolumeUsageInOunces_Mainboard,  0xF190, VolumeInOunces_t,                                   Swap_Yes, Io_None, Sub_Y, NvUnitSetting,            NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_RfidFilterCalendarUsageInSeconds_Mainboard,    0xF191, CalendarUsageInSeconds_t,                           Swap_Yes, Io_None, Sub_Y, NvUnitSetting,            NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_RfidFilterStatus_Mainboard,                    0xF192, RfidFilterStatusMainboard_t,                        Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_Mainboard,  0xF193, RfidFilterNumberOfUnitsFilterHasBeenOn_t,   Swap_No,  Io_None, Sub_Y, NvUnitSetting,            NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_RfidFilterLockByte_Mainboard,                  0xF194, uint8_t,                                            Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterUnitSerialNumber_Mainboard,                  0xF195, UnitSerialNumber_t,                         Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_RfidFilterPreviousUnitSerialNumber_Mainboard,          0xF196, UnitSerialNumber_t,                         Swap_No,  Io_None, Sub_Y, NvUnitSetting,            NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_Mainboard, 0xF197, RfidTwelveMonthsGallonUsage_t,          Swap_No,  Io_None, Sub_Y, NvUnitSetting,            NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_RfidFilterDataRequest,                         0xF198, ReadWriteRequest_t,                                 Swap_No,  Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CondenserFanSpeed_ResolvedVote,                0xF200, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_WinningVoteErd,              0xF201, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_FactoryVote,                 0xF202, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_CoolingSystemOffVote,        0xF203, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_DefrostVote,                 0xF204, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_SabbathVote,                 0xF205, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_CompressorStartUpVote,       0xF206, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CondenserFanSpeed_GridVote,                    0xF207, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_IceCabinetFanSpeed_ResolvedVote,               0xF210, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_WinningVoteErd,             0xF211, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_FactoryVote,                0xF212, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_LoadOffDoorOpenVote,        0xF213, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_DefrostVote,                0xF214, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodEvapFanSpeed_ResolvedVote,            0xF21A, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_WinningVoteErd,          0xF21B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_FactoryVote,             0xF21C, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_CoolingSystemOffVote,    0xF21D, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_LoadOffDoorOpenVote,     0xF21E, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_DefrostVote,             0xF21F, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_SabbathVote,             0xF220, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote,   0xF221, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_GridVote,                0xF222, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerEvapFanSpeed_ResolvedVote,              0xF22A, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_WinningVoteErd,            0xF22B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_FactoryVote,               0xF22C, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_CoolingSystemOffVote,      0xF22D, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_LoadOffDoorOpenVote,       0xF22E, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_DefrostVote,               0xF22F, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_SabbathVote,               0xF230, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_CompressorStartUpVote,     0xF231, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_FreezerIceRateVote,        0xF232, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_GridVote,                  0xF233, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_ResolvedVote,   0xF23A, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_WinningVoteErd, 0xF23B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_FactoryVote,    0xF23C, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_LoadOffDoorOpenVote, 0xF23D, FanVotedSpeed_t,                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_DefrostVote,    0xF23E, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFanSpeed_GridVote,       0xF23F, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DeliFanSpeed_ResolvedVote,                     0xF24A, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFanSpeed_WinningVoteErd,                   0xF24B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFanSpeed_FactoryVote,                      0xF24C, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFanSpeed_LoadOffDoorOpenVote,              0xF24D, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFanSpeed_DefrostVote,                      0xF24E, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFanSpeed_GridVote,                         0xF24F, FanVotedSpeed_t,                                    Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodBackWallLight_ResolvedVote,           0xF25A, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_WinningVoteErd,         0xF25B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_FactoryVote,            0xF25C, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_EnhancedSabbathVote,    0xF25D, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_DoorVote,               0xF25E, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodTopLight_ResolvedVote,                0xF26A, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodTopLight_WinningVoteErd,              0xF26B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodTopLight_FactoryVote,                 0xF26C, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodTopLight_EnhancedSabbathVote,         0xF26D, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodTopLight_DoorVote,                    0xF26E, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerBackWallLight_ResolvedVote,             0xF270, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerBackWallLight_WinningVoteErd,           0xF271, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerBackWallLight_FactoryVote,              0xF272, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerBackWallLight_EnhancedSabbathVote,      0xF273, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerBackWallLight_DoorVote,                 0xF274, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerTopLight_ResolvedVote,                  0xF27A, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerTopLight_WinningVoteErd,                0xF27B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerTopLight_FactoryVote,                   0xF27C, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerTopLight_EnhancedSabbathVote,           0xF27D, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerTopLight_DoorVote,                      0xF27E, RampingPwmDutyCyclePercentageVote_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_IsolationWaterValve_ResolvedVote,              0xF280, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IsolationWaterValve_WinningVoteErd,            0xF281, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IsolationWaterValve_FactoryVote,               0xF282, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IsolationWaterValve_DispensingVote,            0xF283, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IsolationWaterValve_AluminumMoldIceMakerVote,  0xF284, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IsolationWaterValve_TwistTrayIceMakerVote,     0xF285, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AluminumMoldIceMakerWaterValve_ResolvedVote,   0xF28A, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerWaterValve_WinningVoteErd, 0xF28B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerWaterValve_FactoryVote,    0xF28C, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,   0xF28D, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_TwistTrayIceMakerWaterValve_ResolvedVote,      0xF290, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerWaterValve_WinningVoteErd,    0xF291, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerWaterValve_FactoryVote,       0xF292, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerWaterValve_IceMakerVote,      0xF293, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DispenserWaterValve_ResolvedVote,              0xF29A, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DispenserWaterValve_WinningVoteErd,            0xF29B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DispenserWaterValve_FactoryVote,               0xF29C, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DispenserWaterValve_DispensingVote,            0xF29D, WaterValveVotedState_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AluminumMoldIceMakerHeaterRelay_ResolvedVote,  0xF2A0, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerHeaterRelay_WinningVoteErd,0xF2A1, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerHeaterRelay_FactoryVote,   0xF2A2, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,  0xF2A3, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AluminumMoldIceMakerRakeMotor_ResolvedVote,    0xF2AA, AluminumMoldIceMakerMotorVotedState_t,              Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerRakeMotor_WinningVoteErd,  0xF2AB, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerRakeMotor_FactoryVote,     0xF2AC, AluminumMoldIceMakerMotorVotedState_t,              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,    0xF2AD, AluminumMoldIceMakerMotorVotedState_t,              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FillTubeHeater_ResolvedVote,                   0xF2B0, PercentageDutyCycleVote_t,                          Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater_WinningVoteErd,                 0xF2B1, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater_FactoryVote,                    0xF2B2, PercentageDutyCycleVote_t,                          Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater_AluminumMoldIceMakerVote,       0xF2B3, PercentageDutyCycleVote_t,                          Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater_TwistTrayIceMakerVote,          0xF2B4, PercentageDutyCycleVote_t,                          Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater_NonHarvestVote,                 0xF2B5, PercentageDutyCycleVote_t,                          Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_TwistTrayIceMakerMotor_ResolvedVote,           0xF2BA, TwistTrayIceMakerMotorVotedAction_t,                Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerMotor_WinningVoteErd,         0xF2BB, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerMotor_FactoryVote,            0xF2BC, TwistTrayIceMakerMotorVotedAction_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerMotor_IceMakerVote,           0xF2BD, TwistTrayIceMakerMotorVotedAction_t,                Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AugerMotor_ResolvedVote,                       0xF2C0, AugerMotorVotedIceType_t,                           Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AugerMotor_WinningVoteErd,                     0xF2C1, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AugerMotor_FactoryVote,                        0xF2C2, AugerMotorVotedIceType_t,                           Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AugerMotor_DispensingVote,                     0xF2C3, AugerMotorVotedIceType_t,                           Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerSetpoint_ResolvedVote,                  0xF2CA, SetpointVotedTemperature_t,                         Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_WinningVoteErd,                0xF2CB, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_FactoryVote,                   0xF2CC, SetpointVotedTemperature_t,                         Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_EnhancedSabbathVote,           0xF2CD, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_ColdestSetpointVote,           0xF2CE, SetpointVotedTemperature_t,                         Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
\
   ENTRY(Erd_FreezerSetpoint_ColdestSetpointWinningVote,    0xF2CF, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_TurboFreezeVote,               0xF2D0, SetpointVotedTemperature_t,                         Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_IceInDoorVote,                 0xF2D1, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_FreezerIceMakerVote,           0xF2D2, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_FreezerIceRateVote,            0xF2D3, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerSetpoint_UserVote,                      0xF2D4, SetpointVotedTemperature_t,                         Swap_Range_E, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodSetpoint_ResolvedVote,                0xF2DA, SetpointVotedTemperature_t,                         Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_WinningVoteErd,              0xF2DB, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_FactoryVote,                 0xF2DC, SetpointVotedTemperature_t,                         Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_EnhancedSabbathVote,         0xF2DD, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_ColdestSetpointVote,         0xF2DE, SetpointVotedTemperature_t,                         Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodSetpoint_ColdestSetpointWinningVote,  0xF2DF, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_TurboCoolVote,               0xF2E0, SetpointVotedTemperature_t,                         Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_UserVote,                    0xF2E1, SetpointVotedTemperature_t,                         Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_ResolvedVote,   0xF2EA, SetpointVotedTemperature_t,                         Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_WinningVoteErd, 0xF2EB, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_FactoryVote,    0xF2EC, SetpointVotedTemperature_t,                         Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_UserVote,       0xF2ED, SetpointVotedTemperature_t,                         Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodDefrostHeater_ResolvedVote,           0xF2FA, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_WinningVoteErd,         0xF2FB, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_FactoryVote,            0xF2FC, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_CoolingSystemOffVote,   0xF2FD, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_DefrostVote,            0xF2FE, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerDefrostHeater_ResolvedVote,             0xF300, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_WinningVoteErd,           0xF301, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_FactoryVote,              0xF302, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_CoolingSystemOffVote,     0xF303, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_DefrostVote,              0xF304, HeaterVotedState_t,                                 Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodDamperHeater_ResolvedVote,               0xF30A, PercentageDutyCycleVote_t,                       Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater_WinningVoteErd,             0xF30B, WinningVoteErd_t,                                Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater_FactoryVote,                0xF30C, PercentageDutyCycleVote_t,                       Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater_CoolingSystemOffVote,       0xF30D, PercentageDutyCycleVote_t,                       Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater_DamperFreezePreventionVote, 0xF30E, PercentageDutyCycleVote_t,                       Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,      0xF30F, PercentageDutyCycleVote_t,                       Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodDamperPosition_ResolvedVote,               0xF31A, DamperVotedPosition_t,                         Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_WinningVoteErd,             0xF31B, WinningVoteErd_t,                              Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_FactoryVote,                0xF31C, DamperVotedPosition_t,                         Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_CoolingSystemOffVote,       0xF31D, DamperVotedPosition_t,                         Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_EnhancedSabbathVote,        0xF31E, DamperVotedPosition_t,                         Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_DamperFreezePreventionVote, 0xF31F, DamperVotedPosition_t,                         Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_MaxOpenTimeVote,            0xF320, DamperVotedPosition_t,                         Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_DefrostVote,                0xF321, DamperVotedPosition_t,                         Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_GridVote,                   0xF322, DamperVotedPosition_t,                         Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ValvePosition_ResolvedVote,                    0xF32A, ValveVotedPosition_t,                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ValvePosition_WinningVoteErd,                  0xF32B, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ValvePosition_FactoryVote,                     0xF32C, ValveVotedPosition_t,                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ValvePosition_DefrostVote,                     0xF32D, ValveVotedPosition_t,                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ValvePosition_GridVote,                        0xF32E, ValveVotedPosition_t,                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CompressorSpeed_ResolvedVote,                  0xF339, CompressorVotedSpeed_t,                             Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_WinningVoteErd,                0xF33A, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_FactoryVote,                   0xF33B, CompressorVotedSpeed_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_CoolingSystemOffVote,          0xF33C, CompressorVotedSpeed_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_EnhancedSabbathVote,           0xF33D, CompressorVotedSpeed_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_DefrostVote,                   0xF33E, CompressorVotedSpeed_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorSpeed_GridVote,                      0xF33F, CompressorVotedSpeed_t,                             Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_RecessHeater_ResolvedVote,                     0xF340, PercentageDutyCycleVote_t,                          Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_RecessHeater_WinningVoteErd,                   0xF341, WinningVoteErd_t,                                   Swap_Yes,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_RecessHeater_FactoryVote,                      0xF342, PercentageDutyCycleVote_t,                          Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_RecessHeater_CoolingSystemOffVote,             0xF343, PercentageDutyCycleVote_t,                          Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_RecessHeater_VariableAntiSweatVote,            0xF344, PercentageDutyCycleVote_t,                          Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DisableMinimumCompressorTimes_ResolvedVote,        0xF350, BooleanVotedState_t,                            Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DisableMinimumCompressorTimes_WinningVoteErd,      0xF351, WinningVoteErd_t,                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DisableMinimumCompressorTimes_FactoryVote,         0xF352, BooleanVotedState_t,                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DisableMinimumCompressorTimes_EnhancedSabbathVote, 0xF353, BooleanVotedState_t,                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DisableMinimumCompressorTimes_DefrostVote,         0xF354, BooleanVotedState_t,                            Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_SensorsReadyToBeRead,                          0xF400, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SetpointResolverReady,                         0xF401, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentStateResolverReady,      0xF402, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientTemperaturePluginReady,                 0xF403, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridPluginReady,                               0xF404, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PeriodicNvUpdaterReady,                        0xF405, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SabbathPluginReady,                            0xF406, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_GridLineCalculatorIsReady,                     0xF407, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DefrostParameterSelectorReady,                 0xF408, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, 0xF409, bool,                                             Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_SetpointZonePluginReady,                       0xF40A, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AdjustedSetpointPluginReady,                   0xF40B, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_UserSetpointPluginReady,                       0xF40C, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureOverrideRequest,          0xF420, bool,                                     Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureOverrideValueInDegFx100,  0xF421, TemperatureDegFx100_t,                    Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100,       0xF422, TemperatureDegFx100_t,                    Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Freezer_FilteredTemperatureOverrideRequest,              0xF423, bool,                                     Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100,      0xF424, TemperatureDegFx100_t,                    Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperatureResolvedInDegFx100,           0xF425, TemperatureDegFx100_t,                    Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFood_FilteredTemperatureOverrideRequest,            0xF426, bool,                                     Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_FilteredTemperatureOverrideValueInDegFx100,    0xF427, TemperatureDegFx100_t,                    Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,         0xF428, TemperatureDegFx100_t,                    Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodThermistor_IsValidOverrideRequest,                     0xF429, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodThermistor_IsValidOverrideValue,                       0xF430, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodThermistor_IsValidResolved,                            0xF431, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerThermistor_IsValidOverrideRequest,                       0xF432, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerThermistor_IsValidOverrideValue,                         0xF433, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerThermistor_IsValidResolved,                              0xF434, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodEvapThermistor_IsValidOverrideRequest,                 0xF435, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_IsValidOverrideValue,                   0xF436, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_IsValidResolved,                        0xF437, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreezerEvapThermistor_IsValidOverrideRequest,                   0xF438, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_IsValidOverrideValue,                     0xF439, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_IsValidResolved,                          0xF43A, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideRequest, 0xF43B, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideValue,   0xF43C, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved,        0xF43D, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AmbientThermistor_IsValidOverrideRequest,                       0xF43E, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientThermistor_IsValidOverrideValue,                         0xF43F, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientThermistor_IsValidResolved,                              0xF440, bool,                              Swap_No, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideRequest,    0xF441, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideValue,      0xF442, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,           0xF443, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideRequest,      0xF444, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideValue,        0xF445, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved,             0xF446, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_TwistTrayIceMakerThermistor_IsValidOverrideRequest,             0xF447, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerThermistor_IsValidOverrideValue,               0xF448, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerThermistor_IsValidResolved,                    0xF449, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Ambient_FilteredInternalTemperatureOverrideRequest,             0xF44A, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalTemperatureOverrideValueInDegFx100,     0xF44B, TemperatureDegFx100_t,             Swap_Yes,Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalTemperatureResolvedInDegFx100,          0xF44C, RelativeHumidityPercentx100_t,     Swap_Yes,Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Ambient_FilteredTemperatureResolvedInDegFx100,                  0xF44D, TemperatureDegFx100_t,             Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_FilteredHumidityResolvedPercentx100,                    0xF44E, RelativeHumidityPercentx100_t,     Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientTemperature_IsValidResolved,                             0xF44F, bool,                              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientHumidity_IsValidResolved,                                0xF450, bool,                              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideRequest,        0xF451, bool,                              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100,0xF452, TemperatureDegFx100_t,             Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,     0xF453, TemperatureDegFx100_t,             Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_TwistTrayIceMaker_FilteredTemperatureOverrideRequest,           0xF454, bool,                              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_FilteredTemperatureOverrideValueInDegFx100,   0xF455, TemperatureDegFx100_t,             Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMaker_FilteredTemperatureResolvedInDegFx100,        0xF456, TemperatureDegFx100_t,             Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_AmbientHumiditySensor_IsValidOverrideRequest,                   0xF457, bool,                              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientHumiditySensor_IsValidOverrideValue,                     0xF458, bool,                              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AmbientHumiditySensor_IsValidResolved,                          0xF459, bool,                              Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Ambient_FilteredInternalHumidityOverrideRequest,                0xF45A, bool,                              Swap_No, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalHumidityOverrideValueInPercentx100,     0xF45B, TemperatureDegFx100_t,             Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalHumidityResolvedInPercentx100,          0xF45C, TemperatureDegFx100_t,             Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_DimmableLight1PercentLevelRequest,             0xF470, PercentageDutyCycle_t,                              Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DimmableLight1PercentLevelStatus,              0xF471, PercentageDutyCycle_t,                              Swap_No,    Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_MaxLightBrightness, NotFault) \
   ENTRY(Erd_DimmableLight1Configuration,                   0xF472, UserAllowableInteriorLightingBitmap_t,              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DimmableLight2PercentLevelRequest,             0xF473, PercentageDutyCycle_t,                              Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DimmableLight2PercentLevelStatus,              0xF474, PercentageDutyCycle_t,                              Swap_No,    Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_MaxLightBrightness, NotFault) \
   ENTRY(Erd_DimmableLight2Configuration,                   0xF475, UserAllowableInteriorLightingBitmap_t,              Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CalculatedCondenserFanControl,                 0xF480, FanControl_t,                                       Swap_Range_S, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedIceCabinetFanControl,                0xF481, FanControl_t,                                       Swap_Range,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedConvertibleCompartmentFanControl,    0xF482, FanControl_t,                                       Swap_Range,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedDeliFanControl,                      0xF483, FanControl_t,                                       Swap_Range,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedFreezerEvapFanControl,               0xF484, FanControl_t,                                       Swap_Range,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CalculatedFreshFoodEvapFanControl,             0xF485, FanControl_t,                                       Swap_Range_E, Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_GpioGroupInterface,                            0xF49F, I_GpioGroup_t *,                                    Swap_No,    Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   \
   ENTRY(Erd_HeartbeatLed,                                  0xF500, bool,                                               Swap_No,    Io_O,    Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_RelayWatchdog,                                 0xF501, bool,                                               Swap_No,    Io_O,    Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorInverterDriver,                      0xF502, PwmFrequency_t,                                     Swap_Yes,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CompressorRelay,                               0xF503, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FillTubeHeater,                                0xF504, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerDefrostHeaterRelay,                     0xF505, bool,                                               Swap_No,    Io_None, Sub_Y, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeater,                         0xF506, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerWaterValveRelay,           0xF507, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumMoldIceMakerHeaterRelay,               0xF508, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_IceMakerRakeMotorRelay,                        0xF509, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AugerMotorDirection,                           0xF50A, bool,                                               Swap_No,    Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AugerMotorPower,                               0xF50B, bool,                                               Swap_No,    Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DispenserValveRelay,                           0xF50C, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_IsolationValveRelay,                           0xF50D, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_TwistTrayIceMakerWaterValveRelay,              0xF50E, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CondenserFan_Pwm,                              0xF51A, PwmDutyCycle_t,                                     Swap_Range_S, Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFan_Pwm,                             0xF51B, PwmDutyCycle_t,                                     Swap_Range,   Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFan_Pwm,                 0xF51C, PwmDutyCycle_t,                                     Swap_Range,   Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFan_Pwm,                                   0xF51D, PwmDutyCycle_t,                                     Swap_Range,   Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFan_Pwm,                            0xF51E, PwmDutyCycle_t,                                     Swap_Range,   Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFan_Pwm,                          0xF51F, PwmDutyCycle_t,                                     Swap_Range_E, Io_O,    Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CondenserFan_ActualRpm,                        0xF52A, FanRpm_t,                                           Swap_Range_S, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFan_ActualRpm,                       0xF52B, FanRpm_t,                                           Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFan_ActualRpm,           0xF52C, FanRpm_t,                                           Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFan_ActualRpm,                             0xF52D, FanRpm_t,                                           Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFan_ActualRpm,                      0xF52E, FanRpm_t,                                           Swap_Range,   Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFan_ActualRpm,                    0xF52F, FanRpm_t,                                           Swap_Range_E, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_CondenserFan_InputCaptureTime,                 0xF53A, InputCaptureMicroSeconds_t,                         Swap_Range_S, Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_IceCabinetFan_InputCaptureTime,                0xF53B, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFan_InputCaptureTime,    0xF53C, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_DeliFan_InputCaptureTime,                      0xF53D, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerEvapFan_InputCaptureTime,               0xF53E, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodEvapFan_InputCaptureTime,             0xF53F, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_AluminumIceMakerRake_InputCaptureTime,         0xF540, InputCaptureMicroSeconds_t,                         Swap_Range_E, Io_None, Sub_Y, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_FlowMeter_InputCaptureCount,                   0xF541, InputCaptureCounts_t,                               Swap_Yes,   Io_None, Sub_Y, MappedBsp,              NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FreshFoodDamperStepperMotorPositionRequest,    0xF54A, StepperPositionRequest_t,                           Swap_Yes,   Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHomingRequest,                  0xF54B, bool,                                               Swap_No,    Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperCurrentPosition,                0xF54C, DamperPosition_t,                                   Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperStepperMotorControlRequest,     0xF54D, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_TwistIceMakerMotorControlRequest,              0xF54E, bool,                                               Swap_No,    Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperStepperMotorDriveEnable,        0xF54F, bool,                                               Swap_No,    Io_None, Sub_Y, MappedBsp,              NotNv,                                    NotFault) \
   ENTRY(Erd_TwistIceMakerMotorDriveEnable,                 0xF550, bool,                                               Swap_No,    Io_None, Sub_N, MappedBsp,              NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FillTubeHeater_Pwm,                            0xF55A, PwmDutyCycle_t,                                     Swap_Yes, Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_RampingPwm,             0xF55B, RampingPwmDutyCycle_t,                              Swap_Yes, Io_None, Sub_Y, MappedBsp,                NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodTopLight_RampingPwm,                  0xF55C, RampingPwmDutyCycle_t,                              Swap_Yes, Io_None, Sub_Y, MappedBsp,                NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerTopLight_RampingPwm,                    0xF55D, RampingPwmDutyCycle_t,                              Swap_Yes, Io_None, Sub_Y, MappedBsp,                NotNv,                                    NotFault) \
   ENTRY(Erd_FreshFoodDamperHeaterPwmDutyCycle,             0xF55E, PwmDutyCycle_t,                                     Swap_Yes, Io_None, Sub_Y, Ram,                      NotNv,                                    NotFault) \
   ENTRY(Erd_FreezerBackWallLight_RampingPwm,               0xF55F, RampingPwmDutyCycle_t,                              Swap_Yes, Io_None, Sub_Y, MappedBsp,                NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Pwm_PWM_25K_00,                                0xF56F, PwmDutyCycle_t,                                     Swap_Range_S, Io_None, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_Pwm_PWM_25K_01,                                0xF570, PwmDutyCycle_t,                                     Swap_Range,   Io_None, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_Pwm_PWM_25K_02,                                0xF571, PwmDutyCycle_t,                                     Swap_Range,   Io_None, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_Pwm_PWM_25K_03,                                0xF572, PwmDutyCycle_t,                                     Swap_Range,   Io_None, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_Pwm_PWM_25K_04,                                0xF573, PwmDutyCycle_t,                                     Swap_Range_E, Io_None, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_RampingLedPwm_PWM_200_00,                      0xF574, RampingPwmDutyCycle_t,                              Swap_Range_S, Io_None, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_RampingLedPwm_PWM_200_01,                      0xF575, RampingPwmDutyCycle_t,                              Swap_Range,   Io_None, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_RampingLedPwm_PWM_200_02,                      0xF576, RampingPwmDutyCycle_t,                              Swap_Range,   Io_None, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   ENTRY(Erd_RampingLedPwm_PWM_200_03,                      0xF577, RampingPwmDutyCycle_t,                              Swap_Range_E, Io_None, Sub_N, Bsp,                       NotNv,                                    NotFault) \
   \
   ENTRY(Erd_InputCapture_CAPTURE_00,                       0xF58F, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_01,                       0xF590, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_02,                       0xF591, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_03,                       0xF592, InputCaptureCounts_t,                               Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_04,                       0xF593, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_05,                       0xF594, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Adc_ADC_0,                                     0xF59A, AdcCounts_t,                                        Swap_Range_S, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ADC_1,                                     0xF59B, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_475K_TH_00,                         0xF59C, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_00,                          0xF59D, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_01,                          0xF59E, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_02,                          0xF59F, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_03,                          0xF600, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_04,                          0xF601, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_05,                          0xF602, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_06,                          0xF603, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_07,                          0xF604, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_08,                          0xF605, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_LOAD_CURRENT,                              0xF606, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_HW_PERSONALITY_00,                         0xF607, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Adc_HW_PERSONALITY_01,                         0xF608, AdcCounts_t,                                        Swap_Range_E, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Gpio_GPIO_OUT_00,                              0xF609, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_GPIO_OUT_01,                              0xF610, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_GPIO_OUT_02,                              0xF611, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_HTR_00,                                   0xF612, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_HTR_01,                                   0xF613, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_HTR_02,                                   0xF614, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_HTR_03,                                   0xF615, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_HTR_04,                                   0xF616, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_EN_00,                            0xF617, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_EN_01,                            0xF618, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_00,                               0xF619, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_01,                               0xF61A, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_02,                               0xF61B, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_03,                               0xF61C, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_REF_VAL_00,                               0xF61D, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_REF_VAL_01,                               0xF61E, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_REF_VAL_02,                               0xF61F, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_REF_VAL_03,                               0xF620, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_RLY_00,                                   0xF621, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_RLY_01,                                   0xF622, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_RLY_02,                                   0xF623, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_RLY_03,                                   0xF624, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_RLY_04,                                   0xF625, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_RLY_05,                                   0xF626, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_RLY_06,                                   0xF627, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_RLY_07,                                   0xF628, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_RLY_08,                                   0xF629, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_HEARTBEAT,                                0xF62B, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_SBC_RESET,                                0xF62C, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_PERSONALITY,                              0xF62D, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_DOOR_INT,                                 0xF62E, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_DOOR_01,                                  0xF62F, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_DOOR_02,                                  0xF630, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_DOOR_03,                                  0xF631, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_DOOR_04,                                  0xF632, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_GPIO_IN_00,                               0xF633, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_GPIO_IN_01,                               0xF634, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_GPIO_IN_02,                               0xF635, bool,                                               Swap_No, Io_None, Sub_Y, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_GPIO_IN_03,                               0xF636, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_SABBATH,                                  0xF637, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_PWM_VAR_01,                               0xF638, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Gpio_CAPTURE_03,                               0xF639, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData0,     0xF700, FaultSnapshotData_t,                                Swap_Range_S, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData1,     0xF701, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData2,     0xF702, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData3,     0xF703, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData4,     0xF704, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData5,     0xF705, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData6,     0xF706, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData7,     0xF707, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData8,     0xF708, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData9,     0xF709, FaultSnapshotData_t,                                Swap_Range_E, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_SomeFault,                                     0xF710, bool,                                               Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                    FaultId_SomeFault) \
   ENTRY(Erd_SomeOtherFault,                                0xF711, bool,                                               Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                    FaultId_SomeOtherFault) \
   \
   ENTRY(Erd_CacheSyncState,                                0xF712, bool,                                               Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Fault_EepromReadFailure,                       0xF713, bool,                                               Swap_No,  Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Fault_EepromWriteFailure,                      0xF714, bool,                                               Swap_No,  Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_Fault_EepromEraseFailure,                      0xF715, bool,                                               Swap_No,  Io_All,  Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_PersonalityIdOutOfRangeFlag,                   0xF716, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FaultWrapperInterfaceArray,                    0xF717, I_FaultWrapper_t *,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_CoolingStatesGridVotesConstArrayMapInterface,  0xF719, I_ConstArrayMap_t *,                                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_FactoryModeEnableRequestInMinutes,             0xF801, uint8_t,                                            Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_ExternalAmbientFilteredTemperatureInDegFx100,  0xFAFB, TemperatureDegFx100_t,                              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ExternalAmbientThermistor_IsValid,             0xFAFC, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ExternalAmbientFilteredRelativeHumidityPercentx100, 0xFAFD, RelativeHumidityPercentx100_t,                 Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_ExternalAmbientHumidity_IsValid,               0xFAFE, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_EepromEraseRequestSignal,                      0xFAFF, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   \
   ENTRY(Erd_Eeprom_DefrostCompressorOnTimeInSeconds,       0xFB06, uint32_t,                                           Swap_Yes,   Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_FreezerDefrostHeaterOnTimeInMinutes,    0xFB07, uint8_t,                                            Swap_No,    Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_ClearedDefrostEepromStartup,            0xFB08, bool,                                               Swap_No,    Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanTrue, NotFault) \
   ENTRY(Erd_Eeprom_LeftSideFreshFoodScaledDoorAccelerationInSeconds,  0xFB0C, uint32_t,                                Swap_Range_S, Io_None, Sub_N, NvUnitSetting,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_RightSideFreshFoodScaledDoorAccelerationInSeconds, 0xFB0D, uint32_t,                                Swap_Range,   Io_None, Sub_N, NvUnitSetting,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_DoorInDoorScaledDoorAccelerationInSeconds,         0xFB0E, uint32_t,                                Swap_Range,   Io_None, Sub_N, NvUnitSetting,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds,            0xFB0F, uint32_t,                                Swap_Range,   Io_None, Sub_N, NvUnitSetting,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds, 0xFB10, uint32_t,                 Swap_Range,   Io_None, Sub_N, NvUnitSetting,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds,   0xFB11, uint32_t,                 Swap_Range_E, Io_None, Sub_N, NvUnitSetting,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_TurboCoolOnTimeInMinutes,               0xFB12, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_Eeprom_TurboFreezeOnTimeInMinutes,             0xFB13, uint16_t,                                           Swap_Yes,   Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_AndroidUiSignOfLifeFsmState,                   0xFB80, AndroidUiSignOfLifeFsmState_t,                      Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_MainboardIsConnectedToTheAndroidUi,            0xFB81, bool,                                               Swap_No,  Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   ENTRY(Erd_NumberOfTimesMainboardHasLostCommunicationWithAndroidUi, 0xFB82, uint8_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_NumberOfTimesMainboardHasAttemptedToResetAndroidUi,      0xFB83, uint8_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_AndroidUiResetSignal,                                    0xFB84, Signal_t,                                 Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                    NotFault) \
   ENTRY(Erd_DispenserUiInSystem,                                     0xFB85, bool,                                     Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   ENTRY(Erd_RfidBoardInSystem,                                       0xFB86, bool,                                     Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   ENTRY(Erd_DoorBoardInSystem,                                       0xFB87, bool,                                     Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   ENTRY(Erd_EmbeddedWifiInSystem,                                    0xFB88, bool,                                     Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   ENTRY(Erd_AndroidSbcBoardInSystem,                                 0xFB89, bool,                                     Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   ENTRY(Erd_InternalTemperatureUiInSystem,                           0xFB8A, bool,                                     Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse, NotFault) \
   \
   ENTRY(Erd_NvReservedProtected,                           0xFCFA, uint8_t,                                            Swap_No,    Io_None, Sub_N,  NvProtected,           NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedUnitSetting,                         0xFCFB, uint8_t,                                            Swap_No,    Io_None, Sub_N,  NvUnitSetting,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedUserSetting,                         0xFCFC, uint8_t,                                            Swap_No,    Io_None, Sub_N,  NvUserSetting,         NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedRfid,                                0xFCFD, uint8_t,                                            Swap_No,    Io_None, Sub_N,  NvRfid,                NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedFaultSnapshot,                       0xFCFE, uint8_t,                                            Swap_No,    Io_None, Sub_N,  NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   ENTRY(Erd_NvReservedCycleHistory,                        0xFCFF, uint8_t,                                            Swap_No,    Io_None, Sub_N,  NvCycleHistory,        NonVolatileDataSourceDefaultData_Zeros,   NotFault) \
   \
   ENTRY(Erd_PersonalityEepromMetadata,                     0xFFFE, AsyncDataSource_EepromMetadata_t,                   Swap_No,    Io_None, Sub_N,  PersonalityEeprom,     NonVolatileDataSourceDefaultData_Zeros,   NotFault)

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
