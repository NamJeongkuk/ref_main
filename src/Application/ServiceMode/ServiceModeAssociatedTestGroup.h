/*!
 * @file
 * @brief
 * @note Each test group needs its own table.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODEASSOCIATEDTESTGROUP_H
#define SERVICEMODEASSOCIATEDTESTGROUP_H

#include "Gea2Addresses.h"
#include "ServiceModeTestNumber.h"
#include "ServiceModeTest_BoardVersion.h"
#include "ServiceModeTest_Thermistor.h"
#include "ServiceModeTest_Version.h"
#include "SystemErds.h"
#include "utils.h"
#include "XMacroUtils.h"

#define NA

// clang-format off

/*!
 *      nameOfTestGroup,
*/
#define SERVICE_MODE_TEST_GROUPS_CONFIGS(ENTRY) \
   ENTRY(exitServiceModeTestGroup) \
   ENTRY(mainBoardVersionsTestGroup) \
   ENTRY(externalBoardsVersionsTestGroup) \
   ENTRY(thermistorsTestGroup) \

/*!
 *       ServiceTestNumber                                                      AssociatedInput
*/
#define EXIT_SERVICE_MODE_TEST_GROUP_MAPPINGS(ENTRY) \
   ENTRY(ServiceModeTestNumber_ExitServiceMode,                                 NA       ) \

#define MAINBOARD_VERSIONS_TEST_GROUP_MAPPINGS(ENTRY) \
   ENTRY(ServiceModeTestNumber_MainBoardApplicationVersion,                     Erd_ApplicationVersion) \
   ENTRY(ServiceModeTestNumber_MainBoardParametricVersion,                      Erd_ParametricVersion) \

#define EXTERNAL_BOARDS_VERSIONS_TEST_GROUP_MAPPINGS(ENTRY) \
   ENTRY(ServiceModeTestNumber_DispenserBoardVersion,                           Gea2Address_Dispenser) \
   ENTRY(ServiceModeTestNumber_InternalTempBoardUiVersion,                      Gea2Address_InternalTemperatureUi) \
   ENTRY(ServiceModeTestNumber_WifiBoardVersion,                                Gea2Address_EmbeddedWiFi) \
   ENTRY(ServiceModeTestNumber_RfidBoardVersion,                                Gea2Address_RfidBoard) \

#define THERMISTORS_TEST_GROUP_MAPPINGS(ENTRY) \
   ENTRY(ServiceModeTestNumber_AmbientThermistorTest,                           Erd_Ambient_FilteredTemperatureResolvedInDegFx100) \
   ENTRY(ServiceModeTestNumber_FeaturePanCabinetThermistorTest,                 Erd_FeaturePan_UnfilteredTemperatureInDegFx100) \
   ENTRY(ServiceModeTestNumber_FreshFoodCabinetThermistorTest,                  Erd_FreshFood_UnfilteredTemperatureInDegFx100) \
   ENTRY(ServiceModeTestNumber_FreshFoodEvaporatorThermistorTest,               Erd_FreshFoodEvap_UnfilteredTemperatureInDegFx100) \
   ENTRY(ServiceModeTestNumber_FreezerCabinetThermistorTest,                    Erd_Freezer_UnfilteredTemperatureInDegFx100) \
   ENTRY(ServiceModeTestNumber_FreezerEvaporatorThermistorTest,                 Erd_FreezerEvap_UnfilteredTemperatureInDegFx100) \
   ENTRY(ServiceModeTestNumber_IceMaker0MoldThermistorTest,                     Erd_IceMaker0_MoldThermistor_UnfilteredTemperatureInDegFx100) \
   ENTRY(ServiceModeTestNumber_IceMaker1MoldThermistorTest,                     Erd_IceMaker1_MoldThermistor_UnfilteredTemperatureInDegFx100) \
   ENTRY(ServiceModeTestNumber_IceMaker2MoldThermistorTest,                     Erd_IceMaker2_MoldThermistor_UnfilteredTemperatureInDegFx100)

#define EXPAND_AS_GET_ASSOCIATED_TESTS_INPUTS(ServiceTestNumber, AssociatedInput) \
   AssociatedInput COMMA

#define EXPAND_AS_GROUP_TEST_NUMBERS(ServiceTestNumber, AssociatedInput) \
   ServiceTestNumber COMMA

// clang-format on

#endif
