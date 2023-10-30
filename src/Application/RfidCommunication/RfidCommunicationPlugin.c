/*!
 * @file
 * @brief Plugin for RFID Communication
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "RfidCommunicationPlugin.h"
#include "SystemErds.h"

static const RfidCommunicatorRfidReadErds_t rfidReadErds = {
   .readWriteResult = PublicErd_RfidFilterReadWriteResult_RfidBoard,
   .filterUid = PublicErd_RfidFilterUid_RfidBoard,
   .filterIdentifier = PublicErd_RfidFilterIdentifier_RfidBoard,
   .volumeWaterUsageInOuncesX100 = PublicErd_RfidFilterWaterVolumeUsageInOuncesX100_RfidBoard,
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

static const RfidCommunicatorRfidWriteErds_t rfidWriteErds = {
   .uid = PublicErd_RfidFilterUid,
   .volumeWaterUsageInOuncesX100 = PublicErd_Eeprom_TotalWaterVolumeUsageInOuncesX100,
   .calendarUsageInSeconds = PublicErd_Eeprom_WaterFilterCalendarUsageInSeconds,
   .filterStatus = PublicErd_RfidFilterStatus,
   .lockByte = PublicErd_RfidFilterLockByte,
   .numberOfUnitsFilterHasBeenOn = PublicErd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn,
   .unitSerialNumber = PublicErd_RfidFilterUnitSerialNumber,
   .previousUnitSerialNumber = PublicErd_RfidFilterPreviousUnitSerialNumber,
   .filterWaterUsageInGallons = PublicErd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons,
};

static const RfidCommunicatorConfiguration_t config = {
   .rfidReadErds = &rfidReadErds,
   .rfidWriteErds = &rfidWriteErds,
   .dataRequestErd = PublicErd_RfidFilterDataRequest,
   .geaMessageEndpointErd = PublicErd_Gea2MessageEndpoint
};

static const Erd_t rfidFilterErrorFaultErds[] = {
   Erd_RfidBoardHardwareFailureFault,
   Erd_RfidBoardCommunicationFault,
   Erd_RfidBoardTagAuthenticationFailedFault
};

static const Erd_t rfidFilterLeakDetectedFaultErds[] = {
   Erd_RfidBoardLeakDetectedFault
};

static const ErdLogicServiceConfigurationEntry_t rfidFilterFaultLogicServiceConfigurationEntries[] = {
   { 
      .logicalOperator = ErdLogicServiceOperator_Or,
      .operandErds = {
         rfidFilterErrorFaultErds,
         NUM_ELEMENTS(rfidFilterErrorFaultErds) 
      },
      .resultErd = Erd_FilterError 
   },
   { 
      .logicalOperator = ErdLogicServiceOperator_Set, 
      .operandErds = { 
         rfidFilterLeakDetectedFaultErds, 
         NUM_ELEMENTS(rfidFilterLeakDetectedFaultErds) 
      }, 
      .resultErd = Erd_LeakDetected 
   }
};

static const ErdLogicServiceConfiguration_t rfidFilterFaultLogicServiceConfiguration = {
   .array = rfidFilterFaultLogicServiceConfigurationEntries,
   .numberOfEntries = NUM_ELEMENTS(rfidFilterFaultLogicServiceConfigurationEntries)
};

static bool RfidBoardIsAlreadyInSystem(I_DataModel_t *dataModel)
{
   bool boardIsInSystem;
   DataModel_Read(dataModel, Erd_RfidBoardInSystem, &boardIsInSystem);

   return boardIsInSystem;
}

static void RfidErrorAndLeakDetectedErd_Init(RfidCommunicationPlugin_t *instance)
{
   ErdLogicService_Init(
      &instance->_private.rfidFilterFaultLogicService,
      &rfidFilterFaultLogicServiceConfiguration,
      DataModel_AsDataSource(instance->_private.dataModel));
}

static void InitRfidFilterPlugins(RfidCommunicationPlugin_t *instance)
{
   RfidCommunicator_Init(
      &instance->_private.rfidCommunicator,
      instance->_private.externalDataSource,
      &config);

   RfidCommunicationControllerPlugin_Init(
      &instance->_private.rfidCommunicationControllerPlugin,
      instance->_private.dataModel);

   RfidErrorAndLeakDetectedErd_Init(instance);
}

static void RfidBoardIsInSystem(void *context, const void *args)
{
   RfidCommunicationPlugin_t *instance = context;
   const bool *rfidBoardIsInTheSystem = args;

   if(*rfidBoardIsInTheSystem)
   {
      InitRfidFilterPlugins(instance);

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
      InitRfidFilterPlugins(instance);
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
