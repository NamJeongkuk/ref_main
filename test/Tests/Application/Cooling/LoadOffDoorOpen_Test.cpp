/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "LoadOffDoorOpen.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "PersonalityParametricData.h"
#include "BooleanVotedState.h"
#include "VoteType.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomeTimeInMsec = 12 * MSEC_PER_MIN
};

enum
{
   Erd_Boolean_LoadVoteStruct,
   Erd_U8_LoadVoteStruct,
   Erd_U16_LoadVoteStruct,
   Erd_U32_LoadVoteStruct,
   Erd_Door0Status,
   Erd_Door1Status,
   Erd_TimerModule
};

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_Boolean_LoadVoteStruct, sizeof(BooleanVotedState_t) },
   { Erd_U8_LoadVoteStruct, sizeof(U8Vote_t) },
   { Erd_U16_LoadVoteStruct, sizeof(U16Vote_t) },
   { Erd_U32_LoadVoteStruct, sizeof(U32Vote_t) },
   { Erd_Door0Status, sizeof(bool) },
   { Erd_Door1Status, sizeof(bool) },
   { Erd_TimerModule, sizeof(TimerModule_t *) }
};

static bool booleanOffValue = false;
static uint8_t U8OffValue = 0;
static uint16_t U16OffValue = 0;
static uint32_t U32OffValue = 0;

static const LoadVotePair_t loadVotePairs[] = {
   { .loadVoteErd = Erd_Boolean_LoadVoteStruct, .offValue = booleanOffValue },
   { Erd_U8_LoadVoteStruct, U8OffValue },
   { Erd_U16_LoadVoteStruct, U16OffValue },
   { Erd_U32_LoadVoteStruct, U32OffValue }
};

static const CompartmentVoteErdList_t voteErdList = {
   .pairs = loadVotePairs,
   .numberOfPairs = NUM_ELEMENTS(loadVotePairs)
};

static const Erd_t doorErds[] = {
   Erd_Door0Status,
   Erd_Door1Status
};

static const ErdList_t doorList = {
   .erds = doorErds,
   .numberOfErds = NUM_ELEMENTS(doorErds),
};

static const LoadOffDoorOpenConfiguration_t config = {
   .compartmentVoteErdList = voteErdList,
   .doorStatusErdList = doorList,
   .timerModuleErd = Erd_TimerModule
};

static const LoadOffDoorOpenCompartmentData_t data = {
   .enable = true,
   .timeoutInSeconds = 106,
   .restartDelayInSeconds = 16
};

TEST_GROUP(LoadOffDoorOpen)
{
   LoadOffDoorOpen_t instance;
   DataModel_TestDouble_t dataModelTestDouble;
   TimerModule_TestDouble_t timerModuleTestDouble;

   I_DataModel_t *dataModel;
   LoadOffDoorOpenCompartmentData_t dataWithZeroDelay;
   LoadOffDoorOpenCompartmentData_t dataWithZeroOffTimeout;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erdTable, NUM_ELEMENTS(erdTable));
      dataModel = dataModelTestDouble.dataModel;
      TimerModule_TestDouble_Init(&timerModuleTestDouble);

      DataModelErdPointerAccess_Write(
         dataModel,
         Erd_TimerModule,
         TimerModule_TestDouble_GetTimerModule(&timerModuleTestDouble));

      dataWithZeroDelay = data;
      dataWithZeroOffTimeout = data;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, ticks);
   }

   void GivenModuleIsInitialized()
   {
      LoadOffDoorOpen_Init(
         &instance,
         dataModel,
         &data,
         &config);
   }

   void GivenModuleIsInitializedWithZeroDelay()
   {
      dataWithZeroDelay.restartDelayInSeconds = 0;

      LoadOffDoorOpen_Init(
         &instance,
         dataModel,
         &dataWithZeroDelay,
         &config);
   }

   void GivenModuleIsInitializedWithZeroOffTimeout()
   {
      dataWithZeroOffTimeout.timeoutInSeconds = 0;

      LoadOffDoorOpen_Init(
         &instance,
         dataModel,
         &dataWithZeroOffTimeout,
         &config);
   }

   void GivenADoorIsOpen()
   {
      DataModel_Write(dataModel, Erd_Door0Status, set);
   }

   void GivenAllDoorsAreClosed()
   {
      DataModel_Write(dataModel, Erd_Door0Status, clear);
      DataModel_Write(dataModel, Erd_Door1Status, clear);
   }

   void WhenAllDoorsAreClosed()
   {
      GivenAllDoorsAreClosed();
   }

   void WhenADoorOpens()
   {
      GivenADoorIsOpen();
   }

   void WhenAnotherDoorOpens()
   {
      DataModel_Write(dataModel, Erd_Door1Status, set);
   }

   void WhenADoorCloses()
   {
      DataModel_Write(dataModel, Erd_Door0Status, clear);
   }

   void AllLoadsShouldBeVotedWithOffValue()
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(loadVotePairs); i++)
      {
         uint8_t erdSize = DataModel_SizeOf(dataModel, loadVotePairs[i].loadVoteErd);

         switch(erdSize)
         {
            case sizeof(U8Vote_t): {
               U8Vote_t u8VoteErd;
               DataModel_Read(
                  dataModel,
                  loadVotePairs[i].loadVoteErd,
                  &u8VoteErd);

               CHECK_EQUAL(Vote_Care, u8VoteErd.vote);
               CHECK_EQUAL(loadVotePairs[i].offValue, u8VoteErd.value);
            }
            break;

            case sizeof(U16Vote_t): {
               U16Vote_t u16VoteErd;
               DataModel_Read(
                  dataModel,
                  loadVotePairs[i].loadVoteErd,
                  &u16VoteErd);

               CHECK_EQUAL(Vote_Care, u16VoteErd.vote);
               CHECK_EQUAL(loadVotePairs[i].offValue, u16VoteErd.value);
            }
            break;

            case sizeof(U32Vote_t): {
               U32Vote_t u32VoteErd;
               DataModel_Read(
                  dataModel,
                  loadVotePairs[i].loadVoteErd,
                  &u32VoteErd);

               CHECK_EQUAL(Vote_Care, u32VoteErd.vote);
               CHECK_EQUAL(loadVotePairs[i].offValue, u32VoteErd.value);
            }
         }
      }
   }

   void AllLoadsShouldBeVotedDontCare()
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(loadVotePairs); i++)
      {
         uint8_t erdSize = DataModel_SizeOf(dataModel, loadVotePairs[i].loadVoteErd);

         switch(erdSize)
         {
            case sizeof(U8Vote_t): {
               U8Vote_t u8VoteErd;
               DataModel_Read(
                  dataModel,
                  loadVotePairs[i].loadVoteErd,
                  &u8VoteErd);

               CHECK_EQUAL(Vote_DontCare, u8VoteErd.vote);
            }
            break;

            case sizeof(U16Vote_t): {
               U16Vote_t u16VoteErd;
               DataModel_Read(
                  dataModel,
                  loadVotePairs[i].loadVoteErd,
                  &u16VoteErd);

               CHECK_EQUAL(Vote_DontCare, u16VoteErd.vote);
            }
            break;

            case sizeof(U32Vote_t): {
               U32Vote_t u32VoteErd;
               DataModel_Read(
                  dataModel,
                  loadVotePairs[i].loadVoteErd,
                  &u32VoteErd);

               CHECK_EQUAL(Vote_DontCare, u32VoteErd.vote);
            }
         }
      }
   }
};

TEST(LoadOffDoorOpen, ShouldInitialize)
{
   GivenModuleIsInitialized();
}

TEST(LoadOffDoorOpen, ShouldVoteAllLoadsOffIfADoorIsOpenOnInit)
{
   GivenADoorIsOpen();
   GivenModuleIsInitialized();

   AllLoadsShouldBeVotedWithOffValue();
}

TEST(LoadOffDoorOpen, ShouldNotVoteAllLoadsOffIfAllDoorsAreClosedOnInit)
{
   GivenAllDoorsAreClosed();
   GivenModuleIsInitialized();

   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldVoteAllLoadsDontCareAfterSomeTimeFromVotingThemOffIfDoorIsOpenOnInit)
{
   GivenADoorIsOpen();
   GivenModuleIsInitialized();

   After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
   AllLoadsShouldBeVotedWithOffValue();

   After(1);
   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldNotVoteAllLoadsDontCareAfterSomeTimeFromVotingThemOffIfDoorIsOpenOnInitAndOffTimeoutIsZero)
{
   GivenADoorIsOpen();
   GivenModuleIsInitializedWithZeroOffTimeout();
   AllLoadsShouldBeVotedWithOffValue();

   After(SomeTimeInMsec);
   AllLoadsShouldBeVotedWithOffValue();
}

TEST(LoadOffDoorOpen, ShouldVoteDontCareAfterADelayWhenADoorClosesBeforeOffTimeoutExpires)
{
   GivenADoorIsOpen();
   GivenModuleIsInitialized();

   After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
   WhenAllDoorsAreClosed();
   AllLoadsShouldBeVotedWithOffValue();

   After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);
   AllLoadsShouldBeVotedWithOffValue();

   After(1);
   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldVoteDontCareImmediatelyWhenADoorClosesBeforeOffTimeoutExpiresIfDelayIsZero)
{
   GivenADoorIsOpen();
   GivenModuleIsInitializedWithZeroDelay();

   After(dataWithZeroDelay.timeoutInSeconds * MSEC_PER_SEC - 1);
   WhenAllDoorsAreClosed();
   After(0);
   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldVoteAllLoadsOffWhenADoorOpenWhileDoorsAreClosedAndWaitingForDelayTimeToExpire)
{
   GivenADoorIsOpen();
   GivenModuleIsInitialized();

   After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
   WhenAllDoorsAreClosed();
   AllLoadsShouldBeVotedWithOffValue();

   After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);

   WhenADoorOpens();
   AllLoadsShouldBeVotedWithOffValue();
}

TEST(LoadOffDoorOpen, ShouldVoteDontCareForAllLoadsAfterTimeoutGivenADoorOpensWhileAllDoorsAreClosedWhileWaitingForDelayTimeToExpire)
{
   GivenADoorIsOpen();
   GivenModuleIsInitialized();

   After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
   WhenAllDoorsAreClosed();
   AllLoadsShouldBeVotedWithOffValue();

   After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);

   WhenADoorOpens();
   AllLoadsShouldBeVotedWithOffValue();

   After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
   AllLoadsShouldBeVotedWithOffValue();

   After(1);
   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldVoteDontCareForAllLoadsIfAllDoorsAreClosedOnInit)
{
   GivenAllDoorsAreClosed();
   GivenModuleIsInitialized();
   AllLoadsShouldBeVotedDontCare();

   After(data.restartDelayInSeconds * MSEC_PER_SEC);
   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldVoteAllLoadsOffWhenADoorOpensIfAllDoorsAreClosedOnInit)
{
   GivenAllDoorsAreClosed();
   GivenModuleIsInitialized();
   AllLoadsShouldBeVotedDontCare();

   WhenADoorOpens();
   AllLoadsShouldBeVotedWithOffValue();
}

TEST(LoadOffDoorOpen, ShouldNotRestartTimeoutTimeIfAnotherDoorOpensWhileADoorIsAlreadyOpenAndWaitingForTimeout)
{
   GivenADoorIsOpen();
   GivenModuleIsInitialized();

   After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
   WhenAnotherDoorOpens();

   After(1);
   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldRestartTheTimerWithTheDelayTimeWhenAllDoorsCloseWhileADoorIsOpenAndWhileWaitingForTheTimeoutToExpire)
{
   GivenADoorIsOpen();
   GivenModuleIsInitialized();

   After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
   WhenAllDoorsAreClosed();

   After(1);
   AllLoadsShouldBeVotedWithOffValue();

   After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);
   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldStopTheDelayTimerAndNotStartATimeoutTimerWhenADoorOpensWhileAllDoorsAreClosedWithAZeroOffTimeout)
{
   GivenAllDoorsAreClosed();
   GivenModuleIsInitializedWithZeroOffTimeout();

   After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);
   WhenADoorOpens();
   AllLoadsShouldBeVotedWithOffValue();

   After(1);
   AllLoadsShouldBeVotedWithOffValue();
}

TEST(LoadOffDoorOpen, ShouldNotRestartTimeoutWhenADoorOpensWhileAnotherDoorIsOpened)
{
   GivenADoorIsOpen();
   GivenModuleIsInitialized();
   AllLoadsShouldBeVotedWithOffValue();

   After(data.timeoutInSeconds * MSEC_PER_SEC - 3);

   WhenAnotherDoorOpens();
   AllLoadsShouldBeVotedWithOffValue();

   After(1);

   WhenADoorCloses();
   AllLoadsShouldBeVotedWithOffValue();

   After(1);

   WhenADoorOpens();
   AllLoadsShouldBeVotedWithOffValue();

   After(1);
   AllLoadsShouldBeVotedDontCare();
}
