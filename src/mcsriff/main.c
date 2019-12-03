/*!
 * @file
 * @brief Main application entry point.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>

#include "Action_Null.h"
#include "Application.h"
#include "SystemData.h"
#include "Constants_Binary.h"
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
#include "ContextProtector_Stm32.h"
#include "Action_Stm32SystemReset.h"
#include "Action_Stm32Watchdog.h"
#include "FlashBlockGroup_Stm32F3xx.h"
#include "Crc16Calculator_Stm32.h"
#include "Interrupt_Stm32SystemTick.h"

extern char __NvData1_Location;
extern char __NvData2_Location;

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

static const uint8_t staticRoutingTable[] =
   {
      DugnuttGeaAddress
   };

static const uint32_t flashBlockAddresses[] =
   {
      (uint32_t)&__NvData1_Location,
      (uint32_t)&__NvData2_Location
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

int main(void)
{
   Hardware_InitializeStage1();

   ContextProtector_Protect(ContextProtector_Stm32_GetInstance());
   {
      const StackConfiguration_t *stackConfig = StackConfigurator_GetConfiguration();
      INIT_STACK_MEMORY(stackConfig->stackStartAddress, stackConfig->stackSize, stackConfig->stackDirection, stackConfig->pattern, stackConfig->patternSize);
   }
   ContextProtector_Unprotect(ContextProtector_Stm32_GetInstance());

   I_Action_t *watchdogKickAction = Action_Stm32Watchdog_Init();
   I_Action_t *resetAction = Action_Stm32SystemReset_Init();
   TimerModule_t *timerModule = TimerModuleStack_Init(&timerModuleStack, Interrupt_Stm32SystemTick_Init());

   SystemData_Init(
      &systemData,
      timerModule,
      FlashBlockGroup_Stm32F3xx_Init(Action_Null_GetInstance(), timerModule, NUM_ELEMENTS(flashBlockAddresses), flashBlockAddresses),
      Crc16Calculator_Stm32_Init(ContextProtector_Stm32_GetInstance()),
      watchdogKickAction,
      resetAction);

   I_DataModel_t *dataModel = SystemData_DataModel(&systemData);

   TimerModuleStack_WritePointersToDataModel(&timerModuleStack, dataModel);

   Hardware_InitializeStage2(dataModel);

   GeaStack_Init(
      &geaStack,
      dataModel,
      SystemData_ExternalDataSource(&systemData),
      McSriffGeaAddress,
      staticRoutingTable,
      ELEMENT_COUNT(staticRoutingTable));

   Application_Init(
      &application,
      dataModel,
      StackConfigurator_GetConfiguration());

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
