/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ReadyToDefrost.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "uassert.h"

enum
{
   Signal_WaitingToDefrost = Fsm_UserSignalStart,
   Signal_NotWaitingToDefrost,
   Signal_ConditionChangedAffectingTimeBetweenDefrost,
   Signal_CompressorOnTimeChanged,
   Signal_DoorAccelerationsChanged
};

static void State_WaitingToGetReadyToDefrost(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_ReadyAndDefrosting(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static ReadyToDefrost_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(ReadyToDefrost_t, _private.fsm, fsm);
}

static void UpdateWaitingDefrostTimeInSecondsTo(ReadyToDefrost_t *instance, uint32_t timeInSeconds)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waitingForDefrostTimeInSecondsErd,
      &timeInSeconds);
}

static bool ConvertibleCompartmentIsAbnormal(ReadyToDefrost_t *instance)
{
   bool hasConvertibleCompartment;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->hasConvertibleCompartment,
      &hasConvertibleCompartment);

   bool convertibleCompartmentDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd,
      &convertibleCompartmentDefrostWasAbnormal);

   return hasConvertibleCompartment && convertibleCompartmentDefrostWasAbnormal;
}

static bool ShouldUseMinimumTimeBetweenDefrosts(ReadyToDefrost_t *instance)
{
   bool freshFoodDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostWasAbnormalErd,
      &freshFoodDefrostWasAbnormal);

   bool freezerDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostWasAbnormalErd,
      &freezerDefrostWasAbnormal);

   bool invalidFreezerEvaporatorThermistorDuringDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->invalidFreezerEvaporatorThermistorDuringDefrostErd,
      &invalidFreezerEvaporatorThermistorDuringDefrost);

   bool filteredTempTooWarmOnPowerUp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      &filteredTempTooWarmOnPowerUp);

   bool useMinimumReadyToDefrostTime;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->useMinimumReadyToDefrostTimeErd,
      &useMinimumReadyToDefrostTime);

   bool eepromWasCleared;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->eepromClearedErd,
      &eepromWasCleared);

   return (freshFoodDefrostWasAbnormal ||
      freezerDefrostWasAbnormal ||
      ConvertibleCompartmentIsAbnormal(instance) ||
      invalidFreezerEvaporatorThermistorDuringDefrost ||
      filteredTempTooWarmOnPowerUp ||
      useMinimumReadyToDefrostTime ||
      eepromWasCleared);
}

static void CalculateTimeBetweenDefrosts(ReadyToDefrost_t *instance)
{
   instance->_private.timeBetweenDefrostsInMinutes = instance->_private.defrostData->idleData.maxTimeBetweenDefrostsInMinutes;

   if(ShouldUseMinimumTimeBetweenDefrosts(instance))
   {
      instance->_private.timeBetweenDefrostsInMinutes = instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes;
   }
}

static uint32_t CompressorOnTimeInSeconds(ReadyToDefrost_t *instance)
{
   uint32_t compressorOnTimeInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostCompressorOnTimeInSecondsErd,
      &compressorOnTimeInSeconds);

   return compressorOnTimeInSeconds;
}

static uint32_t DoorAccelerationsInSeconds(ReadyToDefrost_t *instance)
{
   uint32_t freshFoodScaledDoorAccelerationInSeconds;
   uint32_t freezerScaledDoorAccelerationInSeconds;
   uint32_t convertibleCompartmentScaledDoorAccelerationInSeconds;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodScaledDoorAccelerationInSecondsErd,
      &freshFoodScaledDoorAccelerationInSeconds);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerScaledDoorAccelerationInSecondsErd,
      &freezerScaledDoorAccelerationInSeconds);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentScaledDoorAccelerationInSecondsErd,
      &convertibleCompartmentScaledDoorAccelerationInSeconds);

   return (freshFoodScaledDoorAccelerationInSeconds +
      freezerScaledDoorAccelerationInSeconds +
      convertibleCompartmentScaledDoorAccelerationInSeconds);
}

static void SetReadyToDefrost(ReadyToDefrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->readyToDefrostErd,
      set);
}

static void ClearReadyToDefrost(ReadyToDefrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->readyToDefrostErd,
      clear);
}

static bool ReadyToDefrost(ReadyToDefrost_t *instance)
{
   return ((CompressorOnTimeInSeconds(instance) >= instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE) &&
      ((CompressorOnTimeInSeconds(instance) + DoorAccelerationsInSeconds(instance)) >= instance->_private.timeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE));
}

static uint32_t CurrentWaitingTime(ReadyToDefrost_t *instance)
{
   if(CompressorOnTimeInSeconds(instance) >= (instance->_private.defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE))
   {
      return CompressorOnTimeInSeconds(instance) + DoorAccelerationsInSeconds(instance);
   }

   return CompressorOnTimeInSeconds(instance);
}

static void SetReadyToDefrostIfReadyToDefrost(ReadyToDefrost_t *instance)
{
   if(ReadyToDefrost(instance))
   {
      SetReadyToDefrost(instance);
   }
}

static void State_WaitingToGetReadyToDefrost(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   ReadyToDefrost_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         ClearReadyToDefrost(instance);
         CalculateTimeBetweenDefrosts(instance);
         UpdateWaitingDefrostTimeInSecondsTo(instance, CurrentWaitingTime(instance));
         SetReadyToDefrostIfReadyToDefrost(instance);
         break;

      case Signal_NotWaitingToDefrost:
         Fsm_Transition(&instance->_private.fsm, State_ReadyAndDefrosting);
         break;

      case Signal_ConditionChangedAffectingTimeBetweenDefrost:
         CalculateTimeBetweenDefrosts(instance);
         SetReadyToDefrostIfReadyToDefrost(instance);
         break;

      case Signal_CompressorOnTimeChanged:
      case Signal_DoorAccelerationsChanged:
         UpdateWaitingDefrostTimeInSecondsTo(instance, CurrentWaitingTime(instance));
         SetReadyToDefrostIfReadyToDefrost(instance);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_ReadyAndDefrosting(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   ReadyToDefrost_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         break;

      case Signal_WaitingToDefrost:
         Fsm_Transition(&instance->_private.fsm, State_WaitingToGetReadyToDefrost);
         break;

      case Fsm_Exit:
         break;
   }
}

static void DataModelChanged(void *context, const void *args)
{
   ReadyToDefrost_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd ||
      erd == instance->_private.config->freshFoodDefrostWasAbnormalErd ||
      erd == instance->_private.config->freezerDefrostWasAbnormalErd ||
      erd == instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd ||
      erd == instance->_private.config->invalidFreezerEvaporatorThermistorDuringDefrostErd ||
      erd == instance->_private.config->useMinimumReadyToDefrostTimeErd ||
      erd == instance->_private.config->eepromClearedErd ||
      erd == instance->_private.config->hasConvertibleCompartment)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_ConditionChangedAffectingTimeBetweenDefrost, NULL);
   }
   else if(erd == instance->_private.config->defrostCompressorOnTimeInSecondsErd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_CompressorOnTimeChanged, NULL);
   }
   else if(erd == instance->_private.config->freshFoodScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->freezerScaledDoorAccelerationInSecondsErd ||
      erd == instance->_private.config->convertibleCompartmentScaledDoorAccelerationInSecondsErd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_DoorAccelerationsChanged, NULL);
   }
   else if(erd == instance->_private.config->waitingToDefrostErd)
   {
      const bool *state = onChangeData->data;
      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_WaitingToDefrost, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_NotWaitingToDefrost, NULL);
      }
   }
}

static FsmState_t InitialState(ReadyToDefrost_t *instance)
{
   bool waitingToDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waitingToDefrostErd,
      &waitingToDefrost);

   return waitingToDefrost ? State_WaitingToGetReadyToDefrost : State_ReadyAndDefrosting;
}

void ReadyToDefrost_Init(
   ReadyToDefrost_t *instance,
   I_DataModel_t *dataModel,
   const ReadyToDefrostConfiguration_t *config,
   const DefrostData_t *defrostData)
{
   bool defrostCompressorOnTimeCounterReady;
   DataModel_Read(
      dataModel,
      config->defrostCompressorOnTimeCounterReadyErd,
      &defrostCompressorOnTimeCounterReady);

   bool doorAccelerationCounterReady;
   DataModel_Read(
      dataModel,
      config->doorAccelerationCounterReadyErd,
      &doorAccelerationCounterReady);

   uassert(defrostCompressorOnTimeCounterReady &&
      doorAccelerationCounterReady);

   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.defrostData = defrostData;

   Fsm_Init(&instance->_private.fsm, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      instance->_private.dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
