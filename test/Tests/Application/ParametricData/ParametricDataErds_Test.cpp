/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ParametricData.h"
#include "ParametricDataErds.h"
#include "PersonalityId.h"
#include "ParametricDataTableOfContents.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "ParametricData_TestDouble.h"
#include "Action_Mock.h"

enum
{
   PersonalityId_Max = 3,
   ImageCrc = 0x1234
};

TEST_GROUP(ParametricDataErds)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   Action_Mock_t actionMock;
   I_Action_t *action;

   AppliancePersonality_t appliancePersonality;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      Action_Mock_Init(&actionMock);
      action = &actionMock.interface;

      appliancePersonality = PersonalityId_Default;
      DataModel_Write(dataModel, Erd_AppliancePersonality, &appliancePersonality);

      PersonalityParametricData_TestDouble_Init();
   }

   void Init(void)
   {
      ParametricDataErds_Init(
         dataModel,
         Erd_ParametricDataImageCrc,
         Erd_AppliancePersonality,
         Erd_PersonalityParametricData,
         Erd_PersonalityIdOutOfRangeFlag);
   }

   void ActionShouldBeInvoked()
   {
      mock().expectOneCall("Invoke").onObject(action);
   }

   void GivenThePersonalityIdOutOfRangeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_PersonalityIdOutOfRangeFlag, &state);
   }

   void PersonalityParametricDataShouldBe(const PersonalityParametricData_t *data)
   {
      PersonalityParametricData_t *pointer;
      DataModel_Read(dataModel, Erd_PersonalityParametricData, &pointer);

      POINTERS_EQUAL(data, pointer);
   }

   void InitializedWithPersonality(PersonalityId_t personality)
   {
      DataModel_Write(dataModel, Erd_AppliancePersonality, &personality);
      Init();
   }

   void ThePersonalityIdOutOfRangeShouldBe(bool state)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_PersonalityIdOutOfRangeFlag, &actualState);

      CHECK_EQUAL(state, actualState);
   }

   void TheParametricImageCrcShouldBe(uint16_t crc)
   {
      uint16_t actualCrc;
      DataModel_Read(dataModel, Erd_ParametricDataImageCrc, &actualCrc);

      CHECK_EQUAL(crc, actualCrc);
   }

   void SetImageHeaderCrcTo(uint16_t crc)
   {
      SetImageHeaderCrc(crc);
   }
};

TEST(ParametricDataErds, ShouldUseDefaultPersonalityParametricDataAndSetPersonalityIdOutOfRangeWhenInitializedWithInvalidPersonality)
{
   GivenThePersonalityIdOutOfRangeIs(CLEAR);
   InitializedWithPersonality(PersonalityId_Max);

   const ParametricDataTableOfContents_t *personalityParametricData = ParametricData_GetParametricTableOfContents();
   PersonalityParametricDataShouldBe(personalityParametricData->personalities[PersonalityId_Default]);
   ThePersonalityIdOutOfRangeShouldBe(SET);
}

TEST(ParametricDataErds, ShouldDefinePersonalityParametricDataErdWhenInitializedWithValidPersonalityId)
{
   for(uint8_t i = 0; i < PersonalityId_Max; i++)
   {
      GivenThePersonalityIdOutOfRangeIs(SET);
      InitializedWithPersonality(i);

      const ParametricDataTableOfContents_t *personalityParametricData = ParametricData_GetParametricTableOfContents();
      PersonalityParametricDataShouldBe(personalityParametricData->personalities[i]);
      ThePersonalityIdOutOfRangeShouldBe(CLEAR);
   }
}

TEST(ParametricDataErds, ShouldWriteParametricImageCrcToErdWhenPersonalityIdIsOutOfRange)
{
   SetImageHeaderCrcTo(ImageCrc);
   InitializedWithPersonality(PersonalityId_Max);

   TheParametricImageCrcShouldBe(ImageCrc);
}

TEST(ParametricDataErds, ShouldWriteParametricImageCrcToErdWhenPersonalityIdIsInRange)
{
   SetImageHeaderCrcTo(ImageCrc);
   InitializedWithPersonality(PersonalityId_Max - 1);

   TheParametricImageCrcShouldBe(ImageCrc);
}
