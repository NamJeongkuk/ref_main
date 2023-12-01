/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Constants_Binary.h"
#include "AluminumMoldIceMakerFullStatusUpdater.h"
#include "DataModelErdPointerAccess.h"
#include "FeelerArmPosition.h"
#include "AluminumMoldIceMakerHsmState.h"

static bool FeelerArmPositionIsBucketFull(AluminumMoldIceMakerFullStatusUpdater_t *instance)
{
   FeelerArmPosition_t position;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->feelerArmPositionErd,
      &position);

   return (position == FeelerArmPosition_BucketFull);
}

static bool AluminumMoldIceMakerHsmStateIsFreeze(AluminumMoldIceMakerFullStatusUpdater_t *instance)
{
   AluminumMoldIceMakerHsmState_t aluminumMoldIceMakerHsmState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->aluminumMoldIceMakerHsmStateErd,
      &aluminumMoldIceMakerHsmState);

   return (aluminumMoldIceMakerHsmState == AluminumMoldIceMakerHsmState_Freeze);
}

static void UpdateIceMakerFullStatus(void *context, const void *args)
{
   AluminumMoldIceMakerFullStatusUpdater_t *instance = context;
   IGNORE(args);

   if(FeelerArmPositionIsBucketFull(instance) && AluminumMoldIceMakerHsmStateIsFreeze(instance))
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->iceMakerFullStatusErd,
         set);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->iceMakerFullStatusErd,
         clear);
   }
}

void AluminumMoldIceMakerFullStatusUpdater_Init(
   AluminumMoldIceMakerFullStatusUpdater_t *instance,
   I_DataModel_t *dataModel,
   const AluminumMoldIceMakerFullStatusUpdaterConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   UpdateIceMakerFullStatus(instance, NULL);

   EventSubscription_Init(
      &instance->_private.feelerArmPositionSubscription,
      instance,
      UpdateIceMakerFullStatus);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->feelerArmPositionErd,
      &instance->_private.feelerArmPositionSubscription);

   EventSubscription_Init(
      &instance->_private.aluminumMoldIceMakerHsmStateSubscription,
      instance,
      UpdateIceMakerFullStatus);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->aluminumMoldIceMakerHsmStateErd,
      &instance->_private.aluminumMoldIceMakerHsmStateSubscription);
}
