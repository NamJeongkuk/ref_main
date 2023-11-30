/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "BoardFeaturesToApplianceApiFeaturesUpdater.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

#define BIT_POSITION(value) __builtin_ctzl(value)

enum
{
   BoardFeatureOne = 0x00000001,
   BoardFeatureTwo = 0x00000002,
   BoardFeatureThree = 0x00000004,
   BoardFeatureFour = 0x08000000,
   BoardFeatureFive = 0x00020000,
   BoardFeatureSix = 0x00000800,
   BoardFeatureSeven = 0x00000008,
   BoardFeatureEight = 0x10000000,
   BoardFeatureNine = 0x40000000
};

enum
{
   ApplianceFeatureOne = 0x00000010,
   ApplianceFeatureTwo = 0x00000020,
   ApplianceFeatureThree = 0x00000040,
   ApplianceFeatureFour = 0x00800000,
   ApplianceFeatureFive = 0x00200000,
   ApplianceFeatureSix = 0x00080000,
   ApplianceFeatureSeven = 0x00000080,
   ApplianceFeatureEight = 0x80000000,
   ApplianceFeatureNine = 0x04000000
};

enum
{
   CurrentAcceptableBoardFeatureVersion = 69,
   BoardTypeSendingFeatures = 1,
   SomeApplianceFeatureType = 23,
   SomeApplianceFeatureVersion = 34,
};

static ApplianceFeatureApiManifest_t initialApplianceManifest = {
   .type = SomeApplianceFeatureType,
   .version = SomeApplianceFeatureVersion,
   .features = 0x00000000
};

static const FeatureMap_t manifestOneMap[] = {
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureOne),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureOne) },
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureTwo),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureTwo) },
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureThree),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureThree) },
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureFour),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureFour) },
};

static const FeatureMap_t manifestTwoMap[] = {
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureFive),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureFive) },
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureSix),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureSix) },
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureSeven),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureSeven) },
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureEight),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureEight) },
};

static const FeatureMap_t manifestMapWithDuplicateMappings[] = {
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureOne),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureOne) },
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureOne),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureTwo) },
   { .remoteBoardFeatureBitPosition = BIT_POSITION(BoardFeatureOne),
      .applianceFeatureBitPosition = BIT_POSITION(ApplianceFeatureThree) },
};

static const FeatureMapData_t manifestOneMapData = {
   .listOfFeatureMaps = manifestOneMap,
   .manifestErd = Erd_ApplianceApiApplianceTypeManifest1,
   .numberOfFeatureMaps = NUM_ELEMENTS(manifestOneMap)
};

static const FeatureMapData_t manifestTwoMapData = {
   .listOfFeatureMaps = manifestTwoMap,
   .manifestErd = Erd_ApplianceApiApplianceTypeManifest2,
   .numberOfFeatureMaps = NUM_ELEMENTS(manifestTwoMap)
};

static const FeatureMapData_t mapWithDuplicatesMapsData = {
   .listOfFeatureMaps = manifestMapWithDuplicateMappings,
   .manifestErd = Erd_ApplianceApiApplianceTypeManifest1,
   .numberOfFeatureMaps = NUM_ELEMENTS(manifestMapWithDuplicateMappings)
};

static const FeatureMapData_t listOfFeatureMaps[] = {
   manifestOneMapData,
   manifestTwoMapData
};

static const FeatureMapData_t soloMapOnlyLookingAtManifestTwo[] = {
   manifestOneMapData,
   manifestTwoMapData
};

static const FeatureMapData_t duplicateMapList[] = {
   mapWithDuplicatesMapsData
};

static const BoardFeaturesToApplianceApiFeaturesUpdaterConfig_t theFeatureMapConfig = {
   .featureMapData = listOfFeatureMaps,
   .numberOfFeatureMapDatas = NUM_ELEMENTS(listOfFeatureMaps),
   .currentVersion = CurrentAcceptableBoardFeatureVersion,
   .remoteBoardFeaturesErd = Erd_DispenserUiReportedFeatures,
};

static const BoardFeaturesToApplianceApiFeaturesUpdaterConfig_t featureMapOnlyForManifestTwoConfig = {
   .featureMapData = soloMapOnlyLookingAtManifestTwo,
   .numberOfFeatureMapDatas = NUM_ELEMENTS(soloMapOnlyLookingAtManifestTwo),
   .currentVersion = CurrentAcceptableBoardFeatureVersion,
   .remoteBoardFeaturesErd = Erd_DispenserUiReportedFeatures,
};

static const BoardFeaturesToApplianceApiFeaturesUpdaterConfig_t aMapWithBoardFeatureMappingMappedToMultipleApplianceFeatures = {
   .featureMapData = duplicateMapList,
   .numberOfFeatureMapDatas = NUM_ELEMENTS(duplicateMapList),
   .currentVersion = CurrentAcceptableBoardFeatureVersion,
   .remoteBoardFeaturesErd = Erd_DispenserUiReportedFeatures,
};

TEST_GROUP(BoardFeaturesToApplianceApiFeaturesUpdater)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   BoardFeaturesToApplianceApiFeaturesUpdater_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization()
   {
      BoardFeaturesToApplianceApiFeaturesUpdater_Init(
         &instance,
         dataModel,
         &theFeatureMapConfig);
   }

   void GivenInitializationOfAMapWithDuplicateFeaturesMapped()
   {
      BoardFeaturesToApplianceApiFeaturesUpdater_Init(
         &instance,
         dataModel,
         &aMapWithBoardFeatureMappingMappedToMultipleApplianceFeatures);
   }

   void GivenInitializationOfAMapWithMappingsOnlyForManifestTwo()
   {
      BoardFeaturesToApplianceApiFeaturesUpdater_Init(
         &instance,
         dataModel,
         &featureMapOnlyForManifestTwoConfig);
   }

   void WhenBoardFeatureErdGetsUpdatedWith(BoardFeatureApi_t data)
   {
      DataModel_Write(dataModel, Erd_DispenserUiReportedFeatures, &data);
   }

   void GivenApplianceApiFeatureManifestOneErdIs(ApplianceFeatureApiManifest_t data)
   {
      DataModel_Write(dataModel, Erd_ApplianceApiApplianceTypeManifest1, &data);
   }

   void GivenApplianceApiFeatureManifestTwoErdIs(ApplianceFeatureApiManifest_t data)
   {
      DataModel_Write(dataModel, Erd_ApplianceApiApplianceTypeManifest2, &data);
   }

   void ApplianceApiFeatureManifestOneErdShouldBe(ApplianceFeatureApiManifest_t expected)
   {
      ApplianceFeatureApiManifest_t actual;
      DataModel_Read(dataModel, Erd_ApplianceApiApplianceTypeManifest1, &actual);
      CHECK_TRUE(MEMORY_IS_EQUAL(expected, actual));
   }

   void ApplianceApiFeatureManifestTwoErdShouldBe(ApplianceFeatureApiManifest_t expected)
   {
      ApplianceFeatureApiManifest_t actual;
      DataModel_Read(dataModel, Erd_ApplianceApiApplianceTypeManifest2, &actual);
      CHECK_TRUE(MEMORY_IS_EQUAL(expected, actual));
   }
};

TEST(BoardFeaturesToApplianceApiFeaturesUpdater, ShouldMapOneFeatureFromBoardFeaturesToApplianceManifestOneFeatures)
{
   BoardFeatureApi_t boardFeatures;
   boardFeatures.featureApiVersion = CurrentAcceptableBoardFeatureVersion;
   boardFeatures.featureApiType = BoardTypeSendingFeatures;
   boardFeatures.featureBits = BoardFeatureOne;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeatures;
   expectedResultingApplianceFeatures.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeatures.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeatures.features = ApplianceFeatureOne;

   GivenApplianceApiFeatureManifestOneErdIs(initialApplianceManifest);
   GivenApplianceApiFeatureManifestTwoErdIs(initialApplianceManifest);
   GivenInitialization();

   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);
   ApplianceApiFeatureManifestOneErdShouldBe(expectedResultingApplianceFeatures);
   ApplianceApiFeatureManifestTwoErdShouldBe(initialApplianceManifest);
}

TEST(BoardFeaturesToApplianceApiFeaturesUpdater, ShouldMapSomeOfTheAvailableFeaturesFromBoardFeaturesToApplianceManifestOneFeatures)
{
   BoardFeatureApi_t boardFeatures;
   boardFeatures.featureApiVersion = CurrentAcceptableBoardFeatureVersion;
   boardFeatures.featureApiType = BoardTypeSendingFeatures;
   boardFeatures.featureBits = BoardFeatureOne | BoardFeatureThree | BoardFeatureFour;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeatures;
   expectedResultingApplianceFeatures.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeatures.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeatures.features = ApplianceFeatureOne | ApplianceFeatureThree | ApplianceFeatureFour;

   GivenApplianceApiFeatureManifestOneErdIs(initialApplianceManifest);
   GivenApplianceApiFeatureManifestTwoErdIs(initialApplianceManifest);
   GivenInitialization();

   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);
   ApplianceApiFeatureManifestOneErdShouldBe(expectedResultingApplianceFeatures);
   ApplianceApiFeatureManifestTwoErdShouldBe(initialApplianceManifest);
}

TEST(BoardFeaturesToApplianceApiFeaturesUpdater, ShouldMapAllOfTheAvailableFeaturesFromBoardFeaturesToApplianceManifestOneAndTwoFeatures)
{
   BoardFeatureApi_t boardFeatures;
   boardFeatures.featureApiVersion = CurrentAcceptableBoardFeatureVersion;
   boardFeatures.featureApiType = BoardTypeSendingFeatures;
   boardFeatures.featureBits = BoardFeatureOne |
      BoardFeatureTwo |
      BoardFeatureThree |
      BoardFeatureFour |
      BoardFeatureFive |
      BoardFeatureSix |
      BoardFeatureSeven |
      BoardFeatureEight;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeaturesManifestOne;
   expectedResultingApplianceFeaturesManifestOne.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeaturesManifestOne.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeaturesManifestOne.features = ApplianceFeatureOne | ApplianceFeatureTwo | ApplianceFeatureThree | ApplianceFeatureFour;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeaturesManifestTwo;
   expectedResultingApplianceFeaturesManifestTwo.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeaturesManifestTwo.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeaturesManifestTwo.features = ApplianceFeatureFive | ApplianceFeatureSix | ApplianceFeatureSeven | ApplianceFeatureEight;

   GivenApplianceApiFeatureManifestOneErdIs(initialApplianceManifest);
   GivenApplianceApiFeatureManifestTwoErdIs(initialApplianceManifest);
   GivenInitialization();

   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);
   ApplianceApiFeatureManifestOneErdShouldBe(expectedResultingApplianceFeaturesManifestOne);
   ApplianceApiFeatureManifestTwoErdShouldBe(expectedResultingApplianceFeaturesManifestTwo);
}

TEST(BoardFeaturesToApplianceApiFeaturesUpdater, ShouldNotMapAnyFeaturesBetweenBoardAndApplianceIfBoardFeatureVersionsAreIncompatible)
{
   BoardFeatureApi_t boardFeatures;
   boardFeatures.featureApiVersion = CurrentAcceptableBoardFeatureVersion - 1;
   boardFeatures.featureApiType = BoardTypeSendingFeatures;
   boardFeatures.featureBits = BoardFeatureOne |
      BoardFeatureTwo |
      BoardFeatureThree |
      BoardFeatureFour |
      BoardFeatureFive |
      BoardFeatureSix |
      BoardFeatureSeven |
      BoardFeatureEight;

   GivenApplianceApiFeatureManifestOneErdIs(initialApplianceManifest);
   GivenApplianceApiFeatureManifestTwoErdIs(initialApplianceManifest);
   GivenInitialization();

   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);
   ApplianceApiFeatureManifestOneErdShouldBe(initialApplianceManifest);
   ApplianceApiFeatureManifestTwoErdShouldBe(initialApplianceManifest);
}

TEST(BoardFeaturesToApplianceApiFeaturesUpdater, ShouldNotOverwriteAnyExistingFeaturesAlreadyInApplianceManifestsWhenBoardFeaturesReceived)
{
   BoardFeatureApi_t boardFeatures;
   boardFeatures.featureApiVersion = CurrentAcceptableBoardFeatureVersion;
   boardFeatures.featureApiType = BoardTypeSendingFeatures;
   boardFeatures.featureBits = BoardFeatureOne |
      BoardFeatureTwo |
      BoardFeatureThree |
      BoardFeatureFour |
      BoardFeatureFive |
      BoardFeatureSix |
      BoardFeatureSeven |
      BoardFeatureEight;

   ApplianceFeatureApiManifest_t initialApplianceManifestOne;
   initialApplianceManifestOne.type = SomeApplianceFeatureType;
   initialApplianceManifestOne.version = SomeApplianceFeatureVersion;
   initialApplianceManifestOne.features = 0x12344321;

   ApplianceFeatureApiManifest_t initialApplianceManifestTwo;
   initialApplianceManifestTwo.type = SomeApplianceFeatureType;
   initialApplianceManifestTwo.version = SomeApplianceFeatureVersion;
   initialApplianceManifestTwo.features = 0x369CCDE2;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeaturesManifestOne;
   expectedResultingApplianceFeaturesManifestOne.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeaturesManifestOne.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeaturesManifestOne.features = 0x12344321 | ApplianceFeatureOne | ApplianceFeatureTwo | ApplianceFeatureThree | ApplianceFeatureFour;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeaturesManifestTwo;
   expectedResultingApplianceFeaturesManifestTwo.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeaturesManifestTwo.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeaturesManifestTwo.features = 0x369CCDE2 | ApplianceFeatureFive | ApplianceFeatureSix | ApplianceFeatureSeven | ApplianceFeatureEight;

   GivenApplianceApiFeatureManifestOneErdIs(initialApplianceManifestOne);
   GivenApplianceApiFeatureManifestTwoErdIs(initialApplianceManifestTwo);
   GivenInitialization();

   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);
   ApplianceApiFeatureManifestOneErdShouldBe(expectedResultingApplianceFeaturesManifestOne);
   ApplianceApiFeatureManifestTwoErdShouldBe(expectedResultingApplianceFeaturesManifestTwo);
}

TEST(BoardFeaturesToApplianceApiFeaturesUpdater, ShouldHandleDynamicallyChangingFeatures)
{
   BoardFeatureApi_t boardFeatures;
   boardFeatures.featureApiVersion = CurrentAcceptableBoardFeatureVersion;
   boardFeatures.featureApiType = BoardTypeSendingFeatures;
   boardFeatures.featureBits = BoardFeatureOne | BoardFeatureFive;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeaturesManifestOne;
   expectedResultingApplianceFeaturesManifestOne.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeaturesManifestOne.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeaturesManifestOne.features = ApplianceFeatureOne;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeaturesManifestTwo;
   expectedResultingApplianceFeaturesManifestTwo.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeaturesManifestTwo.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeaturesManifestTwo.features = ApplianceFeatureFive;

   GivenApplianceApiFeatureManifestOneErdIs(initialApplianceManifest);
   GivenApplianceApiFeatureManifestTwoErdIs(initialApplianceManifest);
   GivenInitialization();

   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);
   ApplianceApiFeatureManifestOneErdShouldBe(expectedResultingApplianceFeaturesManifestOne);
   ApplianceApiFeatureManifestTwoErdShouldBe(expectedResultingApplianceFeaturesManifestTwo);

   boardFeatures.featureBits |= BoardFeatureTwo | BoardFeatureSix;
   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);

   expectedResultingApplianceFeaturesManifestOne.features |= ApplianceFeatureTwo;
   expectedResultingApplianceFeaturesManifestTwo.features |= ApplianceFeatureSix;

   ApplianceApiFeatureManifestOneErdShouldBe(expectedResultingApplianceFeaturesManifestOne);
   ApplianceApiFeatureManifestTwoErdShouldBe(expectedResultingApplianceFeaturesManifestTwo);

   boardFeatures.featureBits ^= BoardFeatureOne | BoardFeatureFive;
   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);

   expectedResultingApplianceFeaturesManifestOne.features ^= ApplianceFeatureOne;
   expectedResultingApplianceFeaturesManifestTwo.features ^= ApplianceFeatureFive;

   ApplianceApiFeatureManifestOneErdShouldBe(expectedResultingApplianceFeaturesManifestOne);
   ApplianceApiFeatureManifestTwoErdShouldBe(expectedResultingApplianceFeaturesManifestTwo);
}

TEST(BoardFeaturesToApplianceApiFeaturesUpdater, ShouldNotPayAttentionToExtraneousFeaturesSetInBoardFeaturesThatAreNotMapped)
{
   BoardFeatureApi_t boardFeatures;
   boardFeatures.featureApiVersion = CurrentAcceptableBoardFeatureVersion;
   boardFeatures.featureApiType = BoardTypeSendingFeatures;
   boardFeatures.featureBits = BoardFeatureNine;

   GivenApplianceApiFeatureManifestOneErdIs(initialApplianceManifest);
   GivenApplianceApiFeatureManifestTwoErdIs(initialApplianceManifest);
   GivenInitialization();

   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);
   ApplianceApiFeatureManifestOneErdShouldBe(initialApplianceManifest);
   ApplianceApiFeatureManifestTwoErdShouldBe(initialApplianceManifest);
}

TEST(BoardFeaturesToApplianceApiFeaturesUpdater, ShouldNotModifyManifestsOutsideOfItsKnowledge)
{
   BoardFeatureApi_t boardFeatures;
   boardFeatures.featureApiVersion = CurrentAcceptableBoardFeatureVersion;
   boardFeatures.featureApiType = BoardTypeSendingFeatures;
   boardFeatures.featureBits = BoardFeatureFive;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeatures;
   expectedResultingApplianceFeatures.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeatures.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeatures.features = ApplianceFeatureFive;

   GivenApplianceApiFeatureManifestOneErdIs(initialApplianceManifest);
   GivenApplianceApiFeatureManifestTwoErdIs(initialApplianceManifest);
   GivenInitialization();

   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);
   ApplianceApiFeatureManifestOneErdShouldBe(initialApplianceManifest);
   ApplianceApiFeatureManifestTwoErdShouldBe(expectedResultingApplianceFeatures);
}

TEST(BoardFeaturesToApplianceApiFeaturesUpdater, ShouldMapOneFeatureFromBoardFeaturesToMultipleApplianceManifestFeatures)
{
   BoardFeatureApi_t boardFeatures;
   boardFeatures.featureApiVersion = CurrentAcceptableBoardFeatureVersion;
   boardFeatures.featureApiType = BoardTypeSendingFeatures;
   boardFeatures.featureBits = BoardFeatureOne;

   ApplianceFeatureApiManifest_t expectedResultingApplianceFeatures;
   expectedResultingApplianceFeatures.type = SomeApplianceFeatureType;
   expectedResultingApplianceFeatures.version = SomeApplianceFeatureVersion;
   expectedResultingApplianceFeatures.features = ApplianceFeatureOne | ApplianceFeatureTwo | ApplianceFeatureThree;

   GivenApplianceApiFeatureManifestOneErdIs(initialApplianceManifest);
   GivenApplianceApiFeatureManifestTwoErdIs(initialApplianceManifest);
   GivenInitializationOfAMapWithDuplicateFeaturesMapped();

   WhenBoardFeatureErdGetsUpdatedWith(boardFeatures);
   ApplianceApiFeatureManifestOneErdShouldBe(expectedResultingApplianceFeatures);
   ApplianceApiFeatureManifestTwoErdShouldBe(initialApplianceManifest);
}
