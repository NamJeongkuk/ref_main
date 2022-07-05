/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DoorAccelerationCounter.h"
#include "PersonalityParametricData.h"
#include "DataModelErdPointerAccess.h"
#include "CalculatedGridLines.h"
#include "DoorAccelerationCounterFsmState.h"
#include "utils.h"
#include "Constants_Time.h"
#include "ConvertibleCompartmentStateType.h"

enum
{
   PeriodicTimerTicksInMs = 1 * MSEC_PER_SEC
};

enum
{
   Signal_ActivelyWaitingForNextDefrost = Fsm_UserSignalStart,
   Signal_NotActivelyWaitingForNextDefrost,
   Signal_PeriodicTimeout
};

static void State_Stop(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Pause(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Run(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static DoorAccelerationCounter_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DoorAccelerationCounter_t, _private.fsm, fsm);
}

static void SetFsmStateTo(DoorAccelerationCounter_t *instance, DoorAccelerationCounterFsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->doorAccelerationCounterFsmStateErd,
      &state);
}

static void ActivelyWaitingForNextDefrostChanged(void *context, const void *args)
{
   DoorAccelerationCounter_t *instance = context;
   const bool *state = args;

   if(*state)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_ActivelyWaitingForNextDefrost, NULL);
   }
   else
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_NotActivelyWaitingForNextDefrost, NULL);
   }
}

static void ResetFreshFoodDoorAccelerationCountToZero(DoorAccelerationCounter_t *instance)
{
   uint32_t count = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDoorAccelerationCountErd,
      &count);
}

static void ResetFreezerDoorAccelerationCountToZero(DoorAccelerationCounter_t *instance)
{
   uint32_t count = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorAccelerationCountErd,
      &count);
}

static void ResetConvertibleCompartmentDoorAccelerationCountToZero(DoorAccelerationCounter_t *instance)
{
   uint32_t count = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDoorAccelerationCountErd,
      &count);
}

static void PeriodicTimeout(void *context)
{
   DoorAccelerationCounter_t *instance = context;

   Fsm_SendSignal(&instance->_private.fsm, Signal_PeriodicTimeout, NULL);
}

static void StartPeriodicTimer(DoorAccelerationCounter_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer,
      PeriodicTimerTicksInMs,
      PeriodicTimeout,
      instance);
}

static bool AnyFreshFoodDoorIsOpen(DoorAccelerationCounter_t *instance)
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

static bool FreezerDoorIsOpen(DoorAccelerationCounter_t *instance)
{
   bool freezerFoodDoorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorIsOpenErd,
      &freezerFoodDoorIsOpen);

   return freezerFoodDoorIsOpen;
}

static bool ConvertibleCompartmentDoorIsOpen(DoorAccelerationCounter_t *instance)
{
   bool convertibleCompartmentDoorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDoorIsOpenErd,
      &convertibleCompartmentDoorIsOpen);

   return convertibleCompartmentDoorIsOpen;
}

static void IncrementFreshFoodDoorAccelerationCount(DoorAccelerationCounter_t *instance)
{
   uint32_t count;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDoorAccelerationCountErd,
      &count);

   count = TRUNCATE_UNSIGNED_ADDITION(
      count,
      instance->_private.defrostParametricData->freshFoodDoorIncrementFactorInSecondsPerSecond,
      UINT32_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDoorAccelerationCountErd,
      &count);
}

static void IncrementFreezerDoorAccelerationCount(DoorAccelerationCounter_t *instance)
{
   uint32_t count;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorAccelerationCountErd,
      &count);

   count = TRUNCATE_UNSIGNED_ADDITION(
      count,
      instance->_private.defrostParametricData->freezerDoorIncrementFactorInSecondsPerSecond,
      UINT32_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorAccelerationCountErd,
      &count);
}

static void IncrementConvertibleCompartmentDoorAccelerationCount(DoorAccelerationCounter_t *instance)
{
   uint32_t count;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDoorAccelerationCountErd,
      &count);

   ConvertibleCompartmentStateType_t state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentStateErd,
      &state);

   if(state == ConvertibleCompartmentState_FreshFood)
   {
      count = TRUNCATE_UNSIGNED_ADDITION(
         count,
         instance->_private.defrostParametricData->freshFoodDoorIncrementFactorInSecondsPerSecond,
         UINT32_MAX);
   }
   else if(state == ConvertibleCompartmentState_Freezer)
   {
      count = TRUNCATE_UNSIGNED_ADDITION(
         count,
         instance->_private.defrostParametricData->freezerDoorIncrementFactorInSecondsPerSecond,
         UINT32_MAX);
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDoorAccelerationCountErd,
      &count);
}

static void State_Run(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DoorAccelerationCounter_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DoorAccelerationCounterFsmState_Run);
         StartPeriodicTimer(instance);
         break;

      case Signal_NotActivelyWaitingForNextDefrost:
         Fsm_Transition(&instance->_private.fsm, State_Stop);
         break;

      case Signal_PeriodicTimeout:
         if(AnyFreshFoodDoorIsOpen(instance))
         {
            IncrementFreshFoodDoorAccelerationCount(instance);
         }

         if(FreezerDoorIsOpen(instance))
         {
            IncrementFreezerDoorAccelerationCount(instance);
         }

         if(ConvertibleCompartmentDoorIsOpen(instance))
         {
            IncrementConvertibleCompartmentDoorAccelerationCount(instance);
         }
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Pause(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DoorAccelerationCounter_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DoorAccelerationCounterFsmState_Pause);
         break;

      case Signal_ActivelyWaitingForNextDefrost:
         Fsm_Transition(&instance->_private.fsm, State_Run);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Stop(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DoorAccelerationCounter_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DoorAccelerationCounterFsmState_Stop);
         ResetFreshFoodDoorAccelerationCountToZero(instance);
         ResetFreezerDoorAccelerationCountToZero(instance);
         ResetConvertibleCompartmentDoorAccelerationCountToZero(instance);
         break;

      case Signal_ActivelyWaitingForNextDefrost:
         Fsm_Transition(&instance->_private.fsm, State_Run);
         break;

      case Fsm_Exit:
         break;
   }
}

static FsmState_t InitStateBasedOnFreezerFilteredTemperature(DoorAccelerationCounter_t *instance)
{
   TemperatureDegFx100_t freezerFilteredResolvedTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureResolvedErd,
      &freezerFilteredResolvedTemperature);

   CalculatedGridLines_t calcGridLines;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridLinesErd,
      &calcGridLines);

   TemperatureDegFx100_t gridFreezerExtremeHystTemperature = calcGridLines.gridLines[GridDelta_Freezer].gridLinesDegFx100[GridLine_FreezerExtremeHigh];

   if(freezerFilteredResolvedTemperature > gridFreezerExtremeHystTemperature ||
      freezerFilteredResolvedTemperature >= instance->_private.defrostParametricData->freezerDefrostTerminationTemperatureInDegFx100)
   {
      return State_Stop;
   }
   else
   {
      return State_Pause;
   }
}

void DoorAccelerationCounter_Init(
   DoorAccelerationCounter_t *instance,
   I_DataModel_t *dataModel,
   const DoorAccelerationCounterConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.gridParametricData = PersonalityParametricData_Get(dataModel)->gridData;

   FsmState_t state = InitStateBasedOnFreezerFilteredTemperature(instance);
   Fsm_Init(&instance->_private.fsm, state);

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      ActivelyWaitingForNextDefrostChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->activelyWaitingForNextDefrostErd,
      &instance->_private.subscription);
}