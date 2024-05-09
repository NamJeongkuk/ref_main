/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "PeriodicDamperCycling.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DamperData.h"
#include "TddPersonality.h"
#include "DamperVotedPosition.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When
#define Then
#define And

static const PeriodicDamperCyclingConfiguration_t config = {
   .damperCurrentPositionErd = Erd_DamperCurrentPosition,
   .damperFullCycleRequestErd = Erd_DamperFullCycleRequest
};

static const DamperData_t damperData = {
   .damperId = DamperId_NormalDamper,
   .damperStepData = {},
   .stepsToHome = 1850,
   .delayBetweenStepEventsInHundredsOfMicroseconds = 33,
   .maxTimeForDamperToBeOpenInMinutes = 5,
   .maxTimeForDamperToBeClosedInMinutes = 2,
   .targetCompartmentMinimumTemperatureChangeTimeInMinutes = 3,
   .targetCompartmentDamperHeaterOnTimeInMinutes = 2,
   .sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 = 500,
   .targetCompartmentMinimumTemperatureChangeInDegFx100 = 10
};

static const DamperData_t damperDataWithZeroOpenTime = {
   .damperId = DamperId_NormalDamper,
   .damperStepData = {},
   .stepsToHome = 1850,
   .delayBetweenStepEventsInHundredsOfMicroseconds = 33,
   .maxTimeForDamperToBeOpenInMinutes = 0,
   .maxTimeForDamperToBeClosedInMinutes = 2,
   .targetCompartmentMinimumTemperatureChangeTimeInMinutes = 3,
   .targetCompartmentDamperHeaterOnTimeInMinutes = 2,
   .sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 = 500,
   .targetCompartmentMinimumTemperatureChangeInDegFx100 = 10
};

static const DamperData_t damperDataWithZeroClosedTime = {
   .damperId = DamperId_NormalDamper,
   .damperStepData = {},
   .stepsToHome = 1850,
   .delayBetweenStepEventsInHundredsOfMicroseconds = 33,
   .maxTimeForDamperToBeOpenInMinutes = 2,
   .maxTimeForDamperToBeClosedInMinutes = 0,
   .targetCompartmentMinimumTemperatureChangeTimeInMinutes = 3,
   .targetCompartmentDamperHeaterOnTimeInMinutes = 2,
   .sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 = 500,
   .targetCompartmentMinimumTemperatureChangeInDegFx100 = 10
};

static const DamperData_t damperDataWithZeroOpenAndClosedTime = {
   .damperId = DamperId_NormalDamper,
   .damperStepData = {},
   .stepsToHome = 1850,
   .delayBetweenStepEventsInHundredsOfMicroseconds = 33,
   .maxTimeForDamperToBeOpenInMinutes = 0,
   .maxTimeForDamperToBeClosedInMinutes = 0,
   .targetCompartmentMinimumTemperatureChangeTimeInMinutes = 3,
   .targetCompartmentDamperHeaterOnTimeInMinutes = 2,
   .sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 = 500,
   .targetCompartmentMinimumTemperatureChangeInDegFx100 = 10
};

TEST_GROUP(PeriodicDamperCycling)
{
   PeriodicDamperCycling_t instance;
   I_DataModel_t *dataModel;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenDamperCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(dataModel, Erd_DamperCurrentPosition, &position);
   }

   void WhenDamperCurrentPositionIs(DamperPosition_t position)
   {
      GivenDamperCurrentPositionIs(position);
   }

   void GivenTheModuleIsInitializedWithDamperData(const DamperData_t *damperData)
   {
      PeriodicDamperCycling_Init(&instance, dataModelDouble.dataModel, &config, damperData);
   }

   void GivenFullCycleRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DamperFullCycleRequest, &state);
   }

   void FullCycleRequestShouldBeClear()
   {
      bool actualRequest;
      DataModel_Read(dataModel, Erd_DamperFullCycleRequest, &actualRequest);
      CHECK_FALSE(actualRequest);
   }

   void FullCycleRequestShouldBeSet()
   {
      bool actualRequest;
      DataModel_Read(dataModel, Erd_DamperFullCycleRequest, &actualRequest);
      CHECK_TRUE(actualRequest);
   }

   void GivenDamperClosedDuringMaxOpenTime()
   {
      GivenDamperCurrentPositionIs(DamperPosition_Closed);
      GivenTheModuleIsInitializedWithDamperData(&damperData);
      WhenDamperCurrentPositionIs(DamperPosition_Open);

      After(damperData.maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);

      WhenDamperCurrentPositionIs(DamperPosition_Closed);

      After(1);
      FullCycleRequestShouldBeClear();
   }
};

TEST(PeriodicDamperCycling, ShouldRequestFullCycleAfterDamperIsOpenForMaxTime)
{
   GivenDamperCurrentPositionIs(DamperPosition_Open);
   GivenTheModuleIsInitializedWithDamperData(&damperData);

   After(damperData.maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeSet();
}

TEST(PeriodicDamperCycling, ShouldRequestFullCycleAfterDamperIsClosedForMaxTime)
{
   GivenDamperCurrentPositionIs(DamperPosition_Closed);
   GivenTheModuleIsInitializedWithDamperData(&damperData);

   After(damperData.maxTimeForDamperToBeClosedInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeSet();
}

TEST(PeriodicDamperCycling, ShouldRequestFullCycleAfterDamperOpensAndStaysOpenForMaxTime)
{
   GivenDamperCurrentPositionIs(DamperPosition_Closed);
   GivenTheModuleIsInitializedWithDamperData(&damperData);
   WhenDamperCurrentPositionIs(DamperPosition_Open);

   After(damperData.maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeSet();
}

TEST(PeriodicDamperCycling, ShouldNotRequestFullCycleWhenDamperClosesBeforeMaxOpenTimeExpires)
{
   GivenDamperCurrentPositionIs(DamperPosition_Closed);
   GivenTheModuleIsInitializedWithDamperData(&damperData);
   WhenDamperCurrentPositionIs(DamperPosition_Open);

   After(damperData.maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);

   WhenDamperCurrentPositionIs(DamperPosition_Closed);

   After(1);
   FullCycleRequestShouldBeClear();
}

TEST(PeriodicDamperCycling, ShouldRestartOpenTimerWhenDamperClosesDuringMaxOpenTimeAndThenReopens)
{
   GivenDamperClosedDuringMaxOpenTime();

   WhenDamperCurrentPositionIs(DamperPosition_Open);

   After(damperData.maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeSet();
}

TEST(PeriodicDamperCycling, ShouldNotRequestFullCycleAfterDamperIsOpenedIfMaxTimeForDamperToBeOpenInMinutesIsZero)
{
   GivenDamperCurrentPositionIs(DamperPosition_Closed);
   GivenTheModuleIsInitializedWithDamperData(&damperDataWithZeroOpenTime);

   WhenDamperCurrentPositionIs(DamperPosition_Open);

   After(damperDataWithZeroOpenTime.maxTimeForDamperToBeClosedInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeClear();
};

TEST(PeriodicDamperCycling, ShouldRequestFullCycleIfMaxTimeForDamperToBeClosedIsNonZeroAndMaxTimeForDamperToBeOpenIsZero)
{
   GivenDamperCurrentPositionIs(DamperPosition_Open);
   GivenTheModuleIsInitializedWithDamperData(&damperDataWithZeroOpenTime);

   WhenDamperCurrentPositionIs(DamperPosition_Closed);

   After(damperDataWithZeroOpenTime.maxTimeForDamperToBeClosedInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeSet();
}

TEST(PeriodicDamperCycling, ShouldNotRequestFullCycleAfterDamperIsClosedIfMaxTimeForDamperToBeClosedInMinutesIsZero)
{
   GivenDamperCurrentPositionIs(DamperPosition_Open);
   GivenTheModuleIsInitializedWithDamperData(&damperDataWithZeroClosedTime);

   WhenDamperCurrentPositionIs(DamperPosition_Closed);

   After(damperDataWithZeroOpenTime.maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeClear();
}

TEST(PeriodicDamperCycling, ShouldRequestFullCycleIfMaxTimeForDamperToBeOpenIsNonZeroAndMaxTimeForDamperToBeClosedIsZero)
{
   GivenDamperCurrentPositionIs(DamperPosition_Closed);
   GivenTheModuleIsInitializedWithDamperData(&damperDataWithZeroClosedTime);

   WhenDamperCurrentPositionIs(DamperPosition_Open);

   After(damperDataWithZeroClosedTime.maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeSet();
}

TEST(PeriodicDamperCycling, ShouldNotRequestFullCycleIfMaxTimeForDamperToBeClosedInMinutesIsZeroAndMaxTimeForDamperToBeOpenIsZero)
{
   GivenDamperCurrentPositionIs(DamperPosition_Open);
   GivenTheModuleIsInitializedWithDamperData(&damperDataWithZeroOpenAndClosedTime);

   WhenDamperCurrentPositionIs(DamperPosition_Closed);

   After(damperDataWithZeroOpenAndClosedTime.maxTimeForDamperToBeClosedInMinutes * MSEC_PER_MIN);

   FullCycleRequestShouldBeClear();
}

TEST(PeriodicDamperCycling, ShouldNotRequestFullCycleIfMaxTimeForDamperToBeOpenInMinutesIsZerondMaxTimeForDamperToBeClosedIsZero)
{
   GivenDamperCurrentPositionIs(DamperPosition_Closed);
   GivenTheModuleIsInitializedWithDamperData(&damperDataWithZeroOpenAndClosedTime);

   WhenDamperCurrentPositionIs(DamperPosition_Open);

   After(damperDataWithZeroOpenAndClosedTime.maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN);

   FullCycleRequestShouldBeClear();
}
