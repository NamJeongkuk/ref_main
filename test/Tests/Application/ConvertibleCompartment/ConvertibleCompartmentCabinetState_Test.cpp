/*!
 * @file
 * @brief ConvertibleCompartment state tests
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ConvertibleCompartmentState.h"
#include "SystemErds.h"
#include "Setpoint.h"
#include "ConvertibleCompartmentData.h"
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

static const ConvertibleCompartmentStateConfig_t config = {
   .convertibleCompartmentResolvedSetpointErd = Erd_ConvertibleCompartmentSetpoint_ResolvedVote,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState
};

static const ConvertibleCompartmentData_t convertibleCompartmentData = {
   .convertibleCompartmentThresholdDegFx100 = CabinetThresholdDegFx100
};

TEST_GROUP(ConvertibleCompartmentState)
{
   ConvertibleCompartmentState_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetConvertibleCompartment(&personalityParametricData, &convertibleCompartmentData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void GivenConvertibleCompartmentStateIsInitialized()
   {
      ConvertibleCompartmentState_Init(&instance, dataModel, &config);
   }

   void ConvertibleCompartmentStateShouldBeA(uint8_t expectedCabinetState)
   {
      uint8_t realCabinetState;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentState, &realCabinetState);
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

TEST(ConvertibleCompartmentState, ShouldSetCabinetStateWhenInitialized)
{
   GivenConvertibleCompartmentSetpointIsNow(BelowThreshold);
   And GivenConvertibleCompartmentStateIsInitialized();

   The ConvertibleCompartmentStateShouldBeA(ConvertibleCompartmentState_Freezer);
}

TEST(ConvertibleCompartmentState, ShouldChangeCabinetStateWhenSetpointChangesFromBelowThresholdToAboveThreshold)
{
   GivenConvertibleCompartmentSetpointIsNow(BelowThreshold);
   And GivenConvertibleCompartmentStateIsInitialized();

   The ConvertibleCompartmentStateShouldBeA(ConvertibleCompartmentState_Freezer);

   GivenConvertibleCompartmentSetpointIsNow(AboveThreshold);
   The ConvertibleCompartmentStateShouldBeA(ConvertibleCompartmentState_FreshFood);
}

TEST(ConvertibleCompartmentState, ShouldChangeCabinetStateWhenSetpointChangesFromAboveThresholdToBelowThreshold)
{
   GivenConvertibleCompartmentSetpointIsNow(AboveThreshold);
   And GivenConvertibleCompartmentStateIsInitialized();

   The ConvertibleCompartmentStateShouldBeA(ConvertibleCompartmentState_FreshFood);

   GivenConvertibleCompartmentSetpointIsNow(BelowThreshold);
   The ConvertibleCompartmentStateShouldBeA(ConvertibleCompartmentState_Freezer);
}

TEST(ConvertibleCompartmentState, ShouldSetCabinetStateToFreshFoodWhenEqualToThreshold)
{
   GivenConvertibleCompartmentSetpointIsNow(BelowThreshold);
   And GivenConvertibleCompartmentStateIsInitialized();

   The ConvertibleCompartmentStateShouldBeA(ConvertibleCompartmentState_Freezer);

   GivenConvertibleCompartmentSetpointIsNow(AtThreshold);
   The ConvertibleCompartmentStateShouldBeA(ConvertibleCompartmentState_FreshFood);
}