/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "VariableSweatHeaterDutyCycleCalculator.h"
#include "PersonalityParametricData.h"
#include "TemperatureDegFx100.h"
#include "PercentageDutyCycleVote.h"
#include "utils.h"

static void CalculateDutyCycle(VariableSweatHeaterDutyCycleCalculator_t *instance)
{
   TemperatureDegFx100_t ambientFilteredTemperature;
   RelativeHumidityPercentx100_t ambientFilteredHumidity;
   SetpointVotedTemperature_t freshFoodSetpoint;
   SetpointVotedTemperature_t freezerSetpoint;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientFilteredTemperatureResolvedInDegFx100Erd,
      &ambientFilteredTemperature);

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientFilteredHumidityResolvedPercentx100Erd,
      &ambientFilteredHumidity);

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodSetpointResolvedVoteErd,
      &freshFoodSetpoint);

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointResolvedVoteErd,
      &freezerSetpoint);

   int32_t dutyCycle = 0;
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->temperatureCoefficient * ambientFilteredTemperature) / 100);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->humidityCoefficient * ambientFilteredHumidity) / 100);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->freshFoodCoefficient * freshFoodSetpoint.temperatureInDegFx100) / 100);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->freezerCoefficient * freezerSetpoint.temperatureInDegFx100) / 100);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->temperatureSquaredCoefficient * ambientFilteredTemperature * ambientFilteredTemperature) / 10000);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->humiditySquaredCoefficient * ambientFilteredHumidity * ambientFilteredHumidity) / 10000);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->freshFoodSquaredCoefficient * freshFoodSetpoint.temperatureInDegFx100 * freshFoodSetpoint.temperatureInDegFx100) / 10000);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->freezerSquaredCoefficient * freezerSetpoint.temperatureInDegFx100 * freezerSetpoint.temperatureInDegFx100) / 10000);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->temperatureHumidityCoefficient * ambientFilteredTemperature * ambientFilteredHumidity) / 10000);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->temperatureFreshFoodCoefficient * ambientFilteredTemperature * freshFoodSetpoint.temperatureInDegFx100) / 10000);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->temperatureFreezerCoefficient * ambientFilteredTemperature * freezerSetpoint.temperatureInDegFx100) / 10000);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->humidityFreshFoodCoefficient * ambientFilteredHumidity * freshFoodSetpoint.temperatureInDegFx100) / 10000);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->humidityFreezerCoefficient * ambientFilteredHumidity * freezerSetpoint.temperatureInDegFx100) / 10000);
   dutyCycle += (int32_t)(((int64_t)instance->_private.variableSweatHeaterData->freshFoodFreezerCoefficient * freshFoodSetpoint.temperatureInDegFx100 * freezerSetpoint.temperatureInDegFx100) / 10000);
   dutyCycle += (int32_t)instance->_private.variableSweatHeaterData->interceptCoefficient;
   dutyCycle /= 1000;

   PercentageDutyCycle_t correctedDutyCyle = CLAMP(dutyCycle, PercentageDutyCycle_Min, PercentageDutyCycle_Max);
   PercentageDutyCycleVote_t dutyCycleVote = {
      .percentageDutyCycle = correctedDutyCyle,
      .care = Vote_Care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->recessHeaterVariableAntiSweatVoteErd,
      &dutyCycleVote);
}

static void DataModelChanged(void *context, const void *args)
{
   VariableSweatHeaterDutyCycleCalculator_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if((erd == instance->_private.config->ambientFilteredTemperatureResolvedInDegFx100Erd) ||
      (erd == instance->_private.config->ambientFilteredHumidityResolvedPercentx100Erd) ||
      (erd == instance->_private.config->freshFoodSetpointResolvedVoteErd) ||
      (erd == instance->_private.config->freezerSetpointResolvedVoteErd))
   {
      CalculateDutyCycle(instance);
   }
}

void VariableSweatHeaterDutyCycleCalculator_Init(
   VariableSweatHeaterDutyCycleCalculator_t *instance,
   I_DataModel_t *dataModel,
   const VariableSweatHeaterData_t *data,
   const VariableSweatHeaterDutyCycleCalculatorConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.variableSweatHeaterData = data;
   instance->_private.config = config;

   CalculateDutyCycle(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);
   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
