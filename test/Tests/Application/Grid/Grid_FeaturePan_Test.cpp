/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Grid_FeaturePan.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "utils.h"
#include "Constants_Binary.h"
#include "ConstArrayMap_FeaturePan.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

#define When
#define Given
#define And
#define The

enum
{
   Valid = true,
   Invalid = false
};

TEST_GROUP(Grid_FeaturePan_Test)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   ConstArrayMap_FeaturePan_t featurePanConstArrayMap;
   EventSubscription_t dataModelOnChangeSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_SwqaMultidoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      DataModelErdPointerAccess_Write(dataModel, Erd_FeaturePanCoolingStatesGridVotesConstArrayMapInterface, ConstArrayMap_FeaturePan_Init(&featurePanConstArrayMap));
   }

   void GridBlockBecomes(GridBlockNumber_t actual)
   {
      DataModel_Write(dataModel, Erd_FeaturePanGrid_BlockNumber, &actual);
   }

   void GridIsRun()
   {
      Grid_FeaturePan(dataModel);
   }

   void FeaturePanCoolingModeIs(FeaturePanCoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_FeaturePanCoolingMode, &coolingMode);
   }

   void FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_t expected)
   {
      FeaturePanCoolingMode_t actual;
      DataModel_Read(dataModel, Erd_FeaturePanCoolingMode, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FeaturePanForcedHeatingModeShouldBe(CoolingSpeed_t expected)
   {
      CoolingSpeed_t actual;
      DataModel_Read(dataModel, Erd_FeaturePanForcedHeatingMode, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CoolingModeIs(CoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &coolingMode);
   }

   void CoolConvertibleCompartmentBeforeOffIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CoolConvertibleCompartmentBeforeOff, &state);
   }

   void DelayConvertibleCompartmentCoolingIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DelayConvertibleCompartmentCooling, &state);
   }

   void UseDelayedConvertibleCompartmentCoolingSpeedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_UseDelayedConvertibleCompartmentCoolingSpeedEnabledByGrid, &state);
   }

   void FeaturePanGridBlockNumberIs(GridBlockNumber_t gridBlockNumber)
   {
      DataModel_Write(dataModel, Erd_FeaturePanGrid_BlockNumber, &gridBlockNumber);
   }

   void CoolingConditionParametersAre(
      bool coolConvertibleCompartmentBeforeOff,
      CoolingMode_t coolingMode,
      bool delayConvertibleCompartmentCooling)
   {
      CoolConvertibleCompartmentBeforeOffIs(coolConvertibleCompartmentBeforeOff);
      CoolingModeIs(coolingMode);
      DelayConvertibleCompartmentCoolingIs(delayConvertibleCompartmentCooling);
   }

   void GridVotesShouldBe(
      DamperPosition_t expectedDamperPosition,
      FanSpeed_t expectedFanSpeed,
      PercentageDutyCycle_t expectedHeaterPercentageDutyCycle)
   {
      DamperVotedPosition_t actualCCDamperPosition;
      DamperVotedPosition_t actualDeliPanDamperPosition;
      FanVotedSpeed_t actualCCFanSpeed;
      FanVotedSpeed_t actualDeliPanFanSpeed;
      PercentageDutyCycleVote_t actualCcHeaterDutyCycle;
      PercentageDutyCycleVote_t actualDeliPanHeaterDutyCycle;

      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDamperPosition_GridVote, &actualCCDamperPosition);
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentEvapFanSpeed_GridVote, &actualCCFanSpeed);
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentHeater_GridVote, &actualCcHeaterDutyCycle);
      DataModel_Read(dataModel, Erd_DeliDamperPosition_GridVote, &actualDeliPanDamperPosition);
      DataModel_Read(dataModel, Erd_DeliFanSpeed_GridVote, &actualDeliPanFanSpeed);
      DataModel_Read(dataModel, Erd_DeliPanHeater_GridVote, &actualDeliPanHeaterDutyCycle);

      CHECK_EQUAL(actualCCDamperPosition.position, actualDeliPanDamperPosition.position);
      CHECK_EQUAL(actualCCDamperPosition.care, actualDeliPanDamperPosition.care);
      CHECK_EQUAL(actualCCFanSpeed.speed, actualDeliPanFanSpeed.speed);
      CHECK_EQUAL(actualCCFanSpeed.care, actualDeliPanFanSpeed.care);
      CHECK_EQUAL(actualCcHeaterDutyCycle.percentageDutyCycle, actualDeliPanHeaterDutyCycle.percentageDutyCycle);
      CHECK_EQUAL(actualCcHeaterDutyCycle.care, actualDeliPanHeaterDutyCycle.care);

      CHECK_EQUAL(expectedDamperPosition, actualCCDamperPosition.position);
      CHECK_EQUAL(expectedFanSpeed, actualCCFanSpeed.speed);
      CHECK_EQUAL(expectedHeaterPercentageDutyCycle, actualCcHeaterDutyCycle.percentageDutyCycle);

      CHECK_TRUE(actualCCDamperPosition.care);
      CHECK_TRUE(actualCCFanSpeed.care);
      CHECK_TRUE(actualCcHeaterDutyCycle.care);
   }
};

TEST(Grid_FeaturePan_Test, ShouldOutputCorrectValuesForBlock0)
{
   When GridBlockBecomes(0);
   And The GridIsRun();

   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ActiveCooling);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);

   GridVotesShouldBe(DamperPosition_Open, FanSpeed_On, PercentageDutyCycle_Min);
}

TEST(Grid_FeaturePan_Test, ShouldOutputCorrectValuesForBlock1)
{
   When CoolingConditionParametersAre(true, CoolingMode_ConvertibleCompartment, false);
   And GridBlockBecomes(1);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ActiveCooling);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Open, FanSpeed_On, PercentageDutyCycle_Min);

   When CoolingConditionParametersAre(false, CoolingMode_Off, true);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_Neutral);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_Off, PercentageDutyCycle_Min);

   When CoolingConditionParametersAre(false, CoolingMode_Off, false);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_Neutral);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_Off, PercentageDutyCycle_Min);

   When CoolingConditionParametersAre(false, CoolingMode_ConvertibleCompartment, false);
   And GridBlockBecomes(1);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ActiveCooling);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Open, FanSpeed_On, PercentageDutyCycle_Min);
}

TEST(Grid_FeaturePan_Test, ShouldOutputCorrectValuesForBlock2)
{
   When CoolingConditionParametersAre(true, CoolingMode_Freezer, false);
   And GridBlockBecomes(2);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ActiveCooling);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Open, FanSpeed_On, PercentageDutyCycle_Min);

   When CoolingConditionParametersAre(false, CoolingMode_ConvertibleCompartment, false);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_Neutral);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_Off, PercentageDutyCycle_Min);

   When CoolingConditionParametersAre(false, CoolingMode_Freezer, true);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_Neutral);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_Off, PercentageDutyCycle_Min);

   When CoolingConditionParametersAre(false, CoolingMode_Freezer, false);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ActiveCooling);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Open, FanSpeed_On, PercentageDutyCycle_Min);
}

TEST(Grid_FeaturePan_Test, ShouldOutputCorrectValuesForBlock3)
{
   When FeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveHeating);
   And GridBlockBecomes(3);
   And The GridIsRun();

   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_Neutral);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);

   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_Off, PercentageDutyCycle_Min);
}

TEST(Grid_FeaturePan_Test, ShouldMaintainPreviousCoolingModeForBlock3WhenCoolingModeIsNotActiveHeating)
{
   When FeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   And GridBlockBecomes(3);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ActiveCooling);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Open, FanSpeed_On, PercentageDutyCycle_Min);

   When FeaturePanCoolingModeIs(FeaturePanCoolingMode_ForcedHeating);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ForcedHeating);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_On, PercentageDutyCycle_Max);
}

TEST(Grid_FeaturePan_Test, ShouldOutputCorrectValuesForBlock4)
{
   When FeaturePanCoolingModeIs(FeaturePanCoolingMode_Neutral);
   And GridBlockBecomes(4);
   And The GridIsRun();

   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);

   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_Off, PercentageDutyCycle_Min);
}

TEST(Grid_FeaturePan_Test, ShouldOutputCorrectValuesForBlock5)
{
   When GridBlockBecomes(5);
   And The GridIsRun();

   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_Neutral);

   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_Off, PercentageDutyCycle_Min);
}

TEST(Grid_FeaturePan_Test, ShouldMaintainPreviousCoolingModeForBlock5WhenCoolingModeIsNotActiveCooling)
{
   When FeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveHeating);
   When GridBlockBecomes(5);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ActiveHeating);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_On, PercentageDutyCycle_Max);

   When FeaturePanCoolingModeIs(FeaturePanCoolingMode_ForcedHeating);
   When GridBlockBecomes(5);
   And The GridIsRun();
   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ForcedHeating);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Inactive);
   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_On, PercentageDutyCycle_Max);
}

TEST(Grid_FeaturePan_Test, ShouldOutputCorrectValuesForBlock6)
{
   When GridBlockBecomes(6);
   And The GridIsRun();

   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ActiveHeating);

   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_On, PercentageDutyCycle_Max);
}

TEST(Grid_FeaturePan_Test, ShouldOutputCorrectValuesForBlock7)
{
   When GridBlockBecomes(7);
   And The GridIsRun();

   FeaturePanCoolingModeShouldBe(FeaturePanCoolingMode_ActiveHeating);
   FeaturePanForcedHeatingModeShouldBe(FeaturePanForcedHeatingMode_Active);

   GridVotesShouldBe(DamperPosition_Closed, FanSpeed_On, PercentageDutyCycle_Max);
}
