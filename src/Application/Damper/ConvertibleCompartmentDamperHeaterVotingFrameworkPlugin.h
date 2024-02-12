/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTDAMPERHEATERVOTINGFRAMEWORKPLUGIN_H
#define CONVERTIBLECOMPARTMENTDAMPERHEATERVOTINGFRAMEWORKPLUGIN_H

#include "ErdResolver.h"
#include "I_DataModel.h"
#include "PercentageDutyCycleVoteToPwmDutyCycleConverter.h"

typedef struct
{
   struct
   {
      ErdResolver_t erdResolver;
      PercentageDutyCycleVoteToPwmDutyCycleConverter_t dutyCycleToPercentageCalculator;
   } _private;
} ConvertibleCompartmentDamperHeaterVotingFrameworkPlugin_t;

/*!
 * @brief Plugin for the convertible compartment damper heater voting
 * @param instance
 * @param dataModel
 */
void ConvertibleCompartmentDamperHeaterVotingFrameworkPlugin_Init(
   ConvertibleCompartmentDamperHeaterVotingFrameworkPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
