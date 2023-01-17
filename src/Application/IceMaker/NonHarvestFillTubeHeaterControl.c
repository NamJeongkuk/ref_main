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

static bool IceAlgorithmIsActive(NonHarvestFillTubeHeaterControl_t *instance)
{
   bool iceAlgorithmIsActive;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceAlgorithmIsActiveErd,
      &iceAlgorithmIsActive);
   return iceAlgorithmIsActive;
}

static void UpdateNonHarvestFillTubeHeaterControlVoteWhenAlgorithmActiveErdChanges(NonHarvestFillTubeHeaterControl_t *instance)
{
   Vote_t shouldVoteFillTubeHeaterOn = !IceAlgorithmIsActive(instance);

   VoteForFillTubeHeater(
      instance,
      instance->_private.fillTubeHeaterData->nonHarvestFillTubeHeaterDutyCyclePercentage,
      shouldVoteFillTubeHeaterOn);
}

static void IceAlgorithmActiveErdChanged(void *context, const void *args)
{
   IGNORE(args);
   UpdateNonHarvestFillTubeHeaterControlVoteWhenAlgorithmActiveErdChanges(context);
}

void NonHarvestFillTubeHeaterControl_Init(
   NonHarvestFillTubeHeaterControl_t *instance,
   I_DataModel_t *dataModel,
   const NonHarvestFillTubeHeaterControlConfig_t *config,
   const FillTubeHeaterData_t *fillTubeHeaterData)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.fillTubeHeaterData = fillTubeHeaterData;

   if(fillTubeHeaterData->nonHarvestFillTubeHeaterDutyCyclePercentage > PercentageDutyCycle_Min)
   {
      UpdateNonHarvestFillTubeHeaterControlVoteWhenAlgorithmActiveErdChanges(instance);

      EventSubscription_Init(
         &instance->_private.dataModelOnChangeSubscription,
         instance,
         IceAlgorithmActiveErdChanged);

      DataModel_Subscribe(
         dataModel,
         instance->_private.config->iceAlgorithmIsActiveErd,
         &instance->_private.dataModelOnChangeSubscription);
   }
   else
   {
      VoteForFillTubeHeater(instance, PercentageDutyCycle_Min, Vote_DontCare);
   }
}
