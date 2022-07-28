/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "TimeThatPrechillConditionsAreMet.h"
#include "DataModelErdPointerAccess.h"
#include "EvaporatorData.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

static const EvaporatorData_t singleEvaporatorData = {
   .numberOfEvaporators = 1
};

static const EvaporatorData_t dualEvaporatorData = {
   .numberOfEvaporators = 2
};

static const EvaporatorData_t tripleEvaporatorData = {
   .numberOfEvaporators = 3
};

static const TimeThatPrechillConditionsAreMetConfiguration_t config = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .sealedSystemValvePositionResolvedVoteErd = Erd_ValvePosition_ResolvedVote,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(TimeThatPrechillConditionsAreMet_SingleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   TimeThatPrechillConditionsAreMet_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &singleEvaporatorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void TimeThatPrechillConditionsAreMetIsInitialized()
   {
      TimeThatPrechillConditionsAreMet_Init(&instance, dataModel, &config);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t minutes)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &minutes);
   }

   void TimeThatPrechillConditionsAreMetInMinutesShouldBe(uint16_t expectedMinutes)
   {
      uint16_t actualMinutes;
      DataModel_Read(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &actualMinutes);

      CHECK_EQUAL(expectedMinutes, actualMinutes);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void SealedSystemValveIsInPosition(ValvePosition_t position)
   {
      ValveVotedPosition_t vote;

      vote.care = true;
      vote.position = position;

      DataModel_Write(dataModel, Erd_ValvePosition_ResolvedVote, &vote);
   }

   void AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne()
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(1);
   }
};

TEST(TimeThatPrechillConditionsAreMet_SingleEvap, ShouldInitialize)
{
   TimeThatPrechillConditionsAreMet_Init(&instance, dataModel, &config);
}

TEST(TimeThatPrechillConditionsAreMet_SingleEvap, ShouldResetTimeThatPrechillConditionsAreMetToZeroOnInit)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(999);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
}

TEST(TimeThatPrechillConditionsAreMet_SingleEvap, ShouldCountUpEachMinuteThatTheCompressorIsOnIfItIsOnOnInit)
{
   Given CompressorIs(ON);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST(TimeThatPrechillConditionsAreMet_SingleEvap, ShouldStopCountingUpAndResetTimeToZeroWhenTheCompressorTurnsOff)
{
   Given CompressorIs(ON);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CompressorIs(OFF);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_SingleEvap, ShouldRestartCountingUpWhenTheCompressorTurnsOnAgain)
{
   Given CompressorIs(ON);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CompressorIs(OFF);
   TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

   When CompressorIs(ON);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST_GROUP(TimeThatPrechillConditionsAreMet_DualEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   TimeThatPrechillConditionsAreMet_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &dualEvaporatorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void TimeThatPrechillConditionsAreMetIsInitialized()
   {
      TimeThatPrechillConditionsAreMet_Init(&instance, dataModel, &config);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t minutes)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &minutes);
   }

   void TimeThatPrechillConditionsAreMetInMinutesShouldBe(uint16_t expectedMinutes)
   {
      uint16_t actualMinutes;
      DataModel_Read(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &actualMinutes);

      CHECK_EQUAL(expectedMinutes, actualMinutes);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void SealedSystemValveIsInPosition(ValvePosition_t position)
   {
      ValveVotedPosition_t vote;

      vote.care = true;
      vote.position = position;

      DataModel_Write(dataModel, Erd_ValvePosition_ResolvedVote, &vote);
   }

   void AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne()
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(1);
   }
};

TEST(TimeThatPrechillConditionsAreMet_DualEvap, ShouldCountUpEachMinuteThatTheCompressorIsOnAndValveIsInPositionBIfItIsOnOnInit)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST(TimeThatPrechillConditionsAreMet_DualEvap, ShouldStopCountingUpAndResetToZeroWhenTheCompressorTurnsOffAndValveStaysInPositionB)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CompressorIs(OFF);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_DualEvap, ShouldStopCountingUpAndResetToZeroWhenTheValveMovesToAPositionOtherThanAndCompressorStaysOn)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When SealedSystemValveIsInPosition(ValvePosition_A);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_DualEvap, ShouldRestartCountingUpWhenValveMovesBackToPositionB)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When SealedSystemValveIsInPosition(ValvePosition_A);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }

   When SealedSystemValveIsInPosition(ValvePosition_B);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST_GROUP(TimeThatPrechillConditionsAreMet_TripleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   TimeThatPrechillConditionsAreMet_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &tripleEvaporatorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void TimeThatPrechillConditionsAreMetIsInitialized()
   {
      TimeThatPrechillConditionsAreMet_Init(&instance, dataModel, &config);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t minutes)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &minutes);
   }

   void TimeThatPrechillConditionsAreMetInMinutesShouldBe(uint16_t expectedMinutes)
   {
      uint16_t actualMinutes;
      DataModel_Read(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &actualMinutes);

      CHECK_EQUAL(expectedMinutes, actualMinutes);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void SealedSystemValveIsInPosition(ValvePosition_t position)
   {
      ValveVotedPosition_t vote;

      vote.care = true;
      vote.position = position;

      DataModel_Write(dataModel, Erd_ValvePosition_ResolvedVote, &vote);
   }

   void ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t type)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &type);
   }

   void AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne()
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(1);
   }
};

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldCountUpEachMinuteThatTheCompressorIsOnAndValveIsInPositionBAndConvertibleCompartmentIsActingAsAFreshFoodOnInit)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldCountUpEachMinuteThatTheCompressorIsOnAndValveIsInPositionCAndConvertibleCompartmentIsActingAsAFreezerOnInit)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_C);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetToZeroWhenTheCompressorTurnsOffAndValveStaysInPositionBAndConvertibleCompartmentIsActingAsAFreshFood)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CompressorIs(OFF);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetToZeroWhenTheCompressorTurnsOffAndValveStaysInPositionCAndConvertibleCompartmentIsActingAsAFreezer)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_C);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CompressorIs(OFF);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetToZeroWhenTheValveMovesToAPositionOtherThanBAndCompressorStaysOnAndConvertibleCompartmentIsActingAsAFreshFood)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When SealedSystemValveIsInPosition(ValvePosition_A);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldRestartCountingWhenValveMovesBackIntoPositionBAndConvertibleCompartmentIsActingAsAFreshFood)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When SealedSystemValveIsInPosition(ValvePosition_A);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }

   When SealedSystemValveIsInPosition(ValvePosition_B);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetToZeroWhenTheValveMovesToAPositionOtherThanCAndCompressorStaysOnAndConvertibleCompartmentIsActingAsAFreezer)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_C);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When SealedSystemValveIsInPosition(ValvePosition_A);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldRestartCountingWhenValveMovesBackIntoPositionBAndConvertibleCompartmentIsActingAsAFreezer)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_C);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When SealedSystemValveIsInPosition(ValvePosition_A);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }

   When SealedSystemValveIsInPosition(ValvePosition_C);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetTimeToZeroWhenTheConvertibleCompartmentChangesFromActingLikeAFreshFoodToAFreezerIfPrechillConditionsWereOtherwiseMet)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetTimeToZeroWhenTheConvertibleCompartmentChangesFromActingLikeAFreezerToAFreshFoodIfPrechillConditionsWereOtherwiseMet)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_C);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldRestartCountingWhenConvertibleCompartmentActsAsAFreshFoodAgainAndPrechillConditionsAreMet)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_B);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

   When ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldRestartCountingWhenConvertibleCompartmentActsAsAFreezerAgainAndPrechillConditionsAreMet)
{
   Given CompressorIs(ON);
   And SealedSystemValveIsInPosition(ValvePosition_C);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

   When ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}
