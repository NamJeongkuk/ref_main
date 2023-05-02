/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "NonHarvestFillTubeHeaterControl.h"
#include "PercentageDutyCycleVote.h"
#include "AluminumMoldIceMakerHsmState.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TddPersonality.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When
#define And

enum
{
   Inactive = false,
   Active = true,
   ANonZeroPercentage = 10,
};

const NonHarvestFillTubeHeaterControlConfig_t config = {
   .iceAlgorithmIsActiveErd = Erd_Freezer_IceRateIsActive,
   .nonHarvestFillTubeHeaterVoteErd = Erd_FillTubeHeater_NonHarvestVote,
};

TEST_GROUP(NonZeroNonHarvestFillTubeHeaterControl)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   NonHarvestFillTubeHeaterControl_t instance;
   const FillTubeHeaterData_t *fillTubeHeaterData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      fillTubeHeaterData = &PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData->fillTubeHeaterData;
   }

   void ModuleIsInitialized()
   {
      NonHarvestFillTubeHeaterControl_Init(
         &instance,
         dataModel,
         &config,
         fillTubeHeaterData);
   }

   void IceRateIs(bool active)
   {
      DataModel_Write(dataModel, Erd_Freezer_IceRateIsActive, &active);
   }

   void NonHarvestPercentageVoteShouldBe(PercentageDutyCycle_t percentage, Vote_t care)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(dataModel, Erd_FillTubeHeater_NonHarvestVote, &actual);
      CHECK_EQUAL(percentage, actual.percentageDutyCycle);
      CHECK_EQUAL(care, actual.care);
   }

   void InitializedWithInactiveIceRate()
   {
      IceRateIs(Inactive);
      ModuleIsInitialized();
   }
};

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldInitialize)
{
   Given ModuleIsInitialized();
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteDontCareWhenIceRateIsActiveAfterInit)
{
   Given InitializedWithInactiveIceRate();
   NonHarvestPercentageVoteShouldBe(ANonZeroPercentage, Vote_Care);

   When IceRateIs(Active);
   NonHarvestPercentageVoteShouldBe(ANonZeroPercentage, Vote_DontCare);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteDontCareWhenIceRateIsActiveBeforeInit)
{
   Given IceRateIs(Active);

   When ModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(ANonZeroPercentage, Vote_DontCare);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteDontCareIfIceRateChangesFromInactiveToActive)
{
   Given InitializedWithInactiveIceRate();

   When IceRateIs(Active);
   NonHarvestPercentageVoteShouldBe(ANonZeroPercentage, Vote_DontCare);
}

TEST_GROUP(ZeroNonHarvestFillTubeHeaterControl)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   NonHarvestFillTubeHeaterControl_t instance;
   const FillTubeHeaterData_t *fillTubeHeaterData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleEvapFillTubeHeaterNonHarvestControlZeroPercentDutyCycle);
      dataModel = dataModelDouble.dataModel;
      fillTubeHeaterData = &PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData->fillTubeHeaterData;
   }

   void ModuleIsInitialized()
   {
      NonHarvestFillTubeHeaterControl_Init(
         &instance,
         dataModel,
         &config,
         fillTubeHeaterData);
   }

   void IceRateIs(bool active)
   {
      DataModel_Write(dataModel, Erd_Freezer_IceRateIsActive, &active);
   }

   void NonHarvestPercentageVoteShouldBe(PercentageDutyCycle_t percentage, Vote_t care)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(dataModel, Erd_FillTubeHeater_NonHarvestVote, &actual);
      CHECK_EQUAL(percentage, actual.percentageDutyCycle);
      CHECK_EQUAL(care, actual.care);
   }
};

TEST(ZeroNonHarvestFillTubeHeaterControl, ShouldVoteNotCareEvenIfIceRateIsInactiveOnInit)
{
   Given IceRateIs(Inactive);

   When ModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(0, Vote_DontCare);
}

TEST(ZeroNonHarvestFillTubeHeaterControl, ShouldNotReactToChangesInIceRate)
{
   Given IceRateIs(Inactive);

   When ModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(0, Vote_DontCare);

   When IceRateIs(Active);
   NonHarvestPercentageVoteShouldBe(0, Vote_DontCare);
}
