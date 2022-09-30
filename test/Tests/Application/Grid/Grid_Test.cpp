/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Grid.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

#define And

enum
{
   OneSecondInMs = 1000
};

void Grid_SingleEvap(void *context)
{
   IGNORE(context);
   mock().actualCall("Grid_SingleEvap");
}

void Grid_DualEvap(void *context)
{
   IGNORE(context);
   mock().actualCall("Grid_DualEvap");
}

void Grid_TripleEvap(void *context)
{
   IGNORE(context);
   mock().actualCall("Grid_TripleEvap");
}

static const GridFunction_t grids[] = {
   Grid_SingleEvap,
   Grid_DualEvap,
   Grid_TripleEvap
};

static const GridFunctionArray_t functionArray = {
   grids,
   NUM_ELEMENTS(grids)
};

static const GridConfiguration_t gridConfig = {
   .timerModuleErd = Erd_TimerModule,
   .gridFunctions = &functionArray
};

TEST_GROUP(Grid_SingleEvap)
{
   Grid_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void WhenTheModuleIsInitialized()
   {
      Grid_Init(
         &instance,
         &gridConfig,
         dataModel);
   }

   void NothingShouldHappen()
   {
   }

   void GridFunctionShouldBeCalled(SimpleString functionName)
   {
      mock().expectOneCall(functionName);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }
};

TEST(Grid_SingleEvap, ShouldNotRunCallbackOnInit)
{
   WhenTheModuleIsInitialized();
   NothingShouldHappen();
}

TEST(Grid_SingleEvap, ShouldRunGridFunctionAfterGivenPeriod)
{
   WhenTheModuleIsInitialized();

   NothingShouldHappen();
   After(OneSecondInMs - 1);

   GridFunctionShouldBeCalled("Grid_SingleEvap");
   After(1);
}

TEST_GROUP(Grid_DualEvap)
{
   Grid_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void WhenTheModuleIsInitialized()
   {
      Grid_Init(
         &instance,
         &gridConfig,
         dataModel);
   }

   void NothingShouldHappen()
   {
   }

   void GridFunctionShouldBeCalled(SimpleString functionName)
   {
      mock().expectOneCall(functionName);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }
};

TEST(Grid_DualEvap, ShouldNotRunCallbackOnInit)
{
   WhenTheModuleIsInitialized();
   NothingShouldHappen();
}

TEST(Grid_DualEvap, ShouldRunGridFunctionAfterGivenPeriod)
{
   WhenTheModuleIsInitialized();

   NothingShouldHappen();
   After(OneSecondInMs - 1);

   GridFunctionShouldBeCalled("Grid_DualEvap");
   After(1);
}
