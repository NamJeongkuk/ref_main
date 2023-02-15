/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "PostDwellCompletionSignalRequester.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When
#define And

static const PostDwellCompletionSignalRequesterConfig_t config = {
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .defrostHsmStateErd = Erd_DefrostHsmState
};

TEST_GROUP(PostDwellCompletionSignalRequester)
{
   PostDwellCompletionSignalRequester_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void TheModuleIsInitialized()
   {
      PostDwellCompletionSignalRequester_Init(&instance, dataModel, &config);
   }

   void DefrostHsmStateIs(DefrostHsmState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostHsmState, &state);
   }

   void PostDwellCompletionSignalShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_PostDwellCompletionSignal, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(PostDwellCompletionSignalRequester, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(PostDwellCompletionSignalRequester, ShouldSignalPostDwellCompletionWhenDefrostHsmStateChangesFromPostDefrostToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDwellCompletionSignalShouldBe(1);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromPrechillPrepToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_PrechillPrep);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromPrechillToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_Prechill);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromHeaterOnEntryToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_HeaterOnEntry);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromHeaterOnToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_HeaterOn);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromDwellToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_Dwell);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromDisabledToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_Disabled);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromPostDwellToPrechillPrep)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_PrechillPrep);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromPostDwellToPrechill)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Prechill);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromPostDwellToHeaterOnEntry)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_HeaterOnEntry);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromPostDwellToHeaterOn)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_HeaterOn);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromPostDwellToDwell)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Dwell);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldNotSignalPostDwellCompletionWhenDefrostHsmStateChangesFromPostDwellToDisabled)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Disabled);
   PostDwellCompletionSignalShouldBe(0);
}

TEST(PostDwellCompletionSignalRequester, ShouldSignalPostDwellCompletionMultipleTimes)
{
   Given DefrostHsmStateIs(DefrostHsmState_Idle);
   Given TheModuleIsInitialized();
   PostDwellCompletionSignalShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_PostDwell);
   And DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDwellCompletionSignalShouldBe(1);

   When DefrostHsmStateIs(DefrostHsmState_PostDwell);
   And DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDwellCompletionSignalShouldBe(2);
}
