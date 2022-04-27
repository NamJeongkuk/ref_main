/*!
 * @file
 * @brief CcCabinet state tests
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CcCabinetState.h"
#include "SystemErds.h"
#include "Setpoint.h"
#include "CcCabinetData.h"
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

static const CcCabinetStateConfig_t config = {
   .ccResolvedSetpointErd = Erd_CcSetpoint_ResolvedVote,
   .ccStateErd = Erd_CcCabinetState
};

static const CcCabinetData_t ccCabinetData = {
   .ccCabinetThresholdDegFx100 = CabinetThresholdDegFx100
};

TEST_GROUP(CcCabinetState)
{
   CcCabinetState_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetCcCabinet(&personalityParametricData, &ccCabinetData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void GivenCcCabinetStateIsInitialized()
   {
      CcCabinetState_Init(&instance, dataModel, &config);
   }

   void CcCabinetStateShouldBeA(uint8_t expectedCabinetState)
   {
      uint8_t realCabinetState;
      DataModel_Read(dataModel, Erd_CcCabinetState, &realCabinetState);
      CHECK_EQUAL(expectedCabinetState, realCabinetState);
   }

   void GivenCcSetpointIsNow(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t ccVotedTemp;
      ccVotedTemp.care = true;
      ccVotedTemp.temperature = temperature;
      DataModel_Write(dataModel, Erd_CcSetpoint_ResolvedVote, &ccVotedTemp);
   }
};

TEST(CcCabinetState, ShouldSetCabinetStateWhenInitialized)
{
   GivenCcSetpointIsNow(BelowThreshold);
   And GivenCcCabinetStateIsInitialized();

   The CcCabinetStateShouldBeA(CcCabinetState_Freezer);
}

TEST(CcCabinetState, ShouldChangeCabinetStateWhenSetpointChangesFromBelowThresholdToAboveThreshold)
{
   GivenCcSetpointIsNow(BelowThreshold);
   And GivenCcCabinetStateIsInitialized();

   The CcCabinetStateShouldBeA(CcCabinetState_Freezer);

   GivenCcSetpointIsNow(AboveThreshold);
   The CcCabinetStateShouldBeA(CcCabinetState_FreshFood);
}

TEST(CcCabinetState, ShouldChangeCabinetStateWhenSetpointChangesFromAboveThresholdToBelowThreshold)
{
   GivenCcSetpointIsNow(AboveThreshold);
   And GivenCcCabinetStateIsInitialized();

   The CcCabinetStateShouldBeA(CcCabinetState_FreshFood);

   GivenCcSetpointIsNow(BelowThreshold);
   The CcCabinetStateShouldBeA(CcCabinetState_Freezer);
}

TEST(CcCabinetState, ShouldSetCabinetStateToFreshFoodWhenEqualToThreshold)
{
   GivenCcSetpointIsNow(BelowThreshold);
   And GivenCcCabinetStateIsInitialized();

   The CcCabinetStateShouldBeA(CcCabinetState_Freezer);

   GivenCcSetpointIsNow(AtThreshold);
   The CcCabinetStateShouldBeA(CcCabinetState_FreshFood);
}
