/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>

#include "Action_Null.h"
#include "Action_Rx62tReset.h"
#include "Watchdog.h"
#include "Application.h"
#include "SystemData.h"
#include "Constants_Binary.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "DataSource_ApplianceApiRevision1.h"
#include "FlashBlockGroup_Rx62t.h"
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
#include "Crc16Calculator_Table.h"
#include "ContextProtector_Rx62t.h"
#include "Crc16Calculator_Rx62t.h"
#include "UlTestsPlugin.h"
#include "InitializeStackMemory.h"
#include "StackConfigurator.h"
#include "Interrupt_Cmt0.h"
#include "Rx62tResetSource.h"
#include "ApplicationUassert.h"
#include "Interrupts.h"
#include "uassert.h"

enum
{
   UlTestsRunPeriodInMSec = 10,
   UlTestsWatchdogTimeoutInMSec = 1000,
   BytesToCrcPerRomCheck = 1024,

#ifdef DEBUG
   RomCheckErrorEnabled = false,
#else
   RomCheckErrorEnabled = true
#endif
};

static SystemData_t systemData;
static Application_t application;
static TimerModuleStack_t timerModuleStack;
static InvokeActionOnTimerPeriodic_t watchdogPetter;
static GeaStack_t geaStack;
static UlTestsPlugin_t ulTestsPlugin;

static const uint8_t staticRoutingTable[] = {
   Rx130Gea2Address
};

static void UpdateBuildInfo(
   I_DataModel_t *dataModel,
   const ImageHeader_t *header)
{
   static const GitHash_t gitHash = { GIT_HASH_U8_ARRAY_RX };
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
   STACK_ALLOC_GEA2PACKET(packet, 5);
   packet->destination = 0xFF;
   packet->payload[0] = 0x00;
   packet->payload[1] = 0xDE;
   packet->payload[2] = 0xAF;
   packet->payload[3] = 0xBA;
   packet->payload[4] = 0xBE;

   Gea2PacketEndpoint_Send(gea2PacketEndpoint, packet, 2);
}

static void SetReadyToEnterBootLoader(I_DataModel_t *dataModel)
{
   ReadyToEnterBootLoaderState_t ready = ReadyToEnterBootLoaderState_Ready;
   DataModel_Write(dataModel, Erd_ReadyToEnterBootLoader, &ready);
}

int main(void)
{
   Hardware_InitializeStage1();

   ContextProtector_Protect(ContextProtector_Rx62t_GetInstance());
   {
      const StackConfiguration_t *stackConfig = StackConfigurator_GetConfiguration();
      INIT_STACK_MEMORY(stackConfig->stackStartAddress, stackConfig->stackSize, stackConfig->stackDirection, stackConfig->pattern, stackConfig->patternSize);
   }
   ContextProtector_Unprotect(ContextProtector_Rx62t_GetInstance());

   Watchdog_Init();
   I_Action_t *resetAction = Action_Rx62tReset_Init();
   TimerModule_t *timerModule = TimerModuleStack_Init(&timerModuleStack, Interrupt_Cmt0_Init());
   Watchdog_EnableAutoKick(timerModule);
   I_Action_t *watchdogKickAction = Watchdog_KickAction();

   SystemData_Init(
      &systemData,
      timerModule,
      FlashBlockGroup_Rx62t_Init(watchdogKickAction, resetAction, timerModule),
      Crc16Calculator_Table,
      watchdogKickAction,
      resetAction);

   I_DataModel_t *dataModel = SystemData_DataModel(&systemData);
   TimerModuleStack_WritePointersToDataModel(&timerModuleStack, dataModel);

   ApplicationUassert_Init(
      resetAction,
      DataModel_GetOutput(dataModel, Erd_ProgramCounterAddressAtLastUassert),
      timerModule);

   Hardware_InitializeStage2(dataModel);

   GeaStack_Init(
      &geaStack,
      dataModel,
      SystemData_ExternalDataSource(&systemData),
      Rx62tGea2Address,
      staticRoutingTable,
      ELEMENT_COUNT(staticRoutingTable));

   Application_Init(
      &application,
      dataModel,
      StackConfigurator_GetConfiguration(),
      Rx62tResetSource_GetResetReason());

   InvokeActionOnTimerPeriodic_Init(
      &watchdogPetter,
      watchdogKickAction,
      timerModule,
      1);

   SendStartupMessage(GeaStack_GetGea2PacketEndpoint(&geaStack));

   UpdateBuildInfo(
      dataModel,
      Header_GetImageHeader(ImageType_Application));

   UlTestsPlugin_Init(
      &ulTestsPlugin,
      timerModule,
      resetAction,
      Crc16Calculator_Rx62t_Init(ContextProtector_Rx62t_GetInstance()),
      Header_GetImageHeader(ImageType_Application),
      RomCheckErrorEnabled,
      UlTestsRunPeriodInMSec,
      UlTestsWatchdogTimeoutInMSec,
      BytesToCrcPerRomCheck,
      watchdogKickAction);

   SetReadyToEnterBootLoader(dataModel);

   while(1)
   {
      GeaStack_Run(&geaStack);

      if(!TimerModule_Run(timerModule))
      {
         Interrupts_WaitForInterrupt();
      }
   }
}