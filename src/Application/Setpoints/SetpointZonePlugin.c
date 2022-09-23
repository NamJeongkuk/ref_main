/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SetpointZonePlugin.h"
#include "SystemErds.h"

static const SetpointZoneResolverConfig_t freezerSetpointZoneResolverConfig = {
   .setpointVotedTemperatureErd = Erd_FreezerSetpoint_ResolvedVote,
   .setpointZoneErd = Erd_FreezerSetpointZone,
   .setpointZoneTemperatureBoundsErd = Erd_FreezerSetpoint_TemperatureBounds,
};

static const SetpointZoneResolverConfig_t freshFoodSetpointZoneResolverConfig = {
   .setpointVotedTemperatureErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .setpointZoneErd = Erd_FreshFoodSetpointZone,
   .setpointZoneTemperatureBoundsErd = Erd_FreshFoodSetpoint_TemperatureBounds,
};

static const SetpointZoneResolverConfig_t convertibleCompartmentSetpointZoneResolverConfig = {
   .setpointVotedTemperatureErd = Erd_ConvertibleCompartmentSetpoint_ResolvedVote,
   .setpointZoneErd = Erd_ConvertibleCompartmentSetpointZone,
   .setpointZoneTemperatureBoundsErd = Erd_ConvertibleCompartmentSetpoint_TemperatureBounds,
};

void SetpointZonePlugin_Init(SetpointZonePlugin_t *instance, I_DataModel_t *dataModel)
{
   const SetpointData_t *setpointParametricData = PersonalityParametricData_Get(dataModel)->setpointData;
   SetpointZoneTemperatureBounds_t freezerTemperatureBounds = {
      .coldSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freezerColdSetpointZoneLimitInDegFx100,
      .warmSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freezerWarmSetpointZoneLimitInDegFx100
   };
   SetpointZoneTemperatureBounds_t freshFoodTemperatureBounds = {
      .coldSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freshFoodColdSetpointZoneLimitInDegFx100,
      .warmSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->freshFoodWarmSetpointZoneLimitInDegFx100
   };
   SetpointZoneTemperatureBounds_t convertibleCompartmentTemperatureBounds = {
      .coldSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->convertibleCompartmentSetpointZoneLimitInDegFx100,
      .warmSetpointBoundInDegFx100 = setpointParametricData->setpointZoneData->convertibleCompartmentSetpointZoneLimitInDegFx100
   };

   DataModel_Write(dataModel, Erd_FreezerSetpoint_TemperatureBounds, &freezerTemperatureBounds);
   DataModel_Write(dataModel, Erd_FreshFoodSetpoint_TemperatureBounds, &freshFoodTemperatureBounds);
   DataModel_Write(dataModel, Erd_ConvertibleCompartmentSetpoint_TemperatureBounds, &convertibleCompartmentTemperatureBounds);

   SetpointZoneResolver_Init(
      &instance->_private.freezerSetpointZoneResolver,
      &freezerSetpointZoneResolverConfig,
      dataModel);

   SetpointZoneResolver_Init(
      &instance->_private.freshFoodSetpointZoneResolver,
      &freshFoodSetpointZoneResolverConfig,
      dataModel);

   SetpointZoneResolver_Init(
      &instance->_private.convertibleCompartmentSetpointZoneResolver,
      &convertibleCompartmentSetpointZoneResolverConfig,
      dataModel);
}
