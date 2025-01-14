/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentState.h"
#include "utils.h"
#include "Setpoint.h"
#include "PersonalityParametricData.h"
#include "ConvertibleCompartmentStateType.h"
#include "SetpointVotedTemperature.h"

static void ChangeConvertibleCompartmentStateUsingSetpoint(ConvertibleCompartmentState_t *instance, TemperatureDegFx100_t convertibleCompartmentSetpoint)
{
   ConvertibleCompartmentStateType_t convertibleCompartmentState;
   if(convertibleCompartmentSetpoint >= instance->_private.convertibleCompartmentData->convertibleCompartmentThresholdDegFx100)
   {
      convertibleCompartmentState = ConvertibleCompartmentStateType_FreshFood;
   }
   else
   {
      convertibleCompartmentState = ConvertibleCompartmentStateType_Freezer;
   }
   DataModel_Write(instance->_private.dataModel, instance->_private.config->convertibleCompartmentStateErd, &convertibleCompartmentState);
}

static void ConvertibleCompartmentSetpointChangedCallback(void *context, const void *args)
{
   REINTERPRET(instance, context, ConvertibleCompartmentState_t *);
   REINTERPRET(convertibleCompartmentSetpoint, args, const SetpointVotedTemperature_t *);

   ChangeConvertibleCompartmentStateUsingSetpoint(instance, convertibleCompartmentSetpoint->temperatureInDegFx100);
}

static void InitConvertibleCompartmentState(ConvertibleCompartmentState_t *instance)
{
   SetpointVotedTemperature_t convertibleCompartmentSetpoint;
   DataModel_Read(instance->_private.dataModel, instance->_private.config->featurePanResolvedSetpointErd, &convertibleCompartmentSetpoint);
   ChangeConvertibleCompartmentStateUsingSetpoint(instance, convertibleCompartmentSetpoint.temperatureInDegFx100);
}

void ConvertibleCompartmentState_Init(
   ConvertibleCompartmentState_t *instance,
   I_DataModel_t *dataModel,
   const ConvertibleCompartmentStateConfig_t *config)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.convertibleCompartmentData = PersonalityParametricData_Get(dataModel)->convertibleCompartmentData;

   InitConvertibleCompartmentState(instance);

   EventSubscription_Init(&instance->_private.onSetpointChanged, instance, ConvertibleCompartmentSetpointChangedCallback);
   DataModel_Subscribe(instance->_private.dataModel, instance->_private.config->featurePanResolvedSetpointErd, &instance->_private.onSetpointChanged);
}
