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
#include "ApplicationDataModel.h"
#include "Constants_Binary.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "DataSource_ApplianceApiRevision1.h"
#include "FlashBlockGroup_Rx130.h"
#include "GeaStack.h"
#include "Hardware.h"
#include "Header.h"
#include "I_Gea2PacketEndpoint.h"
#include "InvokeActionOnTimerPeriodic.h"
#include "SystemErds.h"
#include "Timer.h"
#include "TimerModuleStack.h"
#include "Version.h"

static ApplicationDataModel_t applicationDataModel;
static Application_t application;
static TimerModuleStack_t timerModuleStack;
static InvokeActionOnTimerPeriodic_t watchdogPetter;
static GeaStack_t geaStack;

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

   I_Action_t *watchdog = Action_Rx2xxWatchdog_Init();
   TimerModule_t *timerModule = TimerModuleStack_Init(&timerModuleStack);

   ApplicationDataModel_Init(
      &applicationDataModel,
      timerModule,
      Action_Rx2xxSystemReset_Init(),
      watchdog,
      Crc16Calculator_Table,
      FlashBlockGroup_Rx130_Init(Action_Null_GetInstance(), Action_Null_GetInstance(), timerModule),
      Header_GetImageHeader(ImageType_Application),
      Header_GetImageHeader(ImageType_BootLoader));

   I_DataModel_t *dataModel = ApplicationDataModel_GetInternalDataModel(&applicationDataModel);

   TimerModuleStack_WritePointersToDataModel(&timerModuleStack, dataModel);

   Hardware_InitializeStage2(dataModel);

   GeaStack_Init(
      &geaStack,
      dataModel,
      ApplicationDataModel_GetExternalDataSource(&applicationDataModel));

   Application_Init(
      &application,
      dataModel);

   InvokeActionOnTimerPeriodic_Init(
      &watchdogPetter,
      watchdog,
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
