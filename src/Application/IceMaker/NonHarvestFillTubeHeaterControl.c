/*!
 * @file fill tube heater non harvest operation
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "NonHarvestFillTubeHeaterControl.h"
#include "PercentageDutyCycleVote.h"
#include "utils.h"
#include "Vote.h"
#include "TemperatureDegFx100.h"
#include "FanSpeed.h"

static void VoteForFillTubeHeater(NonHarvestFillTubeHeaterControl_t *instance, PercentageDutyCycle_t percentageDutyCycle, Vote_t care)
{
   PercentageDutyCycleVote_t dutyCyclePercentageVote = {
      .percentageDutyCycle = percentageDutyCycle,
      .care = care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->nonHarvestFillTubeHeaterVoteErd,
      &dutyCyclePercentageVote);
}

static bool IceMakingIsActive(NonHarvestFillTubeHeaterControl_t *instance)
{
   bool iceMakingIsActive;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceMakingActiveErd,
      &iceMakingIsActive);
   return iceMakingIsActive;
}

static bool CabinetTemperatureIsCold(NonHarvestFillTubeHeaterControl_t *instance)
{
   TemperatureDegFx100_t cabinetTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cabinetTemperatureErd,
      &cabinetTemperature);
   return (cabinetTemperature < instance->_private.fillTubeHeaterData->maximumCabinetTemperatureInDegFx100);
}

static bool CompressorIsOn(NonHarvestFillTubeHeaterControl_t *instance)
{
   bool compressorIsOn;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &compressorIsOn);
   return compressorIsOn;
}

static bool FanIsOn(NonHarvestFillTubeHeaterControl_t *instance)
{
   FanVotedSpeed_t fanVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fanResolvedVoteErd,
      &fanVote);
   return (fanVote.speed != FanSpeed_Off);
}

static void UpdateNonHarvestFillTubeHeaterControlVote(NonHarvestFillTubeHeaterControl_t *instance)
{
   bool iceMakingIsActive = IceMakingIsActive(instance);
   bool cabinetTemperatureIsCold = CabinetTemperatureIsCold(instance);
   bool compressorIsOn = CompressorIsOn(instance);
   bool fanIsOn = FanIsOn(instance);
   PercentageDutyCycle_t newDutyCycle;

   if(cabinetTemperatureIsCold && compressorIsOn && fanIsOn)
   {
      if(iceMakingIsActive)
      {
         newDutyCycle = instance->_private.fillTubeHeaterData->formationDutyCyclePercentage;
      }
      else
      {
         newDutyCycle = instance->_private.fillTubeHeaterData->maintenanceDutyCyclePercentage;
      }
   }
   else
   {
      newDutyCycle = instance->_private.fillTubeHeaterData->offDutyCyclePercentage;
   }

   VoteForFillTubeHeater(
      instance,
      newDutyCycle,
      Vote_Care);
}

static bool ErdWeCareAboutChanged(NonHarvestFillTubeHeaterControl_t *instance, Erd_t erd)
{
   return (
      erd == instance->_private.config->cabinetTemperatureErd ||
      erd == instance->_private.config->compressorIsOnErd ||
      erd == instance->_private.config->fanResolvedVoteErd ||
      erd == instance->_private.config->iceMakingActiveErd);
}

static void DataModelChanged(void *context, const void *_args)
{
   const DataModelOnDataChangeArgs_t *args = _args;
   NonHarvestFillTubeHeaterControl_t *instance = context;

   if(ErdWeCareAboutChanged(instance, args->erd))
   {
      UpdateNonHarvestFillTubeHeaterControlVote(instance);
   }
}

void NonHarvestFillTubeHeaterControl_Init(
   NonHarvestFillTubeHeaterControl_t *instance,
   I_DataModel_t *dataModel,
   const NonHarvestFillTubeHeaterControlConfig_t *config,
   const NonHarvestFillTubeHeaterData_t *fillTubeHeaterData)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.fillTubeHeaterData = fillTubeHeaterData;

   if(instance->_private.fillTubeHeaterData->formationDutyCyclePercentage > PercentageDutyCycle_Min ||
      instance->_private.fillTubeHeaterData->maintenanceDutyCyclePercentage > PercentageDutyCycle_Min ||
      instance->_private.fillTubeHeaterData->offDutyCyclePercentage > PercentageDutyCycle_Min)
   {
      UpdateNonHarvestFillTubeHeaterControlVote(instance);

      EventSubscription_Init(
         &instance->_private.dataModelOnChangeSubscription,
         instance,
         DataModelChanged);

      Event_Subscribe(
         instance->_private.dataModel->OnDataChange,
         &instance->_private.dataModelOnChangeSubscription);
   }
   else
   {
      VoteForFillTubeHeater(instance, PercentageDutyCycle_Min, Vote_DontCare);
   }
}
