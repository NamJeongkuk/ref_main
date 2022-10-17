/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "PostDefrostShiftSeedingRequester.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When
#define And

static const PostDefrostShiftSeedingRequesterConfig_t config = {
   .RequestPostDefrostShiftSeedingErd = Erd_PostDefrostShiftSeedingRequest,
   .DefrostHsmStateErd = Erd_DefrostHsmState
};

TEST_GROUP(PostDefrostShiftSeedingRequester)
{
   PostDefrostShiftSeedingRequester_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void TheModuleIsInitialized()
   {
      PostDefrostShiftSeedingRequester_Init(&instance, dataModel, &config);
   }

   void DefrostHsmStateIs(DefrostHsmState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostHsmState, &state);
   }

   void PostDefrostShiftSeedingRequestShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_PostDefrostShiftSeedingRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(PostDefrostShiftSeedingRequester, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(PostDefrostShiftSeedingRequester, ShouldRequestPostDefrostShiftSeedingWhenDefrostHsmStateChangesFromPostDefrostToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDefrostShiftSeedingRequestShouldBe(1);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromPrechillPrepToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_PrechillPrep);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromPrechillToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_Prechill);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromHeaterOnEntryToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_HeaterOnEntry);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromHeaterOnToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_HeaterOn);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromDwellToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_Dwell);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromDisabledToIdle)
{
   Given DefrostHsmStateIs(DefrostHsmState_Disabled);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromPostDwellToPrechillPrep)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_PrechillPrep);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromPostDwellToPrechill)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Prechill);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromPostDwellToHeaterOnEntry)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_HeaterOnEntry);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromPostDwellToHeaterOn)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_HeaterOn);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromPostDwellToDwell)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Dwell);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldNotRequestDefrostShiftSeedingWhenDefrostHsmStateChangesFromPostDwellToDisabled)
{
   Given DefrostHsmStateIs(DefrostHsmState_PostDwell);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_Disabled);
   PostDefrostShiftSeedingRequestShouldBe(0);
}

TEST(PostDefrostShiftSeedingRequester, ShouldRequestDefrostShiftSeedingMultipleTimes)
{
   Given DefrostHsmStateIs(DefrostHsmState_Idle);
   Given TheModuleIsInitialized();
   PostDefrostShiftSeedingRequestShouldBe(0);

   When DefrostHsmStateIs(DefrostHsmState_PostDwell);
   And DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDefrostShiftSeedingRequestShouldBe(1);

   When DefrostHsmStateIs(DefrostHsmState_PostDwell);
   And DefrostHsmStateIs(DefrostHsmState_Idle);
   PostDefrostShiftSeedingRequestShouldBe(2);
}
