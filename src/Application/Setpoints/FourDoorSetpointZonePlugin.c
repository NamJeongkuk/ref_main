/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorSetpointZonePlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static const SetpointZoneResolverConfig_t freezerSetpointZoneResolverConfig = {
   .setpointVotedTemperatureErd = Erd_FreezerSetpoint_ResolvedVote,
   .setpointZoneErd = Erd_FreezerSetpointZone,
   .setpointZoneTemperatureBoundsErd = Erd_FreezerSetpoint_TemperatureBounds
};

static const SetpointZoneResolverConfig_t freshFoodSetpointZoneResolverConfig = {
   .setpointVotedTemperatureErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .setpointZoneErd = Erd_FreshFoodSetpointZone,
   .setpointZoneTemperatureBoundsErd = Erd_FreshFoodSetpoint_TemperatureBounds
};

void FourDoorSetpointZonePlugin_Init(FourDoorSetpointZonePlugin_t *instance, I_DataModel_t *dataModel)
{
   const SetpointData_t *setpointParametricData = PersonalityParametricData_Get(dataModel)->setpointData;
   SetpointZoneTemperatureBounds_t freezerTemperatureBounds = {
      .coldSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freezerTriggerColdSetpointZoneInDegFx100,
      .warmSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freezerTriggerWarmSetpointZoneInDegFx100
   };
   SetpointZoneTemperatureBounds_t freshFoodTemperatureBounds = {
      .coldSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freshFoodTriggerColdSetpointZoneInDegFx100,
      .warmSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freshFoodTriggerWarmSetpointZoneInDegFx100
   };

   DataModel_Write(dataModel, Erd_FreezerSetpoint_TemperatureBounds, &freezerTemperatureBounds);
   DataModel_Write(dataModel, Erd_FreshFoodSetpoint_TemperatureBounds, &freshFoodTemperatureBounds);

   SetpointZoneResolver_Init(
      &instance->_private.freezerSetpointZoneResolver,
      &freezerSetpointZoneResolverConfig,
      dataModel);

   SetpointZoneResolver_Init(
      &instance->_private.freshFoodSetpointZoneResolver,
      &freshFoodSetpointZoneResolverConfig,
      dataModel);

   DataModel_Write(dataModel, Erd_SetpointZonePluginReady, set);
}
