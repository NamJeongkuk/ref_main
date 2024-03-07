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
#include "PlatformData.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

static const TimeThatPrechillConditionsAreMetConfiguration_t config = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingModeErd = Erd_CoolingMode,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(TimeThatPrechillConditionsAreMet_SingleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   TimeThatPrechillConditionsAreMet_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
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

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
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

TEST(TimeThatPrechillConditionsAreMet_SingleEvap, ShouldCountUpEachMinuteThatCompressorIsOnAndCoolingModeIsFreezerOnInit)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
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
   And CoolingModeIs(CoolingMode_Freezer);
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

TEST(TimeThatPrechillConditionsAreMet_SingleEvap, ShouldStopCountingUpAndResetTimeToZeroWhenTheCoolingModeIsNotFreezer)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CoolingModeIs(CoolingMode_FreshFood);

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
   And CoolingModeIs(CoolingMode_Freezer);
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

TEST(TimeThatPrechillConditionsAreMet_SingleEvap, ShouldRestartCountingUpWhenTheCoolingModeBecomesFreezerAgain)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CoolingModeIs(CoolingMode_FreshFood);
   TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

   When CoolingModeIs(CoolingMode_Freezer);

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
   TimeThatPrechillConditionsAreMet_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvapFourDoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
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

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne()
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(1);
   }
};

TEST(TimeThatPrechillConditionsAreMet_DualEvap, ShouldCountUpEachMinuteThatCompressorIsOnAndCoolingModeIsFreezerOnInit)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST(TimeThatPrechillConditionsAreMet_DualEvap, ShouldStopCountingUpAndResetTimeToZeroWhenTheCompressorTurnsOff)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
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

TEST(TimeThatPrechillConditionsAreMet_DualEvap, ShouldStopCountingUpAndResetToZeroWhenCoolingModeChangesToNotFreezerAndCompressorStaysOn)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CoolingModeIs(CoolingMode_FreshFood);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_DualEvap, ShouldRestartCountingUpWhenCoolingModeIsFreezerAgain)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CoolingModeIs(CoolingMode_FreshFood);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }

   When CoolingModeIs(CoolingMode_Freezer);

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
   TimeThatPrechillConditionsAreMet_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentTripleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
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

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
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

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldCountUpEachMinuteThatTheCompressorIsOnAndCoolingModeIsFreezerAndConvertibleCompartmentIsActingAsAFreshFoodOnInit)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
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

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldCountUpEachMinuteThatTheCompressorIsOnAndCoolingModeIsConvertibleCompartmentAndConvertibleCompartmentIsActingAsAFreezerOnInit)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
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

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetToZeroWhenTheCompressorTurnsOffAndCoolingModeIsFreezerAndConvertibleCompartmentIsActingAsAFreshFood)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
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

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetToZeroWhenTheCompressorTurnsOffAndCoolingModeIsConvertibleCompartmentAndConvertibleCompartmentIsActingAsAFreezer)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
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

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetToZeroWhenTheCoolingModeChangesToNotFreezerAndCompressorStaysOnAndConvertibleCompartmentIsActingAsAFreshFood)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CoolingModeIs(CoolingMode_ConvertibleCompartment);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldRestartCountingWhenCoolingModeChangesBackToFreezerAndConvertibleCompartmentIsActingAsAFreshFood)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_Freezer);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CoolingModeIs(CoolingMode_ConvertibleCompartment);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }

   When CoolingModeIs(CoolingMode_Freezer);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(i + 1);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldStopCountingUpAndResetToZeroWhenTheCoolingModeChangesToNotConvertibleCompartmentAndCompressorStaysOnAndConvertibleCompartmentIsActingAsAFreezer)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CoolingModeIs(CoolingMode_Freezer);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }
}

TEST(TimeThatPrechillConditionsAreMet_TripleEvap, ShouldRestartCountingWhenCoolingModeChangesBackToConvertibleCompartmentAndConvertibleCompartmentIsActingAsAFreezer)
{
   Given CompressorIs(ON);
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   And TimeThatPrechillConditionsAreMetIsInitialized();

   AfterOneMinuteTimeThatPrechillConditionsAreMetShouldBeOne();

   When CoolingModeIs(CoolingMode_Freezer);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MSEC_PER_MIN - 1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);

      After(1);
      TimeThatPrechillConditionsAreMetInMinutesShouldBe(0);
   }

   When CoolingModeIs(CoolingMode_ConvertibleCompartment);

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
   And CoolingModeIs(CoolingMode_Freezer);
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
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
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
   And CoolingModeIs(CoolingMode_Freezer);
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
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
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
