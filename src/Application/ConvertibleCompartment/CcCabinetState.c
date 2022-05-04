/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CcCabinetState.h"
#include "utils.h"
#include "Setpoint.h"
#include "PersonalityParametricData.h"
#include "CcCabinetStateType.h"

static void ChangeCcStateUsingSetpoint(CcCabinetState_t *instance, TemperatureDegFx100_t ccSetpoint)
{
   CcCabinetStateType_t ccCabinetState;
   if(ccSetpoint >= instance->_private.ccCabinetData->ccCabinetThresholdDegFx100)
   {
      ccCabinetState = CcCabinetState_FreshFood;
   }
   else
   {
      ccCabinetState = CcCabinetState_Freezer;
   }
   DataModel_Write(instance->_private.dataModel, instance->_private.config->ccStateErd, &ccCabinetState);
}

static void CcSetpointChangedCallback(void *context, const void *args)
{
   REINTERPRET(instance, context, CcCabinetState_t *);
   REINTERPRET(ccSetpoint, args, const SetpointVotedTemperature_t *);

   ChangeCcStateUsingSetpoint(instance, ccSetpoint->temperature);
}

static void InitCcCabinetState(CcCabinetState_t *instance)
{
   SetpointVotedTemperature_t ccSetpoint;
   DataModel_Read(instance->_private.dataModel, instance->_private.config->ccResolvedSetpointErd, &ccSetpoint);
   ChangeCcStateUsingSetpoint(instance, ccSetpoint.temperature);
}

void CcCabinetState_Init(
   CcCabinetState_t *instance,
   I_DataModel_t *dataModel,
   const CcCabinetStateConfig_t *config)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.ccCabinetData = PersonalityParametricData_Get(dataModel)->ccCabinetData;

   InitCcCabinetState(instance);

   EventSubscription_Init(&instance->_private.onSetpointChanged, instance, CcSetpointChangedCallback);
   DataModel_Subscribe(instance->_private.dataModel, instance->_private.config->ccResolvedSetpointErd, &instance->_private.onSetpointChanged);
}
