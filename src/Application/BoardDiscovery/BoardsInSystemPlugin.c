/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BoardsInSystemPlugin.h"
#include "Gea2Addresses.h"
#include "utils.h"
#include "SystemErds.h"

static const GeaAddressToNvInSystemMap_t geaAddressNvErdPairs[] = {
   { Gea2Address_InternalTemperatureUi, Erd_InternalTemperatureUiInSystem },
   { Gea2Address_Dispenser, Erd_DispenserUiInSystem },
   { Gea2Address_DoorBoard, Erd_DoorBoardInSystem },
   { Gea2Address_RfidBoard, Erd_RfidBoardInSystem },
   { Gea2Address_EmbeddedWiFi, Erd_EmbeddedWifiInSystem },
   { Gea2Address_AndroidSbc, Erd_AndroidSbcBoardInSystem }
};

static const BoardsInSystemConfig_t boardsInSystemConfig = {
   .addressToErdMapArray = geaAddressNvErdPairs,
   .addressToErdMapArrayNumberEntries = NUM_ELEMENTS(geaAddressNvErdPairs),
   .retryPeriodInSec = 5,
   .initialDelayTimeInSec = 3,
   .numberOfRetryRequests = 5,
   .geaMessageEndpointErd = Erd_Gea2MessageEndpoint,
   .TimerModuleErd = Erd_TimerModule,
};

void BoardsInSystemPlugin_Init(
   BoardsInSystemPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   BoardsInSystem_Init(
      &instance->_private.boardsInSystem,
      dataModel,
      &boardsInSystemConfig);
}
