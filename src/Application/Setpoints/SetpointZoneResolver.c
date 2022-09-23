/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SetpointZoneResolver.h"
#include "SystemErds.h"
#include "PersonalityParametricData.h"
#include "SetpointZoneTemperatureBounds.h"

static void SetSetpointZone(SetpointZoneResolver_t *instance)
{
   SetpointVotedTemperature_t setpointVotedTemperature;
   SetpointZone_t setpointZone;
   SetpointZoneTemperatureBounds_t setpointBounds;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->setpointVotedTemperatureErd,
      &setpointVotedTemperature);

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->setpointZoneTemperatureBoundsErd,
      &setpointBounds);

   if(setpointVotedTemperature.temperature <= setpointBounds.coldSetpointBoundInDegFx100)
   {
      setpointZone = SetpointZone_Cold;
   }
   else if(setpointVotedTemperature.temperature >= setpointBounds.warmSetpointBoundInDegFx100)
   {
      setpointZone = SetpointZone_Warm;
   }
   else
   {
      setpointZone = SetpointZone_Middle;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->setpointZoneErd,
      &setpointZone);
}

static void OnDataModelChange(void *context, const void *_args)
{
   IGNORE(_args);
   SetpointZoneResolver_t *instance = context;

   SetSetpointZone(instance);
}

void SetpointZoneResolver_Init(
   SetpointZoneResolver_t *instance,
   const SetpointZoneResolverConfig_t *config,
   I_DataModel_t *dataModel)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   SetSetpointZone(instance);

   EventSubscription_Init(
      &instance->_private.dataModelOnChangedSubscription,
      instance,
      OnDataModelChange);

   DataModel_Subscribe(
      dataModel,
      instance->_private.config->setpointVotedTemperatureErd,
      &instance->_private.dataModelOnChangedSubscription);
}
