/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetHeaterControl.h"
#include "PercentageDutyCycleVote.h"
#include "TemperatureDegFx100.h"
#include "FanSpeed.h"
#include "utils.h"

static TemperatureDegFx100_t FreezerResolvedSetpoint(IceCabinetHeaterControl_t *instance)
{
   TemperatureDegFx100_t freezerResolvedSetpoint;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerResolvedSetpointErd,
      &freezerResolvedSetpoint);

   return freezerResolvedSetpoint;
}

static bool IceFormationIsActive(IceCabinetHeaterControl_t *instance)
{
   bool iceFormationIsActive;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceFormationIsActiveErd,
      &iceFormationIsActive);

   return iceFormationIsActive;
}

static bool IceCabinetFanIsOn(IceCabinetHeaterControl_t *instance)
{
   FanVotedSpeed_t iceCabinetFanVotedSpeed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetFanSpeedVoteErd,
      &iceCabinetFanVotedSpeed);

   return (iceCabinetFanVotedSpeed.care == Vote_Care) && (iceCabinetFanVotedSpeed.speed == FanSpeed_On);
}

static void CalculateIceCabinetHeaterDutyCycle(IceCabinetHeaterControl_t *instance)
{
   int32_t freezerSetpointAdder = instance->_private.iceCabinetHeaterData->iceCabinetHeaterFreezerSetpointSlopeCoefficient * FreezerResolvedSetpoint(instance);
   int32_t iceCabinetFanAdder = IceCabinetFanIsOn(instance) ? instance->_private.iceCabinetHeaterData->iceCabinetHeaterIceCabinetFanCoefficient : 0;
   int32_t iceFormationAdder = (IceFormationIsActive(instance) && IceCabinetFanIsOn(instance))
      ? instance->_private.iceCabinetHeaterData->iceCabinetHeaterIceFormationModeCoefficient
      : 0;
   PercentageDutyCycle_t dutyCycle = CLAMP(
      (freezerSetpointAdder + iceCabinetFanAdder + iceFormationAdder + instance->_private.iceCabinetHeaterData->iceCabinetHeaterInterceptCoefficient) / 100,
      instance->_private.iceCabinetHeaterData->iceCabinetHeaterMinDutyCyclePercentage,
      PercentageDutyCycle_Max);
   PercentageDutyCycleVote_t dutyCycleVote = {
      .percentageDutyCycle = dutyCycle,
      .care = Vote_Care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetHeaterVoteErd,
      &dutyCycleVote);
}

static void OnDataModelChange(void *context, const void *_args)
{
   IceCabinetHeaterControl_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->iceFormationIsActiveErd ||
      args->erd == instance->_private.config->iceCabinetFanSpeedVoteErd ||
      args->erd == instance->_private.config->freezerResolvedSetpointErd)
   {
      CalculateIceCabinetHeaterDutyCycle(instance);
   }
}

void IceCabinetHeaterControl_Init(
   IceCabinetHeaterControl_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetHeaterControlConfig_t *config,
   const IceCabinetHeaterData_t *iceCabinetHeaterData)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.iceCabinetHeaterData = iceCabinetHeaterData;

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
