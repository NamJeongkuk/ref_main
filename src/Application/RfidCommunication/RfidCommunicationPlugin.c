/*!
 * @file
 * @brief Plugin for RFID Communication
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "RfidCommunicationPlugin.h"
#include "SystemErds.h"

static const RfidCommunicatorRfidBoardErds_t rfidBoardErds = {
   .readWriteResult = PublicErd_RfidFilterReadWriteResult_RfidBoard,
   .filterUid = PublicErd_RfidFilterUid_RfidBoard,
   .filterIdentifier = PublicErd_RfidFilterIdentifier_RfidBoard,
   .volumeWaterUsageInOunces = PublicErd_RfidFilterWaterVolumeUsageInOunces_RfidBoard,
   .calendarUsageInSeconds = PublicErd_RfidFilterCalendarUsageInSeconds_RfidBoard,
   .leakDetectedTimeInSeconds = PublicErd_RfidFilterLeakDetectedTimeInSeconds_RfidBoard,
   .numberOfWriteOperations = PublicErd_RfidFilterNumberOfWriteOperations_RfidBoard,
   .filterStatus = PublicErd_RfidFilterStatus_RfidBoard,
   .initializationErrorBitfield = PublicErd_RfidFilterInitializationErrorBitfield_RfidBoard,
   .lockByte = PublicErd_RfidFilterLockByte_RfidBoard,
   .numberOfUnitsFilterHasBeenOn = PublicErd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_RfidBoard,
   .unitSerialNumber = PublicErd_RfidFilterUnitSerialNumber_RfidBoard,
   .previousUnitSerialNumber = PublicErd_RfidFilterPreviousUnitSerialNumber_RfidBoard,
   .filterWaterUsageInGallons = PublicErd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_RfidBoard,
};

static const RfidCommunicatorMainboardErds_t mainboardErds = {
   .uid = PublicErd_RfidFilterUid_Mainboard,
   .volumeWaterUsageInOunces = PublicErd_RfidFilterWaterVolumeUsageInOunces_Mainboard,
   .calendarUsageInSeconds = PublicErd_RfidFilterCalendarUsageInSeconds_Mainboard,
   .filterStatus = PublicErd_RfidFilterStatus_Mainboard,
   .lockByte = PublicErd_RfidFilterLockByte_Mainboard,
   .numberOfUnitsFilterHasBeenOn = PublicErd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_Mainboard,
   .unitSerialNumber = PublicErd_RfidFilterUnitSerialNumber_Mainboard,
   .previousUnitSerialNumber = PublicErd_RfidFilterPreviousUnitSerialNumber_Mainboard,
   .filterWaterUsageInGallons = PublicErd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_Mainboard,
};

static const RfidCommunicatorConfiguration_t config = {
   .rfidBoardErds = &rfidBoardErds,
   .mainboardErds = &mainboardErds,
   .dataRequestErd = PublicErd_RfidFilterDataRequest,
   .geaMessageEndpointErd = PublicErd_Gea2MessageEndpoint
};

static bool RfidBoardIsAlreadyInSystem(I_DataModel_t *dataModel)
{
   bool boardIsInSystem;
   DataModel_Read(dataModel, Erd_RfidBoardInSystem, &boardIsInSystem);

   return boardIsInSystem;
}

static void RfidBoardIsInSystem(void *context, const void *args)
{
   RfidCommunicationPlugin_t *instance = context;
   const bool *rfidBoardIsInTheSystem = args;

   if(*rfidBoardIsInTheSystem)
   {
      RfidCommunicator_Init(
         &instance->_private.rfidCommunicator,
         instance->_private.externalDataSource,
         &config);

      RfidCommunicationControllerPlugin_Init(
         &instance->_private.rfidCommunicationControllerPlugin, 
         instance->_private.dataModel);

      DataModel_Unsubscribe(
         instance->_private.dataModel,
         Erd_RfidBoardInSystem,
         &instance->_private.rfidBoardInSystemSubscription);
   }
}

void RfidCommunicationPlugin_Init(
   RfidCommunicationPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->_private.externalDataSource = DataModel_ReadPointer(
      dataModel,
      Erd_ExternalDataSource);

   if(RfidBoardIsAlreadyInSystem(dataModel))
   {
      RfidCommunicator_Init(
         &instance->_private.rfidCommunicator,
         instance->_private.externalDataSource,
         &config);

      RfidCommunicationControllerPlugin_Init(
         &instance->_private.rfidCommunicationControllerPlugin, 
         instance->_private.dataModel);
   }
   else
   {
      EventSubscription_Init(
         &instance->_private.rfidBoardInSystemSubscription,
         instance,
         RfidBoardIsInSystem);
      DataModel_Subscribe(
         dataModel,
         Erd_RfidBoardInSystem,
         &instance->_private.rfidBoardInSystemSubscription);
   }
}
