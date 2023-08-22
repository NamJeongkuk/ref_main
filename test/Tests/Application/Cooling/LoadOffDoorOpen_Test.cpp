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
   Erd_RightSideFreshFoodDoorStatus,
   Erd_LeftSideFreshFoodDoorStatus,
   Erd_LeftSideFreezerDoorStatus,
   Erd_ConvertibleCompartmentDoorStatus,
   Erd_TimerModule
};

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_Boolean_LoadVoteStruct, sizeof(BooleanVotedState_t) },
   { Erd_U8_LoadVoteStruct, sizeof(U8Vote_t) },
   { Erd_U16_LoadVoteStruct, sizeof(U16Vote_t) },
   { Erd_U32_LoadVoteStruct, sizeof(U32Vote_t) },
   { Erd_RightSideFreshFoodDoorStatus, sizeof(bool) },
   { Erd_LeftSideFreshFoodDoorStatus, sizeof(bool) },
   { Erd_LeftSideFreezerDoorStatus, sizeof(bool) },
   { Erd_ConvertibleCompartmentDoorStatus, sizeof(bool) },
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

static const Erd_t associatedDoors[] = {
   Erd_RightSideFreshFoodDoorStatus,
   Erd_LeftSideFreezerDoorStatus,
};

static const Erd_t nonAssociatedDoors[] = {
   Erd_LeftSideFreshFoodDoorStatus,
   Erd_ConvertibleCompartmentDoorStatus,
};

static const LoadOffDoorOpenConfiguration_t config = {
   .compartmentVoteErdList = voteErdList,
   .timerModuleErd = Erd_TimerModule,
   .doorStatus = {
      .leftSideFreshFoodDoorErd = Erd_LeftSideFreshFoodDoorStatus,
      .rightSideFreshFoodDoorErd = Erd_RightSideFreshFoodDoorStatus,
      .leftSideFreezerDoorErd = Erd_LeftSideFreezerDoorStatus,
      .rightSideFreezerDoorErd = Erd_Invalid,
      .convertibleCompartmentDoorErd = Erd_ConvertibleCompartmentDoorStatus,
      .doorInDoorErd = Erd_Invalid,
   }
};

TEST_GROUP(LoadOffDoorOpen)
{
   LoadOffDoorOpen_t instance;
   DataModel_TestDouble_t dataModelTestDouble;
   TimerModule_TestDouble_t timerModuleTestDouble;

   I_DataModel_t *dataModel;
   LoadOffDoorOpenDoorsBitmap_t doorsBitmap;
   LoadOffDoorOpenCompartmentData_t data;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erdTable, NUM_ELEMENTS(erdTable));
      dataModel = dataModelTestDouble.dataModel;
      TimerModule_TestDouble_Init(&timerModuleTestDouble);

      DataModelErdPointerAccess_Write(
         dataModel,
         Erd_TimerModule,
         TimerModule_TestDouble_GetTimerModule(&timerModuleTestDouble));

      BITMAP_SET(doorsBitmap.bitmap, DoorIndex_RightSideFreshFood);
      BITMAP_SET(doorsBitmap.bitmap, DoorIndex_LeftSideFreezer);

      data = {
         .enable = true,
         .doorsBitmap = doorsBitmap,
         .timeoutInSeconds = 106,
         .restartDelayInSeconds = 16
      };
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, ticks);
   }

   void GivenDataModelIsReset()
   {
      setup();
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
      data.restartDelayInSeconds = 0;

      LoadOffDoorOpen_Init(
         &instance,
         dataModel,
         &data,
         &config);
   }

   void GivenModuleIsInitializedWithZeroOffTimeout()
   {
      data.timeoutInSeconds = 0;

      LoadOffDoorOpen_Init(
         &instance,
         dataModel,
         &data,
         &config);
   }

   void GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(DoorIndex_t doorIndex)
   {
      DataModel_Write(dataModel, associatedDoors[doorIndex], set);

      for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
      {
         if(i != doorIndex)
         {
            DataModel_Write(dataModel, associatedDoors[i], clear);
         }
      }

      for(uint8_t i = 0; i < NUM_ELEMENTS(nonAssociatedDoors); i++)
      {
         DataModel_Write(dataModel, nonAssociatedDoors[i], clear);
      }
   }

   void GivenANonAssociatedDoorIsOpen(DoorIndex_t doorIndex)
   {
      DataModel_Write(dataModel, nonAssociatedDoors[doorIndex], set);
   }

   void GivenAllAssociatedDoorsAreClosed()
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
      {
         DataModel_Write(dataModel, associatedDoors[i], clear);
      }
   }

   void WhenAllAssociatedDoorsAreClosed()
   {
      GivenAllAssociatedDoorsAreClosed();
   }

   void WhenAnAssociatedDoorOpens(uint8_t doorIndex)
   {
      DataModel_Write(dataModel, associatedDoors[doorIndex], set);
   }

   void WhenAnotherAssociatedDoorOpens(uint8_t doorIndex)
   {
      DataModel_Write(dataModel, associatedDoors[doorIndex], set);
   }

   void WhenANonAssociatedDoorOpens(uint8_t doorIndex)
   {
      GivenANonAssociatedDoorIsOpen(doorIndex);
   }

   void WhenAnAssociatedDoorCloses(uint8_t doorIndex)
   {
      DataModel_Write(dataModel, associatedDoors[doorIndex], clear);
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

TEST(LoadOffDoorOpen, ShouldHaveSameNumberOfDoorStatusErdsInConfigAsTheMaxDoorIndexAndShouldMatchWhenIndexedByDoorIndexOrByName)
{
   CHECK_EQUAL(sizeof(config.doorStatus) / sizeof(Erd_t), DoorIndex_MaxNumberOfDoors);

   CHECK_EQUAL(((const Erd_t *)&config.doorStatus)[DoorIndex_LeftSideFreshFood], config.doorStatus.leftSideFreshFoodDoorErd);
   CHECK_EQUAL(((const Erd_t *)&config.doorStatus)[DoorIndex_RightSideFreshFood], config.doorStatus.rightSideFreshFoodDoorErd);
   CHECK_EQUAL(((const Erd_t *)&config.doorStatus)[DoorIndex_LeftSideFreezer], config.doorStatus.leftSideFreezerDoorErd);
   CHECK_EQUAL(((const Erd_t *)&config.doorStatus)[DoorIndex_RightSideFreezer], config.doorStatus.rightSideFreezerDoorErd);
   CHECK_EQUAL(((const Erd_t *)&config.doorStatus)[DoorIndex_ConvertibleCompartment], config.doorStatus.convertibleCompartmentDoorErd);
   CHECK_EQUAL(((const Erd_t *)&config.doorStatus)[DoorIndex_DoorInDoor], config.doorStatus.doorInDoorErd);
}

TEST(LoadOffDoorOpen, ShouldVoteAllLoadsOffIfAnAssociatedDoorIsOpenOnInit)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitialized();

      AllLoadsShouldBeVotedWithOffValue();
   }
}

TEST(LoadOffDoorOpen, ShouldNotVoteAllLoadsOffIfAllAssociatedDoorsAreClosedOnInit)
{
   GivenAllAssociatedDoorsAreClosed();
   GivenModuleIsInitialized();

   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldVoteAllLoadsDontCareAfterSomeTimeFromVotingThemOffIfAnAssociatedDoorIsOpenOnInit)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitialized();

      After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
      AllLoadsShouldBeVotedWithOffValue();

      After(1);
      AllLoadsShouldBeVotedDontCare();
   }
}

TEST(LoadOffDoorOpen, ShouldNotVoteAllLoadsDontCareAfterSomeTimeFromVotingThemOffIfAnAssociatedDoorIsOpenOnInitAndOffTimeoutIsZero)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitializedWithZeroOffTimeout();
      AllLoadsShouldBeVotedWithOffValue();

      After(SomeTimeInMsec);
      AllLoadsShouldBeVotedWithOffValue();
   }
}

TEST(LoadOffDoorOpen, ShouldVoteDontCareAfterADelayWhenAnAssociatedDoorClosesBeforeOffTimeoutExpires)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitialized();

      After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
      WhenAnAssociatedDoorCloses(i);
      AllLoadsShouldBeVotedWithOffValue();

      After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);
      AllLoadsShouldBeVotedWithOffValue();

      After(1);
      AllLoadsShouldBeVotedDontCare();
   }
}

TEST(LoadOffDoorOpen, ShouldVoteDontCareImmediatelyWhenAnAssociatedDoorClosesBeforeOffTimeoutExpiresIfDelayIsZero)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitializedWithZeroDelay();

      After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
      WhenAnAssociatedDoorCloses(i);
      After(0);
      AllLoadsShouldBeVotedDontCare();
   }
}

TEST(LoadOffDoorOpen, ShouldVoteAllLoadsOffWhenAnAssociatedDoorOpensWhileAllAssociatedDoorsAreClosedAndWaitingForDelayTimeToExpire)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitialized();

      After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
      WhenAnAssociatedDoorCloses(i);
      AllLoadsShouldBeVotedWithOffValue();

      After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);

      WhenAnAssociatedDoorOpens(i);
      AllLoadsShouldBeVotedWithOffValue();
   }
}

TEST(LoadOffDoorOpen, ShouldVoteDontCareForAllLoadsAfterTimeoutGivenAnAssociatedDoorOpensWhileAllAssociatedDoorsAreClosedWhileWaitingForDelayTimeToExpire)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitialized();

      After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
      WhenAnAssociatedDoorCloses(i);
      AllLoadsShouldBeVotedWithOffValue();

      After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);

      WhenAnAssociatedDoorOpens(i);
      AllLoadsShouldBeVotedWithOffValue();

      After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
      AllLoadsShouldBeVotedWithOffValue();

      After(1);
      AllLoadsShouldBeVotedDontCare();
   }
}

TEST(LoadOffDoorOpen, ShouldVoteDontCareForAllLoadsIfAllAssociatedDoorsAreClosedOnInit)
{
   GivenAllAssociatedDoorsAreClosed();
   GivenModuleIsInitialized();
   AllLoadsShouldBeVotedDontCare();

   After(data.restartDelayInSeconds * MSEC_PER_SEC);
   AllLoadsShouldBeVotedDontCare();
}

TEST(LoadOffDoorOpen, ShouldVoteAllLoadsOffWhenAnAssociatedDoorOpensIfAllAssociatedDoorsAreClosedOnInit)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenDataModelIsReset();
      GivenAllAssociatedDoorsAreClosed();
      GivenModuleIsInitialized();
      AllLoadsShouldBeVotedDontCare();

      WhenAnAssociatedDoorOpens(i);
      AllLoadsShouldBeVotedWithOffValue();
   }
}

TEST(LoadOffDoorOpen, ShouldNotRestartTimeoutTimeIfAnotherAssociatedDoorOpensWhileAnAssociatedDoorIsAlreadyOpenAndWaitingForTimeout)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitialized();

      After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
      WhenAnotherAssociatedDoorOpens(i == 0 ? 1 : 0);

      After(1);
      AllLoadsShouldBeVotedDontCare();
   }
}

TEST(LoadOffDoorOpen, ShouldRestartTheTimerWithTheDelayTimeWhenAllAssociatedDoorsCloseWhileAnAssociatedDoorIsOpenAndWhileWaitingForTheTimeoutToExpire)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitialized();

      After(data.timeoutInSeconds * MSEC_PER_SEC - 1);
      WhenAnAssociatedDoorCloses(i);

      After(1);
      AllLoadsShouldBeVotedWithOffValue();

      After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);
      AllLoadsShouldBeVotedDontCare();
   }
}

TEST(LoadOffDoorOpen, ShouldStopTheDelayTimerAndNotStartATimeoutTimerWhenAnAssociatedDoorOpensWhileAllAssociatedDoorsAreClosedWithAZeroOffTimeout)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAllAssociatedDoorsAreClosed();
      GivenModuleIsInitializedWithZeroOffTimeout();

      After(data.restartDelayInSeconds * MSEC_PER_SEC - 1);
      WhenAnAssociatedDoorOpens(i);
      AllLoadsShouldBeVotedWithOffValue();

      After(1);
      AllLoadsShouldBeVotedWithOffValue();
   }
}

TEST(LoadOffDoorOpen, ShouldNotRestartTimeoutWhenAnAssociatedDoorOpensWhileAnotherAssociatedDoorIsOpened)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenModuleIsInitialized();
      AllLoadsShouldBeVotedWithOffValue();

      After(data.timeoutInSeconds * MSEC_PER_SEC - 3);

      WhenAnotherAssociatedDoorOpens(i == 0 ? 1 : 0);
      AllLoadsShouldBeVotedWithOffValue();

      After(1);

      WhenAnAssociatedDoorCloses(i);
      AllLoadsShouldBeVotedWithOffValue();

      After(1);

      WhenAnAssociatedDoorOpens(i);
      AllLoadsShouldBeVotedWithOffValue();

      After(1);
      AllLoadsShouldBeVotedDontCare();
   }
}

TEST(LoadOffDoorOpen, ShouldNotVoteWhenANonAssociatedDoorOpens)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenModuleIsInitialized();

      WhenANonAssociatedDoorOpens(i);
      AllLoadsShouldBeVotedDontCare();
   }
}

TEST(LoadOffDoorOpen, ShouldVoteDontCareAfterAllAssociatedDoorsAreClosedWithZeroDelay)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(associatedDoors); i++)
   {
      GivenAnAssociatedDoorIsOpenAndAllOtherDoorsAreClosed(i);
      GivenANonAssociatedDoorIsOpen(i);
      GivenModuleIsInitializedWithZeroDelay();
      AllLoadsShouldBeVotedWithOffValue();

      WhenAllAssociatedDoorsAreClosed();
      After(0);
      AllLoadsShouldBeVotedDontCare();
   }
}
