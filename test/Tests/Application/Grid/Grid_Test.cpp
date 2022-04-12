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
#include "PersonalityParametricData_TestDouble.h"
#include "uassert_test.h"

#define And

enum
{
   OneSecondInMs = 1000
};

void Grid_MultiDoor(void *context)
{
   IGNORE(context);
   mock().actualCall("Grid_MultiDoor");
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

static const GridFunction_t grids[] = { Grid_MultiDoor, Grid_DualEvap, Grid_TripleEvap };

static const GridFunctionArray_t functionArray = {
   grids,
   NUM_ELEMENTS(grids)
};

static GridConfiguration_t gridConfig = {
   .timerModuleErd = Erd_TimerModule,
   .gridFunctions = &functionArray
};

static GridData_t gridData = {
   .gridId = 1,
   .gridPeriodicRunRateInMSec = OneSecondInMs
};

TEST_GROUP(Grid)
{
   Grid_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetGrid(&personalityParametricData, &gridData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void GivenGridIdIs(GridId_t id)
   {
      gridData.gridId = id;
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

TEST(Grid, ShouldNotRunCallbackOnInit)
{
   GivenGridIdIs(0);
   WhenTheModuleIsInitialized();
   NothingShouldHappen();
}

TEST(Grid, ShouldRunGridFunctionAfterGivenPeriod)
{
   GivenGridIdIs(0);
   WhenTheModuleIsInitialized();

   NothingShouldHappen();
   After(OneSecondInMs - 1);

   GridFunctionShouldBeCalled("Grid_MultiDoor");
   After(1);
}

TEST(Grid, ShouldRunDifferentGridFunctionAfterGivenPeriod)
{
   GivenGridIdIs(1);
   WhenTheModuleIsInitialized();

   NothingShouldHappen();
   After(OneSecondInMs - 1);

   GridFunctionShouldBeCalled("Grid_DualEvap");
   After(1);
}

TEST(Grid, ShouldRunAnotherGridFunctionAfterGivenPeriod)
{
   GivenGridIdIs(2);
   WhenTheModuleIsInitialized();

   NothingShouldHappen();
   After(OneSecondInMs - 1);

   GridFunctionShouldBeCalled("Grid_TripleEvap");
   After(1);
}

TEST(Grid, ShouldNotRunAnyGridFunctionIfGridIdIsInvalid)
{
   GivenGridIdIs(30);
   WhenTheModuleIsInitialized();

   NothingShouldHappen();
   After(OneSecondInMs);
}
