/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SensorFiltering.h"
#include "SensorFilterPlugin.h"
#include "Mapper_LookupTable.h"
#include "PersonalityParametricData.h"
#include "Filter_ExponentiallyWeightedMovingAverage.h"
#include "Filter_Scaled.h"
#include "Filter_Slew.h"
#include "Filter_Fallback.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"

static const SensorFilteringConfig_t freezerCabinetThermistorConfig = {
   .sensorAdcCountErd = Erd_FreezerThermistor_AdcCount,
   .sensorUnfilteredTemperatureErd = Erd_Freezer_UnfilteredTemperature,
   .sensorFilteredTemperatureErd = Erd_Freezer_FilteredTemperature,
   .sensorIsValidErd = Erd_Freezer_ThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t freshFoodCabinetThermistorConfig = {
   .sensorAdcCountErd = Erd_FreshFoodThermistor_AdcCount,
   .sensorUnfilteredTemperatureErd = Erd_FreshFood_UnfilteredTemperature,
   .sensorFilteredTemperatureErd = Erd_FreshFood_FilteredTemperature,
   .sensorIsValidErd = Erd_FreshFood_ThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t freezerEvapThermistorConfig = {
   .sensorAdcCountErd = Erd_FreezerEvapThermistor_AdcCount,
   .sensorUnfilteredTemperatureErd = Erd_FreezerEvap_UnfilteredTemperature,
   .sensorFilteredTemperatureErd = Erd_FreezerEvap_FilteredTemperature,
   .sensorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t freshFoodEvapThermistorConfig = {
   .sensorAdcCountErd = Erd_FreshFoodEvapThermistor_AdcCount,
   .sensorUnfilteredTemperatureErd = Erd_FreshFoodEvap_UnfilteredTemperature,
   .sensorFilteredTemperatureErd = Erd_FreshFoodEvap_FilteredTemperature,
   .sensorIsValidErd = Erd_FreshFoodEvaporatorThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t ambientThermistorConfig = {
   .sensorAdcCountErd = Erd_AmbientThermistor_AdcCount,
   .sensorUnfilteredTemperatureErd = Erd_Ambient_UnfilteredTemperature,
   .sensorFilteredTemperatureErd = Erd_Ambient_FilteredTemperature,
   .sensorIsValidErd = Erd_Ambient_ThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t convertibleCompartmentEvapThermistorConfig = {
   .sensorAdcCountErd = Erd_ConvertibleCompartmentEvapThermistor_AdcCount,
   .sensorUnfilteredTemperatureErd = Erd_ConvertibleCompartmentEvap_UnfilteredTemperature,
   .sensorFilteredTemperatureErd = Erd_ConvertibleCompartmentEvap_FilteredTemperature,
   .sensorIsValidErd = Erd_ConvertibleCompartmentEvaporatorThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static struct
{
   SensorFiltering_t freezerCabinetThermistor;
   SensorFiltering_t freshFoodCabinetThermistor;
   SensorFiltering_t freezerEvapThermistor;
   SensorFiltering_t freshFoodEvapThermistor;
   SensorFiltering_t convertibleCompartmentCabinetThermistor;
   SensorFiltering_t ambientThermistor;
   SensorFiltering_t convertibleCompartmentEvapThermistor;
} instance;

void SensorFilterPlugin(I_DataModel_t *dataModel)
{
   const SensorData_t *sensorData = PersonalityParametricData_Get(dataModel)->sensorData;

   SensorFiltering_Init(
      &instance.freezerCabinetThermistor,
      dataModel,
      &freezerCabinetThermistorConfig,
      sensorData->freezerCabinetThermistor,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance.freshFoodCabinetThermistor,
      dataModel,
      &freshFoodCabinetThermistorConfig,
      sensorData->freshFoodCabinetThermistor,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance.freezerEvapThermistor,
      dataModel,
      &freezerEvapThermistorConfig,
      sensorData->freezerEvapThermistor,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance.freshFoodEvapThermistor,
      dataModel,
      &freshFoodEvapThermistorConfig,
      sensorData->freshFoodEvapThermistor,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance.ambientThermistor,
      dataModel,
      &ambientThermistorConfig,
      sensorData->ambientThermistor,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance.convertibleCompartmentEvapThermistor,
      dataModel,
      &convertibleCompartmentEvapThermistorConfig,
      sensorData->convertibleCompartmentEvapThermistor,
      sensorData->periodicUpdateRateInMs);

   DataModel_Write(
      dataModel,
      Erd_SensorsReadyToBeRead,
      set);
}
