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
   .gridOverrideSignalErd = Erd_GridOverrideSignal,
   .gridOverrideEnableErd = Erd_GridOverrideEnable,
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

   void WhenTheGridOverrideEnableIs(bool state)
   {
      DataModel_Write(dataModel, Erd_GridOverrideEnable, &state);
   }

   void WhenTheGridOverrideSignalChanges()
   {
      Signal_t overrideSignal;
      DataModel_Read(dataModel, Erd_GridOverrideSignal, &overrideSignal);

      overrideSignal++;
      DataModel_Write(dataModel, Erd_GridOverrideSignal, &overrideSignal);
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

TEST(Grid_SingleEvap, ShouldNotRunGridFunctionAfterOverrideIsEnabled)
{
   WhenTheModuleIsInitialized();
   WhenTheGridOverrideEnableIs(true);

   NothingShouldHappen();
   After(OneSecondInMs - 1);

   NothingShouldHappen();
   After(1);
}

TEST(Grid_SingleEvap, ShouldRunGridFunctionWhenOverrideSignalIsDetectedIfOverrideIsEnabled)
{
   WhenTheModuleIsInitialized();
   WhenTheGridOverrideEnableIs(true);

   GridFunctionShouldBeCalled("Grid_SingleEvap");
   WhenTheGridOverrideSignalChanges();
}

TEST(Grid_SingleEvap, ShouldNotRunGridFunctionWhenOverrideSignalIsDetectedIfOverrideIsDisabled)
{
   WhenTheModuleIsInitialized();
   WhenTheGridOverrideEnableIs(false);

   NothingShouldHappen();
   WhenTheGridOverrideSignalChanges();
}

TEST(Grid_SingleEvap, ShouldReenablePeriodicGridFunctionWhenOverrideIsEnabledThenDisabled)
{
   WhenTheModuleIsInitialized();
   WhenTheGridOverrideEnableIs(true);

   GridFunctionShouldBeCalled("Grid_SingleEvap");
   WhenTheGridOverrideSignalChanges();

   WhenTheGridOverrideEnableIs(false);

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

   void WhenTheGridOverrideEnableIs(bool state)
   {
      DataModel_Write(dataModel, Erd_GridOverrideEnable, &state);
   }

   void WhenTheGridOverrideSignalChanges()
   {
      Signal_t overrideSignal;
      DataModel_Read(dataModel, Erd_GridOverrideSignal, &overrideSignal);

      overrideSignal++;
      DataModel_Write(dataModel, Erd_GridOverrideSignal, &overrideSignal);
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

TEST(Grid_DualEvap, ShouldNotRunGridFunctionAfterOverrideIsEnabled)
{
   WhenTheModuleIsInitialized();
   WhenTheGridOverrideEnableIs(true);

   NothingShouldHappen();
   After(OneSecondInMs - 1);

   NothingShouldHappen();
   After(1);
}

TEST(Grid_DualEvap, ShouldRunGridFunctionWhenOverrideSignalIsDetectedIfOverrideIsEnabled)
{
   WhenTheModuleIsInitialized();
   WhenTheGridOverrideEnableIs(true);

   GridFunctionShouldBeCalled("Grid_DualEvap");
   WhenTheGridOverrideSignalChanges();
}

TEST(Grid_DualEvap, ShouldNotRunGridFunctionWhenOverrideSignalIsDetectedIfOverrideIsDisabled)
{
   WhenTheModuleIsInitialized();
   WhenTheGridOverrideEnableIs(false);

   NothingShouldHappen();
   WhenTheGridOverrideSignalChanges();
}

TEST(Grid_DualEvap, ShouldReenablePeriodicGridFunctionWhenOverrideIsEnabledThenDisabled)
{
   WhenTheModuleIsInitialized();
   WhenTheGridOverrideEnableIs(true);

   GridFunctionShouldBeCalled("Grid_DualEvap");
   WhenTheGridOverrideSignalChanges();

   WhenTheGridOverrideEnableIs(false);

   NothingShouldHappen();
   After(OneSecondInMs - 1);

   GridFunctionShouldBeCalled("Grid_DualEvap");
   After(1);
}
