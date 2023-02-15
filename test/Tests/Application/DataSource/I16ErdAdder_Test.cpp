/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "I16ErdAdder.h"
#include "ErdList.h"
#include "utils.h"
#include "DataModelErdPointerAccess.h"
}
#include "DataModel_TestDouble.h"
#include "CppUTest/TestHarness.h"

#define Given
#define When

enum
{
   Erd_Result,
   Erd_Number1,
   Erd_Number2,
   Erd_Number3,
   Erd_Number4,
   Erd_NotInConfig
};

static const DataModel_TestDoubleConfigurationEntry_t dataModelTestDoubleConfig[] = {
   { Erd_Result, sizeof(int16_t) },
   { Erd_Number1, sizeof(int16_t) },
   { Erd_Number2, sizeof(int16_t) },
   { Erd_Number3, sizeof(int16_t) },
   { Erd_Number4, sizeof(int16_t) },
   { Erd_NotInConfig, sizeof(int16_t) }
};

static const Erd_t erdsToBeAdded[] = {
   Erd_Number1,
   Erd_Number2,
   Erd_Number3,
   Erd_Number4
};

static const I16ErdAdderConfiguration_t erdAdderConfiguration = {
   .resultErd = Erd_Result,
   .i16ErdsToBeAdded = { erdsToBeAdded, NUM_ELEMENTS(erdsToBeAdded) }
};

TEST_GROUP(I16ErdAdder)
{
   I16ErdAdder_t instance;
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, dataModelTestDoubleConfig, NUM_ELEMENTS(dataModelTestDoubleConfig));
      dataModel = dataModelTestDouble.dataModel;
   }

   void ModuleIsInitialized()
   {
      I16ErdAdder_Init(&instance, dataModel, &erdAdderConfiguration);
   }

   void I16ErdIs(Erd_t erd, int16_t value)
   {
      DataModel_Write(dataModel, erd, &value);
   }

   void AllErdsAreZero(void)
   {
      I16ErdIs(Erd_Number1, 0);
      I16ErdIs(Erd_Number2, 0);
      I16ErdIs(Erd_Number3, 0);
      I16ErdIs(Erd_Number4, 0);
      I16ErdIs(Erd_NotInConfig, 0);
   }

   void ResultErdShouldBe(int16_t expected)
   {
      int16_t actual;
      DataModel_Read(dataModel, Erd_Result, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ModuleIsInitializedWhileAllErdsAreZero(void)
   {
      Given AllErdsAreZero();
      Given ModuleIsInitialized();
      ResultErdShouldBe(0);
   }
};

TEST(I16ErdAdder, ShouldInitialize)
{
   Given ModuleIsInitialized();
}

TEST(I16ErdAdder, ShouldCalculateTheAddedErdsOnInit)
{
   Given I16ErdIs(Erd_Number1, 1);
   Given I16ErdIs(Erd_Number2, 1);
   Given I16ErdIs(Erd_Number3, 1);
   Given I16ErdIs(Erd_Number4, 1);

   When ModuleIsInitialized();
   ResultErdShouldBe(4);
}

TEST(I16ErdAdder, ShouldCalculateTheAddedErdsWhenTheConfigurationErdIsUpdated)
{
   Given AllErdsAreZero();
   Given ModuleIsInitialized();
   ResultErdShouldBe(0);

   When I16ErdIs(Erd_Number1, 1);
   ResultErdShouldBe(1);
}

TEST(I16ErdAdder, ShouldRecalculateTheAddedErdsWhenTheOtherConfigurationErdIsUpdated)
{
   Given ModuleIsInitializedWhileAllErdsAreZero();

   When I16ErdIs(Erd_Number1, 1);
   ResultErdShouldBe(1);

   When I16ErdIs(Erd_Number2, 2);
   ResultErdShouldBe(3);

   When I16ErdIs(Erd_Number3, 3);
   ResultErdShouldBe(6);

   When I16ErdIs(Erd_Number4, 4);
   ResultErdShouldBe(10);
}

TEST(I16ErdAdder, ShouldNotCalculateTheAddedErdsWhenIrrelavantErdIsChanged)
{
   Given ModuleIsInitializedWhileAllErdsAreZero();

   When I16ErdIs(Erd_NotInConfig, 1);
   ResultErdShouldBe(0);
}

TEST(I16ErdAdder, ShouldLimitToAddTheErdsWhenResultErdOverflows)
{
   Given ModuleIsInitializedWhileAllErdsAreZero();

   When I16ErdIs(Erd_Number1, INT16_MAX - 1);
   ResultErdShouldBe(INT16_MAX - 1);

   When I16ErdIs(Erd_Number2, 1);
   ResultErdShouldBe(INT16_MAX);

   When I16ErdIs(Erd_Number3, 2);
   ResultErdShouldBe(INT16_MAX);

   When I16ErdIs(Erd_Number4, 3);
   ResultErdShouldBe(INT16_MAX);
}

TEST(I16ErdAdder, ShouldLimitToAddTheErdsWhenResultErdUnderflows)
{
   Given ModuleIsInitializedWhileAllErdsAreZero();

   When I16ErdIs(Erd_Number1, INT16_MIN + 1);
   ResultErdShouldBe(INT16_MIN + 1);

   When I16ErdIs(Erd_Number2, -1);
   ResultErdShouldBe(INT16_MIN);

   When I16ErdIs(Erd_Number3, -2);
   ResultErdShouldBe(INT16_MIN);

   When I16ErdIs(Erd_Number4, -3);
   ResultErdShouldBe(INT16_MIN);
}
