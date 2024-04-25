/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreezerSetpointToIceCabinetSetpointMapper.h"
#include "SetpointVotedTemperature.h"

static void WriteMappedSetpoint(
   FreezerSetpointToIceCabinetSetpointMapper_t *instance,
   TemperatureDegFx100_t freezerResolvedSetpoint)
{
   const FreezerSetpointToIceCabinetSetpointTable_t *mappingTable = instance->_private.mappingTable;

   TemperatureDegFx100_t mappedSetpoint = mappingTable->entries[mappingTable->numEntries - 1].iceCabinetSetpointInDegFx100;

   for(uint8_t i = 0; i < mappingTable->numEntries; i++)
   {
      if(freezerResolvedSetpoint <= mappingTable->entries[i].freezerResolvedSetpointInDegFx100)
      {
         mappedSetpoint = mappingTable->entries[i].iceCabinetSetpointInDegFx100;
         break;
      }
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetSetpoint,
      &mappedSetpoint);
}

static void FreezerResolvedSetpointChanged(void *context, const void *args)
{
   FreezerSetpointToIceCabinetSetpointMapper_t *instance = context;
   const TemperatureDegFx100_t *freezerResolvedSetpoint = args;

   WriteMappedSetpoint(instance, *freezerResolvedSetpoint);
}

void FreezerSetpointToIceCabinetSetpointMapper_Init(
   FreezerSetpointToIceCabinetSetpointMapper_t *instance,
   I_DataModel_t *dataModel,
   const FreezerSetpointToIceCabinetSetpointMapperConfig_t *config,
   const FreezerSetpointToIceCabinetSetpointTable_t *mappingTable)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.mappingTable = mappingTable;

   TemperatureDegFx100_t freezerResolvedSetpoint;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerResolvedSetpoint,
      &freezerResolvedSetpoint);

   WriteMappedSetpoint(instance, freezerResolvedSetpoint);

   EventSubscription_Init(
      &instance->_private.freezerSetpointSubscription,
      instance,
      FreezerResolvedSetpointChanged);

   DataModel_Subscribe(
      dataModel,
      instance->_private.config->freezerResolvedSetpoint,
      &instance->_private.freezerSetpointSubscription);
}
