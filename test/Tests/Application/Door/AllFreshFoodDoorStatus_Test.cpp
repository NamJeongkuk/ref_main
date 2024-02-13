/*!
 * @file
 * @brief Door status tests
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "AllFreshFoodDoorStatus.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When
#define And
#define The

enum
{
   Close,
   Opens,

   Closed = Close,
   Open = Opens
};

static const AllFreshFoodDoorStatusConfiguration_t AllFreshFoodDoorStatusConfiguration = {
   .rightFreshDoorStatusErd = Erd_RightSideFreshFoodDoorStatusResolved,
   .leftFreshDoorStatusErd = Erd_LeftSideFreshFoodDoorStatusResolved,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed
};

TEST_GROUP(AllFreshFoodDoorStatus)
{
   AllFreshFoodDoorStatus_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void AllFreshFoodDoorStatusInitialized()
   {
      AllFreshFoodDoorStatus_Init(&instance, dataModel, &AllFreshFoodDoorStatusConfiguration);
   }

   void RightFreshFoodDoorIs(bool actual)
   {
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatusResolved, &actual);
   }

   void LeftFreshFoodDoorIs(bool actual)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreshFoodDoorStatusResolved, &actual);
   }

   void BothFreshFoodDoorsAre(bool actual)
   {
      LeftFreshFoodDoorIs(actual);
      RightFreshFoodDoorIs(actual);
   }

   void AllFreshFoodDoorsShouldBe(bool expected)
   {
      bool actualState;
      bool expectedState;

      if(expected == Open)
      {
         expectedState = false;
      }
      else if(expected == Closed)
      {
         expectedState = true;
      }

      DataModel_Read(dataModel, Erd_AllFreshFoodDoorsAreClosed, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }
};

TEST(AllFreshFoodDoorStatus, ShouldUpdateWhenInitialized)
{
   Given The RightFreshFoodDoorIs(Open);

   When AllFreshFoodDoorStatusInitialized();
   AllFreshFoodDoorsShouldBe(Open);

   When The RightFreshFoodDoorIs(Closed);
   And The LeftFreshFoodDoorIs(Open);
   AllFreshFoodDoorsShouldBe(Open);
}

TEST(AllFreshFoodDoorStatus, ShouldIndicateAFreshFoodDoorOpenWhenRightDoorOpens)
{
   Given BothFreshFoodDoorsAre(Closed);
   And AllFreshFoodDoorStatusInitialized();

   AllFreshFoodDoorsShouldBe(Closed);

   When RightFreshFoodDoorIs(Open);
   AllFreshFoodDoorsShouldBe(Open);
}

TEST(AllFreshFoodDoorStatus, ShouldIndicateAFreshFoodDoorOpenWhenLeftDoorOpens)
{
   Given BothFreshFoodDoorsAre(Closed);
   And AllFreshFoodDoorStatusInitialized();

   AllFreshFoodDoorsShouldBe(Closed);

   When LeftFreshFoodDoorIs(Open);
   AllFreshFoodDoorsShouldBe(Open);
}

TEST(AllFreshFoodDoorStatus, ShouldIndicateAFreshFoodDoorOpenWhenBothDoorsOpen)
{
   Given BothFreshFoodDoorsAre(Closed);
   And AllFreshFoodDoorStatusInitialized();

   When LeftFreshFoodDoorIs(Open);
   When RightFreshFoodDoorIs(Open);
   AllFreshFoodDoorsShouldBe(Open);
}

TEST(AllFreshFoodDoorStatus, ShouldIndicateAFreshFoodDoorOpenWhenBothDoorsOpenAndRightDoorCloses)
{
   Given BothFreshFoodDoorsAre(Closed);
   And AllFreshFoodDoorStatusInitialized();

   When BothFreshFoodDoorsAre(Open);
   AllFreshFoodDoorsShouldBe(Open);

   When RightFreshFoodDoorIs(Closed);
   AllFreshFoodDoorsShouldBe(Open);
}

TEST(AllFreshFoodDoorStatus, ShouldIndicateAFreshFoodDoorClosedWhenBothDoorsOpenAndBothClose)
{
   Given BothFreshFoodDoorsAre(Closed);
   And AllFreshFoodDoorStatusInitialized();

   When LeftFreshFoodDoorIs(Open);
   When RightFreshFoodDoorIs(Open);
   AllFreshFoodDoorsShouldBe(Open);

   When BothFreshFoodDoorsAre(Closed);
   AllFreshFoodDoorsShouldBe(Close);
}
