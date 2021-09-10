/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>
#include "cmsis_gcc.h"
#include "Action_Null.h"
#include "Application.h"
#include "SystemData.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "DataSource_ApplianceApiRevision1.h"
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
#include "InitializeStackMemory.h"
#include "StackConfigurator.h"
#include "ContextProtector_Ra.h"
#include "Action_RaWatchdog.h"
#include "Action_RaSystemReset.h"
#include "Interrupt_RaSystemTick.h"
#include "CrcCalculator_Ra.h"
#include "FlashBlockGroup_Ra.h"
#include "FlashBlockGroup_MergedBlocks.h"
#include "ApplicationUassert.h"
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
static FlashBlockGroup_MergedBlocks_t mergedBlocksFlashBlockGroup;

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

   ContextProtector_Protect(ContextProtector_Ra_GetInstance());
   {
      const StackConfiguration_t *stackConfig = StackConfigurator_GetConfiguration();
      INIT_STACK_MEMORY(stackConfig->stackStartAddress, stackConfig->stackSize, stackConfig->stackDirection, stackConfig->pattern, stackConfig->patternSize);
   }
   ContextProtector_Unprotect(ContextProtector_Ra_GetInstance());

   I_Action_t *watchdogKickAction = Action_RaWatchdog_Init();
   I_Action_t *resetAction = Action_RaSystemReset_Init();
   I_Interrupt_t *systemTickInterrupt = Interrupt_RaSystemTick_Init();
   TimerModule_t *timerModule = TimerModuleStack_Init(&timerModuleStack, systemTickInterrupt);

   FlashBlockGroup_MergedBlocks_Init(
      &mergedBlocksFlashBlockGroup,
      FlashBlockGroup_Ra_Init(watchdogKickAction, Action_Null_GetInstance(), timerModule),
      2);

   SystemData_Init(
      &systemData,
      timerModule,
      &mergedBlocksFlashBlockGroup.interface,
      Crc16Calculator_Ra_Init(ContextProtector_Ra_GetInstance()),
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
      Ra6m3Gea2Address,
      staticRoutingTable,
      ELEMENT_COUNT(staticRoutingTable));

   static ResetReason_t resetReason = {
      .source = ResetSource_PowerOn,
      .metadata = 0
   };

   Application_Init(
      &application,
      dataModel,
      StackConfigurator_GetConfiguration(),
      resetReason); //FIXME

   InvokeActionOnTimerPeriodic_Init(
      &watchdogPetter,
      watchdogKickAction,
      timerModule,
      1);

   SendStartupMessage(GeaStack_GetGea2PacketEndpoint(&geaStack));

   UpdateBuildInfo(
      dataModel,
      Header_GetImageHeader(ImageType_Application));

   SetReadyToEnterBootLoader(dataModel);

   while(1)
   {
      GeaStack_Run(&geaStack);

      if(!TimerModule_Run(timerModule))
      {
         __WFI();
      }
   }
}
