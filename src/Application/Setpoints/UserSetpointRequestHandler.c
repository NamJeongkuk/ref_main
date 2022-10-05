/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "UserSetpointRequestHandler.h"
#include "Setpoint.h"
#include "UserSetpointData.h"
#include "utils.h"

enum
{
   ResetValue = INT8_MAX,
   ScalingFactorDegFx100 = 100,
};

static void ResetUserSetpointRequest(UserSetpointRequestHandler_t *instance)
{
   Setpoint_t requestedSetpoint = ResetValue;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->userSetpointRequestErd,
      &requestedSetpoint);
}

static void VoteUserSetpoint(UserSetpointRequestHandler_t *instance, const Setpoint_t *setpointStatus)
{
   SetpointVotedTemperature_t votedSetpoint = {
      .temperature = (TemperatureDegFx100_t)*setpointStatus * ScalingFactorDegFx100,
      .care = true
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->userSetpointVoteErd,
      &votedSetpoint);
}

static void UpdateSetpointStatus(UserSetpointRequestHandler_t *instance, const Setpoint_t *setpointStatus)
{
   UserSetpointRangeData_t setpointRangeData;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->userSetpointRangeDataErd,
      &setpointRangeData);

   if(IN_RANGE(setpointRangeData.lowTemperatureSetpoint, *setpointStatus, setpointRangeData.highTemperatureSetpoint))
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->userSetpointStatusErd,
         setpointStatus);
   }
}

static void RequestedSetpointUpdated(UserSetpointRequestHandler_t *instance, const Setpoint_t *requestedSetpoint)
{
   if(*requestedSetpoint != ResetValue)
   {
      UpdateSetpointStatus(instance, requestedSetpoint);
      ResetUserSetpointRequest(instance);
   }
}

static void OnDataModelChange(void *context, const void *_args)
{
   UserSetpointRequestHandler_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->userSetpointRequestErd)
   {
      const Setpoint_t *requestedSetpoint = args->data;
      RequestedSetpointUpdated(instance, requestedSetpoint);
   }
   else if(args->erd == instance->_private.config->userSetpointStatusErd)
   {
      const Setpoint_t *userSetpoint = args->data;
      VoteUserSetpoint(instance, userSetpoint);
   }
}

static void VoteUserSetpointBasedOnInitializedData(UserSetpointRequestHandler_t *instance)
{
   Setpoint_t setpointStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->userSetpointStatusErd,
      &setpointStatus);

   if(setpointStatus == ResetValue)
   {
      UserSetpointRangeData_t setpointRangeData;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->userSetpointRangeDataErd,
         &setpointRangeData);

      setpointStatus = setpointRangeData.defaultTemperatureSetpoint;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->userSetpointStatusErd,
         &setpointStatus);
   }

   VoteUserSetpoint(instance, &setpointStatus);
}

void UserSetpointRequestHandler_Init(
   UserSetpointRequestHandler_t *instance,
   const UserSetpointRequestHandlerConfig_t *config,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   ResetUserSetpointRequest(instance);

   VoteUserSetpointBasedOnInitializedData(instance);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
