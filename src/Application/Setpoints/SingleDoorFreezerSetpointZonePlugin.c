/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerSetpointZonePlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static const SetpointZoneResolverConfig_t freezerSetpointZoneResolverConfig = {
   .setpointVotedTemperatureErd = Erd_FreezerSetpoint_ResolvedVote,
   .setpointZoneErd = Erd_FreezerSetpointZone,
   .setpointZoneTemperatureBoundsErd = Erd_FreezerSetpoint_TemperatureBounds
};

void SingleDoorFreezerSetpointZonePlugin_Init(
   SingleDoorFreezerSetpointZonePlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const SetpointData_t *setpointParametricData = PersonalityParametricData_Get(dataModel)->setpointData;
   SetpointZoneTemperatureBounds_t freezerTemperatureBounds = {
      .coldSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freezerTriggerColdSetpointZoneInDegFx100,
      .warmSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freezerTriggerWarmSetpointZoneInDegFx100
   };

   DataModel_Write(
      dataModel, 
      Erd_FreezerSetpoint_TemperatureBounds, 
      &freezerTemperatureBounds);

   SetpointZoneResolver_Init(
      &instance->_private.freezerSetpointZoneResolver,
      &freezerSetpointZoneResolverConfig,
      dataModel);

   DataModel_Write(
      dataModel, 
      Erd_SetpointZonePluginReady, 
      set);
}
