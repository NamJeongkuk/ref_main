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
#include "HeaterControlType.h"
#include "SetpointVotedTemperature.h"
#include "utils.h"

static bool HeaterControlTypeCaresAboutAmbientHumidity(HeaterControlType_t heaterControlType)
{
   return !((heaterControlType == HeaterControlType_1) ||
      (heaterControlType == HeaterControlType_3) ||
      (heaterControlType == HeaterControlType_4));
}

static void CalculateDutyCycle(VariableSweatHeaterDutyCycleCalculator_t *instance)
{
   TemperatureDegFx100_t ambientFilteredTemperature;
   RelativeHumidityPercentx100_t ambientFilteredHumidity;
   SetpointVotedTemperature_t freshFoodSetpoint;
   SetpointVotedTemperature_t freezerSetpoint;
   bool ambientTemperatureIsValid;
   bool ambientHumidityIsValid;

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

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientTemperatureIsValidResolvedErd,
      &ambientTemperatureIsValid);

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientHumidityIsValidResolvedErd,
      &ambientHumidityIsValid);

   int32_t dutyCycle = 0;
   if(!ambientTemperatureIsValid || (!ambientHumidityIsValid && HeaterControlTypeCaresAboutAmbientHumidity(instance->_private.variableSweatHeaterData->heaterControlType)))
   {
      dutyCycle = instance->_private.variableSweatHeaterData->variableHeaterFallbackDutyCycleInPercent;
   }
   else
   {
      int32_t temperatureCoefficient = 0;
      int32_t humidityCoefficient = 0;
      int32_t freshFoodCoefficient = 0;
      int32_t freezerCoefficient = 0;
      int16_t temperatureSquaredCoefficient = 0;
      int16_t humiditySquaredCoefficient = 0;
      int16_t freshFoodSquaredCoefficient = 0;
      int16_t freezerSquaredCoefficient = 0;
      int16_t temperatureHumidityCoefficient = 0;
      int16_t temperatureFreshFoodCoefficient = 0;
      int16_t temperatureFreezerCoefficient = 0;
      int16_t humidityFreshFoodCoefficient = 0;
      int16_t humidityFreezerCoefficient = 0;
      int16_t freshFoodFreezerCoefficient = 0;
      int32_t interceptCoefficient = 0;

      switch(instance->_private.variableSweatHeaterData->heaterControlType)
      {
         case HeaterControlType_1:
            temperatureCoefficient = instance->_private.variableSweatHeaterData->heaterType1Data->temperatureCoefficient;
            temperatureSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType1Data->temperatureSquaredCoefficient;
            interceptCoefficient = instance->_private.variableSweatHeaterData->heaterType1Data->interceptCoefficient;
            break;

         case HeaterControlType_2:
            temperatureCoefficient = instance->_private.variableSweatHeaterData->heaterType2Data->temperatureCoefficient;
            humidityCoefficient = instance->_private.variableSweatHeaterData->heaterType2Data->humidityCoefficient;
            temperatureSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType2Data->temperatureSquaredCoefficient;
            temperatureHumidityCoefficient = instance->_private.variableSweatHeaterData->heaterType2Data->temperatureHumidityCoefficient;
            interceptCoefficient = instance->_private.variableSweatHeaterData->heaterType2Data->interceptCoefficient;
            break;

         case HeaterControlType_3:
            temperatureCoefficient = instance->_private.variableSweatHeaterData->heaterType3Data->temperatureCoefficient;
            freshFoodCoefficient = instance->_private.variableSweatHeaterData->heaterType3Data->freshFoodCoefficient;
            temperatureSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType3Data->temperatureSquaredCoefficient;
            freshFoodSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType3Data->freshFoodSquaredCoefficient;
            temperatureFreshFoodCoefficient = instance->_private.variableSweatHeaterData->heaterType3Data->temperatureFreshFoodCoefficient;
            interceptCoefficient = instance->_private.variableSweatHeaterData->heaterType3Data->interceptCoefficient;
            break;

         case HeaterControlType_4:
            temperatureCoefficient = instance->_private.variableSweatHeaterData->heaterType4Data->temperatureCoefficient;
            freezerCoefficient = instance->_private.variableSweatHeaterData->heaterType4Data->freezerCoefficient;
            temperatureSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType4Data->temperatureSquaredCoefficient;
            freezerSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType4Data->freezerSquaredCoefficient;
            temperatureFreezerCoefficient = instance->_private.variableSweatHeaterData->heaterType4Data->temperatureFreezerCoefficient;
            interceptCoefficient = instance->_private.variableSweatHeaterData->heaterType4Data->interceptCoefficient;
            break;

         case HeaterControlType_5:
            temperatureCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->temperatureCoefficient;
            humidityCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->humidityCoefficient;
            freshFoodCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->freshFoodCoefficient;
            temperatureSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->temperatureSquaredCoefficient;
            humiditySquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->humiditySquaredCoefficient;
            freshFoodSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->freshFoodSquaredCoefficient;
            temperatureHumidityCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->temperatureHumidityCoefficient;
            temperatureFreshFoodCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->temperatureFreshFoodCoefficient;
            humidityFreshFoodCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->humidityFreshFoodCoefficient;
            interceptCoefficient = instance->_private.variableSweatHeaterData->heaterType5Data->interceptCoefficient;
            break;

         case HeaterControlType_6:
            temperatureCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->temperatureCoefficient;
            humidityCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->humidityCoefficient;
            freezerCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->freezerCoefficient;
            temperatureSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->temperatureSquaredCoefficient;
            humiditySquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->humiditySquaredCoefficient;
            freezerSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->freezerSquaredCoefficient;
            temperatureHumidityCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->temperatureHumidityCoefficient;
            temperatureFreezerCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->temperatureFreezerCoefficient;
            humidityFreezerCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->humidityFreezerCoefficient;
            interceptCoefficient = instance->_private.variableSweatHeaterData->heaterType6Data->interceptCoefficient;
            break;

         case HeaterControlType_7:
            temperatureCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->temperatureCoefficient;
            humidityCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->humidityCoefficient;
            freshFoodCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->freshFoodCoefficient;
            freezerCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->freezerCoefficient;
            temperatureSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->temperatureSquaredCoefficient;
            humiditySquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->humiditySquaredCoefficient;
            freshFoodSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->freshFoodSquaredCoefficient;
            freezerSquaredCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->freezerSquaredCoefficient;
            temperatureHumidityCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->temperatureHumidityCoefficient;
            temperatureFreshFoodCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->temperatureFreshFoodCoefficient;
            temperatureFreezerCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->temperatureFreezerCoefficient;
            humidityFreshFoodCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->humidityFreshFoodCoefficient;
            humidityFreezerCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->humidityFreezerCoefficient;
            freshFoodFreezerCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->freshFoodFreezerCoefficient;
            interceptCoefficient = instance->_private.variableSweatHeaterData->heaterType7Data->interceptCoefficient;
            break;
      }

      dutyCycle += (int32_t)(((int64_t)temperatureCoefficient * ambientFilteredTemperature) / 100);
      dutyCycle += (int32_t)(((int64_t)humidityCoefficient * ambientFilteredHumidity) / 100);
      dutyCycle += (int32_t)(((int64_t)freshFoodCoefficient * freshFoodSetpoint.temperatureInDegFx100) / 100);
      dutyCycle += (int32_t)(((int64_t)freezerCoefficient * freezerSetpoint.temperatureInDegFx100) / 100);
      dutyCycle += (int32_t)(((int64_t)temperatureSquaredCoefficient * ambientFilteredTemperature * ambientFilteredTemperature) / 10000);
      dutyCycle += (int32_t)(((int64_t)humiditySquaredCoefficient * ambientFilteredHumidity * ambientFilteredHumidity) / 10000);
      dutyCycle += (int32_t)(((int64_t)freshFoodSquaredCoefficient * freshFoodSetpoint.temperatureInDegFx100 * freshFoodSetpoint.temperatureInDegFx100) / 10000);
      dutyCycle += (int32_t)(((int64_t)freezerSquaredCoefficient * freezerSetpoint.temperatureInDegFx100 * freezerSetpoint.temperatureInDegFx100) / 10000);
      dutyCycle += (int32_t)(((int64_t)temperatureHumidityCoefficient * ambientFilteredTemperature * ambientFilteredHumidity) / 10000);
      dutyCycle += (int32_t)(((int64_t)temperatureFreshFoodCoefficient * ambientFilteredTemperature * freshFoodSetpoint.temperatureInDegFx100) / 10000);
      dutyCycle += (int32_t)(((int64_t)temperatureFreezerCoefficient * ambientFilteredTemperature * freezerSetpoint.temperatureInDegFx100) / 10000);
      dutyCycle += (int32_t)(((int64_t)humidityFreshFoodCoefficient * ambientFilteredHumidity * freshFoodSetpoint.temperatureInDegFx100) / 10000);
      dutyCycle += (int32_t)(((int64_t)humidityFreezerCoefficient * ambientFilteredHumidity * freezerSetpoint.temperatureInDegFx100) / 10000);
      dutyCycle += (int32_t)(((int64_t)freshFoodFreezerCoefficient * freshFoodSetpoint.temperatureInDegFx100 * freezerSetpoint.temperatureInDegFx100) / 10000);
      dutyCycle += interceptCoefficient;
      dutyCycle /= 1000;
   }

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
      (erd == instance->_private.config->freezerSetpointResolvedVoteErd) ||
      (erd == instance->_private.config->ambientTemperatureIsValidResolvedErd) ||
      (erd == instance->_private.config->ambientHumidityIsValidResolvedErd))
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
