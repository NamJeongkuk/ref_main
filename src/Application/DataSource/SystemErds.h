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
#include "FaultId.h"
#include "FaultSnapshot.h"
#include "ServiceDiagnosticsRevision3.h"
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
#include "SealedSystemValvePosition.h"
#include "Setpoint.h"
#include "SetpointVotedTemperature.h"
#include "DefrostState.h"
#include "DefrostHsmState.h"
#include "DefrostTestRequest.h"
#include "DefrostTestStateRequestMessage.h"
#include "HeaterVotedState.h"
#include "EnergyDemandLevel.h"
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
#include "IceMakerMotorVotedState.h"
#include "WaterValveVotedState.h"
#include "FeelerArmPosition.h"
#include "GridOffsetAdder.h"
#include "RakePosition.h"
#include "IceMakerMoldHeaterControlRequest.h"
#include "PercentageDutyCycleVote.h"
#include "DispenseSelection.h"
#include "ReadyToDefrostHsmState.h"
#include "PwmFrequency.h"
#include "IceMakerMotorActionResult.h"
#include "IceMakerMotorRequestedState.h"
#include "IceMakerMotorErrorReason.h"
#include "IceMakerMotorOperationState.h"
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
#include "I_OneWire.h"
#include "DispensingInhibitedReasonBitmap.h"
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
#include "UserAllowableInteriorLightState.h"
#include "RfidTypes.h"
#include "TimeHms.h"
#include "IceMakerFillInhibitReasonBitmap.h"
#include "WaterFilterLifeStatus.h"
#include "RfidCommunicationControllerState.h"
#include "ConvertedCallbacks.h"
#include "WaterFilterRemainingUsage.h"
#include "WaterFilterUsageSinceExpiration.h"
#include "ServiceModeTestRequest.h"
#include "ServiceModeTestNumber.h"
#include "ServiceModeTestStatus.h"
#include "NotificationsBitmap.h"
#include "BoardFeatureApi.h"
#include "CalendarUsageInSeconds.h"
#include "VolumeInOuncesX100.h"
#include "RfidFaultHandler.h"
#include "NonVolatileUsageMonitor.h"
#include "RecessLightOnRequest.h"
#include "IceMakerStateMachineState.h"
#include "SealedSystemValvePosition.h"

// clang-format off

#define INCLUDE_RAM_Ram(_x) _x
#define INCLUDE_RAM_Virtual(_x)
#define INCLUDE_RAM_Converted(_x)
#define INCLUDE_RAM_Nv(_x)
#define INCLUDE_RAM_NvProtected(_x)
#define INCLUDE_RAM_NvUnitSetting(_x)
#define INCLUDE_RAM_NvUserSetting(_x)
#define INCLUDE_RAM_NvRfid(_x)
#define INCLUDE_RAM_NvUsageProfile(_x)
#define INCLUDE_RAM_NvFaultSnapshot(_x)
#define INCLUDE_RAM_NvCycleHistory(_x)
#define INCLUDE_RAM_Fault(_x)
#define INCLUDE_RAM_Bsp(_x)
#define INCLUDE_RAM_MappedBsp(_x)
#define INCLUDE_RAM_PersonalityEeprom(_x)

#define INCLUDE_VIRTUAL_Ram(_x)
#define INCLUDE_VIRTUAL_Virtual(_x) _x
#define INCLUDE_VIRTUAL_Converted(_x)
#define INCLUDE_VIRTUAL_Nv(_x)
#define INCLUDE_VIRTUAL_NvProtected(_x)
#define INCLUDE_VIRTUAL_NvUnitSetting(_x)
#define INCLUDE_VIRTUAL_NvUserSetting(_x)
#define INCLUDE_VIRTUAL_NvRfid(_x) _x
#define INCLUDE_VIRTUAL_NvUsageProfile(_x) _x
#define INCLUDE_VIRTUAL_NvFaultSnapshot(_x)
#define INCLUDE_VIRTUAL_NvCycleHistory(_x)
#define INCLUDE_VIRTUAL_Fault(_x)
#define INCLUDE_VIRTUAL_Bsp(_x)
#define INCLUDE_VIRTUAL_MappedBsp(_x)
#define INCLUDE_VIRTUAL_PersonalityEeprom(_x)

#define INCLUDE_CONVERTED_Ram(_x)
#define INCLUDE_CONVERTED_Virtual(_x)
#define INCLUDE_CONVERTED_Converted(_x) _x
#define INCLUDE_CONVERTED_Nv(_x)
#define INCLUDE_CONVERTED_NvProtected(_x)
#define INCLUDE_CONVERTED_NvUnitSetting(_x)
#define INCLUDE_CONVERTED_NvUserSetting(_x)
#define INCLUDE_CONVERTED_NvRfid(_x) _x
#define INCLUDE_CONVERTED_NvUsageProfile(_x)
#define INCLUDE_CONVERTED_NvFaultSnapshot(_x)
#define INCLUDE_CONVERTED_NvCycleHistory(_x)
#define INCLUDE_CONVERTED_Fault(_x)
#define INCLUDE_CONVERTED_Bsp(_x)
#define INCLUDE_CONVERTED_MappedBsp(_x)
#define INCLUDE_CONVERTED_PersonalityEeprom(_x)

#define INCLUDE_NV_Ram(_x)
#define INCLUDE_NV_Virtual(_x)
#define INCLUDE_NV_Converted(_x)
#define INCLUDE_NV_Nv(_x) _x
#define INCLUDE_NV_NvProtected(_x) _x
#define INCLUDE_NV_NvUnitSetting(_x) _x
#define INCLUDE_NV_NvUserSetting(_x) _x
#define INCLUDE_NV_NvRfid(_x) _x
#define INCLUDE_NV_NvUsageProfile(_x) _x
#define INCLUDE_NV_NvFaultSnapshot(_x) _x
#define INCLUDE_NV_NvCycleHistory(_x) _x
#define INCLUDE_NV_Fault(_x)
#define INCLUDE_NV_Bsp(_x)
#define INCLUDE_NV_MappedBsp(_x)
#define INCLUDE_NV_PersonalityEeprom(_x)

#define INCLUDE_FAULT_Ram(_x)
#define INCLUDE_FAULT_Virtual(_x)
#define INCLUDE_FAULT_Converted(_x)
#define INCLUDE_FAULT_Nv(_x)
#define INCLUDE_FAULT_NvProtected(_x)
#define INCLUDE_FAULT_NvUnitSetting(_x)
#define INCLUDE_FAULT_NvUserSetting(_x)
#define INCLUDE_FAULT_NvRfid(_x)
#define INCLUDE_FAULT_NvUsageProfile(_x)
#define INCLUDE_FAULT_NvFaultSnapshot(_x)
#define INCLUDE_FAULT_NvCycleHistory(_x)
#define INCLUDE_FAULT_Fault(_x) _x
#define INCLUDE_FAULT_Bsp(_x)
#define INCLUDE_FAULT_MappedBsp(_x)
#define INCLUDE_FAULT_PersonalityEeprom(_x)

#define INCLUDE_BSP_Ram(_x)
#define INCLUDE_BSP_Virtual(_x)
#define INCLUDE_BSP_Converted(_x)
#define INCLUDE_BSP_Nv(_x)
#define INCLUDE_BSP_NvProtected(_x)
#define INCLUDE_BSP_NvUnitSetting(_x)
#define INCLUDE_BSP_NvUserSetting(_x)
#define INCLUDE_BSP_NvRfid(_x)
#define INCLUDE_BSP_NvUsageProfile(_x)
#define INCLUDE_BSP_NvFaultSnapshot(_x)
#define INCLUDE_BSP_NvCycleHistory(_x)
#define INCLUDE_BSP_Fault(_x)
#define INCLUDE_BSP_Bsp(_x) _x
#define INCLUDE_BSP_MappedBsp(_x)
#define INCLUDE_BSP_PersonalityEeprom(_x)

#define INCLUDE_NVONLY_Ram(_x)
#define INCLUDE_NVONLY_Virtual(_x)
#define INCLUDE_NVONLY_Converted(_x)
#define INCLUDE_NVONLY_Nv(_x) _x
#define INCLUDE_NVONLY_NvProtected(_x)
#define INCLUDE_NVONLY_NvUnitSetting(_x)
#define INCLUDE_NVONLY_NvUserSetting(_x)
#define INCLUDE_NVONLY_NvRfid(_x)
#define INCLUDE_NVONLY_NvUsageProfile(_x)
#define INCLUDE_NVONLY_NvFaultSnapshot(_x)
#define INCLUDE_NVONLY_NvCycleHistory(_x)
#define INCLUDE_NVONLY_Fault(_x)
#define INCLUDE_NVONLY_Bsp(_x)
#define INCLUDE_NVONLY_MappedBsp(_x)
#define INCLUDE_NVONLY_PersonalityEeprom(_x)

#define INCLUDE_NVALL_Ram(_x)
#define INCLUDE_NVALL_Virtual(_x)
#define INCLUDE_NVALL_Converted(_x)
#define INCLUDE_NVALL_Nv(_x) _x
#define INCLUDE_NVALL_NvProtected(_x) _x
#define INCLUDE_NVALL_NvUnitSetting(_x) _x
#define INCLUDE_NVALL_NvUserSetting(_x) _x
#define INCLUDE_NVALL_NvRfid(_x) _x
#define INCLUDE_NVALL_NvUsageProfile(_x) _x
#define INCLUDE_NVALL_NvFaultSnapshot(_x) _x
#define INCLUDE_NVALL_NvCycleHistory(_x) _x
#define INCLUDE_NVALL_Fault(_x)
#define INCLUDE_NVALL_Bsp(_x)
#define INCLUDE_NVALL_MappedBsp(_x)
#define INCLUDE_NVALL_PersonalityEeprom(_x) _x

#define INCLUDE_NVPROTECTED_Ram(_x)
#define INCLUDE_NVPROTECTED_Virtual(_x)
#define INCLUDE_NVPROTECTED_Converted(_x)
#define INCLUDE_NVPROTECTED_Nv(_x) _x
#define INCLUDE_NVPROTECTED_NvProtected(_x) _x
#define INCLUDE_NVPROTECTED_NvUnitSetting(_x)
#define INCLUDE_NVPROTECTED_NvUserSetting(_x)
#define INCLUDE_NVPROTECTED_NvRfid(_x)
#define INCLUDE_NVPROTECTED_NvUsageProfile(_x)
#define INCLUDE_NVPROTECTED_NvFaultSnapshot(_x)
#define INCLUDE_NVPROTECTED_NvCycleHistory(_x)
#define INCLUDE_NVPROTECTED_Fault(_x)
#define INCLUDE_NVPROTECTED_Bsp(_x)
#define INCLUDE_NVPROTECTED_MappedBsp(_x)
#define INCLUDE_NVPROTECTED_PersonalityEeprom(_x)

#define INCLUDE_NVUNITSETTING_Ram(_x)
#define INCLUDE_NVUNITSETTING_Virtual(_x)
#define INCLUDE_NVUNITSETTING_Converted(_x)
#define INCLUDE_NVUNITSETTING_Nv(_x) _x
#define INCLUDE_NVUNITSETTING_NvProtected(_x)
#define INCLUDE_NVUNITSETTING_NvUnitSetting(_x) _x
#define INCLUDE_NVUNITSETTING_NvUserSetting(_x)
#define INCLUDE_NVUNITSETTING_NvRfid(_x)
#define INCLUDE_NVUNITSETTING_NvUsageProfile(_x)
#define INCLUDE_NVUNITSETTING_NvFaultSnapshot(_x)
#define INCLUDE_NVUNITSETTING_NvCycleHistory(_x)
#define INCLUDE_NVUNITSETTING_Fault(_x)
#define INCLUDE_NVUNITSETTING_Bsp(_x)
#define INCLUDE_NVUNITSETTING_MappedBsp(_x)
#define INCLUDE_NVUNITSETTING_PersonalityEeprom(_x)

#define INCLUDE_NVUSERSETTING_Ram(_x)
#define INCLUDE_NVUSERSETTING_Virtual(_x)
#define INCLUDE_NVUSERSETTING_Converted(_x)
#define INCLUDE_NVUSERSETTING_Nv(_x) _x
#define INCLUDE_NVUSERSETTING_NvProtected(_x)
#define INCLUDE_NVUSERSETTING_NvUnitSetting(_x)
#define INCLUDE_NVUSERSETTING_NvUserSetting(_x) _x
#define INCLUDE_NVUSERSETTING_NvRfid(_x)
#define INCLUDE_NVUSERSETTING_NvUsageProfile(_x)
#define INCLUDE_NVUSERSETTING_NvFaultSnapshot(_x)
#define INCLUDE_NVUSERSETTING_NvCycleHistory(_x)
#define INCLUDE_NVUSERSETTING_Fault(_x)
#define INCLUDE_NVUSERSETTING_Bsp(_x)
#define INCLUDE_NVUSERSETTING_MappedBsp(_x)
#define INCLUDE_NVUSERSETTING_PersonalityEeprom(_x)

#define INCLUDE_NVRFID_Ram(_x)
#define INCLUDE_NVRFID_Virtual(_x)
#define INCLUDE_NVRFID_Converted(_x)
#define INCLUDE_NVRFID_Nv(_x) _x
#define INCLUDE_NVRFID_NvProtected(_x)
#define INCLUDE_NVRFID_NvUnitSetting(_x)
#define INCLUDE_NVRFID_NvUserSetting(_x)
#define INCLUDE_NVRFID_NvRfid(_x) _x
#define INCLUDE_NVRFID_NvUsageProfile(_x)
#define INCLUDE_NVRFID_NvFaultSnapshot(_x)
#define INCLUDE_NVRFID_NvCycleHistory(_x)
#define INCLUDE_NVRFID_Fault(_x)
#define INCLUDE_NVRFID_Bsp(_x)
#define INCLUDE_NVRFID_MappedBsp(_x)
#define INCLUDE_NVRFID_PersonalityEeprom(_x)

#define INCLUDE_NVUSAGEPROFILE_Ram(_x)
#define INCLUDE_NVUSAGEPROFILE_Virtual(_x)
#define INCLUDE_NVUSAGEPROFILE_Converted(_x)
#define INCLUDE_NVUSAGEPROFILE_Nv(_x) _x
#define INCLUDE_NVUSAGEPROFILE_NvProtected(_x)
#define INCLUDE_NVUSAGEPROFILE_NvUnitSetting(_x)
#define INCLUDE_NVUSAGEPROFILE_NvUserSetting(_x)
#define INCLUDE_NVUSAGEPROFILE_NvRfid(_x)
#define INCLUDE_NVUSAGEPROFILE_NvUsageProfile(_x) _x
#define INCLUDE_NVUSAGEPROFILE_NvFaultSnapshot(_x)
#define INCLUDE_NVUSAGEPROFILE_NvCycleHistory(_x)
#define INCLUDE_NVUSAGEPROFILE_Fault(_x)
#define INCLUDE_NVUSAGEPROFILE_Bsp(_x)
#define INCLUDE_NVUSAGEPROFILE_MappedBsp(_x)
#define INCLUDE_NVUSAGEPROFILE_PersonalityEeprom(_x)

#define INCLUDE_NVFAULTSNAPSHOT_Ram(_x)
#define INCLUDE_NVFAULTSNAPSHOT_Virtual(_x)
#define INCLUDE_NVFAULTSNAPSHOT_Converted(_x)
#define INCLUDE_NVFAULTSNAPSHOT_Nv(_x) _x
#define INCLUDE_NVFAULTSNAPSHOT_NvProtected(_x)
#define INCLUDE_NVFAULTSNAPSHOT_NvUnitSetting(_x)
#define INCLUDE_NVFAULTSNAPSHOT_NvUserSetting(_x)
#define INCLUDE_NVFAULTSNAPSHOT_NvRfid(_x)
#define INCLUDE_NVFAULTSNAPSHOT_NvUsageProfile(_x)
#define INCLUDE_NVFAULTSNAPSHOT_NvFaultSnapshot(_x) _x
#define INCLUDE_NVFAULTSNAPSHOT_NvCycleHistory(_x)
#define INCLUDE_NVFAULTSNAPSHOT_Fault(_x)
#define INCLUDE_NVFAULTSNAPSHOT_Bsp(_x)
#define INCLUDE_NVFAULTSNAPSHOT_MappedBsp(_x)
#define INCLUDE_NVFAULTSNAPSHOT_PersonalityEeprom(_x)

#define INCLUDE_PERSONALITYEEPROM_Ram(_x)
#define INCLUDE_PERSONALITYEEPROM_Virtual(_x)
#define INCLUDE_PERSONALITYEEPROM_Converted(_x)
#define INCLUDE_PERSONALITYEEPROM_Nv(_x) _x
#define INCLUDE_PERSONALITYEEPROM_NvProtected(_x)
#define INCLUDE_PERSONALITYEEPROM_NvUnitSetting(_x)
#define INCLUDE_PERSONALITYEEPROM_NvUserSetting(_x)
#define INCLUDE_PERSONALITYEEPROM_NvRfid(_x)
#define INCLUDE_PERSONALITYEEPROM_NvUsageProfile(_x)
#define INCLUDE_PERSONALITYEEPROM_NvFaultSnapshot(_x)
#define INCLUDE_PERSONALITYEEPROM_NvCycleHistory(_x)
#define INCLUDE_PERSONALITYEEPROM_Fault(_x)
#define INCLUDE_PERSONALITYEEPROM_Bsp(_x)
#define INCLUDE_PERSONALITYEEPROM_MappedBsp(_x)
#define INCLUDE_PERSONALITYEEPROM_PersonalityEeprom(_x) _x

#define INCLUDE_NVCYCLEHISTORY_Ram(_x)
#define INCLUDE_NVCYCLEHISTORY_Virtual(_x)
#define INCLUDE_NVCYCLEHISTORY_Converted(_x)
#define INCLUDE_NVCYCLEHISTORY_Nv(_x) _x
#define INCLUDE_NVCYCLEHISTORY_NvProtected(_x)
#define INCLUDE_NVCYCLEHISTORY_NvUnitSetting(_x)
#define INCLUDE_NVCYCLEHISTORY_NvUserSetting(_x)
#define INCLUDE_NVCYCLEHISTORY_NvRfid(_x)
#define INCLUDE_NVCYCLEHISTORY_NvUsageProfile(_x)
#define INCLUDE_NVCYCLEHISTORY_NvFaultSnapshot(_x)
#define INCLUDE_NVCYCLEHISTORY_NvCycleHistory(_x) _x
#define INCLUDE_NVCYCLEHISTORY_Fault(_x)
#define INCLUDE_NVCYCLEHISTORY_Bsp(_x)
#define INCLUDE_NVCYCLEHISTORY_MappedBsp(_x)
#define INCLUDE_NVCYCLEHISTORY_PersonalityEeprom(_x)

#define INCLUDE_NON_RAM_Ram(_x)
#define INCLUDE_NON_RAM_Virtual(_x) _x
#define INCLUDE_NON_RAM_Converted(_x) _x
#define INCLUDE_NON_RAM_Nv(_x) _x
#define INCLUDE_NON_RAM_NvProtected(_x) _x
#define INCLUDE_NON_RAM_NvUnitSetting(_x) _x
#define INCLUDE_NON_RAM_NvUserSetting(_x) _x
#define INCLUDE_NON_RAM_NvRfid(_x) _x
#define INCLUDE_NON_RAM_NvUsageProfile(_x) _x
#define INCLUDE_NON_RAM_NvFaultSnapshot(_x) _x
#define INCLUDE_NON_RAM_NvCycleHistory(_x) _x
#define INCLUDE_NON_RAM_Fault(_x) _x
#define INCLUDE_NON_RAM_Bsp(_x) _x
#define INCLUDE_NON_RAM_MappedBsp(_x) _x
#define INCLUDE_NON_RAM_PersonalityEeprom(_x) _x

#define INCLUDE_MAPPED_BSP_Ram(_x)
#define INCLUDE_MAPPED_BSP_Virtual(_x)
#define INCLUDE_MAPPED_BSP_Converted(_x)
#define INCLUDE_MAPPED_BSP_Nv(_x)
#define INCLUDE_MAPPED_BSP_NvProtected(_x)
#define INCLUDE_MAPPED_BSP_NvUnitSetting(_x)
#define INCLUDE_MAPPED_BSP_NvUserSetting(_x)
#define INCLUDE_MAPPED_BSP_NvRfid(_x)
#define INCLUDE_MAPPED_BSP_NvUsageProfile(_x)
#define INCLUDE_MAPPED_BSP_NvFaultSnapshot(_x)
#define INCLUDE_MAPPED_BSP_NvCycleHistory(_x)
#define INCLUDE_MAPPED_BSP_Fault(_x)
#define INCLUDE_MAPPED_BSP_Bsp(_x)
#define INCLUDE_MAPPED_BSP_MappedBsp(_x) _x
#define INCLUDE_MAPPED_BSP_PersonalityEeprom(_x)

#define INCLUDE_BSP_ALL_Ram(_x)
#define INCLUDE_BSP_ALL_Virtual(_x)
#define INCLUDE_BSP_ALL_Converted(_x)
#define INCLUDE_BSP_ALL_Nv(_x)
#define INCLUDE_BSP_ALL_NvProtected(_x)
#define INCLUDE_BSP_ALL_NvUnitSetting(_x)
#define INCLUDE_BSP_ALL_NvUserSetting(_x)
#define INCLUDE_BSP_ALL_NvRfid(_x)
#define INCLUDE_BSP_ALL_NvUsageProfile(_x)
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

//       Name,                                              Number, DataType,                                           Swap,     Io,      Sub,   StorageType,            NvDefaultData,                                               YearlyWrites,  FaultId
#define ERD_TABLE(ENTRY)  \
   ENTRY(Erd_ModelNumber,                                   0x0001, ModelNumber_t,                                      Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SerialNumber,                                  0x0002, SerialNumber_t,                                     Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ControlLockStatus,                             0x0004, ControlLockStatus_t,                                Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_UnlockedControlLockStatus,  1,             NotFault) \
   ENTRY(Erd_ClockTime,                                     0x0005, TimeHms_t,                                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TemperatureDisplayUnits,                       0x0007, TemperatureDisplayUnit_t,                           Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ApplianceType,                                 0x0008, ApplianceType_t,                                    Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SabbathModeEnable,                             0x0009, bool,                                               Swap_No,  Io_All,  Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ReadyToEnterBootLoader,                        0x0030, ReadyToEnterBootLoaderState_t,                      Swap_No,  Io_All,  Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BuildNumber,                                   0x0031, uint32_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Reset,                                         0x0032, uint8_t,                                            Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AppliancePersonality,                          0x0035, AppliancePersonality_t,                             Swap_Yes, Io_None, Sub_N, PersonalityEeprom,      NonVolatileDataSourceDefaultData_DefaultPersonality,         1,             NotFault) \
   ENTRY(Erd_SupportedImageTypes,                           0x0038, uint8_t,                                            Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BootLoaderVersion,                             0x0039, Version_t,                                          Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ApplicationVersion,                            0x003A, Version_t,                                          Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ParametricVersion,                             0x003B, Version_t,                                          Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AuxiliaryVersion,                              0x003C, Version_t,                                          Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_EnhancedSabbathModeSupported,                  0x004E, bool,                                               Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_EnhancedSabbathModeEnable,                     0x004F, bool,                                               Swap_No,  Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_GitHash,                                       0x007F, GitHash_t,                                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsApiVersion,                  0x008F, ServiceDiagnosticsApiVersion_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsRevision3Manifest,           0x0090, ServiceDiagnosticsRevision3Manifest_t,              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ApplianceApiManifest,                          0x0092, ApplianceApiManifest_t,                             Swap_Yes, Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ApplianceApiApplianceTypeManifest1,            0x0093, ApplianceFeatureApiManifest_t,                      Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ApplianceApiApplianceTypeManifest2,            0x0094, ApplianceFeatureApiManifest_t,                      Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ResetCount,                                    0x0700, ResetCount_t,                                       Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ResetReason,                                   0x0701, ResetReason_t,                                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SecondsSinceLastReset,                         0x0702, SecondsSinceLastReset_t,                            Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ProgramCounterAddressAtLastUassert,            0x0703, ProgramCounterAddress_t,                            Swap_Yes, Io_O,    Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   \
   ENTRY(Erd_PrimaryUiSignOfLife,                           0x0900, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_UiSoundLevelRequest,                           0x0902, uint8_t,                                            Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_UiSoundLevelState,                             0x0903, uint8_t,                                            Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSoundDefaultData_LowSoundLevel,               1,             NotFault) \
   ENTRY(Erd_NumberOfSoundLevelSettings,                    0x0904, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_WaterFilterType,                               0x1019, WaterFilterType_t,                                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_RefrigeratorModel,                             0x101D, RefrigeratorModel_t,                                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_CumulativeWaterDispensedOunces,                0x1060, uint32_t,                                           Swap_Yes, Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   \
   ENTRY(Erd_FreshFoodSetpointRequest,                      0x1200, Setpoint_t,                                         Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpointRangeData,                    0x1201, UserSetpointRangeData_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpointStatus,                       0x1202, Setpoint_t,                                         Swap_No,  Io_None, Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_Int8Max,                    1,             NotFault) \
   ENTRY(Erd_FreezerSetpointRequest,                        0x1203, Setpoint_t,                                         Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpointRangeData,                      0x1204, UserSetpointRangeData_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpointStatus,                         0x1205, Setpoint_t,                                         Swap_No,  Io_None, Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_Int8Max,                    1,             NotFault) \
   ENTRY(Erd_DispenseSelectionRequest,                      0x1206, DispenseSelection_t,                                Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispenseSelectionStatus,                       0x1207, DispenseSelection_t,                                Swap_No,  Io_None, Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_DispenseSelection,          1,             NotFault) \
   ENTRY(Erd_DispensingRequest,                             0x1208, DispensingRequest_t,                                Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispensingRequestStatus,                       0x1209, DispensingRequestStatus_t,                          Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_CoolingOffRequest,                             0x120A, CoolingSystemRequest_t,                             Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CoolingOffStatus,                              0x120B, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_TurboCoolOnOffRequest,                         0x120C, TurboModeRequest_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TurboCoolOnOffStatus,                          0x120D, bool,                                               Swap_No,  Io_None, Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_RightSideFreshFoodDoorStatusResolved,          0x120E, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreezerDoorStatusResolved,             0x120F, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_EnableRequest,                       0x1210, IceMakerEnableRequest_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_EnableStatus,                        0x1211, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,                1,             NotFault) \
   ENTRY(Erd_IceMaker0_FullStatus,                          0x1212, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_TypeInformation,                     0x1213, IceMakerTypeInformation_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_WaterFilterState,                              0x1215, WaterFilterState_t,                                 Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_EnableRequest,                       0x1216, IceMakerEnableRequest_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_EnableStatus,                        0x1217, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,                1,             NotFault) \
   ENTRY(Erd_IceMaker1_FullStatus,                          0x1218, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_TypeInformation,                     0x1219, IceMakerTypeInformation_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TurboFreezeOnOffRequest,                       0x121A, TurboModeRequest_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TurboFreezeOnOffStatus,                        0x121B, bool,                                               Swap_No,  Io_None, Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   \
   ENTRY(Erd_DimmableLight0PercentLevelRequest,             0x121E, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DimmableLight0PercentLevelStatus,              0x121F, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_MaxLightBrightness,         1,             NotFault) \
   ENTRY(Erd_DimmableLight0Configuration,                   0x1220, UserAllowableInteriorLightingBitmap_t,              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_PresenceSensingEnableRequest,                  0x1222, PresenceSensingEnableRequest_t,                     Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_PresenceSensingEnableStatus,                   0x1223, bool,                                               Swap_No,  Io_None, Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,                1,             NotFault) \
   ENTRY(Erd_PresenceSensedActivatesRecessLightRequest,     0x1224, PresenceActivatesRecessLightEnableRequest_t,        Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_PresenceSensedActivatesRecessLightStatus,      0x1225, bool,                                               Swap_No,  Io_None, Sub_Y, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   \
   ENTRY(Erd_PresenceDetected,                              0x1226, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DoorAlarmEnableRequest,                        0x1227, DoorAlarmEnableRequest_t,                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DoorAlarmEnableStatus,                         0x1228, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,                1,             NotFault) \
   ENTRY(Erd_NightTimeSnackModeEnableRequest,               0x1229, SnackModeEnableRequest_t,                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NightTimeSnackModeEnableStatus,                0x122A, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   \
   ENTRY(Erd_DispenserRecessHeaterDutyCyclePercent,         0x122C, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SecondarySignOfLife,                           0x122D, uint8_t,                                            Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NotificationsBitmap,                           0x122E, NotificationsBitmap_t,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispensingInhibitedReason,                     0x122F, DispensingInhibitedReasonBitmap_t,                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_EnableRequest,                       0x1230, IceMakerEnableRequest_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_EnableStatus,                        0x1231, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,                1,             NotFault) \
   ENTRY(Erd_IceMaker2_FullStatus,                          0x1232, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_TypeInformation,                     0x1233, IceMakerTypeInformation_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_HasBeenDiscovered,                   0x1234, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_IceMaker1_HasBeenDiscovered,                   0x1235, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_IceMaker2_HasBeenDiscovered,                   0x1236, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_WaterFilterRemainingUsage,                     0x1237, WaterFilterRemainingUsage_t,                        Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_WaterFilterUsageSinceExpiration,               0x1238, WaterFilterUsageSinceExpiration_t,                  Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_EnableDemoModeRequest,                         0x1239, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_EnableDemoModeStatus,                          0x123A, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   \
   ENTRY(Erd_ServiceModeTestNumberMaximum,                  0x123B, ServiceModeTestNumber_t,                            Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceModeTestNumberRequest,                  0x123C, ServiceModeTestRequest_t,                           Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceModeTestNumberStatus,                   0x123D, ServiceModeTestStatus_t,                            Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_RecessLightOnRequest,                          0x1255, RecessLightOnRequest_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RecessLightOnStatus,                           0x1256, bool,                                               Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsEntityLocation,              0xE000, Erd_t,                                              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsEntityManifest,              0xE100, ServiceDiagnosticsRevision3EntityManifest_t,        Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsEntityCycleCount,            0xE101, uint32_t,                                           Swap_Yes, Io_All,  Sub_N, NvCycleHistory,         NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsRunTimeInMinutes,            0xE102, ApplianceRunTimeMinutes_t,                          Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableStatus,            0xE103, ServiceDiagnosticsRevision3TableStatus_t,           Swap_Yes, Io_None, Sub_N, NvFaultSnapshot,        NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableClear,             0xE104, ServiceDiagnosticsRevision3TableClearRequest_t,     Swap_No,  Io_O,    Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry0,            0xE105, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range_S, Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry1,            0xE106, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry2,            0xE107, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry3,            0xE108, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry4,            0xE109, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry5,            0xE10A, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry6,            0xE10B, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry7,            0xE10C, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry8,            0xE10D, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry9,            0xE10E, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry10,           0xE10F, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry11,           0xE110, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry12,           0xE111, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry13,           0xE112, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry14,           0xE113, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry15,           0xE114, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry16,           0xE115, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry17,           0xE116, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry18,           0xE117, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableEntry19,           0xE118, ServiceDiagnosticsRevision3FaultTableEntry_t,       Swap_Range_E, Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot0,         0xE119, FaultSnapshot_t,                                    Swap_Range_S, Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot1,         0xE11A, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot2,         0xE11B, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot3,         0xE11C, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot4,         0xE11D, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot5,         0xE11E, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot6,         0xE11F, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot7,         0xE120, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot8,         0xE121, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot9,         0xE122, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot10,        0xE123, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot11,        0xE124, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot12,        0xE125, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot13,        0xE126, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot14,        0xE127, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot15,        0xE128, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot16,        0xE129, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot17,        0xE12A, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot18,        0xE12B, FaultSnapshot_t,                                    Swap_Range,   Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshot19,        0xE12C, FaultSnapshot_t,                                    Swap_Range_E, Io_None, Sub_N, Virtual,            NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceStatus,         0xE12D, ServiceDiagnosticsRevision3SequenceStatus_t,        Swap_Yes,   Io_None, Sub_N, NvFaultSnapshot,      NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceClear,          0xE12E, ServiceDiagnosticsRevision3SequenceClearRequest_t,  Swap_No,    Io_O,    Sub_N, Virtual,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry0,         0xE12F, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range_S, Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry1,         0xE130, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry2,         0xE131, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry3,         0xE132, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry4,         0xE133, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry5,         0xE134, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry6,         0xE135, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultSequenceEntry7,         0xE136, ServiceDiagnosticsRevision3FaultSequenceEntry_t,    Swap_Range_E, Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistorySequenceStatus,  0xE137, ServiceDiagnosticsRevision3SequenceStatus_t,        Swap_Yes,   Io_None, Sub_N, NvFaultSnapshot,      NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistorySequenceClear,   0xE138, ServiceDiagnosticsRevision3SequenceClearRequest_t,  Swap_No,    Io_O,    Sub_N, Virtual,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord0,                0xE139, CycleHistoryRecord_t,                               Swap_Range_S, Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord1,                0xE13A, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord2,                0xE13B, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord3,                0xE13C, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord4,                0xE13D, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord5,                0xE13E, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord6,                0xE13F, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord7,                0xE140, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord8,                0xE141, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DiagnosticsCycleHistoryRecord9,                0xE142, CycleHistoryRecord_t,                               Swap_Range,   Io_None, Sub_N, NvCycleHistory,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsCycleHistoryAddEntry,        0xE143, CycleHistoryRecord_t,                               Swap_Range_E, Io_O,    Sub_N, Virtual,            NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   \
   ENTRY(Erd_OldApplicationVersion,                         0xF000, Version_t,                                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimerModuleDiagnosticsEnable,                  0xF001, bool,                                               Swap_No,  Io_All,  Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimerModuleDiagnosticsResult,                  0xF002, TimerModuleDiagnosticsResults_t,                    Swap_Yes, Io_O,    Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ParametricDataImageCrc,                        0xF003, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SomeData,                                      0xF004, uint32_t,                                           Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Code,                       1,             NotFault) \
   ENTRY(Erd_ApplianceRunTimeInMinutesUpdatedHourly,        0xF005, ApplianceRunTimeMinutes_t,                          Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_StackPercentFree,                              0xF006, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ParametricShortGitHash,                        0xF007, ShortGitHash_t,                                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_HeartbeatTick,                                 0xF008, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_PersonalityParametricData,                     0xF009, PersonalityParametricData_t *,                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NonVolatileDataSourceCacheSyncState,           0xF00A, bool,                                               Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimeAcceleration_Enable,                       0xF00B, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimeAcceleration_Ticks,                        0xF00C, uint32_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimeAcceleration_CompleteSignal,               0xF00D, Signal_t,                                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_SystemTickInterrupt,                           0xF010, I_Interrupt_t *,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimeSource,                                    0xF011, I_TimeSource_t *,                                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimerModule,                                   0xF012, TimerModule_t *,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CrcCalcTable,                                  0xF013, I_Crc16Calculator_t *,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gea2MessageEndpoint,                           0xF014, I_Gea2MessageEndpoint_t *,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_JumpToBootLoaderAction,                        0xF015, I_Action_t *,                                       Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SystemResetAction,                             0xF016, I_Action_t *,                                       Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ExternalDataSource,                            0xF017, I_DataSource_t *,                                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FaultCodeTableInputGroup,                      0xF018, I_InputGroup_t *,                                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BufferedUartA,                                 0xF019, I_BufferedUart_t *,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BufferedUartC,                                 0xF01A, I_BufferedUart_t *,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BufferedUartD,                                 0xF01B, I_BufferedUart_t *,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BufferedUartE,                                 0xF01C, I_BufferedUart_t *,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gea2Uart,                                      0xF01D, I_Uart_t *,                                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FastTickInterrupt,                             0xF01E, I_Interrupt_t *,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Grid_BlockNumber,                              0xF030, GridBlockNumber_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Grid_PreviousBlocks,                           0xF031, PreviousGridBlockNumbers_t,                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Grid_CalculatedGridLines,                      0xF032, CalculatedGridLines_t,                              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanAntiSweatBehaviorEnabledByGrid, 0xF033, bool,                                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_GridArea,                                      0xF034, GridArea_t,                                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_GridOverrideSignal,                            0xF035, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_GridOverrideEnable,                            0xF036, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_GridBlockNumberOverrideRequest,                0xF037, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_GridBlockNumberOverrideValue,                  0xF038, GridBlockNumber_t,                                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_GridBlockNumberOverrideResolved,               0xF039, GridBlockNumber_t,                                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentGridBlockNumber,         0xF03A, GridBlockNumber_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SingleEvaporatorPulldownActive,                0xF03B, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorTripMitigationActive,                0xF03C, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CondenserFanAntiSweatBehaviorEnabledByGrid,    0xF03D, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanAntiSweatBehaviorEnabledByGrid,  0xF03E, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DelayConvertibleCompartmentCooling,            0xF03F, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CoolConvertibleCompartmentBeforeOff,           0xF040, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DelayedConvertibleCompartmentCoolingLowSpeed,  0xF041, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodPulldownOffsetEnabled,                0xF042, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_MaxValveTimeInPosAEnabled,                     0xF043, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CoolingSpeed,                                  0xF044, CoolingSpeed_t,                                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CoolingMode,                                   0xF045, CoolingMode_t,                                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_PulldownInMediumCompressorSpeedEnabled,        0xF046, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimeInMinutesInValvePositionB,                 0xF047, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CrossAmbientHysteresisAdjustmentInDegFx100,    0xF048, TemperatureDegFx100_t,                              Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_EnhancedSabbathModeHsmState,                   0xF049, EnhancedSabbathModeHsmState_t,                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_DamperFreezePreventionFsmState,                0xF04A, DamperFreezePreventionFsmState_t,                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentState,                   0xF04B, ConvertibleCompartmentStateType_t,                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorControllerSpeedRequest,              0xF04C, CompressorSpeed_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorState,                               0xF04D, CompressorState_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_HasConvertibleCompartment,                     0xF04E, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_EnhancedSabbathIsRequestingDefrost,            0xF04F, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_EnhancedSabbathStageFreshFoodCoolingIsActive,  0xF050, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_EnhancedSabbathStageFreezerCoolingIsActive,    0xF051, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_EnhancedSabbathRunTimeInMinutes,               0xF052, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NumberOfConvertibleCompartmentDefrosts,        0xF053, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NumberOfFreshFoodAbnormalDefrosts,             0xF054, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NumberOfConvertibleCompartmentAbnormalDefrosts,0xF055, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeaterOnTimeInMinutes, 0xF056, uint16_t,                                      Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_DefrostState,                                  0xF060, DefrostState_t,                                     Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_DefrostHsmState,                               0xF061, DefrostHsmState_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_WaitingToDefrost,                              0xF062, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Defrosting,                                    0xF063, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CurrentDefrostType,                            0xF064, DefrostType_t,                                      Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_FullDefrost,                1,             NotFault) \
   ENTRY(Erd_NextDefrostTypeOverride,                       0xF065, DefrostType_t,                                      Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_FullDefrost,                1,             NotFault) \
   ENTRY(Erd_NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost,   0xF066, uint8_t,                                        Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_NumberOfFreezerDefrosts,                       0xF067, uint16_t,                                           Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_NumberOfSecondaryOnlyDefrosts,                 0xF068, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimeBetweenDefrostsInMinutes,                  0xF069, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_WaitingForDefrostTimeInSeconds,                0xF06A, uint32_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DefrostCompressorOnTimeInSeconds,              0xF06B, uint32_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds,  0xF06C, uint32_t,                                       Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RightSideFreshFoodScaledDoorAccelerationInSeconds, 0xF06D, uint32_t,                                       Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DoorInDoorScaledDoorAccelerationInSeconds,         0xF06E, uint32_t,                                       Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerScaledDoorAccelerationInSeconds,            0xF06F, uint32_t,                                       Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds, 0xF070, uint32_t,                        Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds,   0xF071, uint32_t,                        Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ReadyToDefrost,                                0xF072, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ReadyToDefrostHsmState,                        0xF073, ReadyToDefrostHsmState_t,                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TimeThatPrechillConditionsAreMetInMinutes,     0xF074, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerDefrostHeaterOnTimeInMinutes,           0xF076, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,        0xF077, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,      0xF078, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes, 0xF079, uint8_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerDefrostWasAbnormal,                     0xF07A, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_NumberOfFreezerAbnormalDefrosts,               0xF07B, uint16_t,                                           Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_FreshFoodDefrostWasAbnormal,                   0xF07C, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostWasAbnormal,      0xF07D, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_PostDwellCompletionSignal,                     0xF07F, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_InvalidFreezerEvaporatorThermistorDuringDefrost,0xF080,bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerFilteredTemperatureTooWarmOnPowerUp,    0xF081, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorIsOn,                                0xF082, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DisableDefrost,                                0xF083, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DefrostTestRequest,                            0xF084, DefrostTestRequest_t,                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DefrostTestStateRequest,                       0xF085, DefrostTestStateRequestMessage_t,                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DefrostTestRequestStatus,                      0xF086, DefrostTestRequest_t,                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DontSkipDefrostPrechill,                       0xF087, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts, 0xF088, bool,                                      Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SabbathIsReadyToDefrost,                       0xF089, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SabbathTimeBetweenDefrostsInMinutes,           0xF08A, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SabbathWaitingForDefrostTimeInMinutes,         0xF08B, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeaterOnTimeInMinutes,         0xF08C, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_LeftSideFreshFoodDoorStatus,                   0xF0A0, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DoorInDoorIsOpen,                              0xF0A1, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDoorStatus,              0xF0A2, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentAsFreshFoodDoorStatus,   0xF0A3, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentAsFreezerDoorStatus,     0xF0A4, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AllFreshFoodDoorsAreClosed,                    0xF0A5, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreshFoodDoorStatusResolved,           0xF0A6, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DoorInDoorIsOpenResolved,                      0xF0A7, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RightSideFreshFoodDoorStatus,                  0xF0A8, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreezerDoorStatus,                     0xF0A9, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDoorStatusResolved,      0xF0AA, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BottomFreezerDrawerStatus,                     0xF0AB, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BottomFreezerDrawerStatusResolved,             0xF0AC, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_MoldThermistor_AdcCount,             0xF0B0, AdcCounts_t,                                        Swap_Range_S, Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MoldThermistor_AdcCount,             0xF0B1, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMakerCabinetThermistor_AdcCount,            0xF0B2, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodThermistor_AdcCount,                  0xF0B3, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerThermistor_AdcCount,                    0xF0B4, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_AdcCount,              0xF0B5, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_AdcCount,                0xF0B6, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_AdcCount, 0xF0B7, AdcCounts_t,                                     Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientThermistor_AdcCount,                    0xF0B8, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientHumidity_AdcCount,                      0xF0B9, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_AdcCount, 0xF0BA, AdcCounts_t,                                        Swap_Range_E, Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MoldThermistor_AdcCount,             0xF0BB, AdcCounts_t,                                        Swap_Yes,     Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFood_UnfilteredTemperatureInDegFx100,                    0xF0C0, TemperatureDegFx100_t,               Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_UnfilteredTemperatureInDegFx100,                      0xF0C1, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvap_UnfilteredTemperatureInDegFx100,                  0xF0C2, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_UnfilteredTemperatureInDegFx100,                      0xF0C3, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MoldThermistor_UnfilteredTemperatureInDegFx100,     0xF0C4, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MoldThermistor_UnfilteredTemperatureInDegFx100,     0xF0C5, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientHumidity_UnfilteredRelativeHumidityPercentx100,        0xF0C6, RelativeHumidityPercentx100_t,       Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MoldThermistor_UnfilteredTemperatureInDegFx100,     0xF0C7, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvap_UnfilteredTemperatureInDegFx100,                0xF0C8, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinet_UnfilteredTemperatureInDegFx100,0xF0C9, TemperatureDegFx100_t,               Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFood_FilteredTemperatureInDegFx100,                      0xF0D0, TemperatureDegFx100_t,               Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperatureInDegFx100,                        0xF0D1, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureInDegFx100,                    0xF0D2, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalTemperatureInDegFx100,                0xF0D3, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MoldThermistor_FilteredTemperatureInDegFx100,       0xF0D4, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MoldThermistor_FilteredTemperatureInDegFx100,       0xF0D5, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientHumidity_FilteredInternalRelativeHumidityPercentx100,  0xF0D6, RelativeHumidityPercentx100_t,       Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MoldThermistor_FilteredTemperatureInDegFx100,       0xF0D7, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartment_FilteredTemperatureResolvedInDegFx100, 0xF0D8, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvap_FilteredTemperatureInDegFx100,                  0xF0D9, TemperatureDegFx100_t,               Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinet_FilteredTemperatureInDegFx100,  0xF0DA, TemperatureDegFx100_t,               Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Ambient_WindowAveragedTemperatureInDegFx100,   0xF0E0, TemperatureDegFx100_t,                              Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_LongTermAverageInDegFx100,           0xF0E1, TemperatureDegFx100_t,                              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_LongTermAverageInDegFx100,             0xF0E2, TemperatureDegFx100_t,                              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFood_ResetThermalShiftOffsetSignal,       0xF0E7, Signal_t,                                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_ResetThermalShiftOffsetSignal,         0xF0E8, Signal_t,                                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerSetpoint_TemperatureBounds,             0xF0ED, SetpointZoneTemperatureBounds_t,                    Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_TemperatureBounds,           0xF0EE, SetpointZoneTemperatureBounds_t,                    Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_TemperatureBounds, 0xF0EF, SetpointZoneTemperatureBounds_t,                 Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerEvaporatorThermistorIsValid,            0xF0F0, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvaporatorThermistorIsValid,          0xF0F1, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvaporatorThermistorIsValid, 0xF0F2, bool,                                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_ThermistorIsValid,                     0xF0F3, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_ThermistorIsValid,                   0xF0F4, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_ThermistorIsValid,                     0xF0F5, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MoldThermistorIsValid,               0xF0F6, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MoldThermistorIsValid,               0xF0F7, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_HumiditySensorIsValid,                 0xF0F8, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MoldThermistorIsValid,               0xF0F9, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistorIsValid,0xF0FA, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFood_AdjustedSetpointInDegFx100,             0xF0FF, TemperatureDegFx100_t,                           Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_AdjustedSetpointInDegFx100,               0xF100, TemperatureDegFx100_t,                           Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100, 0xF101, TemperatureDegFx100_t,                           Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_AdjustedSetpointWithoutShiftInDegFx100,   0xF102, TemperatureDegFx100_t,                           Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFood_ResolvedSetpointInDegFx100,          0xF105, TemperatureDegFx100_t,                              Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_CabinetOffsetInDegFx100,             0xF106, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_SetpointOffsetInDegFx100,            0xF107, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_CrossAmbientOffsetInDegFx100,        0xF108, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_PulldownOffsetInDegFx100,            0xF109, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_ThermalShiftInDegFx100,              0xF10A, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100, 0xF10B, TemperatureDegFx100_t,                          Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_HighAmbientOffsetInDegFx100,         0xF10C, TemperatureDegFx100_t,                              Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Freezer_ResolvedSetpointInDegFx100,            0xF110, TemperatureDegFx100_t,                              Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_CabinetOffsetInDegFx100,               0xF111, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_CrossAmbientOffsetInDegFx100,          0xF112, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_ThermalShiftInDegFx100,                0xF113, TemperatureDegFx100_t,                              Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100, 0xF114, TemperatureDegFx100_t,                            Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_HighAmbientOffsetInDegFx100,           0xF115, TemperatureDegFx100_t,                              Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerSetpointZone,                           0xF120, SetpointZone_t,                                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpointZone,                         0xF121, SetpointZone_t,                                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TurboCoolOnTimeInMinutes,                      0xF122, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TurboFreezeOnTimeInMinutes,                    0xF123, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Freezer_IceRateIsActive,                       0xF12A, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerIceRateTriggerSignal,                   0xF12B, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMakerEnabledByGrid,                         0xF12D, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_EnabledResolved,                     0xF12E, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_FeelerArmMonitoringRequest,          0xF133, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FeelerArmIsReadyToEnterHarvest,      0xF134, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFood_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100, 0xF135, TemperatureDegFx100_t,       Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_AveragedTemperatureOverrideRequest,                         0xF137, bool,                        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Freezer_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100,   0xF138, TemperatureDegFx100_t,       Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_AveragedTemperatureOverrideRequest,                           0xF139, bool,                        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_FreezeIntegrationCount,                         0xF13A, uint32_t,                                Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FreezeIntegrationCount,                         0xF13B, uint32_t,                                Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FreezeIntegrationCount,                         0xF13C, uint32_t,                                Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_HarvestCountIsReadyToHarvest,                   0xF13D, bool,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_HarvestCountIsReadyToHarvest,                   0xF13E, bool,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_HarvestCountIsReadyToHarvest,                   0xF13F, bool,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MotorActionResult,                              0xF140, IceMakerMotorActionResult_t,             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MotorActionResult,                              0xF141, IceMakerMotorActionResult_t,             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MotorActionResult,                              0xF142, IceMakerMotorActionResult_t,             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_TestRequest,                                    0xF146, IceMakerTestRequest_t,                   Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_HarvestCountCalculationRequest,                 0xF147, bool,                                    Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_HarvestCountCalculationRequest,                 0xF148, bool,                                    Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_HarvestCountCalculationRequest,                 0xF149, bool,                                    Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_EnabledResolved,                     0xF14A, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_EnabledResolved,                     0xF14B, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_RakeControlRequest,                  0xF14C, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_RakeControlRequest,                  0xF14D, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_RakePosition,                        0xF14E, RakePosition_t,                                     Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_RakePosition,                        0xF14F, RakePosition_t,                                     Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_StateMachineState,                   0xF150, IceMakerStateMachineState_t,                        Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FeelerArmPosition,                   0xF151, FeelerArmPosition_t,                                Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MoldHeaterControlRequest,            0xF152, IceMakerMoldHeaterControlRequest_t,                 Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_RakeCompletedRevolution,             0xF153, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_SkipFillRequest,                     0xF154, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_RakeControlRequest,                  0xF155, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_RakePosition,                        0xF156, RakePosition_t,                                     Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_RakeHasNotBeenHome,                  0xF157, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FeelerArmHasBeenBucketFull,          0xF158, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_WaterFillMonitoringRequest,          0xF159, IceMakerWaterFillMonitoringRequest_t,               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_StopFillSignal,                      0xF15A, Signal_t,                                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FlowMeterWaterDispensedOzX100,       0xF15B, uint32_t,                                           Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FlowMeterMonitoringRequest,          0xF15C, FlowMeterMonitoringRequest_t,                       Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_WaterFillMonitoringRequest,          0xF15D, IceMakerWaterFillMonitoringRequest_t,               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_StopFillSignal,                      0xF15E, Signal_t,                                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FlowMeterWaterDispensedOzX100,       0xF15F, uint32_t,                                           Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FlowMeterMonitoringRequest,          0xF160, FlowMeterMonitoringRequest_t,                       Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_TestRequest,                         0xF161, IceMakerTestRequest_t,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_AugerMotorControllerFsmState,                  0xF162, AugerMotorControllerFsmState_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_PrivateDispensingRequest,                      0xF163, DispensingRequest_t,                                Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_PrivateDispensingResultStatus,                 0xF164, DispenseStatus_t,                                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AutofillSensorError,                           0xF168, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispensingDisabled,                            0xF169, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker2_WaterFillMonitoringRequest,          0xF16A, IceMakerWaterFillMonitoringRequest_t,               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_StopFillSignal,                      0xF16B, Signal_t,                                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FlowMeterWaterDispensedOzX100,       0xF16C, uint32_t,                                           Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FlowMeterMonitoringRequest,          0xF16D, FlowMeterMonitoringRequest_t,                       Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_TestRequest,                         0xF16E, IceMakerTestRequest_t,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_WaterFilterVolumeUsageInOuncesX100,            0xF170, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_WaterFilterCalendarUsageInSeconds,             0xF171, CalendarUsageInSeconds_t,                           Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_WaterFilterLifeStatus,                         0xF172, WaterFilterLifeStatus_t,                            Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_UnitLifetimeWaterVolumeUsageInOuncesX100,      0xF173, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_N, NvUsageProfile,         NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_LastDispensedWaterVolumeUsageInOuncesX100,     0xF176, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_UnitLifetimeDispensedWaterInOuncesX100,        0xF177, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_LastWaterVolumeUsageInOuncesX100,    0xF178, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_LastWaterVolumeUsageInOuncesX100,    0xF179, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_LastWaterVolumeUsageInOuncesX100,    0xF17A, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CurrentWaterFilterMonthTimeInMinutes,          0xF17B, uint16_t,                                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_PreviousWaterFilterVolumeUsageInOuncesX100,    0xF17C, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterReadWriteResult_RfidBoard,           0xF181, RfidFilterReadWriteResult_t,                        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterUid_RfidBoard,                       0xF182, RfidUid_t,                                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterIdentifier_RfidBoard,                0xF183, RfidFilterIdentifier_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterWaterVolumeUsageInOuncesX100_RfidBoard,  0xF184, VolumeInOuncesX100_t,                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterCalendarUsageInSeconds_RfidBoard,    0xF185, CalendarUsageInSeconds_t,                           Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterLeakDetectedTimeInSeconds_RfidBoard, 0xF186, LeakDetectedInSeconds_t,                            Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterNumberOfWriteOperations_RfidBoard,   0xF187, NumberOfWriteOperations_t,                          Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterStatus_RfidBoard,                    0xF188, FilterStatusRfidBoard_t,                            Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterInitializationErrorBitfield_RfidBoard,       0xF189, RfidFilterInitializationErrorBitfield_t,    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterLockByte_RfidBoard,                          0xF18A, uint8_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_RfidBoard,  0xF18B, RfidFilterNumberOfUnitsFilterHasBeenOn_t,   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterUnitSerialNumber_RfidBoard,                  0xF18C, UnitSerialNumber_t,                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterPreviousUnitSerialNumber_RfidBoard,          0xF18D, UnitSerialNumber_t,                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_RfidBoard, 0xF18E, RfidTwelveMonthsGallonUsage_t,          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_RfidFilterUid,                                 0xF18F, RfidUid_t,                                          Swap_No,  Io_None, Sub_N, NvRfid,                 NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_RfidFilterStatus,                              0xF190, RfidFilterStatus_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn,    0xF191, RfidFilterNumberOfUnitsFilterHasBeenOn_t,           Swap_No,  Io_None, Sub_N, NvRfid,                 NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_RfidFilterLockByte,                            0xF192, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterUnitSerialNumber,                    0xF193, UnitSerialNumber_t,                                 Swap_No,  Io_None, Sub_N, Converted,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFilterPreviousUnitSerialNumber,            0xF194, UnitSerialNumber_t,                                 Swap_No,  Io_None, Sub_N, NvRfid,                 NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons, 0xF195, RfidTwelveMonthsGallonUsage_t,                    Swap_No,  Io_None, Sub_N, NvRfid,                 NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   \
   ENTRY(Erd_RfidFilterDataRequest,                         0xF196, ReadWriteRequest_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidCommunicationControllerState,              0xF197, RfidCommunicationControllerState_t,                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RfidFaultRequest,                              0xF198, RfidFaultRequest_t,                                 Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_StateMachineState,                   0xF19A, IceMakerStateMachineState_t,                        Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_StateMachineState,                   0xF19B, IceMakerStateMachineState_t,                        Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_FeelerArmMonitoringRequest,          0xF19C, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FeelerArmIsReadyToEnterHarvest,      0xF19D, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FeelerArmMonitoringRequest,          0xF19E, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FeelerArmIsReadyToEnterHarvest,      0xF19F, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMakerFillInhibitedReason,                   0xF1A0, IceMakerFillInhibitedReasonBitmap_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_NewFilterInstalledSignal,                      0xF1A1, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BypassPlugInstalled,                           0xF1A2, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeakDetected,                                  0xF1A3, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FilterError,                                   0xF1A4, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_MinimumFreezeTimeCounterInMinutes,   0xF1A5, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MinimumFreezeTimeCounterInMinutes,   0xF1A6, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MinimumFreezeTimeCounterInMinutes,   0xF1A7, uint8_t,                                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MotorOperationState,                 0xF1A8, IceMakerMotorOperationState_t,                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MotorOperationState,                 0xF1A9, IceMakerMotorOperationState_t,                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MotorOperationState,                 0xF1AA, IceMakerMotorOperationState_t,                      Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MotorErrorReason,                    0xF1AB, IceMakerMotorErrorReason_t,                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MotorErrorReason,                    0xF1AC, IceMakerMotorErrorReason_t,                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MotorErrorReason,                    0xF1AD, IceMakerMotorErrorReason_t,                         Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MotorRequestedState,                 0xF1AE, IceMakerMotorRequestedState_t,                      Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MotorRequestedState,                 0xF1AF, IceMakerMotorRequestedState_t,                      Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MotorRequestedState,                 0xF1B0, IceMakerMotorRequestedState_t,                      Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_FeelerArmPosition,                   0xF1B1, FeelerArmPosition_t,                                Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FeelerArmPosition,                   0xF1B2, FeelerArmPosition_t,                                Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_CondenserFanSpeed_ResolvedVote,                0xF200, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CondenserFanSpeed_WinningVoteErd,              0xF201, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CondenserFanSpeed_FactoryServiceVote,          0xF202, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CondenserFanSpeed_CoolingSystemOffVote,        0xF203, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CondenserFanSpeed_DemoVote,                    0xF204, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CondenserFanSpeed_DefrostVote,                 0xF205, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CondenserFanSpeed_SabbathVote,                 0xF206, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CondenserFanSpeed_CompressorStartUpVote,       0xF207, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CondenserFanSpeed_GridVote,                    0xF208, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceCabinetFanSpeed_ResolvedVote,               0xF210, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_WinningVoteErd,             0xF211, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_FactoryServiceVote,         0xF212, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_CoolingSystemOffVote,       0xF213, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_DemoVote,                   0xF214, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_LoadOffDoorOpenVote,        0xF215, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceCabinetFanSpeed_DefrostVote,                0xF216, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodEvapFanSpeed_ResolvedVote,            0xF21A, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_WinningVoteErd,          0xF21B, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_FactoryServiceVote,      0xF21C, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_CoolingSystemOffVote,    0xF21D, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_DemoVote,                0xF21E, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_LoadOffDoorOpenVote,     0xF21F, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_DefrostVote,             0xF220, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_SabbathVote,             0xF221, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote,   0xF222, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFanSpeed_GridVote,                0xF223, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerEvapFanSpeed_ResolvedVote,              0xF22A, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_WinningVoteErd,            0xF22B, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_FactoryServiceVote,        0xF22C, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_CoolingSystemOffVote,      0xF22D, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_DemoVote,                  0xF22E, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_LoadOffDoorOpenVote,       0xF22F, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_DefrostVote,               0xF230, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_SabbathVote,               0xF231, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_CompressorStartUpVote,     0xF232, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_FreezerIceRateVote,        0xF233, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFanSpeed_GridVote,                  0xF234, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentEvapFanSpeed_ResolvedVote,         0xF23A, FanVotedSpeed_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapFanSpeed_WinningVoteErd,       0xF23B, WinningVoteErd_t,                         Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapFanSpeed_FactoryServiceVote,   0xF23C, FanVotedSpeed_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapFanSpeed_CoolingSystemOffVote, 0xF23D, FanVotedSpeed_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapFanSpeed_DemoVote,             0xF23E, FanVotedSpeed_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapFanSpeed_LoadOffDoorOpenVote,  0xF23F, FanVotedSpeed_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapFanSpeed_DefrostVote,          0xF240, FanVotedSpeed_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapFanSpeed_GridVote,             0xF241, FanVotedSpeed_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_DeliFanSpeed_ResolvedVote,                     0xF24A, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFanSpeed_WinningVoteErd,                   0xF24B, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFanSpeed_FactoryServiceVote,               0xF24C, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFanSpeed_CoolingSystemOffVote,             0xF24D, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFanSpeed_DemoVote,                         0xF24E, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFanSpeed_LoadOffDoorOpenVote,              0xF24F, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFanSpeed_DefrostVote,                      0xF250, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFanSpeed_GridVote,                         0xF251, FanVotedSpeed_t,                                    Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodBackWallLight_ResolvedVote,           0xF25A, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_WinningVoteErd,         0xF25B, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_FactoryVote,            0xF25C, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_EnhancedSabbathVote,    0xF25D, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_DoorVote,               0xF25E, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodTopLight_ResolvedVote,                0xF26A, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodTopLight_WinningVoteErd,              0xF26B, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodTopLight_FactoryVote,                 0xF26C, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodTopLight_EnhancedSabbathVote,         0xF26D, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodTopLight_DoorVote,                    0xF26E, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerBackWallLight_ResolvedVote,             0xF270, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerBackWallLight_WinningVoteErd,           0xF271, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerBackWallLight_FactoryVote,              0xF272, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerBackWallLight_EnhancedSabbathVote,      0xF273, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerBackWallLight_DoorVote,                 0xF274, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerTopLight_ResolvedVote,                  0xF27A, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerTopLight_WinningVoteErd,                0xF27B, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerTopLight_FactoryVote,                   0xF27C, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerTopLight_EnhancedSabbathVote,           0xF27D, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerTopLight_DoorVote,                      0xF27E, RampingPwmDutyCyclePercentageVote_t,                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IsolationWaterValve_ResolvedVote,              0xF27F, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IsolationWaterValve_WinningVoteErd,            0xF280, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IsolationWaterValve_FactoryVote,               0xF281, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IsolationWaterValve_DispensingVote,            0xF282, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IsolationWaterValve_IceMaker0Vote,             0xF283, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IsolationWaterValve_IceMaker1Vote,             0xF284, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IsolationWaterValve_IceMaker2Vote,             0xF285, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_WaterValve_ResolvedVote,             0xF286, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_WaterValve_WinningVoteErd,           0xF287, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_WaterValve_FactoryVote,              0xF288, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_WaterValve_IceMakerVote,             0xF289, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_WaterValve_ResolvedVote,             0xF28A, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_WaterValve_WinningVoteErd,           0xF28B, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_WaterValve_FactoryVote,              0xF28C, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_WaterValve_IceMakerVote,             0xF28D, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker2_WaterValve_ResolvedVote,             0xF28E, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_WaterValve_WinningVoteErd,           0xF28F, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_WaterValve_FactoryVote,              0xF290, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_WaterValve_IceMakerVote,             0xF291, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker2_FillTubeHeater_ResolvedVote,         0xF294, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FillTubeHeater_WinningVoteErd,       0xF295, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FillTubeHeater_FactoryVote,          0xF296, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FillTubeHeater_IceMakerVote,         0xF297, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FillTubeHeater_NonHarvestVote,       0xF298, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_DispenserWaterValve_ResolvedVote,              0xF299, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispenserWaterValve_WinningVoteErd,            0xF29A, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispenserWaterValve_FactoryVote,               0xF29B, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispenserWaterValve_DispensingVote,            0xF29C, WaterValveVotedState_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_HeaterRelay_ResolvedVote,            0xF29D, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_HeaterRelay_WinningVoteErd,          0xF29E, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_HeaterRelay_FactoryVote,             0xF29F, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_HeaterRelay_IceMakerVote,            0xF2A0, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_HeaterRelay_ResolvedVote,            0xF2A1, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_HeaterRelay_WinningVoteErd,          0xF2A2, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_HeaterRelay_FactoryVote,             0xF2A3, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_HeaterRelay_IceMakerVote,            0xF2A4, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker2_HeaterRelay_ResolvedVote,            0xF2A5, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_HeaterRelay_WinningVoteErd,          0xF2A6, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_HeaterRelay_FactoryVote,             0xF2A7, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_HeaterRelay_IceMakerVote,            0xF2A8, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_RakeMotor_ResolvedVote,              0xF2A9, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_RakeMotor_WinningVoteErd,            0xF2AA, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_RakeMotor_FactoryVote,               0xF2AB, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_RakeMotor_IceMakerVote,              0xF2AC, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_RakeMotor_ResolvedVote,              0xF2AD, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_RakeMotor_WinningVoteErd,            0xF2AE, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_RakeMotor_FactoryVote,               0xF2AF, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_RakeMotor_IceMakerVote,              0xF2B0, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker2_RakeMotor_ResolvedVote,              0xF2B1, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_RakeMotor_WinningVoteErd,            0xF2B2, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_RakeMotor_FactoryVote,               0xF2B3, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_RakeMotor_IceMakerVote,              0xF2B4, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_FillTubeHeater_ResolvedVote,         0xF2B5, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FillTubeHeater_WinningVoteErd,       0xF2B6, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FillTubeHeater_FactoryVote,          0xF2B7, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FillTubeHeater_IceMakerVote,         0xF2B8, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FillTubeHeater_NonHarvestVote,       0xF2B9, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_TwistMotor_ResolvedVote,             0xF2BA, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_TwistMotor_WinningVoteErd,           0xF2BB, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_TwistMotor_FactoryVote,              0xF2BC, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_TwistMotor_IceMakerVote,             0xF2BD, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_TwistMotor_ResolvedVote,             0xF2BE, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_TwistMotor_WinningVoteErd,           0xF2BF, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_TwistMotor_FactoryVote,              0xF2C0, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_TwistMotor_IceMakerVote,             0xF2C1, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker2_TwistMotor_ResolvedVote,             0xF2C2, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_TwistMotor_WinningVoteErd,           0xF2C3, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_TwistMotor_FactoryVote,              0xF2C4, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_TwistMotor_IceMakerVote,             0xF2C5, IceMakerMotorVotedState_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_FillTubeHeater_ResolvedVote,         0xF2C6, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FillTubeHeater_WinningVoteErd,       0xF2C7, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FillTubeHeater_FactoryVote,          0xF2C8, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FillTubeHeater_IceMakerVote,         0xF2C9, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FillTubeHeater_NonHarvestVote,       0xF2CA, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerSetpoint_ColdestSetpointWinningVote,    0xF2CF, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpoint_TurboFreezeVote,               0xF2D0, SetpointVotedTemperature_t,                         Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpoint_IceInDoorVote,                 0xF2D1, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpoint_FreezerIceMakerVote,           0xF2D2, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpoint_FreezerIceRateVote,            0xF2D3, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpoint_UserVote,                      0xF2D4, SetpointVotedTemperature_t,                         Swap_Range_E, Io_None, Sub_Y, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_AugerMotor_ResolvedVote,                       0xF2D5, AugerMotorVotedIceType_t,                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AugerMotor_WinningVoteErd,                     0xF2D6, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AugerMotor_FactoryVote,                        0xF2D7, AugerMotorVotedIceType_t,                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AugerMotor_DispensingVote,                     0xF2D8, AugerMotorVotedIceType_t,                           Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerSetpoint_ResolvedVote,                  0xF2D9, SetpointVotedTemperature_t,                         Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpoint_WinningVoteErd,                0xF2DA, WinningVoteErd_t,                                   Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpoint_FactoryVote,                   0xF2DB, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpoint_EnhancedSabbathVote,           0xF2DC, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerSetpoint_ColdestSetpointVote,           0xF2DE, SetpointVotedTemperature_t,                         Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodSetpoint_ColdestSetpointWinningVote,  0xF2DF, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_TurboCoolVote,               0xF2E0, SetpointVotedTemperature_t,                         Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_UserVote,                    0xF2E1, SetpointVotedTemperature_t,                         Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodSetpoint_ResolvedVote,                0xF2E3, SetpointVotedTemperature_t,                         Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_WinningVoteErd,              0xF2E4, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_FactoryVote,                 0xF2E5, SetpointVotedTemperature_t,                         Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_EnhancedSabbathVote,         0xF2E6, SetpointVotedTemperature_t,                         Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodSetpoint_ColdestSetpointVote,         0xF2E7, SetpointVotedTemperature_t,                         Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_ResolvedVote,   0xF2EA, SetpointVotedTemperature_t,                         Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_WinningVoteErd, 0xF2EB, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_FactoryVote,    0xF2EC, SetpointVotedTemperature_t,                         Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentSetpoint_UserVote,       0xF2ED, SetpointVotedTemperature_t,                         Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentHeater_ResolvedVote,     0xF2F0, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentHeater_WinningVoteErd,   0xF2F1, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentHeater_FactoryVote,      0xF2F2, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodDefrostHeater_ResolvedVote,           0xF2FA, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_WinningVoteErd,         0xF2FB, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_FactoryVote,            0xF2FC, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_CoolingSystemOffVote,   0xF2FD, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeater_DefrostVote,            0xF2FE, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerDefrostHeater_ResolvedVote,             0xF300, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_WinningVoteErd,           0xF301, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_FactoryVote,              0xF302, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_CoolingSystemOffVote,     0xF303, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerDefrostHeater_DefrostVote,              0xF304, HeaterVotedState_t,                                 Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeater_ResolvedVote,             0xF305, HeaterVotedState_t,                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeater_WinningVoteErd,           0xF306, WinningVoteErd_t,                    Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeater_FactoryVote,              0xF307, HeaterVotedState_t,                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeater_CoolingSystemOffVote,     0xF308, HeaterVotedState_t,                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeater_DefrostVote,              0xF309, HeaterVotedState_t,                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_DamperHeater_ResolvedVote,                     0xF30A, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperHeater_WinningVoteErd,                   0xF30B, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperHeater_FactoryVote,                      0xF30C, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperHeater_CoolingSystemOffVote,             0xF30D, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperHeater_DamperFreezePreventionVote,       0xF30E, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperHeater_DefrostHeaterSyncVote,            0xF30F, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodDamperPosition_ResolvedVote,                   0xF31A, DamperVotedPosition_t,                     Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_WinningVoteErd,                 0xF31B, WinningVoteErd_t,                          Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_FactoryVote,                    0xF31C, DamperVotedPosition_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_CoolingSystemOffVote,           0xF31D, DamperVotedPosition_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_EnhancedSabbathVote,            0xF31E, DamperVotedPosition_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_DamperFreezePreventionVote,     0xF31F, DamperVotedPosition_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_MaxOpenTimeVote,                0xF320, DamperVotedPosition_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_DefrostVote,                    0xF321, DamperVotedPosition_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDamperPosition_GridVote,                       0xF322, DamperVotedPosition_t,                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentDamperPosition_ResolvedVote,                   0xF323, DamperVotedPosition_t,        Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDamperPosition_WinningVoteErd,                 0xF324, WinningVoteErd_t,             Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDamperPosition_FactoryVote,                    0xF325, DamperVotedPosition_t,        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDamperPosition_CoolingSystemOffVote,           0xF326, DamperVotedPosition_t,        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDamperPosition_EnhancedSabbathVote,            0xF327, DamperVotedPosition_t,        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDamperPosition_DamperFreezePreventionVote,     0xF328, DamperVotedPosition_t,        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDamperPosition_MaxOpenTimeVote,                0xF329, DamperVotedPosition_t,        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDamperPosition_DefrostVote,                    0xF32A, DamperVotedPosition_t,        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDamperPosition_GridVote,                       0xF32B, DamperVotedPosition_t,        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_SealedSystemValvePosition_ResolvedVote,        0xF32C, SealedSystemValveVotedPosition_t,                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SealedSystemValvePosition_WinningVoteErd,      0xF32D, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SealedSystemValvePosition_FactoryVote,         0xF32E, SealedSystemValveVotedPosition_t,                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SealedSystemValvePosition_DefrostVote,         0xF32F, SealedSystemValveVotedPosition_t,                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SealedSystemValvePosition_GridVote,            0xF330, SealedSystemValveVotedPosition_t,                   Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_CompressorSpeed_ResolvedVote,                  0xF339, CompressorVotedSpeed_t,                             Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorSpeed_WinningVoteErd,                0xF33A, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorSpeed_FactoryVote,                   0xF33B, CompressorVotedSpeed_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorSpeed_CoolingSystemOffVote,          0xF33C, CompressorVotedSpeed_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorSpeed_EnhancedSabbathVote,           0xF33D, CompressorVotedSpeed_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorSpeed_DefrostVote,                   0xF33E, CompressorVotedSpeed_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorSpeed_GridVote,                      0xF33F, CompressorVotedSpeed_t,                             Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_RecessHeater_ResolvedVote,                     0xF340, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RecessHeater_WinningVoteErd,                   0xF341, WinningVoteErd_t,                                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RecessHeater_FactoryVote,                      0xF342, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RecessHeater_CoolingSystemOffVote,             0xF343, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RecessHeater_VariableAntiSweatVote,            0xF344, PercentageDutyCycleVote_t,                          Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_DisableMinimumCompressorTimes_ResolvedVote,        0xF350, BooleanVotedState_t,                            Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DisableMinimumCompressorTimes_WinningVoteErd,      0xF351, WinningVoteErd_t,                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DisableMinimumCompressorTimes_FactoryVote,         0xF352, BooleanVotedState_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DisableMinimumCompressorTimes_EnhancedSabbathVote, 0xF353, BooleanVotedState_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DisableMinimumCompressorTimes_DefrostVote,         0xF354, BooleanVotedState_t,                            Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_SensorsReadyToBeRead,                          0xF400, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SetpointResolverReady,                         0xF401, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentStateResolverReady,      0xF402, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientTemperaturePluginReady,                 0xF403, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_GridPluginReady,                               0xF404, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_PeriodicNvUpdaterReady,                        0xF405, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SabbathPluginReady,                            0xF406, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_GridLineCalculatorIsReady,                     0xF407, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DefrostParameterSelectorReady,                 0xF408, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, 0xF409, bool,                                             Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SetpointZonePluginReady,                       0xF40A, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AdjustedSetpointPluginReady,                   0xF40B, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_UserSetpointPluginReady,                       0xF40C, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_MoldHeaterControlRequest,            0xF410, IceMakerMoldHeaterControlRequest_t,                 Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MoldHeaterControlRequest,            0xF411, IceMakerMoldHeaterControlRequest_t,                 Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_RakeCompletedRevolution,             0xF412, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_RakeCompletedRevolution,             0xF413, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_SkipFillRequest,                     0xF414, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_SkipFillRequest,                     0xF415, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_RakeHasNotBeenHome,                  0xF416, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_RakeHasNotBeenHome,                  0xF417, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FeelerArmHasBeenBucketFull,          0xF418, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FeelerArmHasBeenBucketFull,          0xF419, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_RakeMotorRelay,                      0xF41A, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_RakeMotorRelay,                      0xF41B, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodEvap_FilteredTemperatureOverrideRequest,               0xF41D, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvap_FilteredTemperatureOverrideValueInDegFx100,       0xF41E, TemperatureDegFx100_t,             Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvap_FilteredTemperatureResolvedInDegFx100,            0xF41F, TemperatureDegFx100_t,             Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureOverrideRequest,          0xF420, bool,                                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureOverrideValueInDegFx100,  0xF421, TemperatureDegFx100_t,                    Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100,       0xF422, TemperatureDegFx100_t,                    Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Freezer_FilteredTemperatureOverrideRequest,              0xF423, bool,                                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100,      0xF424, TemperatureDegFx100_t,                    Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Freezer_FilteredTemperatureResolvedInDegFx100,           0xF425, TemperatureDegFx100_t,                    Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFood_FilteredTemperatureOverrideRequest,            0xF426, bool,                                     Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_FilteredTemperatureOverrideValueInDegFx100,    0xF427, TemperatureDegFx100_t,                    Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,         0xF428, TemperatureDegFx100_t,                    Swap_Range_E, Io_None, Sub_Y, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodThermistor_IsValidOverrideRequest,                     0xF429, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodThermistor_IsValidOverrideValue,                       0xF42A, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodThermistor_IsValidResolved,                            0xF42B, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerThermistor_IsValidOverrideRequest,                       0xF42C, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerThermistor_IsValidOverrideValue,                         0xF42D, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerThermistor_IsValidResolved,                              0xF42E, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodEvapThermistor_IsValidOverrideRequest,                 0xF42F, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_IsValidOverrideValue,                   0xF430, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapThermistor_IsValidResolved,                        0xF431, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreezerEvapThermistor_IsValidOverrideRequest,                   0xF432, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_IsValidOverrideValue,                     0xF433, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapThermistor_IsValidResolved,                          0xF434, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideRequest, 0xF435, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideValue,   0xF436, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved,        0xF437, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_AmbientThermistor_IsValidOverrideRequest,                       0xF438, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientThermistor_IsValidOverrideValue,                         0xF439, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientThermistor_IsValidResolved,                              0xF43A, bool,                              Swap_No, Io_None, Sub_Y, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideRequest,    0xF43B, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideValue,      0xF43C, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,           0xF43D, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_MoldThermistor_IsValidOverrideRequest,      0xF43E, bool,                                        Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MoldThermistor_IsValidOverrideValue,        0xF43F, bool,                                        Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MoldThermistor_IsValidResolved,             0xF440, bool,                                        Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_MoldThermistor_IsValidOverrideRequest,      0xF441, bool,                                        Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MoldThermistor_IsValidOverrideValue,        0xF442, bool,                                        Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MoldThermistor_IsValidResolved,             0xF443, bool,                                        Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker2_MoldThermistor_IsValidOverrideRequest,      0xF444, bool,                                        Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MoldThermistor_IsValidOverrideValue,        0xF445, bool,                                        Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MoldThermistor_IsValidResolved,             0xF446, bool,                                        Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Ambient_FilteredInternalTemperatureOverrideRequest,             0xF447, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalTemperatureOverrideValueInDegFx100,     0xF448, TemperatureDegFx100_t,             Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalTemperatureResolvedInDegFx100,          0xF449, TemperatureDegFx100_t,             Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_AmbientTemperature_IsValidResolved,                             0xF44A, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientHumidity_IsValidResolved,                                0xF44B, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_FilteredTemperatureResolvedInDegFx100,                  0xF44D, TemperatureDegFx100_t,             Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_FilteredHumidityResolvedPercentx100,                    0xF44E, RelativeHumidityPercentx100_t,     Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_MoldThermistor_FilteredTemperatureOverrideRequest,        0xF44F, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MoldThermistor_FilteredTemperatureOverrideValueInDegFx100,0xF450, TemperatureDegFx100_t,             Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_MoldThermistor_FilteredTemperatureResolvedInDegFx100,     0xF451, TemperatureDegFx100_t,             Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker1_MoldThermistor_FilteredTemperatureOverrideRequest,           0xF452, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MoldThermistor_FilteredTemperatureOverrideValueInDegFx100,   0xF453, TemperatureDegFx100_t,             Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_MoldThermistor_FilteredTemperatureResolvedInDegFx100,        0xF454, TemperatureDegFx100_t,             Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker2_MoldThermistor_FilteredTemperatureOverrideRequest,        0xF455, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MoldThermistor_FilteredTemperatureOverrideValueInDegFx100,0xF456, TemperatureDegFx100_t,             Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_MoldThermistor_FilteredTemperatureResolvedInDegFx100,     0xF457, TemperatureDegFx100_t,             Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_AmbientHumiditySensor_IsValidOverrideRequest,                   0xF458, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientHumiditySensor_IsValidOverrideValue,                     0xF459, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AmbientHumiditySensor_IsValidResolved,                          0xF45A, bool,                              Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Ambient_FilteredInternalHumidityOverrideRequest,                0xF45B, bool,                              Swap_No, Io_None, Sub_N, Ram,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalHumidityOverrideValueInPercentx100,     0xF45C, RelativeHumidityPercentx100_t,     Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Ambient_FilteredInternalHumidityResolvedInPercentx100,          0xF45D, RelativeHumidityPercentx100_t,     Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_RightSideFreshFoodDoorStatus_SabbathOverrideRequest,            0xF45E, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RightSideFreshFoodDoorStatus_SabbathOverrideValue,              0xF45F, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RightSideFreshFoodDoorStatus_TestingOverrideRequest,            0xF460, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RightSideFreshFoodDoorStatus_TestingOverrideValue,              0xF461, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_LeftSideFreezerDoorStatus_SabbathOverrideRequest,               0xF462, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreezerDoorStatus_SabbathOverrideValue,                 0xF463, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreezerDoorStatus_TestingOverrideRequest,               0xF464, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreezerDoorStatus_TestingOverrideValue,                 0xF465, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_LeftSideFreshFoodDoorStatus_SabbathOverrideRequest,             0xF466, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreshFoodDoorStatus_SabbathOverrideValue,               0xF467, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreshFoodDoorStatus_TestingOverrideRequest,             0xF468, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_LeftSideFreshFoodDoorStatus_TestingOverrideValue,               0xF469, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentDoorStatus_SabbathOverrideRequest,        0xF46A, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDoorStatus_SabbathOverrideValue,          0xF46B, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDoorStatus_TestingOverrideRequest,        0xF46C, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDoorStatus_TestingOverrideValue,          0xF46D, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_BottomFreezerDrawerStatus_SabbathOverrideRequest,               0xF46E, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BottomFreezerDrawerStatus_SabbathOverrideValue,                 0xF46F, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BottomFreezerDrawerStatus_TestingOverrideRequest,               0xF470, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_BottomFreezerDrawerStatus_TestingOverrideValue,                 0xF471, bool,                              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_DimmableLight1PercentLevelRequest,             0xF472, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DimmableLight1PercentLevelStatus,              0xF473, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_MaxLightBrightness,         1,             NotFault) \
   ENTRY(Erd_DimmableLight1Configuration,                   0xF474, UserAllowableInteriorLightingBitmap_t,              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DimmableLight2PercentLevelRequest,             0xF475, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DimmableLight2PercentLevelStatus,              0xF476, PercentageDutyCycle_t,                              Swap_No,  Io_None, Sub_N, NvUserSetting,          NonVolatileDataSourceDefaultData_MaxLightBrightness,         1,             NotFault) \
   ENTRY(Erd_DimmableLight2Configuration,                   0xF477, UserAllowableInteriorLightingBitmap_t,              Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodBackLightUserAllowableState,          0xF478, UserAllowableInteriorLightState_t,                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodTopLightUserAllowableState,           0xF479, UserAllowableInteriorLightState_t,                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerBackLightUserAllowableState,            0xF47A, UserAllowableInteriorLightState_t,                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerTopLightUserAllowableState,             0xF47B, UserAllowableInteriorLightState_t,                  Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ConvertibleCompartmentCabinet_FilteredTemperatureOverrideRequest,         0xF47C, bool,                    Swap_No,      Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinet_FilteredTemperatureOverrideValueInDegFx100, 0xF47D, TemperatureDegFx100_t,   Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100,      0xF47E, TemperatureDegFx100_t,   Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_CalculatedCondenserFanControl,                 0xF480, FanControl_t,                                       Swap_Range_S, Io_None, Sub_Y, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CalculatedIceCabinetFanControl,                0xF481, FanControl_t,                                       Swap_Range,   Io_None, Sub_Y, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CalculatedConvertibleCompartmentFanControl,    0xF482, FanControl_t,                                       Swap_Range,   Io_None, Sub_Y, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CalculatedDeliFanControl,                      0xF483, FanControl_t,                                       Swap_Range,   Io_None, Sub_Y, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CalculatedFreezerEvapFanControl,               0xF484, FanControl_t,                                       Swap_Range,   Io_None, Sub_Y, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CalculatedFreshFoodEvapFanControl,             0xF485, FanControl_t,                                       Swap_Range_E, Io_None, Sub_Y, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_GpioGroupInterface,                            0xF49F, I_GpioGroup_t *,                                    Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_HeartbeatLed,                                  0xF500, bool,                                               Swap_No,  Io_O,    Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RelayWatchdog,                                 0xF501, bool,                                               Swap_No,  Io_O,    Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorInverterDriver,                      0xF502, PwmFrequency_t,                                     Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_CompressorRelay,                               0xF503, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_FillTubeHeater,                      0xF504, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerDefrostHeaterRelay,                     0xF505, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperHeater,                                  0xF506, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_WaterValveRelay,                     0xF507, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_HeaterRelay,                         0xF508, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker0_RakeMotorRelay,                      0xF509, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AugerMotorDirection,                           0xF50A, bool,                                               Swap_No,  Io_None, Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AugerMotorPower,                               0xF50B, bool,                                               Swap_No,  Io_None, Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispenserValveRelay,                           0xF50C, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IsolationValveRelay,                           0xF50D, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_WaterValveRelay,                     0xF50E, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodDefrostHeaterRelay,                   0xF50F, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeaterRelay,      0xF510, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentHeater,                  0xF511, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FillTubeHeater,                      0xF512, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FillTubeHeater,                      0xF513, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_CondenserFan_Pwm,                              0xF51A, PwmDutyCycle_t,                                     Swap_Range_S, Io_O, Sub_N, MappedBsp,             NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceCabinetFan_Pwm,                             0xF51B, PwmDutyCycle_t,                                     Swap_Range,   Io_O, Sub_N, MappedBsp,             NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFan_Pwm,                 0xF51C, PwmDutyCycle_t,                                     Swap_Range,   Io_O, Sub_N, MappedBsp,             NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFan_Pwm,                                   0xF51D, PwmDutyCycle_t,                                     Swap_Range,   Io_O, Sub_N, MappedBsp,             NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFan_Pwm,                            0xF51E, PwmDutyCycle_t,                                     Swap_Range,   Io_O, Sub_N, MappedBsp,             NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFan_Pwm,                          0xF51F, PwmDutyCycle_t,                                     Swap_Range_E, Io_O, Sub_N, MappedBsp,             NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_CondenserFan_ActualRpm,                        0xF52A, FanRpm_t,                                           Swap_Range_S, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceCabinetFan_ActualRpm,                       0xF52B, FanRpm_t,                                           Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFan_ActualRpm,           0xF52C, FanRpm_t,                                           Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFan_ActualRpm,                             0xF52D, FanRpm_t,                                           Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFan_ActualRpm,                      0xF52E, FanRpm_t,                                           Swap_Range,   Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFan_ActualRpm,                    0xF52F, FanRpm_t,                                           Swap_Range_E, Io_None, Sub_N, Ram,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_CondenserFan_InputCaptureTime,                 0xF53A, InputCaptureMicroSeconds_t,                         Swap_Range_S, Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceCabinetFan_InputCaptureTime,                0xF53B, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentFan_InputCaptureTime,    0xF53C, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DeliFan_InputCaptureTime,                      0xF53D, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerEvapFan_InputCaptureTime,               0xF53E, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodEvapFan_InputCaptureTime,             0xF53F, InputCaptureMicroSeconds_t,                         Swap_Range,   Io_None, Sub_N, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AluminumIceMakerRake_InputCaptureTime,         0xF540, InputCaptureMicroSeconds_t,                         Swap_Range_E, Io_None, Sub_Y, MappedBsp,          NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FlowMeter_InputCaptureCount,                   0xF541, InputCaptureCounts_t,                               Swap_Yes,   Io_None, Sub_Y, MappedBsp,            NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker2_HeaterRelay,                         0xF542, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_WaterValveRelay,                     0xF544, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_HeaterRelay,                         0xF545, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_DamperStepperMotorPositionRequest,             0xF54A, StepperPositionRequest_t,                           Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperHomingRequest,                           0xF54B, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperCurrentPosition,                         0xF54C, DamperPosition_t,                                   Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperStepperMotorControlRequest,              0xF54D, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TwistIceMakerMotorControlRequest,              0xF54E, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperStepperMotorDriveEnable,                 0xF54F, bool,                                               Swap_No,  Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_TwistIceMakerMotorDriveEnable,                 0xF550, bool,                                               Swap_No,  Io_None, Sub_N, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SealedSystemValveStepperMotorPositionRequest,  0xF551, StepperPositionRequest_t,                           Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SealedSystemValveHomingRequest,                0xF552, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SealedSystemValveCurrentPosition,              0xF553, SealedSystemValvePosition_t,                        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SealedSystemValvePreviousPosition,             0xF554, SealedSystemValvePosition_t,                        Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SealedSystemValveStepperMotorControlRequest,   0xF555, bool,                                               Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_IceMaker0_FillTubeHeater_Pwm,                  0xF55A, PwmDutyCycle_t,                                     Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodBackWallLight_RampingPwm,             0xF55B, RampingPwmDutyCycle_t,                              Swap_Yes, Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreshFoodTopLight_RampingPwm,                  0xF55C, RampingPwmDutyCycle_t,                              Swap_Yes, Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerTopLight_RampingPwm,                    0xF55D, RampingPwmDutyCycle_t,                              Swap_Yes, Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DamperHeater_Pwm,                              0xF55E, PwmDutyCycle_t,                                     Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_FreezerBackWallLight_RampingPwm,               0xF55F, RampingPwmDutyCycle_t,                              Swap_Yes, Io_None, Sub_Y, MappedBsp,              NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker1_FillTubeHeater_Pwm,                  0xF561, PwmDutyCycle_t,                                     Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_IceMaker2_FillTubeHeater_Pwm,                  0xF562, PwmDutyCycle_t,                                     Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ConvertibleCompartmentHeater_Pwm,              0xF563, PwmDutyCycle_t,                                     Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Pwm_PWM_25K_00,                                0xF56F, PwmDutyCycle_t,                                     Swap_Range_S, Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Pwm_PWM_25K_01,                                0xF570, PwmDutyCycle_t,                                     Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Pwm_PWM_25K_02,                                0xF571, PwmDutyCycle_t,                                     Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Pwm_PWM_25K_03,                                0xF572, PwmDutyCycle_t,                                     Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Pwm_PWM_25K_04,                                0xF573, PwmDutyCycle_t,                                     Swap_Range_E, Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RampingLedPwm_PWM_200_00,                      0xF574, RampingPwmDutyCycle_t,                              Swap_Range_S, Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RampingLedPwm_PWM_200_01,                      0xF575, RampingPwmDutyCycle_t,                              Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RampingLedPwm_PWM_200_02,                      0xF576, RampingPwmDutyCycle_t,                              Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_RampingLedPwm_PWM_200_03,                      0xF577, RampingPwmDutyCycle_t,                              Swap_Range_E, Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_InputCapture_CAPTURE_00,                       0xF58F, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_01,                       0xF590, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_02,                       0xF591, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_03,                       0xF592, InputCaptureCounts_t,                               Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_04,                       0xF593, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_InputCapture_CAPTURE_05,                       0xF594, InputCaptureMicroSeconds_t,                         Swap_Yes, Io_None, Sub_N, Bsp,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Adc_ADC_0,                                     0xF59A, AdcCounts_t,                                        Swap_Range_S, Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ADC_1,                                     0xF59B, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_475K_TH_00,                         0xF59C, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_00,                          0xF59D, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_01,                          0xF59E, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_02,                          0xF59F, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_03,                          0xF600, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_04,                          0xF601, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_05,                          0xF602, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_06,                          0xF603, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_07,                          0xF604, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_ANALOG_TH_LOW_08,                          0xF605, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_LOAD_CURRENT,                              0xF606, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_HW_PERSONALITY_00,                         0xF607, AdcCounts_t,                                        Swap_Range,   Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Adc_HW_PERSONALITY_01,                         0xF608, AdcCounts_t,                                        Swap_Range_E, Io_None, Sub_N, Bsp,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Gpio_GPIO_OUT_00,                              0xF609, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_GPIO_OUT_01,                              0xF610, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_GPIO_OUT_02,                              0xF611, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SoftPwmGpio_HTR_00,                            0xF612, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SoftPwmGpio_HTR_01,                            0xF613, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SoftPwmGpio_HTR_02,                            0xF614, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SoftPwmGpio_HTR_03,                            0xF615, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_SoftPwmGpio_HTR_04,                            0xF616, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_EN_00,                            0xF617, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_EN_01,                            0xF618, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_00,                               0xF619, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_01,                               0xF61A, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_02,                               0xF61B, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_MTR_DRV_03,                               0xF61C, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_REF_VAL_00,                               0xF61D, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_REF_VAL_01,                               0xF61E, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_REF_VAL_02,                               0xF61F, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_REF_VAL_03,                               0xF620, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_RLY_00,                                   0xF621, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_RLY_01,                                   0xF622, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_RLY_02,                                   0xF623, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_RLY_03,                                   0xF624, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_RLY_04,                                   0xF625, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_RLY_05,                                   0xF626, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_RLY_06,                                   0xF627, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_RLY_07,                                   0xF628, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_RLY_08,                                   0xF629, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_HEARTBEAT,                                0xF62B, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_SBC_RESET,                                0xF62C, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_PERSONALITY,                              0xF62D, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_DOOR_INT,                                 0xF62E, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_DOOR_01,                                  0xF62F, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_DOOR_02,                                  0xF630, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_DOOR_03,                                  0xF631, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_DOOR_04,                                  0xF632, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_GPIO_IN_00,                               0xF633, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_GPIO_IN_01,                               0xF634, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_GPIO_IN_02,                               0xF635, bool,                                               Swap_No, Io_None, Sub_Y, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_GPIO_IN_03,                               0xF636, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_SABBATH,                                  0xF637, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_PWM_VAR_01,                               0xF638, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Gpio_CAPTURE_03,                               0xF639, bool,                                               Swap_No, Io_None, Sub_N, Bsp,                     NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData0,     0xF680, FaultSnapshotData_t,                                Swap_Range_S, Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData1,     0xF681, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData2,     0xF682, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData3,     0xF683, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData4,     0xF684, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData5,     0xF685, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData6,     0xF686, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData7,     0xF687, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData8,     0xF688, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData9,     0xF689, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData10,    0xF68A, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData11,    0xF68B, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData12,    0xF68C, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData13,    0xF68D, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData14,    0xF68E, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData15,    0xF68F, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData16,    0xF690, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData17,    0xF691, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData18,    0xF692, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData19,    0xF693, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData20,    0xF694, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData21,    0xF695, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData22,    0xF696, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData23,    0xF697, FaultSnapshotData_t,                                Swap_Range,   Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_ServiceDiagnosticsFaultTableSnapshotData24,    0xF698, FaultSnapshotData_t,                                Swap_Range_E, Io_None, Sub_N, NvFaultSnapshot,    NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   \
   ENTRY(Erd_CacheSyncState,                                0xF712, bool,                                               Swap_No,  Io_None, Sub_N, Virtual,                NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_Fault_EepromReadFailure,                       0xF713, bool,                                               Swap_No,  Io_All,  Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Fault_EepromWriteFailure,                      0xF714, bool,                                               Swap_No,  Io_All,  Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_Fault_EepromEraseFailure,                      0xF715, bool,                                               Swap_No,  Io_All,  Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_PersonalityIdOutOfRangeFlag,                   0xF716, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_CoolingStatesGridVotesConstArrayMapInterface,  0xF717, I_ConstArrayMap_t *,                                Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_DoorBoardCommunicationFault,                             0xF720, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_DoorBoardCommunicationFault) \
   ENTRY(Erd_DispenserUiCommunicationFault,                           0xF721, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_DispenserUiCommunicationFault) \
   ENTRY(Erd_AndroidSbcCommunicationFault,                            0xF722, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_AndroidSbcCommunicationFault) \
   ENTRY(Erd_EmbeddedWifiCommunicationFault,                          0xF723, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_EmbeddedWifiCommunicationFault) \
   ENTRY(Erd_InternalTemperatureUiCommunicationFault,                 0xF724, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_InternalTemperatureUiCommunicationFault) \
   ENTRY(Erd_RfidBoardHardwareFailureFault,                           0xF725, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_RfidBoardHardwareFailureFault) \
   ENTRY(Erd_RfidBoardCommunicationFault,                             0xF726, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_RfidBoardCommunicationFault) \
   ENTRY(Erd_RfidBoardTagAuthenticationFailedFault,                   0xF727, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_RfidBoardTagAuthenticationFailedFault) \
   ENTRY(Erd_RfidBoardLeakDetectedFault,                              0xF728, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_RfidBoardLeakDetectedFault) \
   ENTRY(Erd_AutofillSensorCouplingFault,                             0xF729, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_AutofillCouplingFault) \
   ENTRY(Erd_AutofillNoWaterLevelChangeFault,                         0xF72A, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_AutofillNoWaterMovementFault) \
   ENTRY(Erd_AutofillTargetRemovedFault,                              0xF72B, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_AutofillTargetRemovedFault) \
   ENTRY(Erd_AutofillEmiDetectedFault,                                0xF72C, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_AutofillEmiDetectedFault) \
   ENTRY(Erd_RfidBoardBlockedTagFault,                                0xF72D, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_RfidBoardBlockedTagFault) \
   ENTRY(Erd_FreshFoodThermistorIsInvalidFault,                       0xF72E, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreshFoodThermistorIsInvalid) \
   ENTRY(Erd_FreezerThermistorIsInvalidFault,                         0xF72F, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreezerThermistorIsInvalid) \
   ENTRY(Erd_FreezerEvaporatorThermistorIsInvalidFault,               0xF730, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreezerEvaporatorThermistorIsInvalid) \
   ENTRY(Erd_AmbientThermistorIsInvalidFault,                         0xF731, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_AmbientThermistorIsInvalid) \
   ENTRY(Erd_AmbientHumiditySensorIsInvalidFault,                     0xF732, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_AmbientHumiditySensorIsInvalid) \
   ENTRY(Erd_IceMaker0ThermistorIsInvalidFault,                       0xF733, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_IceMaker0ThermistorIsInvalid) \
   ENTRY(Erd_IceMaker1ThermistorIsInvalidFault,                       0xF734, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_IceMaker1ThermistorIsInvalid) \
   ENTRY(Erd_IceMaker2ThermistorIsInvalidFault,                       0xF735, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_IceMaker2ThermistorIsInvalid) \
   ENTRY(Erd_ExcessiveCompressorRunTimeFault,                         0xF736, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_ExcessiveCompressorRunTime) \
   ENTRY(Erd_CompressorOffWhileCoolingSystemOnForMaxTimeFault,        0xF737, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_CompressorOffWhileCoolingSystemOnForMaxTime) \
   ENTRY(Erd_FreshFoodNoFreezeLimitTrippedFault,                      0xF738, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreshFoodNoFreezeLimitTripped) \
   ENTRY(Erd_FreezerDefrostHeaterOnForMaxTimeFault,                   0xF739, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreezerDefrostHeaterOnForMaxTime) \
   ENTRY(Erd_FreezerEvapFanMissingFanFeedbackWhenFanIsRunningFault,   0xF73A, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreezerEvapFanMissingFanFeedbackWhenFanIsRunning) \
   ENTRY(Erd_FreezerEvapFanFeedbackPresentWhenFanIsOffFault,          0xF73B, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreezerEvapFanFeedbackPresentWhenFanIsOff) \
   ENTRY(Erd_FreezerEvapFanCannotReachTargetRpmFault,                 0xF73C, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreezerEvapFanCannotReachTargetRpm) \
   ENTRY(Erd_CondenserFanMissingFanFeedbackWhenFanIsRunningFault,     0xF73D, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_CondenserFanMissingFanFeedbackWhenFanIsRunning) \
   ENTRY(Erd_CondenserFanFeedbackPresentWhenFanIsOffFault,            0xF73E, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_CondenserEvapFanFeedbackPresentWhenFanIsOff) \
   ENTRY(Erd_CondenserFanCannotReachTargetRpmFault,                   0xF73F, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_CondenserEvapFanCannotReachTargetRpm) \
   ENTRY(Erd_FreshFoodEvapFanMissingFanFeedbackWhenFanIsRunningFault, 0xF740, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreshFoodEvapFanMissingFanFeedbackWhenFanIsRunning) \
   ENTRY(Erd_FreshFoodEvapFanFeedbackPresentWhenFanIsOffFault,        0xF741, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreshFoodEvapFanFeedbackPresentWhenFanIsOff) \
   ENTRY(Erd_FreshFoodEvapFanCannotReachTargetRpmFault,               0xF742, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreshFoodEvapFanCannotReachTargetRpm) \
   ENTRY(Erd_FreshFoodTemperatureExceededFault,                       0xF743, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreshFoodTemperatureExceeded) \
   ENTRY(Erd_FreezerTemperatureExceededFault,                         0xF744, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreezerTemperatureExceeded) \
   ENTRY(Erd_AndroidSignOfLifeDebugFault,                             0xF745, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_AndroidSbcSignOfLifeFault) \
   ENTRY(Erd_IceWaterStopsDispensingBasedOnTimeFault,                 0xF746, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_IceWaterStopsDispensingBasedOnTime) \
   ENTRY(Erd_FreshFoodDefrostHeaterOnForMaxTimeFault,                 0xF747, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreshFoodDefrostHeaterOnForMaxTimeFault ) \
   ENTRY(Erd_ConvertibleCompartmentDefrostHeaterOnForMaxTimeFault,    0xF748, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_ConvertibleCompartmentDefrostHeaterOnForMaxTimeFault) \
   ENTRY(Erd_FreshFoodEvaporatorThermistorIsInvalidFault,             0xF749, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_FreshFoodEvaporatorThermistorIsInvalid) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistorIsInvalidFault,   0xF74A, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_ConvertibleCompartmentCabinetThermistorIsInvalid) \
   ENTRY(Erd_IceMaker0_TwistTrayMotorFault,                           0xF74B, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_IceMaker0TwistTrayMotorFault) \
   ENTRY(Erd_IceMaker1_TwistTrayMotorFault,                           0xF74C, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_IceMaker1TwistTrayMotorFault) \
   ENTRY(Erd_IceMaker2_TwistTrayMotorFault,                           0xF74D, bool,                                     Swap_No,  Io_None, Sub_N, Fault,                  NotNv,                                                       NotNv,         FaultId_IceMaker2TwistTrayMotorFault) \
   \
   ENTRY(Erd_FactoryModeEnableRequestInMinutes,             0xF801, uint8_t,                                            Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_NvUsageDetailsRequestErd,                      0xF810, NonVolatileUsageMonitorDetailsRequest_t,            Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NvUsageDetailsResponseErd,                     0xF811, NonVolatileUsageMonitorDetails_t,                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NvUsageWarningOrErrorDetailsErd,               0xF812, NonVolatileUsageMonitorDetails_t,                   Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NvUsageWarningThresholdExceededErd,            0xF813, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NvUsageErrorThresholdExceededErd,              0xF814, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_ExternalAmbientFilteredTemperatureInDegFx100,  0xFAFB, TemperatureDegFx100_t,                              Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ExternalAmbientThermistor_IsValid,             0xFAFC, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ExternalAmbientFilteredRelativeHumidityPercentx100, 0xFAFD, RelativeHumidityPercentx100_t,                 Swap_Yes, Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_ExternalAmbientHumidity_IsValid,               0xFAFE, bool,                                               Swap_No,  Io_None, Sub_N, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_EepromEraseRequestSignal,                      0xFAFF, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FullRoutingTableUartDOverride,                 0xFB00, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   \
   ENTRY(Erd_Eeprom_DefrostCompressorOnTimeInSeconds,       0xFB06, uint32_t,                                           Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_ClearedDefrostEepromStartup,            0xFB08, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanTrue,                1,             NotFault) \
   ENTRY(Erd_Eeprom_LeftSideFreshFoodScaledDoorAccelerationInSeconds,  0xFB0C, uint32_t,                                Swap_Range_S, Io_None, Sub_N, NvUnitSetting,      NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_RightSideFreshFoodScaledDoorAccelerationInSeconds, 0xFB0D, uint32_t,                                Swap_Range,   Io_None, Sub_N, NvUnitSetting,      NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_DoorInDoorScaledDoorAccelerationInSeconds,         0xFB0E, uint32_t,                                Swap_Range,   Io_None, Sub_N, NvUnitSetting,      NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds,            0xFB0F, uint32_t,                                Swap_Range,   Io_None, Sub_N, NvUnitSetting,      NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds, 0xFB10, uint32_t,                 Swap_Range,   Io_None, Sub_N, NvUnitSetting,      NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds,   0xFB11, uint32_t,                 Swap_Range_E, Io_None, Sub_N, NvUnitSetting,      NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_TurboCoolOnTimeInMinutes,               0xFB12, uint16_t,                                           Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_TurboFreezeOnTimeInMinutes,             0xFB13, uint16_t,                                           Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_WaterFilterVolumeUsageInOuncesX100,     0xFB14, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_N, NvUsageProfile,         NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_WaterFilterCalendarUsageInSeconds,      0xFB15, CalendarUsageInSeconds_t,                           Swap_Yes, Io_None, Sub_N, NvUsageProfile,         NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_UnitLifetimeDispensedWaterInOuncesX100, 0xFB16, VolumeInOuncesX100_t,                               Swap_Yes, Io_None, Sub_N, NvUsageProfile,         NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_SabbathWaitingForDefrostTimeInMinutes,  0xFB17, uint16_t,                                           Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_WaterFilterUsageSinceExpiration,        0xFB18, WaterFilterUsageSinceExpiration_t,                  Swap_Yes, Io_None, Sub_N, NvUsageProfile,         NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_EnhancedSabbathRunTimeInMinutes,        0xFB19, uint16_t,                                           Swap_Yes, Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_CurrentWaterFilterMonthTimeInMinutes,   0xFB1A, uint16_t,                                           Swap_Yes, Io_None, Sub_N, NvRfid,                 NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_Eeprom_PreviousWaterFilterVolumeUsageInOuncesX100, 0xFB1B, VolumeInOuncesX100_t,                           Swap_Yes, Io_None, Sub_N, NvRfid,                 NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   \
   ENTRY(Erd_NumberOfTimesMainboardHasLostCommunicationWithAndroidUi, 0xFB82, uint8_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_NumberOfTimesMainboardHasAttemptedToResetAndroidUi,      0xFB83, uint8_t,                                  Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_AndroidUiResetSignal,                          0xFB84, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispenserUiInSystem,                           0xFB85, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_RfidBoardInSystem,                             0xFB86, bool,                                               Swap_No,  Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_DoorBoardInSystem,                             0xFB87, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_EmbeddedWifiInSystem,                          0xFB88, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_AndroidSbcBoardInSystem,                       0xFB89, bool,                                               Swap_No,  Io_None, Sub_Y, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_InternalTemperatureUiInSystem,                 0xFB8A, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_BroadcastResetRequestSignal,                   0xFB8B, Signal_t,                                           Swap_No,  Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   ENTRY(Erd_DispenserUiReportedFeatures,                   0xFB8C, BoardFeatureApi_t,                                  Swap_Yes, Io_None, Sub_Y, Ram,                    NotNv,                                                       NotNv,         NotFault) \
   \
   ENTRY(Erd_FreshFoodThermistorDiscovered,                 0xFB90, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_FreezerThermistorDiscovered,                   0xFB91, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_FreezerEvaporatorThermistorDiscovered,         0xFB92, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_AmbientThermistorDiscovered,                   0xFB93, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_AmbientHumiditySensorDiscovered,               0xFB94, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_FreshFoodEvaporatorThermistorDiscovered,       0xFB95, bool,                                               Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   ENTRY(Erd_ConvertibleCompartmentCabinetThermistorDiscovered, 0xFB96, bool,                                           Swap_No,  Io_None, Sub_N, NvUnitSetting,          NonVolatileDataSourceDefaultData_BooleanFalse,               1,             NotFault) \
   \
   ENTRY(Erd_NvReservedProtected,                           0xFCFA, uint8_t,                                            Swap_No,  Io_None, Sub_N,  NvProtected,           NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_NvReservedUnitSetting,                         0xFCFB, uint8_t,                                            Swap_No,  Io_None, Sub_N,  NvUnitSetting,         NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_NvReservedUserSetting,                         0xFCFC, uint8_t,                                            Swap_No,  Io_None, Sub_N,  NvUserSetting,         NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_NvReservedRfid,                                0xFCFD, uint8_t,                                            Swap_No,  Io_None, Sub_N,  NvRfid,                NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_NvReservedFaultSnapshot,                       0xFCFE, uint8_t,                                            Swap_No,  Io_None, Sub_N,  NvFaultSnapshot,       NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   ENTRY(Erd_NvReservedCycleHistory,                        0xFCFF, uint8_t,                                            Swap_No,  Io_None, Sub_N,  NvCycleHistory,        NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \
   \
   ENTRY(Erd_PersonalityEepromMetadata,                     0xFFFE, AsyncDataSource_EepromMetadata_t,                   Swap_No,  Io_None, Sub_N,  PersonalityEeprom,     NonVolatileDataSourceDefaultData_Zeros,                      1,             NotFault) \

#define CALLBACK_TABLE(ENTRY) \
   ENTRY(Erd_RfidFilterUnitSerialNumber,                    ErdConvertedCallBack_WriteNotAvailable,                     ErdConvertedCallBack_ReadRfidFilterUnitSerialNumber)

#define DEPENDENCY_TABLE(ENTRY) \
   ENTRY(Erd_RfidFilterUnitSerialNumber,                    Erd_SerialNumber)

#define EXPAND_AS_INTERNAL_RAM_ERD_ENUM(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)(Name COMMA)

#define EXPAND_AS_NON_RAM_ERD_ENUM(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NON_RAM_, StorageType)(Name = Number COMMA)

#define EXPAND_AS_PUBLIC_ERD_ENUM(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   Public##Name = Number,

#define EXPAND_AS_FAULT_ID(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId)\
   CONCAT(INCLUDE_FAULT_, StorageType)(FaultId COMMA)

#define EXPAND_AS_FAULT_ERD_TABLE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_FAULT_, StorageType)(Name COMMA)

#define EXPAND_AS_ERD_FAULT_CODE_COUNT(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_FAULT_, StorageType)(+1)

#define EXPAND_AS_INDIRECT_ELEMENTS(VirtualErd, WriteCallback, ReadCallback) \
   { VirtualErd, SizeOf##VirtualErd, WriteCallback, ReadCallback },

#define EXPAND_AS_DEPENDENCY_ELEMENTS(VirtualErd, RealErdForVirtualErd) \
   { VirtualErd, RealErdForVirtualErd },

enum
{
   ERD_TABLE(EXPAND_AS_NON_RAM_ERD_ENUM)
   Erd_RamBase = (RamErdBase - 1),
   ERD_TABLE(EXPAND_AS_INTERNAL_RAM_ERD_ENUM)
};

// The following #define and XMACRO expansion prevent ERD ID duplication.
// The enumeration created should not be used for anything because there is no meaning.
// If a Number is used more than once in the ERD_TABLE, this enumeration will cause the following error in the compile.
//     error: redeclaration of enumerator 'DUPLICATE-ERD_<Number>'
#define EXPAND_AS_DUPLICATE_PREVENTION_ENUM(Name, Number, ...) \
   CONCAT(DUPLICATE_ERD_, Number),

enum
{
   ERD_TABLE(EXPAND_AS_DUPLICATE_PREVENTION_ENUM)
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
