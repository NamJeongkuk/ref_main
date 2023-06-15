/*!
 * @file
 * @brief Calculate the variable sweat heater duty cycle
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef VARIABLESWEATHEATERDUTYCYCLECALCULATOR_H
#define VARIABLESWEATHEATERDUTYCYCLECALCULATOR_H

#include "I_DataModel.h"
#include "EventSubscription.h"
#include "VariableSweatHeaterData.h"

typedef struct
{
   Erd_t ambientFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t ambientFilteredHumidityResolvedPercentx100Erd; // RelativeHumidityPercentx100_t
   Erd_t freshFoodSetpointResolvedVoteErd; // SetpointVotedTemperature_t
   Erd_t freezerSetpointResolvedVoteErd; // SetpointVotedTemperature_t
   Erd_t recessHeaterVariableAntiSweatVoteErd; // PercentageDutyCycleVote_t
   Erd_t ambientTemperatureIsValidResolvedErd; // bool
   Erd_t ambientHumidityIsValidResolvedErd; // bool
} VariableSweatHeaterDutyCycleCalculatorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const VariableSweatHeaterData_t *variableSweatHeaterData;
      const VariableSweatHeaterDutyCycleCalculatorConfig_t *config;
   } _private;
} VariableSweatHeaterDutyCycleCalculator_t;

void VariableSweatHeaterDutyCycleCalculator_Init(
   VariableSweatHeaterDutyCycleCalculator_t *instance,
   I_DataModel_t *dataModel,
   const VariableSweatHeaterData_t *variableSweatHeaterData,
   const VariableSweatHeaterDutyCycleCalculatorConfig_t *config);

#endif
