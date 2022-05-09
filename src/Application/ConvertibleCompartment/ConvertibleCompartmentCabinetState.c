/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentCabinetState.h"
#include "utils.h"
#include "Setpoint.h"
#include "PersonalityParametricData.h"
#include "ConvertibleCompartmentCabinetStateType.h"

static void ChangeConvertibleCompartmentStateUsingSetpoint(ConvertibleCompartmentCabinetState_t *instance, TemperatureDegFx100_t convertibleCompartmentSetpoint)
{
   ConvertibleCompartmentCabinetStateType_t convertibleCompartmentCabinetState;
   if(convertibleCompartmentSetpoint >= instance->_private.convertibleCompartmentCabinetData->convertibleCompartmentCabinetThresholdDegFx100)
   {
      convertibleCompartmentCabinetState = ConvertibleCompartmentCabinetState_FreshFood;
   }
   else
   {
      convertibleCompartmentCabinetState = ConvertibleCompartmentCabinetState_Freezer;
   }
   DataModel_Write(instance->_private.dataModel, instance->_private.config->convertibleCompartmentStateErd, &convertibleCompartmentCabinetState);
}

static void ConvertibleCompartmentSetpointChangedCallback(void *context, const void *args)
{
   REINTERPRET(instance, context, ConvertibleCompartmentCabinetState_t *);
   REINTERPRET(convertibleCompartmentSetpoint, args, const SetpointVotedTemperature_t *);

   ChangeConvertibleCompartmentStateUsingSetpoint(instance, convertibleCompartmentSetpoint->temperature);
}

static void InitConvertibleCompartmentCabinetState(ConvertibleCompartmentCabinetState_t *instance)
{
   SetpointVotedTemperature_t convertibleCompartmentSetpoint;
   DataModel_Read(instance->_private.dataModel, instance->_private.config->convertibleCompartmentResolvedSetpointErd, &convertibleCompartmentSetpoint);
   ChangeConvertibleCompartmentStateUsingSetpoint(instance, convertibleCompartmentSetpoint.temperature);
}

void ConvertibleCompartmentCabinetState_Init(
   ConvertibleCompartmentCabinetState_t *instance,
   I_DataModel_t *dataModel,
   const ConvertibleCompartmentCabinetStateConfig_t *config)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.convertibleCompartmentCabinetData = PersonalityParametricData_Get(dataModel)->convertibleCompartmentCabinetData;

   InitConvertibleCompartmentCabinetState(instance);

   EventSubscription_Init(&instance->_private.onSetpointChanged, instance, ConvertibleCompartmentSetpointChangedCallback);
   DataModel_Subscribe(instance->_private.dataModel, instance->_private.config->convertibleCompartmentResolvedSetpointErd, &instance->_private.onSetpointChanged);
}
