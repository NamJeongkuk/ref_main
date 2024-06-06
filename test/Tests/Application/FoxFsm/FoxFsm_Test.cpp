/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Fsm.h"
#include "FoxFsmPlugin.h"
#include "FoxFsmState.h"
#include "utils.h"
#include <stdint.h>


#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

#include "Constants_Binary.h"
#include "ConstArrayMap_FourDoorDualEvap.h"
};

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "GpioGroup_TestDouble.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And
#define After
#define Then
/*
preview
*/
static const FoxFsmData_t config = { 
   .clockwise = Erd_Clockwise,
   .cclockwise = Erd_CClockwise,
   .printstate = Erd_TestCurState
};

TEST_GROUP(TestingFsmGroup)
{
   FoxFsm_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t * dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }
   void GivenModuleIsInitialized()
   {
      FoxFsm_Init(
         &instance,
         &config,
         dataModel);
   }

   void FoxFsmStateShouldBe(FoxFsmState_t expected){

      FoxFsmState_t actual;
      DataModel_Read(dataModel, Erd_TestCurState, &actual);
      CHECK_EQUAL(expected, actual);

   }

   void ErdClockwiseShouldBe(bool expected){
      bool actual;
      DataModel_Read(dataModel, Erd_Clockwise, &actual);
      CHECK_EQUAL(expected, actual);
   }
   void ErdCounterClockwiseShouldBe(bool expected){
      bool actual;
      DataModel_Read(dataModel, Erd_CClockwise, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenErdClockwise(){
      bool signal = true;
      DataModel_Write(dataModel, Erd_Clockwise, &signal);
   }

   void WhenErdCounterClockwise(){
      bool signal = true;
      DataModel_Write(dataModel, Erd_CClockwise, &signal);
   }


};
/*
               Init 
State          ____A---B----....
Erd_Clockwise  ____F--T-F---....

// */

/*
                           |-----------|
Erd_Clockwise     ---------|           |
                           |           |---------- Erd_TestCurState
Erd_CClockwise    ---------|           |
                           |-----------|
*/

TEST(TestingFsmGroup, ShouldStateAWhenModuleInitialze)
{
   GivenModuleIsInitialized();

   FoxFsmStateShouldBe(FoxFsm_A);
}

TEST(TestingFsmGroup, ShouldErdClockwiseFalseWhenModuleInitialize)
{
   GivenModuleIsInitialized();

   ErdCounterClockwiseShouldBe(false);
}

TEST(TestingFsmGroup, ShouldErdCClockwiseFalseWhenModuleInitialize)
{
   GivenModuleIsInitialized();

   ErdCounterClockwiseShouldBe(false);
}

TEST(TestingFsmGroup, ShouldStateBWhenErdCW1)
{
   GivenModuleIsInitialized();
   WhenErdClockwise();
   FoxFsmStateShouldBe(FoxFsm_B);
}

TEST(TestingFsmGroup, ShouldStateCWhenErdCW2)
{
   GivenModuleIsInitialized();
   WhenErdClockwise();
   WhenErdClockwise();
   FoxFsmStateShouldBe(FoxFsm_C);
}

TEST(TestingFsmGroup, ShouldStateAWhenErdCW3)
{
   GivenModuleIsInitialized();
   WhenErdClockwise();
   WhenErdClockwise();
   WhenErdClockwise();
   FoxFsmStateShouldBe(FoxFsm_A);
}

TEST(TestingFsmGroup, ShouldStateBWhenErdCCW1)
{
   GivenModuleIsInitialized();
   WhenErdCounterClockwise();
   FoxFsmStateShouldBe(FoxFsm_C);
}

TEST(TestingFsmGroup, ShouldStateCWhenErdCCW2)
{
   GivenModuleIsInitialized();
   WhenErdCounterClockwise();
   WhenErdCounterClockwise();
   FoxFsmStateShouldBe(FoxFsm_B);
}

TEST(TestingFsmGroup, ShouldStateAWhenErdCCW3)
{
   GivenModuleIsInitialized();
   WhenErdCounterClockwise();
   WhenErdCounterClockwise();
   WhenErdCounterClockwise();
   FoxFsmStateShouldBe(FoxFsm_A);
}