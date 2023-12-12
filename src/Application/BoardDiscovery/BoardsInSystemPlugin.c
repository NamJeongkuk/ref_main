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

static const BoardInSystemData_t geaErdEntries[] = {
   {
      .geaAddress = Gea2Address_Dispenser,
      .nvErd = Erd_DispenserUiInSystem,
      .faultErd = Erd_DispenserUiCommunicationFault,
   },
   {
      .geaAddress = Gea2Address_RfidBoard,
      .nvErd = Erd_RfidBoardInSystem,
      .faultErd = Erd_RfidBoardCommunicationFault,
   },
   {
      .geaAddress = Gea2Address_AndroidSbc,
      .nvErd = Erd_AndroidSbcBoardInSystem,
      .faultErd = Erd_AndroidSbcCommunicationFault,
   },
   {
      .geaAddress = Gea2Address_DoorBoard,
      .nvErd = Erd_DoorBoardInSystem,
      .faultErd = Erd_DoorBoardCommunicationFault,
   },
   {
      .geaAddress = Gea2Address_InternalTemperatureUi,
      .nvErd = Erd_InternalTemperatureUiInSystem,
      .faultErd = Erd_InternalTemperatureUiCommunicationFault,
   },
   {
      .geaAddress = Gea2Address_EmbeddedWiFi,
      .nvErd = Erd_EmbeddedWifiInSystem,
      .faultErd = Erd_EmbeddedWifiCommunicationFault,
   }
};

static const BoardsInSystemConfig_t boardsInSystemConfig = {
   .boardsInSystemData = geaErdEntries,
   .allBoardsInSystemNumberEntries = NUM_ELEMENTS(geaErdEntries),
   .initialDiscoveryDelayTimeInSec = 3,
   .retryPeriodForDiscoveryInSec = 5,
   .numberOfDiscoveryRetryRequests = 5,
   .retryPeriodForMonitoringInMinutes = 1,
   .numberOfMonitoringRetryRequestsBeforeSettingFault = 5,
   .geaMessageEndpointErd = Erd_Gea2MessageEndpoint,
   .timerModuleErd = Erd_TimerModule,
};

static uint8_t buffer[NUM_ELEMENTS(geaErdEntries)] = { 0 };

void BoardsInSystemPlugin_Init(
   BoardsInSystemPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   BoardsInSystem_Init(
      &instance->_private.boardsInSystem,
      dataModel,
      &boardsInSystemConfig,
      buffer,
      NUM_ELEMENTS(buffer));

   AndroidSignOfLifeMonitor_Init(
      &instance->_private.androidMonitor,
      dataModel);
}
