/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ServiceModePlugin.h"
#include "Gea2Addresses.h"
#include "ServiceModeAssociatedTestGroup.h"
#include "ServiceModeTestNumber.h"
#include "SystemErds.h"

// clang-format off

#define EXPAND_AS_GENERATE_SERVICE_MODE_TEST_GROUP_CONFIGURATIONS(TestGroup, ...) \
   static const ServiceModeTest_TestNumbersMappingTable_t TestGroup##Config = {   \
      .testNumberEntries = TestGroup##Items COMMA                                 \
      .numberOfItems = NUM_ELEMENTS(TestGroup##Items)                             \
   };

// clang-format on

static const ServiceModeTestNumber_t exitServiceModeTestGroupItems[] = {
   EXIT_SERVICE_MODE_TEST_GROUP_MAPPINGS(EXPAND_AS_GROUP_TEST_NUMBERS)
};

static const ServiceModeTestNumber_t mainBoardVersionsTestGroupItems[] = {
   MAINBOARD_VERSIONS_TEST_GROUP_MAPPINGS(EXPAND_AS_GROUP_TEST_NUMBERS)
};

static const ServiceModeTestNumber_t externalBoardsVersionsTestGroupItems[] = {
   EXTERNAL_BOARDS_VERSIONS_TEST_GROUP_MAPPINGS(EXPAND_AS_GROUP_TEST_NUMBERS)
};

static const ServiceModeTestNumber_t thermistorsTestGroupItems[] = {
   THERMISTORS_TEST_GROUP_MAPPINGS(EXPAND_AS_GROUP_TEST_NUMBERS)
};

SERVICE_MODE_TEST_GROUPS_CONFIGS(EXPAND_AS_GENERATE_SERVICE_MODE_TEST_GROUP_CONFIGURATIONS)

static const Erd_t mainBoardVersionsDataTestItems[] = {
   MAINBOARD_VERSIONS_TEST_GROUP_MAPPINGS(EXPAND_AS_GET_ASSOCIATED_TESTS_INPUTS)
};
static const ServiceModeTest_VersionMappingConfig_t mainBoardVersionsTestDataConfig = {
   .versionErds = mainBoardVersionsDataTestItems,
   .numberOfItems = NUM_ELEMENTS(mainBoardVersionsDataTestItems)
};

static const uint8_t externalBoardsVersionsDataTestItems[] = {
   EXTERNAL_BOARDS_VERSIONS_TEST_GROUP_MAPPINGS(EXPAND_AS_GET_ASSOCIATED_TESTS_INPUTS)
};
static const ServiceModeTest_BoardVersionMappingConfig_t externalBoardsVersionsTestDataConfig = {
   .destinationAddresses = externalBoardsVersionsDataTestItems,
   .numberOfItems = NUM_ELEMENTS(externalBoardsVersionsDataTestItems)
};

static const Erd_t thermistorsDataTestItems[] = {
   THERMISTORS_TEST_GROUP_MAPPINGS(EXPAND_AS_GET_ASSOCIATED_TESTS_INPUTS)
};
static const ServiceModeTest_ThermistorMappingConfig_t thermistorsTestDataConfig = {
   .unfilteredTemperatureErds = thermistorsDataTestItems,
   .numberOfItems = NUM_ELEMENTS(thermistorsDataTestItems)
};

void ServiceModePlugin_Init(ServiceModePlugin_t *instance, I_DataModel_t *dataModel)
{
   ServiceModeRequestHandler_Init(
      &instance->_private.serviceModeRequestHandler,
      dataModel,
      ServiceModeTestNumber_Total - 1);

   ServiceModeTest_ExitServiceMode_Init(
      &instance->_private.exitServiceMode.test,
      &exitServiceModeTestGroupConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.exitServiceMode.test.interface,
      &instance->_private.exitServiceMode.component);

   ServiceModeTest_Version_Init(
      &instance->_private.mainBoardVersion.test,
      &mainBoardVersionsTestGroupConfig,
      &mainBoardVersionsTestDataConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.mainBoardVersion.test.interface,
      &instance->_private.mainBoardVersion.component);

   ServiceModeTest_BoardVersion_Init(
      &instance->_private.externalBoardsVersion.test,
      &externalBoardsVersionsTestGroupConfig,
      &externalBoardsVersionsTestDataConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.externalBoardsVersion.test.interface,
      &instance->_private.externalBoardsVersion.component);

   ServiceModeTest_Thermistor_Init(
      &instance->_private.thermistorsTest.test,
      &thermistorsTestGroupConfig,
      &thermistorsTestDataConfig);
   ServiceModeRequestHandler_AddTest(
      &instance->_private.serviceModeRequestHandler,
      &instance->_private.thermistorsTest.test.interface,
      &instance->_private.thermistorsTest.component);
}
