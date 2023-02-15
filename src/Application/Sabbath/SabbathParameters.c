/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SabbathParameters.h"
#include "PersonalityParametricData.h"
#include "utils.h"

static void UpdateMaxTimeBetweenDefrosts(SabbathParameters_t *instance, bool sabbathEnabled)
{
   if(sabbathEnabled)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->maxTimeBetweenDefrostsInMinutesErd,
         &instance->_private.sabbathParametricData->maxTimeBetweenDefrostsInMinutes);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->maxTimeBetweenDefrostsInMinutesErd,
         &instance->_private.defrostParametricData->idleData.maxTimeBetweenDefrostsInMinutes);
   }
}

static void SabbathModeChanged(void *context, const void *args)
{
   REINTERPRET(instance, context, SabbathParameters_t *);
   REINTERPRET(state, args, const bool *);

   UpdateMaxTimeBetweenDefrosts(instance, *state);
}

static void UpdateParametersOnInit(SabbathParameters_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathModeErd,
      &state);

   UpdateMaxTimeBetweenDefrosts(instance, state);
}

void SabbathParameters_Init(
   SabbathParameters_t *instance,
   I_DataModel_t *dataModel,
   const SabbathParametersConfig_t *sabbathParametersConfig)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = sabbathParametersConfig;
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.sabbathParametricData = PersonalityParametricData_Get(dataModel)->sabbathData;

   UpdateParametersOnInit(instance);

   EventSubscription_Init(
      &instance->_private.sabbathModeSubscription,
      instance,
      SabbathModeChanged);

   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->sabbathModeErd,
      &instance->_private.sabbathModeSubscription);
}
