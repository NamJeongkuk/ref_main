/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "RfidCommunicationController.h"
#include "utils.h"
#include "RfidTypes.h"
#include "RfidCommunicationControllerState.h"
#include "WaterFilterType.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"

enum
{
   Signal_DemoModeEnabled = Hsm_UserSignalStart,
   Signal_ReadWriteResultSuccessful,
   Signal_ReadFailure,
   Signal_ReadWriteResultHardwareFailure,
   Signal_WriteFailure,
   Signal_AllFreshFoodDoorsAreClosed,
   Signal_AFreshFoodDoorsIsOpen,
   Signal_FreshFoodDoorsTimeoutTimerExpired,
   Signal_WriteTimeoutTimerExpired,
   Signal_ReadRequestTimerExpired
};

static const uint8_t BypassFilterIdentifier[FilterIdentifierSizeInBytes] = "BPXWFE";

#define HSM_STATE_EXPAND_AS_FUNCTION_DEFINITION(Name) \
   static bool Name(Hsm_t *hsm, HsmSignal_t signal, const void *data);

#define HSM_STATE_TABLE(ENTRY)              \
   ENTRY(State_Active)                      \
   ENTRY(State_AllFreshFoodDoorsJustClosed) \
   ENTRY(State_FreshFoodDoorOpen)           \
   ENTRY(State_DemoMode)                    \
   ENTRY(State_AllFreshFoodDoorsClosed)     \
   ENTRY(State_AllDoorsClosedRead)          \
   ENTRY(State_AllDoorsClosedWrite)

HSM_STATE_TABLE(HSM_STATE_EXPAND_AS_FUNCTION_DEFINITION)

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Active, HSM_NO_PARENT },
   { State_FreshFoodDoorOpen, State_Active },
   { State_DemoMode, HSM_NO_PARENT },
   { State_AllFreshFoodDoorsClosed, State_Active },
   { State_AllFreshFoodDoorsJustClosed, State_AllFreshFoodDoorsClosed },
   { State_AllDoorsClosedRead, State_AllFreshFoodDoorsClosed },
   { State_AllDoorsClosedWrite, State_AllFreshFoodDoorsClosed }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static RfidCommunicationController_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(RfidCommunicationController_t, _private.hsm, hsm);
}

static void UpdateHsmStateTo(RfidCommunicationController_t *instance, RfidCommunicationControllerState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidCommunicationControllerStateErd,
      &state);
}

static void SetFilterTypeTo(RfidCommunicationController_t *instance, WaterFilterType_t waterFilterType)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFilterTypeErd,
      &waterFilterType);
}

static void SetBypassPlugInstalledTo(RfidCommunicationController_t *instance, const bool *state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->bypassPlugInstalledErd,
      state);
}

static void SetFilterErrorTo(RfidCommunicationController_t *instance, const bool *state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->filterErrorErd,
      state);
}

static void IncrementU8ErdValue(RfidCommunicationController_t *instance, Erd_t erd)
{
   uint8_t data;
   DataModel_Read(
      instance->_private.dataModel,
      erd,
      &data);
   data++;
   DataModel_Write(
      instance->_private.dataModel,
      erd,
      &data);
}

static void ClearU8ErdValue(RfidCommunicationController_t *instance, Erd_t erd)
{
   DataModel_Write(
      instance->_private.dataModel,
      erd,
      clear);
}

static void IncrementLeakDetectedCount(RfidCommunicationController_t *instance)
{
   IncrementU8ErdValue(instance, instance->_private.config->rfidFilterLeakDetectedCountErd);
}

static void ClearLeakDetectedCount(RfidCommunicationController_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterLeakDetectedCountErd,
      clear);
}

static void IncrementBlockedCount(RfidCommunicationController_t *instance)
{
   IncrementU8ErdValue(instance, instance->_private.config->rfidFilterBlockedCountErd);
}

static void ClearBlockedCount(RfidCommunicationController_t *instance)
{
   ClearU8ErdValue(instance, instance->_private.config->rfidFilterBlockedCountErd);
}

static void IncrementRfidFilterBadReadCountAndSetFilterError(RfidCommunicationController_t *instance)
{
   IncrementU8ErdValue(instance, instance->_private.config->rfidFilterBadReadCountErd);
   SetFilterErrorTo(instance, set);
}

static void ClearRfidFilterBadReadCount(RfidCommunicationController_t *instance)
{
   ClearU8ErdValue(instance, instance->_private.config->rfidFilterBadReadCountErd);
}

static void IncrementRfidFilterBadWriteCountAndSetFilterError(RfidCommunicationController_t *instance)
{
   IncrementU8ErdValue(instance, instance->_private.config->rfidFilterBadWriteCountErd);
   SetFilterErrorTo(instance, set);
}

static void ClearRfidFilterBadWriteCount(RfidCommunicationController_t *instance)
{
   ClearU8ErdValue(instance, instance->_private.config->rfidFilterBadWriteCountErd);
}

static void ClearRfidFilterHardwareFailureCount(RfidCommunicationController_t *instance)
{
   ClearU8ErdValue(instance, instance->_private.config->rfidFilterHardwareFailureCountErd);
}

static void IncrementRfidFilterHardwareFailureCountAndSetFilterError(RfidCommunicationController_t *instance)
{
   IncrementU8ErdValue(instance, instance->_private.config->rfidFilterHardwareFailureCountErd);
   SetFilterErrorTo(instance, set);
}

static void IncrementNumberOfUnitsFilterHasBeenOn(RfidCommunicationController_t *instance)
{
   IncrementU8ErdValue(instance, instance->_private.config->rfidFilterNumberOfUnitsFilterHasBeenOnErd);
}

static void CopyCalendarUsageToMainboard(RfidCommunicationController_t *instance)
{
   CalendarUsageInSeconds_t calendarUsageInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterCalendarUsageInSecondsRfidBoardErd,
      &calendarUsageInSeconds);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterCalendarUsageInSecondsErd,
      &calendarUsageInSeconds);
}

static void CopyVolumeUsageToMainboard(RfidCommunicationController_t *instance)
{
   VolumeInOuncesX100_t volumeUsageInOuncesX100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterWaterVolumeUsageInOuncesX100RfidBoardErd,
      &volumeUsageInOuncesX100);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterWaterVolumeUsageInOuncesX100Erd,
      &volumeUsageInOuncesX100);
}

static void SendNewFilterInstalledSignal(RfidCommunicationController_t *instance)
{
   Signal_SendViaErd(DataModel_AsDataSource(instance->_private.dataModel), instance->_private.config->newFilterInstalledSignalErd);
}

static bool UidsMatch(RfidUid_t uidOne, RfidUid_t uidTwo)
{
   return memcmp(&uidOne, &uidTwo, sizeof(RfidUid_t)) == 0;
}

static bool NewFilterInstalled(RfidCommunicationController_t *instance)
{
   RfidUid_t rfidFilterUidRfidBoard;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUidRfidBoardErd,
      &rfidFilterUidRfidBoard);

   RfidUid_t rfidFilterUid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUidErd,
      &rfidFilterUid);

   return !UidsMatch(rfidFilterUidRfidBoard, rfidFilterUid);
}

static void CopyFilterUidToMainboard(RfidCommunicationController_t *instance)
{
   RfidUid_t rfidFilterUidRfidBoard;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUidRfidBoardErd,
      &rfidFilterUidRfidBoard);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUidErd,
      &rfidFilterUidRfidBoard);
}

static bool SerialNumbersMatch(UnitSerialNumber_t serialNumber, UnitSerialNumber_t unitSerialNumber)
{
   return !memcmp(&serialNumber, &unitSerialNumber, sizeof(UnitSerialNumber_t));
}

static bool NewSerialNumber(RfidCommunicationController_t *instance)
{
   UnitSerialNumber_t unitSerialNumber;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUnitSerialNumberErd,
      &unitSerialNumber);

   UnitSerialNumber_t rfidFilterUnitSerialNumber;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUnitSerialNumberRfidBoardErd,
      &rfidFilterUnitSerialNumber);

   return !SerialNumbersMatch(unitSerialNumber, rfidFilterUnitSerialNumber);
}

static void CopyCurrentSerialNumberToPreviousSerialNumber(RfidCommunicationController_t *instance)
{
   UnitSerialNumber_t rfidFilterUnitSerialNumber;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUnitSerialNumberRfidBoardErd,
      &rfidFilterUnitSerialNumber);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterPreviousUnitSerialNumberErd,
      &rfidFilterUnitSerialNumber);
}

static bool AllFreshFoodDoorsAreClosed(RfidCommunicationController_t *instance)
{
   bool allFreshFoodDoorsAreClosed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->allFreshFoodDoorsAreClosedErd,
      &allFreshFoodDoorsAreClosed);
   return allFreshFoodDoorsAreClosed;
}

static void SendRfidFilterDataRequest(RfidCommunicationController_t *instance, ReadWriteRequest_t request)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterDataRequestErd,
      &request);
}

static void ReadRequestTimerExpired(void *context)
{
   RfidCommunicationController_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_ReadRequestTimerExpired, NULL);
}

static void StartReadRequestTimer(RfidCommunicationController_t *instance, TimerTicks_t timeInMsec)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readRequestTimer,
      timeInMsec,
      ReadRequestTimerExpired,
      instance);
}

static void StopReadRequestTimer(RfidCommunicationController_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readRequestTimer);
}

static void StartReadWriteRequestTimer(RfidCommunicationController_t *instance, TimerTicks_t timeInMsec)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readWriteRequestTimer,
      timeInMsec,
      ReadRequestTimerExpired,
      instance);
}

static bool ReadWriteTimerIsRunning(RfidCommunicationController_t *instance)
{
   return TimerModule_IsRunning(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readWriteRequestTimer);
}

static void StopReadWriteRequestTimer(RfidCommunicationController_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.readWriteRequestTimer);
}

static void SendReadRequest(RfidCommunicationController_t *instance)
{
   SendRfidFilterDataRequest(instance, ReadWriteRequest_Read);
}

static void StartFastReadRequestTimer(RfidCommunicationController_t *instance, TimerTicks_t timeInMsec)
{
   StartReadRequestTimer(instance, timeInMsec);
}

static void StopFastReadRequestTimer(RfidCommunicationController_t *instance)
{
   StopReadRequestTimer(instance);
}

static void WriteTimeoutTimerExpired(void *context)
{
   RfidCommunicationController_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_WriteTimeoutTimerExpired, NULL);
}

static void FreshFoodDoorsTimeoutTimerExpired(void *context)
{
   RfidCommunicationController_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_FreshFoodDoorsTimeoutTimerExpired, NULL);
}

static void StartTimeoutTimer(RfidCommunicationController_t *instance, TimerTicks_t timeInMsec, TimerCallback_t callback)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timeoutTimer,
      timeInMsec,
      callback,
      instance);
}

static void StopTimeoutTimer(RfidCommunicationController_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timeoutTimer);
}

static bool FilterIsBypassFilter(RfidCommunicationController_t *instance)
{
   RfidFilterIdentifier_t filterId;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterIdentifierErd,
      &filterId);
   return memcmp(&filterId, &BypassFilterIdentifier, sizeof(RfidFilterIdentifier_t)) == 0;
}

static void UpdateUnitWithNewFilterData(RfidCommunicationController_t *instance)
{
   CopyFilterUidToMainboard(instance);
   CopyCalendarUsageToMainboard(instance);
   CopyVolumeUsageToMainboard(instance);
   ClearRfidFilterBadWriteCount(instance);
   ClearU8ErdValue(instance, instance->_private.config->rfidFilterHardwareFailureCountErd);

   if(FilterIsBypassFilter(instance))
   {
      SetBypassPlugInstalledTo(instance, set);
      ClearBlockedCount(instance);
      ClearLeakDetectedCount(instance);
   }
   else
   {
      SetBypassPlugInstalledTo(instance, clear);
   }

   if(NewSerialNumber(instance))
   {
      IncrementNumberOfUnitsFilterHasBeenOn(instance);
      CopyCurrentSerialNumberToPreviousSerialNumber(instance);
   }

   SendNewFilterInstalledSignal(instance);
}

static bool DemoModeIsEnabled(RfidCommunicationController_t *instance)
{
   bool demoModeEnabled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->demoModeEnableErd,
      &demoModeEnabled);
   return demoModeEnabled;
}

static bool RfidFilterStatusLeakDetected(RfidCommunicationController_t *instance)
{
   FilterStatusRfidBoard_t rfidFilterStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterStatusRfidBoardErd,
      &rfidFilterStatus);
   return BIT_STATE(rfidFilterStatus, RfidFilterStatusBits_LeakDetected);
}

static bool RfidFilterStatusRfidBlocked(RfidCommunicationController_t *instance)
{
   FilterStatusRfidBoard_t rfidFilterStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterStatusRfidBoardErd,
      &rfidFilterStatus);
   return BIT_STATE(rfidFilterStatus, RfidFilterStatusBits_Blocked_RfidBoard);
}

static bool ReadWriteResultIsFailure(const RfidFilterReadWriteResult_t *readWriteResult)
{
   return (readWriteResult->result == ReadWriteResult_ReadFailure ||
      readWriteResult->result == ReadWriteResult_ReadBeforeWriteFailure ||
      readWriteResult->result == ReadWriteResult_ReadAfterWriteFailure);
}

static bool LastReadWriteResultWasAFailure(RfidCommunicationController_t *instance)
{
   RfidFilterReadWriteResult_t readWriteResult;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterReadWriteResultErd,
      &readWriteResult);

   return (ReadWriteResultIsFailure(&readWriteResult)) || (readWriteResult.result == ReadWriteResult_HardwareFailure);
}

static void ClearFilterErrorAndReadFailureCounts(RfidCommunicationController_t *instance)
{
   SetFilterErrorTo(instance, clear);
   ClearRfidFilterBadReadCount(instance);
   ClearRfidFilterHardwareFailureCount(instance);
}

static bool State_Active(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_Active);
         SetFilterTypeTo(instance, WaterFilterType_XWFE);
         SetBypassPlugInstalledTo(instance, clear);
         SetFilterErrorTo(instance, clear);
         break;

      case Signal_DemoModeEnabled:
         Hsm_Transition(&instance->_private.hsm, State_DemoMode);
         break;

      case Signal_ReadRequestTimerExpired:
         SendReadRequest(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_AllFreshFoodDoorsJustClosed(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed);
         SendReadRequest(instance);
         StartReadRequestTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorJustClosedFilterReadFrequencyInSeconds * MSEC_PER_SEC);
         StartTimeoutTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorJustClosedFilterReadTimeoutInSeconds * MSEC_PER_SEC,
            FreshFoodDoorsTimeoutTimerExpired);
         break;

      case Signal_FreshFoodDoorsTimeoutTimerExpired:
         Hsm_Transition(&instance->_private.hsm, State_AllDoorsClosedRead);
         break;

      case Signal_ReadWriteResultSuccessful:
         ClearFilterErrorAndReadFailureCounts(instance);
         if(NewFilterInstalled(instance))
         {
            UpdateUnitWithNewFilterData(instance);
            Hsm_Transition(&instance->_private.hsm, State_AllDoorsClosedRead);
         }
         break;

      case Hsm_Exit:
         StopReadRequestTimer(instance);
         StopTimeoutTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_FreshFoodDoorOpen(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_FreshFoodDoorOpen);
         SendReadRequest(instance);
         StartReadRequestTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorOpenFilterReadFrequencyInSeconds * MSEC_PER_SEC);
         break;

      case Signal_ReadWriteResultSuccessful:
         ClearFilterErrorAndReadFailureCounts(instance);
         if(NewFilterInstalled(instance))
         {
            UpdateUnitWithNewFilterData(instance);
         }
         break;

      case Signal_AllFreshFoodDoorsAreClosed:
         Hsm_Transition(&instance->_private.hsm, State_AllFreshFoodDoorsJustClosed);
         break;

      case Hsm_Exit:
         StopReadRequestTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_AllFreshFoodDoorsClosed(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_AllFreshFoodDoorsClosed);
         break;

      case Signal_AFreshFoodDoorsIsOpen:
         Hsm_Transition(&instance->_private.hsm, State_FreshFoodDoorOpen);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_AllDoorsClosedRead(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_AllDoorsClosedRead);
         StartReadWriteRequestTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);

         if(RfidFilterStatusLeakDetected(instance) || LastReadWriteResultWasAFailure(instance))
         {
            StartFastReadRequestTimer(
               instance,
               instance->_private.rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC);
         }
         break;

      case Signal_ReadWriteResultSuccessful:
         ClearFilterErrorAndReadFailureCounts(instance);
         if(NewFilterInstalled(instance))
         {
            UpdateUnitWithNewFilterData(instance);
         }

         if(!ReadWriteTimerIsRunning(instance))
         {
            if(!FilterIsBypassFilter(instance))
            {
               Hsm_Transition(&instance->_private.hsm, State_AllDoorsClosedWrite);
            }
            else
            {
               StartReadWriteRequestTimer(
                  instance,
                  instance->_private.rfidFilterUpdateRateData->doorClosedFilterUpdateTimeInMinutes * MSEC_PER_MIN);
            }
         }
         break;

      case Signal_ReadFailure:
      case Signal_ReadWriteResultHardwareFailure:
         StartFastReadRequestTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->doorClosedRfidErrorDetectedReadFrequencySeconds * MSEC_PER_SEC);
         break;

      case Hsm_Exit:
         StopFastReadRequestTimer(instance);
         StopReadWriteRequestTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_AllDoorsClosedWrite(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_AllDoorsClosedWrite);
         SendRfidFilterDataRequest(instance, ReadWriteRequest_Write);
         StartTimeoutTimer(
            instance,
            instance->_private.rfidFilterUpdateRateData->rfidFilterWriteTimeoutInMinutes * MSEC_PER_MIN,
            WriteTimeoutTimerExpired);
         break;

      case Signal_WriteFailure:
      case Signal_WriteTimeoutTimerExpired:
      case Signal_ReadWriteResultSuccessful:
         Hsm_Transition(&instance->_private.hsm, State_AllDoorsClosedRead);
         break;

      case Hsm_Exit:
         StopTimeoutTimer(instance);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_DemoMode(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   RfidCommunicationController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, RfidCommunicationControllerHsmState_DemoMode);
         SetBypassPlugInstalledTo(instance, clear);
         SetFilterErrorTo(instance, clear);
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static void OnDataModelChange(void *context, const void *_args)
{
   RfidCommunicationController_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   Erd_t erd = args->erd;
   if(erd == instance->_private.config->rfidFilterReadWriteResultErd)
   {
      const RfidFilterReadWriteResult_t *readWriteResult = args->data;

      if(readWriteResult->result == ReadWriteResult_Success)
      {
         if(!FilterIsBypassFilter(instance))
         {
            RfidFilterStatusLeakDetected(instance) ? IncrementLeakDetectedCount(instance) : ClearLeakDetectedCount(instance);
            RfidFilterStatusRfidBlocked(instance) ? IncrementBlockedCount(instance) : ClearBlockedCount(instance);
         }

         Hsm_SendSignal(&instance->_private.hsm, Signal_ReadWriteResultSuccessful, NULL);
      }
      else if(ReadWriteResultIsFailure(readWriteResult))
      {
         IncrementRfidFilterBadReadCountAndSetFilterError(instance);
         Hsm_SendSignal(&instance->_private.hsm, Signal_ReadFailure, NULL);
      }
      else if(readWriteResult->result == ReadWriteResult_HardwareFailure)
      {
         IncrementRfidFilterHardwareFailureCountAndSetFilterError(instance);
         Hsm_SendSignal(&instance->_private.hsm, Signal_ReadWriteResultHardwareFailure, NULL);
      }
      else if(readWriteResult->result == ReadWriteResult_EepromWriteFailure ||
         readWriteResult->result == ReadWriteResult_UidMismatch ||
         readWriteResult->result == ReadWriteResult_TagUidIsInvalid)
      {
         IncrementRfidFilterBadWriteCountAndSetFilterError(instance);
         Hsm_SendSignal(&instance->_private.hsm, Signal_WriteFailure, NULL);
      }
   }
   else if(erd == instance->_private.config->allFreshFoodDoorsAreClosedErd)
   {
      const bool *allFreshFoodDoorsAreClosed = args->data;
      if(*allFreshFoodDoorsAreClosed)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_AllFreshFoodDoorsAreClosed, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_AFreshFoodDoorsIsOpen, NULL);
      }
   }
   else if(erd == instance->_private.config->demoModeEnableErd)
   {
      const bool *demoModeEnabled = args->data;
      if(*demoModeEnabled)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DemoModeEnabled, NULL);
      }
   }
}

void RfidCommunicationController_Init(
   RfidCommunicationController_t *instance,
   I_DataModel_t *dataModel,
   const RfidCommunicationControllerConfig_t *config,
   const RfidFilterUpdateRateData_t *rfidFilterUpdateRateData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.rfidFilterUpdateRateData = rfidFilterUpdateRateData;

   if(DemoModeIsEnabled(instance))
   {
      Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_DemoMode);
   }
   else if(AllFreshFoodDoorsAreClosed(instance))
   {
      Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_AllFreshFoodDoorsJustClosed);
   }
   else
   {
      Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_FreshFoodDoorOpen);
   }

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
