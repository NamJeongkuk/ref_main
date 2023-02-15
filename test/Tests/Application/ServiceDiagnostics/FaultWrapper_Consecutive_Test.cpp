/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FaultWrapper_Consecutive.h"
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
   RequestsToClearFault = 3
};

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_AFault, sizeof(bool) },
};

static FaultWrapper_Consecutive_Configuration_t aConsecutiveFaultWrapperConfiguration = {
   .consecutiveRequestsToSetFault = RequestsToSetFault,
   .consecutiveRequestsToClearFault = RequestsToClearFault,
   .faultOutputErd = Erd_AFault
};

TEST_GROUP(FaultWrapper_Consecutive)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   FaultWrapper_Consecutive_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble,
         erdTable,
         NUM_ELEMENTS(erdTable));

      dataModel = &dataModelTestDouble.interface;
   }

   void ModuleIsInitialized(void)
   {
      FaultWrapper_Consecutive_Init(
         &instance,
         &aConsecutiveFaultWrapperConfiguration,
         dataModel);
   }

   void NothingShouldHappen()
   {
   }

   void OutputFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, instance._private.configuration->faultOutputErd, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FaultSetIsRequested()
   {
      FaultWrapper_SetRequest(&instance.interface);
   }

   void FaultClearIsRequested()
   {
      FaultWrapper_ClearRequest(&instance.interface);
   }

   void FaultWrapperIsReset()
   {
      FaultWrapper_Reset(&instance.interface);
   }

   void FaultIsAlreadySet()
   {
      When for(uint8_t i = 0; i < RequestsToSetFault; i++)
      {
         FaultSetIsRequested();
      }
   }
};

TEST(FaultWrapper_Consecutive, ShouldInitialize)
{
   Given The ModuleIsInitialized();
   NothingShouldHappen();
};

TEST(FaultWrapper_Consecutive, ShouldSetFaultAfterConsecutiveRequests)
{
   Given The ModuleIsInitialized();

   When for(uint8_t i = 0; i < (RequestsToSetFault - 1); i++)
   {
      FaultSetIsRequested();
   }

   OutputFaultShouldBe(CLEAR);

   FaultSetIsRequested();
   OutputFaultShouldBe(SET);
}

TEST(FaultWrapper_Consecutive, ShouldClearFaultAfterConsecutiveRequests)
{
   Given The ModuleIsInitialized();
   And The FaultIsAlreadySet();

   OutputFaultShouldBe(SET);

   When for(uint8_t i = 0; i < (RequestsToSetFault - 1); i++)
   {
      FaultClearIsRequested();
   }

   OutputFaultShouldBe(SET);

   When FaultClearIsRequested();
   OutputFaultShouldBe(CLEAR);
}

TEST(FaultWrapper_Consecutive, ShouldResetSetRequestCountsWhenClientResetsFaultWrapper)
{
   Given The ModuleIsInitialized();

   When for(uint8_t i = 0; i < (RequestsToSetFault - 1); i++)
   {
      FaultSetIsRequested();
   }
   OutputFaultShouldBe(CLEAR);

   When FaultWrapperIsReset();
   OutputFaultShouldBe(CLEAR);

   When FaultSetIsRequested();
   OutputFaultShouldBe(CLEAR);
}

TEST(FaultWrapper_Consecutive, ShouldResetASetFaultAndClearAllCountsIfClientResetsFaultWrapper)
{
   Given The ModuleIsInitialized();
   And FaultIsAlreadySet();

   When The FaultWrapperIsReset();
   OutputFaultShouldBe(CLEAR);

   When for(uint8_t i = 0; i < (RequestsToSetFault - 1); i++)
   {
      When FaultSetIsRequested();
   }
   OutputFaultShouldBe(CLEAR);

   When FaultSetIsRequested();
   OutputFaultShouldBe(SET);
}

TEST(FaultWrapper_Consecutive, ShouldResetSetRequestCountToZeroWhenClearRequestHappens)
{
   Given The ModuleIsInitialized();

   When for(uint8_t i = 0; i < (RequestsToSetFault - 1); i++)
   {
      When FaultSetIsRequested();
   }
   And The FaultClearIsRequested();

   When for(uint8_t i = 0; i < (RequestsToSetFault - 1); i++)
   {
      When FaultSetIsRequested();
   }

   The OutputFaultShouldBe(CLEAR);

   And The FaultSetIsRequested();
   The OutputFaultShouldBe(SET);
}

TEST(FaultWrapper_Consecutive, ShouldResetClearRequestCountToZeroWhenSetRequestHappens)
{
   Given The ModuleIsInitialized();
   And The FaultIsAlreadySet();

   When for(uint8_t i = 0; i < (RequestsToClearFault - 1); i++)
   {
      When FaultClearIsRequested();
   }

   The OutputFaultShouldBe(SET);
   When FaultSetIsRequested();

   When for(uint8_t i = 0; i < (RequestsToClearFault - 1); i++)
   {
      When FaultClearIsRequested();
   }
   The OutputFaultShouldBe(SET);

   And The FaultClearIsRequested();
   The OutputFaultShouldBe(CLEAR);
}
