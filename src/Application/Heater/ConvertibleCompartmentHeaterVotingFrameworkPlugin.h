/*!
 * @file
 * @brief Plugin for the convertible compartment heater voting
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTHEATERVOTINGFRAMEWORKPLUGIN_H
#define CONVERTIBLECOMPARTMENTHEATERVOTINGFRAMEWORKPLUGIN_H

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
} ConvertibleCompartmentHeaterVotingFrameworkPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void ConvertibleCompartmentHeaterVotingFrameworkPlugin_Init(
   ConvertibleCompartmentHeaterVotingFrameworkPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
