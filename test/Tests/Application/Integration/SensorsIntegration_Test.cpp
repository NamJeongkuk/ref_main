/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Rx2xxResetSource.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "CppUTestExt/MockSupport.h"
#include "TddPersonality.h"

enum
{
   InvalidAdcCount = 5375,
   InvalidAdcCount2 = 5374,
   InvalidTemperature = 30000,
   ValidAdcCount1 = 22656,
   MappedValidTemperature1 = 1314,
   ValidAdcCount2 = 22700,
   MappedValidTemperature2 = 1323,
   ValidAdcCount3 = 26432,
   MappedValidTemperature3 = 2137,
   ValidAdcCount4 = 26470,
   MappedValidTemperature4 = 2145,
   ValidAdcCount5 = 30336,
   MappedValidTemperature5 = 2977,
};

TEST_GROUP(SensorsIntegrationFourDoor)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   I_DataModel_t *dataModel;
   Application_t instance;
   ResetReason_t resetReason;
   const SensorData_t *sensorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvapFourDoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      sensorData = PersonalityParametricData_Get(dataModel)->sensorData;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void ConvertibleCompartmentFilteredValuesShouldBeFallback()
   {
      TemperatureDegFx100_t freezerTemp;
      TemperatureDegFx100_t freshFoodTemp;

      DataModel_Read(dataModel, Erd_ConvertibleCompartmentAsFreezer_FilteredTemperatureInDegFx100, &freezerTemp);
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentAsFreshFood_FilteredTemperatureInDegFx100, &freshFoodTemp);

      CHECK_EQUAL(freezerTemp, sensorData->convertibleCompartmentAsFreezer->fallbackValueDegFx100);
      CHECK_EQUAL(freshFoodTemp, sensorData->convertibleCompartmentAsFreshFood->fallbackValueDegFx100);
   }

   void GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      GivenConvertibleCompartmentStateIs(state);
   }

   void ConvertibleCompartmentFilteredTemperatureResolvedValueShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenConvertibleFilteredOverrideIsEnabledWithValue(TemperatureDegFx100_t overrideValue)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinet_FilteredTemperatureOverrideRequest, enabled);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinet_FilteredTemperatureOverrideValueInDegFx100, &overrideValue);
   }

   void WhenConvertibleFilteredOverrideIsEnabledWithValue(TemperatureDegFx100_t overrideValue)
   {
      GivenConvertibleFilteredOverrideIsEnabledWithValue(overrideValue);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }
};

TEST(SensorsIntegrationFourDoor, ShouldResolveConvertibleCompartmentFilteredTemperatureFx100BasedOnConvertibleCompartmentStateType)
{
   GivenApplicationHasBeenInitialized();
   ConvertibleCompartmentFilteredValuesShouldBeFallback();

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   ConvertibleCompartmentFilteredTemperatureResolvedValueShouldBe(sensorData->convertibleCompartmentAsFreezer->fallbackValueDegFx100);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   ConvertibleCompartmentFilteredTemperatureResolvedValueShouldBe(sensorData->convertibleCompartmentAsFreshFood->fallbackValueDegFx100);

   WhenConvertibleFilteredOverrideIsEnabledWithValue(1234);
   ConvertibleCompartmentFilteredTemperatureResolvedValueShouldBe(1234);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   ConvertibleCompartmentFilteredTemperatureResolvedValueShouldBe(1234);
}
