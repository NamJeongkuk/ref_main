/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GeaStack.h"
#include "DoorBoardGea3Mappings.h"
#include "DataModelErdPointerAccess.h"
#include "Gea2Message.h"
#include "Gea2Addresses.h"
#include "Header.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "XMacroUtils.h"
#include "uassert.h"
#include "ContextProtector_Rx2xx.h"

enum
{
   Gea2CrcSeed = 0x1021,
   ParametricHackRequired = false,
   UnlockErdSecurityPeriodMsec = 30 * MSEC_PER_MIN,
   MissedAckLimit = 31,
   RestrictedFullRangeErdStart = 0xF000,
   RestrictedFullRangeErdEnd = 0xFFFF,
   RetryCount = 3,
   RequestTimeoutInMsec = 1 * MSEC_PER_SEC,
   DiagnosticsUpdatePeriodMsec = 1000,
   ErdClientRetryCount = 10,
   SubscriptionRetentionPeriodInMsec = 1 * MSEC_PER_MIN,
   SignOfLifeTimeoutInMsec = 10 * MSEC_PER_SEC,
};

// clang-format off

#define EXPAND_AS_PUBLIC_AND_SERVICE_ERDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_PUBLIC_AND_SERVICE_, Number)(Public##Name COMMA)

// clang-format on

static const Erd_t publicAndServiceErds[] = {
   ERD_TABLE(EXPAND_AS_PUBLIC_AND_SERVICE_ERDS)
};

static const ConstArrayMap_BinarySearchConfiguration_t publicAndServiceErdMapConfiguration = {
   publicAndServiceErds,
   NUM_ELEMENTS(publicAndServiceErds),
   ELEMENT_SIZE(publicAndServiceErds),
   sizeof(Erd_t),
   0,
   false
};

#define INCLUDE_ERD_MAPPING_None(...)
#define INCLUDE_ERD_MAPPING_Published(...) __VA_ARGS__
#define INCLUDE_ERD_MAPPING_PeriodicAndOnChange(...) __VA_ARGS__
#define INCLUDE_ERD_MAPPING_OnChangeOnly(...) __VA_ARGS__
#define WRITE_ONLY_ON_CHANGE_OnChangeOnly true
#define WRITE_ONLY_ON_CHANGE_PeriodicAndOnChange false

#define EXPAND_AS_PUBLISHED_ERD_MAPPING(MainboardErd, PublicErd, Publish, Write) \
   CONCAT(INCLUDE_ERD_MAPPING_, Publish)                                         \
   ({                                                                            \
       .publishedErd = MainboardErd,                                             \
       .internalErd = PublicErd,                                                 \
    }, )
#define EXPAND_AS_WRITTEN_ERD_MAPPING(MainboardErd, PublicErd, Publish, Write) \
   CONCAT(INCLUDE_ERD_MAPPING_, Write)                                         \
   ({                                                                          \
       .writeOnlyOnChange = WRITE_ONLY_ON_CHANGE_##Write,                      \
       .internalErd = PublicErd,                                               \
       .peripheralErd = MainboardErd,                                          \
    }, )

static const ErdGea2ApiRevision2SubscriptionClientPublishedErdMapping_t publishedErdMappings[] = {
   DOORBOARD_ERD_MAPPINGS(EXPAND_AS_PUBLISHED_ERD_MAPPING)
};

static const ErdGea2ApiRevision2SubscriptionClientWrittenErdMapping_t writtenErdMappings[] = {
   DOORBOARD_ERD_MAPPINGS(EXPAND_AS_WRITTEN_ERD_MAPPING)
};

static const ErdGea2ApiRevision2SubscriptionClientConfiguration_t subscriptionClientConfig = {
   .hostGea2Address = Gea2Address_DoorBoard,
   .writeWrittenErdsPeriodically = true,
   .writeWrittenErdsOnChange = true,
   .writePeriod = SubscriptionRetentionPeriodInMsec,
   .subscriptionRetentionPeriod = SubscriptionRetentionPeriodInMsec,
   .signOfLifeTimeout = SignOfLifeTimeoutInMsec,
   .enabledErd = PublicErd_DoorBoardEnable,
   .initialSyncComplete = PublicErd_InitialSyncWithDoorBoardComplete,
   .outgoingSignOfLifeErd = PublicErd_SignOfLifeFromMainboardToDoorBoard,
   .incomingSignOfLifeErd = PublicErd_SignOfLifeFromDoorBoardToMainboard,
   .signOfLifeFromHostMissingErrorErd = PublicErd_DoorBoardSignOfLifeMissingError,
   .subscriptionFailedErrorErd = PublicErd_DoorBoardSubscriptionFailedError,
   .unexpectedPublicationErrorErd = PublicErd_UnexpectedPublicationFromDoorBoardError,
   .unexpectedPublicationSizeErrorErd = PublicErd_UnexpectedPublicationSizeFromDoorBoardError,
   .unableToWriteToHostErrorErd = PublicErd_UnableToWriteToDoorBoardError,
   .unableToQueueErdClientRequestErrorErd = PublicErd_UnableToQueueErdClientRequestError,
   .publishedErdMappings = publishedErdMappings,
   .publishedErdMappingCount = NUM_ELEMENTS(publishedErdMappings),
   .writtenErdMappings = writtenErdMappings,
   .writtenErdMappingCount = NUM_ELEMENTS(writtenErdMappings),
};

static const ErdClient_ApiRevision2Configuration_t erdClientConfiguration = {
   .requestTimeout = RequestTimeoutInMsec,
   .requestRetries = ErdClientRetryCount,
};

static void SubscribeToDoorboard(
   GeaStack_t *instance,
   TimerModule_t *timerModule,
   I_DataSource_t *externalDataSource)
{
   ErdGea2ApiRevision2SubscriptionClient_Init(
      &instance->_private.doorBoardErdSubscriptionClient,
      timerModule,
      externalDataSource,
      &instance->_private.erdClient.interface,
      &subscriptionClientConfig);

   DataSource_Write(
      externalDataSource,
      PublicErd_DoorBoardEnable,
      set);
}

static const uint8_t rangeRestrictedValidatorRestrictedAddressTable[] = {
   Gea2Address_EmbeddedWiFi,
   Gea2Address_ConnectPlusWiFi
};

static const Validator_RestrictedRangeErdConfiguration_t restrictedRangeConfiguration = {
   rangeRestrictedValidatorRestrictedAddressTable,
   NUM_ELEMENTS(rangeRestrictedValidatorRestrictedAddressTable),
   RestrictedFullRangeErdStart,
   RestrictedFullRangeErdEnd
};

static void InitializeErdSecurityComponents(
   GeaStack_t *instance,
   I_DataModel_t *dataModel)
{
   Input_ErdGea2UnlockState_Init(
      &instance->_private.erdSecurity.erdGea2UnlockStateInput,
      instance->_private.packetEndpoint,
      UnlockErdSecurityPeriodMsec,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));

   Validator_RestrictedRangeErd_Init(
      &instance->_private.erdSecurity.restrictedRangeErdValidator,
      &instance->_private.erdSecurity.erdGea2UnlockStateInput.interface,
      &restrictedRangeConfiguration);

   Gea2PacketEndpoint_PacketRestrictor_Init(
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor,
      instance->_private.packetEndpoint,
      &instance->_private.erdSecurity.restrictedRangeErdValidator.interface);
}

static void ConnectGea2MessageEndpointToDataSource(
   GeaStack_t *instance,
   I_DataSource_t *externalDataSource,
   TimerModule_t *timerModule)
{
   ErdGea2ReadWriteApiRevision2_Init(
      &instance->_private.erdApiRevision2ReadWrite,
      externalDataSource,
      NULL,
      NULL,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface);

   ConstArrayMap_BinarySearch_Init(
      &instance->_private.publicAndServiceErdMap,
      &publicAndServiceErdMapConfiguration);

   ErdGea2SubscriptionApiRevision2_Init(
      &instance->_private.erdApiRevision2Subscription,
      externalDataSource,
      NULL,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface,
      timerModule,
      &instance->_private.publicAndServiceErdMap.interface,
      instance->_private.subscriptionResources,
      ErdApiV2SubscriptionClients,
      &instance->_private.subscriptionBuffers[0][0],
      sizeof(instance->_private.subscriptionBuffers[0]),
      true,
      Gea2PacketMaxPayload);

   ErdClient_ApiRevision2_Init(
      &instance->_private.erdClient,
      timerModule,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface,
      instance->_private.queueBuffer,
      NUM_ELEMENTS(instance->_private.queueBuffer),
      &erdClientConfiguration);
}

static void CreatePacketAndMessageEndpoints(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   instance->_private.packetEndpoint =
      Gea2Configurator_CreatePacketEndpointNode(
         &instance->_private.configurator,
         &instance->_private.applicationNode,
         &instance->_private.applicationPacketEndpointResources,
         geaAddress);

   InitializeErdSecurityComponents(instance, dataModel);

   Gea2MessageEndpoint_Gea2PacketEndpointAdapter_Init(
      &instance->_private.messageEndpointAdapter,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface);

   DataModelErdPointerAccess_Write(dataModel, Erd_Gea2MessageEndpoint, &instance->_private.messageEndpointAdapter.interface);
}

static void CreateGea2Node(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_Uart_t *uart = DataModelErdPointerAccess_GetUart(dataModel, Erd_Gea2Uart);

   if(uart)
   {
      Gea2Configurator_CreateDefaultBackgroundSingleWireUartInterfaceNode(
         &instance->_private.configurator,
         &instance->_private.gea2Bus.node,
         &instance->_private.gea2Bus.nodeResources,
         uart,
         DataModelErdPointerAccess_GetTimeSource(dataModel, Erd_TimeSource),
         DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
         geaAddress);

      Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
         &instance->_private.configurator,
         &instance->_private.gea2Bus.node,
         &instance->_private.gea2Bus.dynamicRoutingResources,
         instance->_private.gea2Bus.dynamicRoutingTable,
         ELEMENT_COUNT(instance->_private.gea2Bus.dynamicRoutingTable));
   }
}

static void CreateGea3NodeA(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_BufferedUart_t *bufferedUart = DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_BufferedUartA);

   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.nodeA.node,
      &instance->_private.nodeA.nodeResources,
      bufferedUart,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      geaAddress,
      RetryCount,
      instance->_private.nodeA.buffers.sendBuffer,
      sizeof(instance->_private.nodeA.buffers.sendBuffer),
      instance->_private.nodeA.buffers.receiveBuffer,
      sizeof(instance->_private.nodeA.buffers.receiveBuffer),
      instance->_private.nodeA.buffers.packetQueueStorage,
      sizeof(instance->_private.nodeA.buffers.packetQueueStorage));

   Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
      &instance->_private.configurator,
      &instance->_private.nodeA.node,
      &instance->_private.nodeA.dynamicRoutingResources,
      instance->_private.nodeA.dynamicRoutingTable,
      ELEMENT_COUNT(instance->_private.nodeA.dynamicRoutingTable));
}

static void CreateGea3NodeD(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_BufferedUart_t *bufferedUart = DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_BufferedUartD);

   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.nodeD.node,
      &instance->_private.nodeD.nodeResources,
      bufferedUart,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      geaAddress,
      RetryCount,
      instance->_private.nodeD.buffers.sendBuffer,
      sizeof(instance->_private.nodeD.buffers.sendBuffer),
      instance->_private.nodeD.buffers.receiveBuffer,
      sizeof(instance->_private.nodeD.buffers.receiveBuffer),
      instance->_private.nodeD.buffers.packetQueueStorage,
      sizeof(instance->_private.nodeD.buffers.packetQueueStorage));

   bool useFullRoutingTable;
   DataModel_Read(dataModel, Erd_FullRoutingTableUartDOverride, &useFullRoutingTable);

   if(useFullRoutingTable)
   {
      Gea2Configurator_AddFullRoutingTableToNode(
         &instance->_private.configurator,
         &instance->_private.nodeD.node,
         &instance->_private.nodeD.resources.fullRoutingResources);
   }
   else
   {
      Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
         &instance->_private.configurator,
         &instance->_private.nodeD.node,
         &instance->_private.nodeD.resources.dynamicRoutingResources,
         instance->_private.nodeD.dynamicRoutingTable,
         ELEMENT_COUNT(instance->_private.nodeD.dynamicRoutingTable));
   }
}

static void CreateGea3NodeE(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_BufferedUart_t *bufferedUart = DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_BufferedUartE);

   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.nodeE.node,
      &instance->_private.nodeE.nodeResources,
      bufferedUart,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      geaAddress,
      RetryCount,
      instance->_private.nodeE.buffers.sendBuffer,
      sizeof(instance->_private.nodeE.buffers.sendBuffer),
      instance->_private.nodeE.buffers.receiveBuffer,
      sizeof(instance->_private.nodeE.buffers.receiveBuffer),
      instance->_private.nodeE.buffers.packetQueueStorage,
      sizeof(instance->_private.nodeE.buffers.packetQueueStorage));

   Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
      &instance->_private.configurator,
      &instance->_private.nodeE.node,
      &instance->_private.nodeE.dynamicRoutingResources,
      instance->_private.nodeE.dynamicRoutingTable,
      ELEMENT_COUNT(instance->_private.nodeE.dynamicRoutingTable));
}

static void CreateGea3NodeC(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_BufferedUart_t *bufferedUart = DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_BufferedUartC);

   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.nodeC.node,
      &instance->_private.nodeC.nodeResources,
      bufferedUart,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      geaAddress,
      RetryCount,
      instance->_private.nodeC.buffers.sendBuffer,
      sizeof(instance->_private.nodeC.buffers.sendBuffer),
      instance->_private.nodeC.buffers.receiveBuffer,
      sizeof(instance->_private.nodeC.buffers.receiveBuffer),
      instance->_private.nodeC.buffers.packetQueueStorage,
      sizeof(instance->_private.nodeC.buffers.packetQueueStorage));

   Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
      &instance->_private.configurator,
      &instance->_private.nodeC.node,
      &instance->_private.nodeC.dynamicRoutingResources,
      instance->_private.nodeC.dynamicRoutingTable,
      ELEMENT_COUNT(instance->_private.nodeC.dynamicRoutingTable));
}

void GeaStack_Init(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   I_DataSource_t *externalDataSource,
   uint8_t geaAddress)
{
   Gea2Configurator_Init(&instance->_private.configurator);
   Gea2Configurator_EnablePromiscuousRouting(&instance->_private.configurator);

   CreateGea2Node(instance, dataModel, geaAddress);
   CreateGea3NodeA(instance, dataModel, geaAddress);
   CreateGea3NodeC(instance, dataModel, geaAddress);
   CreateGea3NodeD(instance, dataModel, geaAddress);
   CreateGea3NodeE(instance, dataModel, geaAddress);

   CreatePacketAndMessageEndpoints(instance, dataModel, geaAddress);

   ConnectGea2MessageEndpointToDataSource(
      instance,
      externalDataSource,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));

   SubscribeToDoorboard(
      instance,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      externalDataSource);

   Gea2CommonCommands_Init(
      &instance->_private.commonCommands,
      instance->_private.packetEndpoint,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      Gea2CrcSeed,
      Header_GetImageHeader(ImageType_BootLoader),
      Header_GetImageHeader(ImageType_Application),
      Header_GetImageHeader(ImageType_Parametric),
      Header_GetImageHeader(ImageType_Auxiliary),
      DataModelErdPointerAccess_GetAction(dataModel, Erd_JumpToBootLoaderAction),
      DataModelErdPointerAccess_GetAction(dataModel, Erd_SystemResetAction),
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      NULL,
      0,
      ParametricHackRequired,
      0,
      DataModel_GetInput(dataModel, Erd_ReadyToEnterBootLoader));
}

I_Gea2PacketEndpoint_t *GeaStack_GetGea2PacketEndpoint(GeaStack_t *instance)
{
   return &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface;
}

void GeaStack_Run(GeaStack_t *instance)
{
   Gea2Configurator_Run(&instance->_private.configurator);
}
