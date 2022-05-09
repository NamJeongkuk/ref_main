/*!
 * @file
 * @brief ConvertibleCompartmentCabinet state tests
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ConvertibleCompartmentCabinetState.h"
#include "SystemErds.h"
#include "Setpoint.h"
#include "ConvertibleCompartmentCabinetData.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"

#define Given
#define When
#define And
#define The

enum
{
   CabinetThresholdDegFx100 = 2000,
   AboveThreshold = 2100,
   BelowThreshold = 1900,
   AtThreshold = 2000
};

static const ConvertibleCompartmentCabinetStateConfig_t config = {
   .convertibleCompartmentResolvedSetpointErd = Erd_ConvertibleCompartmentSetpoint_ResolvedVote,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentCabinetState
};

static const ConvertibleCompartmentCabinetData_t convertibleCompartmentCabinetData = {
   .convertibleCompartmentCabinetThresholdDegFx100 = CabinetThresholdDegFx100
};

TEST_GROUP(ConvertibleCompartmentCabinetState)
{
   ConvertibleCompartmentCabinetState_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetConvertibleCompartmentCabinet(&personalityParametricData, &convertibleCompartmentCabinetData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void GivenConvertibleCompartmentCabinetStateIsInitialized()
   {
      ConvertibleCompartmentCabinetState_Init(&instance, dataModel, &config);
   }

   void ConvertibleCompartmentCabinetStateShouldBeA(uint8_t expectedCabinetState)
   {
      uint8_t realCabinetState;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentCabinetState, &realCabinetState);
      CHECK_EQUAL(expectedCabinetState, realCabinetState);
   }

   void GivenConvertibleCompartmentSetpointIsNow(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t convertibleCompartmentVotedTemp;
      convertibleCompartmentVotedTemp.care = true;
      convertibleCompartmentVotedTemp.temperature = temperature;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentSetpoint_ResolvedVote, &convertibleCompartmentVotedTemp);
   }
};

TEST(ConvertibleCompartmentCabinetState, ShouldSetCabinetStateWhenInitialized)
{
   GivenConvertibleCompartmentSetpointIsNow(BelowThreshold);
   And GivenConvertibleCompartmentCabinetStateIsInitialized();

   The ConvertibleCompartmentCabinetStateShouldBeA(ConvertibleCompartmentCabinetState_Freezer);
}

TEST(ConvertibleCompartmentCabinetState, ShouldChangeCabinetStateWhenSetpointChangesFromBelowThresholdToAboveThreshold)
{
   GivenConvertibleCompartmentSetpointIsNow(BelowThreshold);
   And GivenConvertibleCompartmentCabinetStateIsInitialized();

   The ConvertibleCompartmentCabinetStateShouldBeA(ConvertibleCompartmentCabinetState_Freezer);

   GivenConvertibleCompartmentSetpointIsNow(AboveThreshold);
   The ConvertibleCompartmentCabinetStateShouldBeA(ConvertibleCompartmentCabinetState_FreshFood);
}

TEST(ConvertibleCompartmentCabinetState, ShouldChangeCabinetStateWhenSetpointChangesFromAboveThresholdToBelowThreshold)
{
   GivenConvertibleCompartmentSetpointIsNow(AboveThreshold);
   And GivenConvertibleCompartmentCabinetStateIsInitialized();

   The ConvertibleCompartmentCabinetStateShouldBeA(ConvertibleCompartmentCabinetState_FreshFood);

   GivenConvertibleCompartmentSetpointIsNow(BelowThreshold);
   The ConvertibleCompartmentCabinetStateShouldBeA(ConvertibleCompartmentCabinetState_Freezer);
}

TEST(ConvertibleCompartmentCabinetState, ShouldSetCabinetStateToFreshFoodWhenEqualToThreshold)
{
   GivenConvertibleCompartmentSetpointIsNow(BelowThreshold);
   And GivenConvertibleCompartmentCabinetStateIsInitialized();

   The ConvertibleCompartmentCabinetStateShouldBeA(ConvertibleCompartmentCabinetState_Freezer);

   GivenConvertibleCompartmentSetpointIsNow(AtThreshold);
   The ConvertibleCompartmentCabinetStateShouldBeA(ConvertibleCompartmentCabinetState_FreshFood);
}
