/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ActivelyWaitingForDefrostOnCompareMatch.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

TEST_GROUP(ActivelyWaitingForDefrostOnCompareMatch)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void ActivelyWaitingForDefrostOnCompareMatchIsInitialized()
   {
      ActivelyWaitingForDefrostOnCompareMatch(dataModel);
   }

   void GivenDefrostHsmStateIs(DefrostHsmState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostHsmState, &state);
   }

   void WhenDefrostHsmStateIs(DefrostHsmState_t state)
   {
      GivenDefrostHsmStateIs(state);
   }

   void ActivelyWaitingForDefrostShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_ActivelyWaitingForNextDefrost, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }
};

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldInitialize)
{
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToTrueWhenDefrostHsmStateIsIdle)
{
   GivenDefrostHsmStateIs(DefrostHsmState_Idle);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(true);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToTrueWhenDefrostHsmStateIsPostDwell)
{
   GivenDefrostHsmStateIs(DefrostHsmState_PostDwell);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(true);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToFalseWhenDefrostHsmStateIsPowerUp)
{
   GivenDefrostHsmStateIs(DefrostHsmState_PowerUp);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(false);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToFalseWhenDefrostHsmStateIsPrechillPrep)
{
   GivenDefrostHsmStateIs(DefrostHsmState_PrechillPrep);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(false);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToFalseWhenDefrostHsmStateIsPrechill)
{
   GivenDefrostHsmStateIs(DefrostHsmState_Prechill);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(false);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToFalseWhenDefrostHsmStateIsPostPrechill)
{
   GivenDefrostHsmStateIs(DefrostHsmState_PostPrechill);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(false);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToFalseWhenDefrostHsmStateIsHeaterOnEntry)
{
   GivenDefrostHsmStateIs(DefrostHsmState_HeaterOnEntry);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(false);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToFalseWhenDefrostHsmStateIsHeaterOnHeat)
{
   GivenDefrostHsmStateIs(DefrostHsmState_HeaterOnHeat);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(false);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToFalseWhenDefrostHsmStateIsDwell)
{
   GivenDefrostHsmStateIs(DefrostHsmState_Dwell);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(false);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToFalseWhenDefrostHsmStateIsDisabled)
{
   GivenDefrostHsmStateIs(DefrostHsmState_Disabled);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();

   ActivelyWaitingForDefrostShouldBe(false);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToTrueWhenDefrostHsmStateChangesToIdle)
{
   GivenDefrostHsmStateIs(DefrostHsmState_PowerUp);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();
   ActivelyWaitingForDefrostShouldBe(false);

   WhenDefrostHsmStateIs(DefrostHsmState_Idle);

   ActivelyWaitingForDefrostShouldBe(true);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToTrueWhenDefrostHsmStateChangesToPostDwell)
{
   GivenDefrostHsmStateIs(DefrostHsmState_PowerUp);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();
   ActivelyWaitingForDefrostShouldBe(false);

   WhenDefrostHsmStateIs(DefrostHsmState_PostDwell);

   ActivelyWaitingForDefrostShouldBe(true);
}

TEST(ActivelyWaitingForDefrostOnCompareMatch, ShouldSetActivelyWaitingForDefrostToFalseWhenDefrostHsmStateChangesToPrechillPrep)
{
   WhenDefrostHsmStateIs(DefrostHsmState_Idle);
   ActivelyWaitingForDefrostOnCompareMatchIsInitialized();
   ActivelyWaitingForDefrostShouldBe(true);

   WhenDefrostHsmStateIs(DefrostHsmState_PrechillPrep);

   ActivelyWaitingForDefrostShouldBe(false);
}
