/*!
 * @file
 * @brief Main application entry point.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>

#include "Action_Null.h"
#include "Action_Rx2xxSystemReset.h"
#include "Action_Rx2xxWatchdog.h"
#include "Application.h"
#include "SystemData.h"
#include "Constants_Binary.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "DataSource_ApplianceApiRevision1.h"
#include "FlashBlockGroup_Rx231.h"
#include "GeaStack.h"
#include "Hardware.h"
#include "Header.h"
#include "I_Gea2PacketEndpoint.h"
#include "InvokeActionOnTimerPeriodic.h"
#include "SystemErds.h"
#include "Timer.h"
#include "TimerModuleStack.h"
#include "Version.h"
#include "Gea2Addresses.h"

static SystemData_t systemData;
static Application_t application;
static TimerModuleStack_t timerModuleStack;
static InvokeActionOnTimerPeriodic_t watchdogPetter;
static GeaStack_t geaStack;

static const uint8_t staticRoutingTable[] =
   {
      DugnuttGeaAddress
   };

static void UpdateBuildInfo(
   I_DataModel_t *dataModel,
   const ImageHeader_t *header)
{
   static const GitHash_t gitHash =
    { GIT_HASH_U8_ARRAY_RX };
   DataModel_Write(dataModel, Erd_GitHash, &gitHash);

   uint32_t buildNumber = BUILD_NUMBER;
   DataModel_Write(dataModel, Erd_BuildNumber, &buildNumber);

   if(header)
   {
      Version_t version;
      version.criticalMajor = header->criticalMajorVersion;
      version.criticalMinor = header->criticalMinorVersion;
      version.major = header->majorVersion;
      version.minor = header->minorVersion;
      DataModel_Write(dataModel, Erd_OldApplicationVersion, &version);
   }
}

static void SendStartupMessage(I_Gea2PacketEndpoint_t *gea2PacketEndpoint)
{
   STACK_ALLOC_GEA2PACKET(packet, 10);
   packet->destination = 0xFF;
   packet->payloadLength = 5;
   packet->payload[0] = 0x00;
   packet->payload[1] = 0xDE;
   packet->payload[2] = 0xAF;
   packet->payload[3] = 0xBA;
   packet->payload[4] = 0xBE;

   Gea2PacketEndpoint_Send(gea2PacketEndpoint, packet, 2);
}

int main(void)
{
   Hardware_InitializeStage1();

   I_Action_t *watchdogKickAction = Action_Rx2xxWatchdog_Init();
   TimerModule_t *timerModule = TimerModuleStack_Init(&timerModuleStack);

   SystemData_Init(
      &systemData,
      timerModule,
      FlashBlockGroup_Rx231_Init(watchdogKickAction, Action_Null_GetInstance(), timerModule),
      Crc16Calculator_Table,
      watchdogKickAction,
      Action_Rx2xxSystemReset_Init());

   I_DataModel_t *dataModel = SystemData_DataModel(&systemData);

   TimerModuleStack_WritePointersToDataModel(&timerModuleStack, dataModel);

   Hardware_InitializeStage2(dataModel);

   GeaStack_Init(
      &geaStack,
      dataModel,
      SystemData_ExternalDataSource(&systemData),
      TrukGeaAddress,
      staticRoutingTable,
      ELEMENT_COUNT(staticRoutingTable));

   Application_Init(
      &application,
      dataModel);

   InvokeActionOnTimerPeriodic_Init(
      &watchdogPetter,
      watchdogKickAction,
      timerModule,
      1);

   SendStartupMessage(GeaStack_GetGea2PacketEndpoint(&geaStack));

   UpdateBuildInfo(
      dataModel,
      Header_GetImageHeader(ImageType_Application));

   DataModel_Write(dataModel, Erd_ReadyToEnterBootLoader, enabled);

   while(1)
   {
      TimerModule_Run(timerModule);
      GeaStack_Run(&geaStack);
   }
}
