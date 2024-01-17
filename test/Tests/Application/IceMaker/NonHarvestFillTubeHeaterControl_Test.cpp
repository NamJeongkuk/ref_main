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
#include "FanSpeed.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TddPersonality.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   Inactive = false,
   Active = true,
   ANonZeroPercentage = 10,
   BelowThreshold = 1,
   AboveThreshold = 0
};

const NonHarvestFillTubeHeaterControlConfig_t config = {
   .iceMakingActiveErd = Erd_Freezer_IceRateIsActive,
   .cabinetTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .fanResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .nonHarvestFillTubeHeaterVoteErd = Erd_FillTubeHeater_NonHarvestVote
};

TEST_GROUP(NonZeroNonHarvestFillTubeHeaterControl)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   NonHarvestFillTubeHeaterControl_t instance;
   const NonHarvestFillTubeHeaterData_t *data;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      data = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot0Data->nonHarvestFillTubeHeaterData;
   }

   void GivenModuleIsInitialized()
   {
      NonHarvestFillTubeHeaterControl_Init(
         &instance,
         dataModel,
         &config,
         data);
   }

   void GiveIceMakingIs(bool active)
   {
      DataModel_Write(dataModel, config.iceMakingActiveErd, &active);
   }

   void WhenIceMakingBecomes(bool active)
   {
      GiveIceMakingIs(active);
   }

   void GivenCompressorStateIs(bool active)
   {
      DataModel_Write(dataModel, config.compressorIsOnErd, &active);
   }

   void WhenCompressorStateBecomes(bool active)
   {
      GivenCompressorStateIs(active);
   }

   void GivenCabinetTemperatureIs(bool belowThreshold)
   {
      TemperatureDegFx100_t cabinetTemperature = belowThreshold
         ? data->maximumCabinetTemperatureInDegFx100 - 1
         : data->maximumCabinetTemperatureInDegFx100 + 1;

      DataModel_Write(dataModel, config.cabinetTemperatureErd, &cabinetTemperature);
   }

   void GivenTheFanIs(bool state)
   {
      FanVotedSpeed_t fanVotedSpeed;
      fanVotedSpeed.care = Vote_Care;
      fanVotedSpeed.speed = state ? FanSpeed_High : FanSpeed_Off;

      DataModel_Write(dataModel, config.fanResolvedVoteErd, &fanVotedSpeed);
   }

   void WhenTheFanBecomes(bool state)
   {
      GivenTheFanIs(state);
   }

   void WhenTemperatureBecomes(bool belowThreshold)
   {
      GivenCabinetTemperatureIs(belowThreshold);
   }

   void GivenConditionsAreSetForNonHarvestControl(void)
   {
      GivenTheFanIs(ON);
      GivenCompressorStateIs(ON);
      GivenCabinetTemperatureIs(BelowThreshold);
   }

   void NonHarvestPercentageVoteShouldBe(PercentageDutyCycle_t percentage, Vote_t care)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(dataModel, config.nonHarvestFillTubeHeaterVoteErd, &actual);
      CHECK_EQUAL(percentage, actual.percentageDutyCycle);
      CHECK_EQUAL(care, actual.care);
   }
};

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldInitialize)
{
   GivenModuleIsInitialized();
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithOffDutyCycleIfTemperatureIsTooHighBeforeInit)
{
   GivenCabinetTemperatureIs(AboveThreshold);
   GivenModuleIsInitialized();

   NonHarvestPercentageVoteShouldBe(data->offDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithOffDutyCycleIfCompressorIsOffBeforeInit)
{
   GivenCompressorStateIs(OFF);
   GivenModuleIsInitialized();

   NonHarvestPercentageVoteShouldBe(data->offDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithOffDutyCycleIfFanIsOffBeforeInit)
{
   GivenTheFanIs(OFF);
   GivenModuleIsInitialized();

   NonHarvestPercentageVoteShouldBe(data->offDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithFormationDutyCycleIfIceMakingIsActiveBeforeInit)
{
   GivenConditionsAreSetForNonHarvestControl();
   GiveIceMakingIs(ON);
   GivenModuleIsInitialized();

   NonHarvestPercentageVoteShouldBe(data->formationDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithMaintenanceDutyCycleIfIceMakingIsActiveBeforeInit)
{
   GivenConditionsAreSetForNonHarvestControl();
   GiveIceMakingIs(OFF);
   GivenModuleIsInitialized();

   NonHarvestPercentageVoteShouldBe(data->maintenanceDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithOffDutyCycleIfTemperatureRisesPastThreshold)
{
   GivenConditionsAreSetForNonHarvestControl();
   GiveIceMakingIs(ON);
   GivenModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(data->formationDutyCyclePercentage, Vote_Care);

   WhenTemperatureBecomes(AboveThreshold);
   NonHarvestPercentageVoteShouldBe(data->offDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithOffDutyCycleIfCompressorTurnsOff)
{
   GivenConditionsAreSetForNonHarvestControl();
   GiveIceMakingIs(OFF);
   GivenModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(data->maintenanceDutyCyclePercentage, Vote_Care);

   WhenCompressorStateBecomes(OFF);
   NonHarvestPercentageVoteShouldBe(data->offDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithOffDutyCycleIfFanTurnsOff)
{
   GivenConditionsAreSetForNonHarvestControl();
   GiveIceMakingIs(OFF);
   GivenModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(data->maintenanceDutyCyclePercentage, Vote_Care);

   WhenTheFanBecomes(OFF);
   NonHarvestPercentageVoteShouldBe(data->offDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithFormationDutyCycleIfIceMakingBecomesActive)
{
   GivenConditionsAreSetForNonHarvestControl();
   GiveIceMakingIs(OFF);
   GivenModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(data->maintenanceDutyCyclePercentage, Vote_Care);

   WhenIceMakingBecomes(ON);
   NonHarvestPercentageVoteShouldBe(data->formationDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroNonHarvestFillTubeHeaterControl, ShouldVoteWithMaintenanceDutyCycleIfIceMakingBecomesInactive)
{
   GivenConditionsAreSetForNonHarvestControl();
   GiveIceMakingIs(ON);
   GivenModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(data->formationDutyCyclePercentage, Vote_Care);

   WhenIceMakingBecomes(OFF);
   NonHarvestPercentageVoteShouldBe(data->maintenanceDutyCyclePercentage, Vote_Care);
}

TEST_GROUP(ZeroNonHarvestFillTubeHeaterControl)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   NonHarvestFillTubeHeaterControl_t instance;
   const NonHarvestFillTubeHeaterData_t *data;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleEvapFillTubeHeaterNonHarvestControlZeroPercentDutyCycle);
      dataModel = dataModelDouble.dataModel;
      data = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot0Data->nonHarvestFillTubeHeaterData;
   }

   void GivenModuleIsInitialized()
   {
      NonHarvestFillTubeHeaterControl_Init(
         &instance,
         dataModel,
         &config,
         data);
   }

   void GiveIceMakingIs(bool active)
   {
      DataModel_Write(dataModel, Erd_Freezer_IceRateIsActive, &active);
   }

   void WhenIceMakingBecomes(bool active)
   {
      GiveIceMakingIs(active);
   }

   void NonHarvestPercentageVoteShouldBe(PercentageDutyCycle_t percentage, Vote_t care)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(dataModel, Erd_FillTubeHeater_NonHarvestVote, &actual);
      CHECK_EQUAL(percentage, actual.percentageDutyCycle);
      CHECK_EQUAL(care, actual.care);
   }

   void GivenNonHarvestPercentageVoteIs(PercentageDutyCycle_t percentage, Vote_t care)
   {
      PercentageDutyCycleVote_t newValue;
      newValue.percentageDutyCycle = percentage;
      newValue.care = care;
      DataModel_Write(dataModel, Erd_FillTubeHeater_NonHarvestVote, &newValue);
   }
};

TEST(ZeroNonHarvestFillTubeHeaterControl, ShouldVoteDontCareOnInitIfAllDutyCyclesAreZero)
{
   GivenNonHarvestPercentageVoteIs(100, Vote_Care);
   GivenModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(0, Vote_DontCare);
}

TEST(ZeroNonHarvestFillTubeHeaterControl, ShouldNotReactToChangesInIceMaking)
{
   GiveIceMakingIs(Inactive);
   GivenNonHarvestPercentageVoteIs(100, Vote_Care);
   GivenModuleIsInitialized();
   NonHarvestPercentageVoteShouldBe(0, Vote_DontCare);

   WhenIceMakingBecomes(Active);
   NonHarvestPercentageVoteShouldBe(0, Vote_DontCare);
}

TEST_GROUP(NonZeroOffDutyCyclePercentage)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   NonHarvestFillTubeHeaterControl_t instance;
   const NonHarvestFillTubeHeaterData_t *data;
   NonHarvestFillTubeHeaterData_t newData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      data = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot0Data->nonHarvestFillTubeHeaterData;
   }

   void GivenModuleIsInitializedWithNonZeroOffDutyCyclePercentage()
   {
      newData = *data;
      newData.offDutyCyclePercentage = 20;

      NonHarvestFillTubeHeaterControl_Init(
         &instance,
         dataModel,
         &config,
         &newData);
   }

   void GivenModuleIsInitializedWithNonZeroOffDutyCyclePercentageAndZeroFormationAndMaintenanceDutyCycles()
   {
      newData = *data;
      newData.formationDutyCyclePercentage = 0;
      newData.maintenanceDutyCyclePercentage = 0;
      newData.offDutyCyclePercentage = 20;

      NonHarvestFillTubeHeaterControl_Init(
         &instance,
         dataModel,
         &config,
         &newData);
   }

   void GivenModuleIsInitializedWithNonZeroMaintenancePercentageAndZeroFormationAndOffDutyCycles()
   {
      newData = *data;
      newData.formationDutyCyclePercentage = 0;
      newData.maintenanceDutyCyclePercentage = 20;
      newData.offDutyCyclePercentage = 0;

      NonHarvestFillTubeHeaterControl_Init(
         &instance,
         dataModel,
         &config,
         &newData);
   }

   void GivenModuleIsInitializedWithNonZeroFormationDutyCyclePercentageAndZeroMaintenanceAndOffDutyCycles()
   {
      newData = *data;
      newData.formationDutyCyclePercentage = 20;
      newData.maintenanceDutyCyclePercentage = 0;
      newData.offDutyCyclePercentage = 0;

      NonHarvestFillTubeHeaterControl_Init(
         &instance,
         dataModel,
         &config,
         &newData);
   }

   void GivenIceRateIs(bool active)
   {
      DataModel_Write(dataModel, config.iceMakingActiveErd, &active);
   }

   void GivenCompressorStateIs(bool active)
   {
      DataModel_Write(dataModel, config.compressorIsOnErd, &active);
   }

   void WhenCompressorStateBecomes(bool active)
   {
      GivenCompressorStateIs(active);
   }

   void GivenCabinetTemperatureIs(bool belowThreshold)
   {
      TemperatureDegFx100_t cabinetTemperature = belowThreshold
         ? data->maximumCabinetTemperatureInDegFx100 - 1
         : data->maximumCabinetTemperatureInDegFx100 + 1;

      DataModel_Write(dataModel, config.cabinetTemperatureErd, &cabinetTemperature);
   }

   void GivenTheFanIs(bool state)
   {
      FanVotedSpeed_t fanSpeed;
      fanSpeed.care = Vote_Care;
      fanSpeed.speed = state ? FanSpeed_High : FanSpeed_Off;

      DataModel_Write(dataModel, config.fanResolvedVoteErd, &fanSpeed);
   }

   void GivenConditionsAreSetForNonHarvestControl(void)
   {
      GivenTheFanIs(ON);
      GivenCompressorStateIs(ON);
      GivenCabinetTemperatureIs(BelowThreshold);
   }

   void NonHarvestPercentageVoteShouldBe(PercentageDutyCycle_t percentage, Vote_t care)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(dataModel, config.nonHarvestFillTubeHeaterVoteErd, &actual);
      CHECK_EQUAL(percentage, actual.percentageDutyCycle);
      CHECK_EQUAL(care, actual.care);
   }
};

TEST(NonZeroOffDutyCyclePercentage, ShouldVoteWithOffDutyCycleIfCompressorTurnsOff)
{
   GivenConditionsAreSetForNonHarvestControl();
   GivenIceRateIs(OFF);
   GivenModuleIsInitializedWithNonZeroOffDutyCyclePercentage();
   NonHarvestPercentageVoteShouldBe(newData.maintenanceDutyCyclePercentage, Vote_Care);

   WhenCompressorStateBecomes(OFF);
   NonHarvestPercentageVoteShouldBe(newData.offDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroOffDutyCyclePercentage, ShouldVoteWithOffDutyCycleOnInitIfIceCabinetTemperatureIsWarmAndFormationAndMaintenanceDutyCyclesAreZero)
{
   GivenCabinetTemperatureIs(AboveThreshold);
   GivenModuleIsInitializedWithNonZeroOffDutyCyclePercentageAndZeroFormationAndMaintenanceDutyCycles();
   NonHarvestPercentageVoteShouldBe(newData.offDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroOffDutyCyclePercentage, ShouldVoteWithOffDutyCycleOnInitIfIceCabinetTemperatureIsWarmAndFormationAndOffDutyCyclesAreZero)
{
   GivenCabinetTemperatureIs(AboveThreshold);
   GivenModuleIsInitializedWithNonZeroMaintenancePercentageAndZeroFormationAndOffDutyCycles();
   NonHarvestPercentageVoteShouldBe(newData.offDutyCyclePercentage, Vote_Care);
}

TEST(NonZeroOffDutyCyclePercentage, ShouldVoteWithOffDutyCycleOnInitIfIceCabinetTemperatureIsWarmAndMaintenanceAndOffDutyCyclesAreZero)
{
   GivenCabinetTemperatureIs(AboveThreshold);
   GivenModuleIsInitializedWithNonZeroFormationDutyCyclePercentageAndZeroMaintenanceAndOffDutyCycles();
   NonHarvestPercentageVoteShouldBe(newData.offDutyCyclePercentage, Vote_Care);
}
