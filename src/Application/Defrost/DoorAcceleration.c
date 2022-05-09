/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DoorAcceleration.h"
#include "DoorAccelerationRequest.h"
#include "DoorAccelerationFsmState.h"
#include "DataModelErdPointerAccess.h"
#include "PersonalityParametricData.h"
#include "Constants_Time.h"
#include "utils.h"

enum
{
   Signal_EnableRequested = Fsm_UserSignalStart,
   Signal_DisableRequested,
   Signal_ResetRequested,
   Signal_PeriodicTimerComplete
};

enum
{
   OneSecondInMs = 1 * MSEC_PER_SEC
};

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Enabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static DoorAccelerationCalculator_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DoorAccelerationCalculator_t, _private.fsm, fsm);
}

static void SetFsmStateTo(DoorAccelerationCalculator_t *instance, DoorAccelerationFsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->doorAccelerationFsmStateErd,
      &state);
}

static void DoorAccelerationRequested(DoorAccelerationCalculator_t *instance, const DoorAccelerationRequest_t *request)
{
   switch(request->request)
   {
      case DoorAcceleration_Enable:
         Fsm_SendSignal(&instance->_private.fsm, Signal_EnableRequested, NULL);
         break;

      case DoorAcceleration_Disable:
         Fsm_SendSignal(&instance->_private.fsm, Signal_DisableRequested, NULL);
         break;

      case DoorAcceleration_Reset:
         Fsm_SendSignal(&instance->_private.fsm, Signal_ResetRequested, NULL);
         break;

      default:
         break;
   }
}

static void DataModelChanged(void *context, const void *args)
{
   REINTERPRET(instance, context, DoorAccelerationCalculator_t *);
   REINTERPRET(onChangeData, args, const DataModelOnDataChangeArgs_t *);
   REINTERPRET(erd, onChangeData->erd, Erd_t);

   if(erd == instance->_private.config->doorAccelerationRequestErd)
   {
      REINTERPRET(request, onChangeData->data, const DoorAccelerationRequest_t *);

      DoorAccelerationRequested(instance, request);
   }
}

static bool FreshFoodDoorIsOpen(DoorAccelerationCalculator_t *instance)
{
   bool leftHandFreshFoodDoorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->leftHandFreshFoodDoorIsOpenErd,
      &leftHandFreshFoodDoorIsOpen);

   bool rightHandFreshFoodDoorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rightHandFreshFoodDoorIsOpenErd,
      &rightHandFreshFoodDoorIsOpen);

   bool doorInDoorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->doorInDoorIsOpenErd,
      &doorInDoorIsOpen);

   return (leftHandFreshFoodDoorIsOpen || rightHandFreshFoodDoorIsOpen || doorInDoorIsOpen);
}

static bool FreezerDoorIsOpen(DoorAccelerationCalculator_t *instance)
{
   bool freezerDoorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorIsOpenErd,
      &freezerDoorIsOpen);

   return freezerDoorIsOpen;
}

static void ResetDoorAccelerationCounts(DoorAccelerationCalculator_t *instance)
{
   uint32_t counts = 0;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorAccelerationCountsErd,
      &counts);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDoorAccelerationCountsErd,
      &counts);
}

static void IncrementFreshFoodDoorAcceleration(DoorAccelerationCalculator_t *instance)
{
   uint32_t acceleration;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDoorAccelerationCountsErd,
      &acceleration);

   acceleration += instance->_private.defrostParametricData->freshFoodDoorIncrementFactorInSecondsPerSecond;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDoorAccelerationCountsErd,
      &acceleration);
}

static void IncrementFreezerDoorAcceleration(DoorAccelerationCalculator_t *instance)
{
   uint32_t acceleration;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorAccelerationCountsErd,
      &acceleration);

   acceleration += instance->_private.defrostParametricData->freezerDoorIncrementFactorInSecondsPerSecond;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorAccelerationCountsErd,
      &acceleration);
}

static void PeriodicTimerComplete(void *context)
{
   REINTERPRET(instance, context, DoorAccelerationCalculator_t *);

   Fsm_SendSignal(&instance->_private.fsm, Signal_PeriodicTimerComplete, NULL);
}

static void StartPeriodicTimer(DoorAccelerationCalculator_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer,
      OneSecondInMs,
      PeriodicTimerComplete,
      instance);
}

static void State_Enabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DoorAccelerationCalculator_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DoorAccelerationFsmState_Enabled);
         StartPeriodicTimer(instance);
         break;

      case Signal_DisableRequested:
         Fsm_Transition(&instance->_private.fsm, State_Disabled);
         break;

      case Signal_ResetRequested:
         ResetDoorAccelerationCounts(instance);
         break;

      case Signal_PeriodicTimerComplete:
         if(FreshFoodDoorIsOpen(instance))
         {
            IncrementFreshFoodDoorAcceleration(instance);
         }
         if(FreezerDoorIsOpen(instance))
         {
            IncrementFreezerDoorAcceleration(instance);
         }
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DoorAccelerationCalculator_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DoorAccelerationFsmState_Disabled);
         ResetDoorAccelerationCounts(instance);
         break;

      case Signal_EnableRequested:
         Fsm_Transition(&instance->_private.fsm, State_Enabled);
         break;

      case Fsm_Exit:
         break;
   }
}

void DoorAcceleration_Init(
   DoorAccelerationCalculator_t *instance,
   I_DataModel_t *dataModel,
   const DoorAccelerationConfig_t *doorAccelerationConfig)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = doorAccelerationConfig;
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;

   EventSubscription_Init(
      &instance->_private.dataModelChangedSubscription,
      instance,
      DataModelChanged);

   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelChangedSubscription);

   Fsm_Init(&instance->_private.fsm, State_Disabled);
}
