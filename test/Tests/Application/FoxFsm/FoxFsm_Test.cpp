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

   void FoxFsmStateShouldBe(FoxFsmState_t this_state){
      IGNORE(this_state);
   }

   void ErdClockwiseShouldBe(bool this_decision){

   }

   void WhenErdClockwise(){

   }

   void WhenErdCounterClockwise(){

   }


};

TEST(TestingFsmGroup, ShouldStateAWhenModuleInitialze)
{
   GivenModuleIsInitialized();

   FoxFsmStateShouldBe(FoxFsm_A);
}

TEST(TestingFsmGroup, ShouldErdClockwiseFalseWhenModuleInitialize)
{
   GivenModuleIsInitialized();

   ErdClockwiseShouldBe(false);
}

TEST(TestingFsmGroup, ShouldStateBWhenErdCW1)
{
   GivenModuleIsInitialized();
   WhenErdClockwise();
   FoxFsmStateShouldBe(FoxFsm_B);
/*
               Init 
State          ____A---B----....
Erd_Clockwise  ____F--T-F---....

*/
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
