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
#include "Action_Mock.h"

enum
{
   PersonalityId_Max = 3
};

enum
{
   Critical_ValidMajorVersion = 0,
   Critical_ValidMinorVersion = 0,
   Critical_InvalidMajorVersion = Critical_ValidMajorVersion + 1,
   Critical_InvalidMinorVersion = Critical_ValidMinorVersion + 1,
};

static ImageHeader_t imageHeader;
static PersonalityParametricData_t *personalites[PersonalityId_Max];
static ParametricDataTableOfContents_t parametricData = {
   .numberOfPersonalities = PersonalityId_Max,
   .personalities = personalites
};

const ImageHeader_t *ParametricData_GetParametricHeader(void)
{
   return &imageHeader;
}

const ParametricDataTableOfContents_t *ParametricData_GetParametricTableOfContents(void)
{
   return &parametricData;
}

TEST_GROUP(ParametricDataErds)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   Action_Mock_t actionMock;
   I_Action_t *action;

   AppliancePersonality_t appliancePersonality;
   PersonalityParametricData_t personalitesData[PersonalityId_Max];

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      Action_Mock_Init(&actionMock);
      action = &actionMock.interface;

      appliancePersonality = PersonalityId_Default;
      DataModel_Write(dataModel, Erd_AppliancePersonality, &appliancePersonality);

      for(uint8_t i = 0; i < PersonalityId_Max; i++)
      {
         personalites[i] = &personalitesData[i];
      }
   }

   void Init(void)
   {
      ParametricDataErds_Init(
         dataModel,
         Erd_AppliancePersonality,
         Erd_PersonalityParametricData,
         Erd_PersonalityIdOutOfRangeFlag,
         action);
   }

   void ActionShouldBeInvoked()
   {
      mock().expectOneCall("Invoke").onObject(action);
   }

   void GivenThePersonalityIdOutOfRangeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_PersonalityIdOutOfRangeFlag, &state);
   }

   void WhenInitializedWithCriticalMajorVersion(uint8_t version)
   {
      imageHeader.criticalMajorVersion = version;
      imageHeader.criticalMinorVersion = Critical_ValidMinorVersion;
      Init();
   }

   void WhenInitializedWithCriticalMinorVersion(uint8_t version)
   {
      imageHeader.criticalMajorVersion = Critical_ValidMajorVersion;
      imageHeader.criticalMinorVersion = version;
      Init();
   }

   void PersonalityParametricDataShouldBe(const void *data)
   {
      void *pointer;
      DataModel_Read(dataModel, Erd_PersonalityParametricData, &pointer);
      POINTERS_EQUAL(data, pointer);
   }

   void InitializedWithPersonality(PersonalityId_t personality)
   {
      imageHeader.criticalMajorVersion = Critical_ValidMajorVersion;
      imageHeader.criticalMinorVersion = Critical_ValidMinorVersion;
      DataModel_Write(dataModel, Erd_AppliancePersonality, &personality);
      Init();
   }

   void ThePersonalityIdOutOfRangeShouldBe(bool state)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_PersonalityIdOutOfRangeFlag, &actualState);
      CHECK_EQUAL(state, actualState);
   }
};

TEST(ParametricDataErds, ShouldInvokeActionWhenCriticalMajorVersionIsInvalid)
{
   ActionShouldBeInvoked();

   WhenInitializedWithCriticalMajorVersion(Critical_InvalidMajorVersion);
}

TEST(ParametricDataErds, ShouldInvokeActionWhenCriticalMinorVersionIsInvalid)
{
   ActionShouldBeInvoked();

   WhenInitializedWithCriticalMinorVersion(Critical_InvalidMinorVersion);
}

TEST(ParametricDataErds, ShouldUseDefaultPersonalityParametricDataAndSetPersonalityIdOutOfRangeWhenInitializedWithInvalidPersonality)
{
   GivenThePersonalityIdOutOfRangeIs(CLEAR);
   InitializedWithPersonality(PersonalityId_Max);

   PersonalityParametricDataShouldBe(&personalitesData[PersonalityId_Default]);
   ThePersonalityIdOutOfRangeShouldBe(SET);
}

TEST(ParametricDataErds, ShouldDefinePersonalityParametricDataErdWhenInitializedWithValidPersonalityId)
{
   for(uint8_t i = 0; i < PersonalityId_Max; i++)
   {
      GivenThePersonalityIdOutOfRangeIs(SET);
      InitializedWithPersonality(i);

      PersonalityParametricDataShouldBe(&personalitesData[i]);
      ThePersonalityIdOutOfRangeShouldBe(CLEAR);
   }
}
