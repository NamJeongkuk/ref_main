/*!
 * @file
 * @brief Initialize modules related to the recess heater
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RECESSHEATERPLUGIN_H
#define RECESSHEATERPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"
#include "VariableSweatHeaterDutyCycleCalculator.h"
#include "ResolvedVotePercentageDutyCycleConnector.h"

typedef struct
{
   struct
   {
      ErdResolver_t recessHeaterErdResolver;
      VariableSweatHeaterDutyCycleCalculator_t variableSweatHeaterDutyCycleCalculator;
      ResolvedVotePercentageDutyCycleConnector_t resolvedVotePercentageDutyCycleConnector;
   } _private;
} RecessHeaterPlugin_t;

void RecessHeaterPlugin_Init(
   RecessHeaterPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
