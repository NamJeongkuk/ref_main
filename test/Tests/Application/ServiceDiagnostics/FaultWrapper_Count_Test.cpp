/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FaultWrapper_Count.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "DataModel_TestDouble.h"

#define The
#define When
#define Given
#define And

enum
{
   Erd_Start,
   Erd_AFault,

   RequestsToSetFault = 3,
   TotalRequestsConsidered = 10
};

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_AFault, sizeof(bool) },
};

static const FaultWrapper_Count_Configuration_t aCountFaultWrapperConfiguration = {
   .requestsToSetFault = RequestsToSetFault,
   .totalRequestsToConsider = TotalRequestsConsidered,
   .faultOutputErd = Erd_AFault,
};

TEST_GROUP(FaultWrapper_Count)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FaultWrapper_Count_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble,
         erdTable,
         NUM_ELEMENTS(erdTable));
      dataModel = &dataModelTestDouble.interface;
   }

   void ModuleIsInitialized(void)
   {
      FaultWrapper_Count_Init(
         &instance,
         &aCountFaultWrapperConfiguration,
         dataModel);
   }

   void NothingShouldHappen(void)
   {
   }

   void OutputFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, instance._private.configuration->faultOutputErd, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FaultSetIsRequested(void)
   {
      FaultWrapper_SetRequest(&instance.interface);
   }

   void FaultClearIsRequested(void)
   {
      FaultWrapper_ClearRequest(&instance.interface);
   }

   void FaultWrapperIsReset(void)
   {
      FaultWrapper_Reset(&instance.interface);
   }

   void FaultIsAlreadySet(void)
   {
      When for(int i = 0; i < RequestsToSetFault; i++)
      {
         FaultSetIsRequested();
      }
   }
};

TEST(FaultWrapper_Count, ShouldInitialize)
{
   Given The ModuleIsInitialized();
   NothingShouldHappen();
}

TEST(FaultWrapper_Count, ShouldSetFaultAfterConsecutiveRequests)
{
   Given The ModuleIsInitialized();

   When for(int i = 0; i < (RequestsToSetFault - 1); i++)
   {
      FaultSetIsRequested();
      OutputFaultShouldBe(CLEAR);
   }

   When The FaultSetIsRequested();
   The OutputFaultShouldBe(SET);
}

TEST(FaultWrapper_Count, ShouldSetFaultAfterNonConsecutiveSetRequests)
{
   Given The ModuleIsInitialized();

   When for(int i = 0; i < (RequestsToSetFault - 1); i++)
   {
      FaultSetIsRequested();
      OutputFaultShouldBe(CLEAR);
   }

   When for(int i = 0; i < (TotalRequestsConsidered - RequestsToSetFault - 1); i++)
   {
      FaultClearIsRequested();
      The OutputFaultShouldBe(CLEAR);
   }

   When FaultSetIsRequested();
   The OutputFaultShouldBe(SET);
}

TEST(FaultWrapper_Count, ShouldClearASetFaultWithEnoughClearRequestsToOvertakeTheQueue)
{
   Given The ModuleIsInitialized();
   And The FaultIsAlreadySet();

   When for(int i = 0; i < (TotalRequestsConsidered - RequestsToSetFault); i++)
   {
      FaultClearIsRequested();
      OutputFaultShouldBe(SET);
   }

   When FaultClearIsRequested();
   OutputFaultShouldBe(CLEAR);
}

TEST(FaultWrapper_Count, ShouldResetASetFaultAndClearAllCountsIfClientResetsFaultWrapper)
{
   Given The ModuleIsInitialized();
   And FaultIsAlreadySet();

   When The FaultWrapperIsReset();
   OutputFaultShouldBe(CLEAR);

   When for(int i = 0; i < (RequestsToSetFault - 1); i++)
   {
      When FaultSetIsRequested();
   }
   OutputFaultShouldBe(CLEAR);

   When FaultSetIsRequested();
   OutputFaultShouldBe(SET);
}

TEST(FaultWrapper_Count, ShouldResetASetFaultAndClearAllCountsEvenAfterManyFaultSetsAndClears)
{
   Given The ModuleIsInitialized();
   When for(int i = 0; i < TotalRequestsConsidered; i++)
   {
      When FaultSetIsRequested();
   }

   The OutputFaultShouldBe(SET);

   When for(int i = 0; i < (TotalRequestsConsidered - RequestsToSetFault); i++)
   {
      When FaultClearIsRequested();
      The OutputFaultShouldBe(SET);
   }

   When FaultClearIsRequested();
   The OutputFaultShouldBe(CLEAR);
}
