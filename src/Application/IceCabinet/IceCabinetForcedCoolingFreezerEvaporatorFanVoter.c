/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetForcedCoolingFreezerEvaporatorFanVoter.h"
#include "FanSpeed.h"
#include "IceCabinetCoolingMode.h"
#include "utils.h"

static void ForceCoolingFreezerEvaporatorFanWhenCoolingModeIsForcedCooling(IceCabinetForcedCoolingFreezerEvaporatorFanVoter_t *instance)
{
   IceCabinetCoolingMode_t coolingMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetCoolingModeErd,
      &coolingMode);

   FanVotedSpeed_t votedSpeed;

   if(coolingMode == IceCabinetCoolingMode_ForcedCooling)
   {
      FanVotedSpeed_t gridVotedSpeed;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->freezerEvapFanSpeedGridVoteErd,
         &gridVotedSpeed);

      votedSpeed.speed = MAX(FanSpeed_Low, gridVotedSpeed.speed);
      votedSpeed.care = Vote_Care;
   }
   else
   {
      votedSpeed.speed = FanSpeed_Off;
      votedSpeed.care = Vote_DontCare;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanSpeedIceCabinetVoteErd,
      &votedSpeed);
}

static void OnDataModelChange(void *context, const void *_args)
{
   IceCabinetForcedCoolingFreezerEvaporatorFanVoter_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->iceCabinetCoolingModeErd ||
      args->erd == instance->_private.config->freezerEvapFanSpeedGridVoteErd)
   {
      ForceCoolingFreezerEvaporatorFanWhenCoolingModeIsForcedCooling(instance);
   }
}

void IceCabinetForcedCoolingFreezerEvaporatorFanVoter_Init(
   IceCabinetForcedCoolingFreezerEvaporatorFanVoter_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetForcedCoolingFreezerEvaporatorFanVoterConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   ForceCoolingFreezerEvaporatorFanWhenCoolingModeIsForcedCooling(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);
}
