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
#include "Action_RxWatchdog.h"
#include "Application.h"
#include "SystemData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "DataSource_ApplianceApiRevision1.h"
#include "FlashBlockGroup_Rx130.h"
#include "Hardware.h"
#include "Header.h"
#include "I_Gea2PacketEndpoint.h"
#include "InvokeActionOnTimerPeriodic.h"
#include "SystemErds.h"
#include "Timer.h"
#include "TimerModuleStack.h"
#include "Version.h"
#include "Gea2Addresses.h"
#include "Crc16Calculator_Rx2xx.h"
#include "ContextProtector_Rx2xx.h"
#include "UlTestsPlugin.h"
#include "UlRamTest.h"
#include "InitializeStackMemory.h"
#include "StackConfigurator.h"
#include "Interrupt_Cmt0.h"
#include "ReadyToEnterBootLoaderState.h"
#include "Rx2xxResetSource.h"
#include "ApplicationUassert.h"
#include "Interrupts.h"
#include "uassert.h"
#include "EepromStack.h"
#include "ParametricData.h"
#include "TimeSource_Rockhopper.h"
#include "NonVolatileAsyncDataSource.h"
#include "AsyncNvMapConfigurations.h"
#include "BootLoopDefender.h"
#include "MemoryMap.h"
#include "RelayWatchdog.h"
#include "ParametricDataVersionCheck.h"
#include "EepromMonitorPlugin.h"
#include "ParametricDataErds.h"
#include "DataSource_Gpio.h"
#include "GeaStack.h"
#include "ShortGitHash.h"
#include "PersonalityEepromStack.h"
#include "BoardsInSystemPlugin.h"

enum
{
   UlTestsRunPeriodInMSec = 500,
   UlTestsWatchdogTimeoutInMSec = 50 * MSEC_PER_SEC,
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
static UlTestsPlugin_t ulTestsPlugin;
static NonVolatileAsyncDataSource_t nvAsyncDataSource;
static Input_StackUsageCalculator_t stackUsageCalculator;
static Timer_t stackUsageUpdateTimer;
static GeaStack_t geaStack;
static BoardsInSystemPlugin_t boardsInSystemPlugin;

enum
{
   StackUsageUpdatePeriodInMsec = 1000
};

static BootLoopDefender_t bootLoopDefender;

static BootLoopDefenderState_t bootLoopDefenderState __attribute__((section(".bootLoopDefenderState")));

static const BootLoopDefenderConfiguration_t bootLoopDefenderConfiguration = {
   .failedBootLimit = 3,
   .ticksUntilSuccess = 3000,
   .bootLoaderHeader = BootLoaderImageHeader,
   .state = &bootLoopDefenderState
};

static void UpdateBuildInfo(
   I_DataModel_t *dataModel,
   const ImageHeader_t *header)
{
   static const GitHash_t gitHash = { GIT_HASH_U8_ARRAY_RX };
   DataModel_Write(dataModel, Erd_GitHash, &gitHash);

   DataModel_Write(dataModel, Erd_ParametricShortGitHash, &ParametricData_GetParametricTableOfContents()->shortGitHash);

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

static void UpdateStackUsage(void *context)
{
   REINTERPRET(instance, context, I_DataModel_t *);
   uint8_t stackUsage;

   Input_Read(&stackUsageCalculator.interface, &stackUsage);
   DataModel_Write(instance, Erd_StackPercentFree, &stackUsage);
}

int main(void)
{
   Hardware_InitializeStage1();

   ContextProtector_Protect(ContextProtector_Rx2xx_GetInstance());
   {
      const StackConfiguration_t *stackConfig = StackConfigurator_GetConfiguration();
      INIT_STACK_MEMORY(stackConfig->stackStartAddress, stackConfig->stackSize, stackConfig->stackDirection, stackConfig->pattern, stackConfig->patternSize);
   }
   ContextProtector_Unprotect(ContextProtector_Rx2xx_GetInstance());

   I_Action_t *watchdogKickAction = Action_RxWatchdog_InitWithConfiguration(Action_RxWatchdogConfiguration_2Seconds);
   I_Action_t *resetAction = Action_Rx2xxSystemReset_Init();
   I_Action_t *jumpToBootLoaderAction = Header_GetEnterBootLoaderAction();

   I_Interrupt_t *interrupt = Interrupt_Cmt0_Init();
   TimerModule_t *timerModule = TimerModuleStack_Init(&timerModuleStack, interrupt);

   BootLoopDefender_Init(
      &bootLoopDefender,
      timerModule,
      &bootLoopDefenderConfiguration);

   TimeSource_Interrupt_t *timeSourceInterrupt = TimeSource_Rockhopper_Init(interrupt);

   EepromStack_Init(watchdogKickAction, timerModule, timeSourceInterrupt);
   PersonalityEepromStack_Init(timerModule);

   AsyncNvMapConfigurations_Init();

   NonVolatileAsyncDataSource_Init(
      &nvAsyncDataSource,
      Crc16Calculator_Table,
      timerModule,
      EepromStack_GetEeprom(),
      PersonalityEepromStack_GetEeprom(),
      EepromPartitionConfiguration_Init(),
      AsyncNvMapConfigurations_GetInputGroup(),
      AsyncNvMapConfigurations_GetAsyncDataSourceResources(),
      AsyncNvMapConfigurations_GetAsyncDataSourceConfiguration());

   ParametricDataVersionCheck_Init(jumpToBootLoaderAction);

   SystemData_Init(
      &systemData,
      timerModule,
      NonVolatileAsyncDataSource_GetAsyncDataSource(&nvAsyncDataSource),
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

   EepromMonitorPlugin_Init(dataModel);

   ParametricDataErds_Init(
      dataModel,
      Erd_ParametricDataImageCrc,
      Erd_AppliancePersonality,
      Erd_PersonalityParametricData,
      Erd_PersonalityIdOutOfRangeFlag);

   // The BSP ERDs are parametrically mapped, so they need to be initialized after Parametric ERDs are established.
   SystemData_AddBspDataSource(
      &systemData,
      dataModel,
      timerModule,
      interrupt);

   RelayWatchdogPlugin_Init(dataModel);

   GeaStack_Init(
      &geaStack,
      dataModel,
      SystemData_ExternalDataSource(&systemData),
      Gea2Address_Mainboard);

   BoardsInSystemPlugin_Init(
      &boardsInSystemPlugin,
      dataModel);

   Application_Init(
      &application,
      dataModel,
      Rx2xxResetSource_GetResetReason());

   Input_StackUsageCalculator_Init(&stackUsageCalculator, StackConfigurator_GetConfiguration());

   TimerModule_StartPeriodic(
      timerModule,
      &stackUsageUpdateTimer,
      StackUsageUpdatePeriodInMsec,
      UpdateStackUsage,
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

   UlTestsPlugin_Init(
      &ulTestsPlugin,
      timerModule,
      resetAction,
      Crc16Calculator_Rx2xx_Init(ContextProtector_Rx2xx_GetInstance()),
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

      DataSource_Gpio_Run();

      if(!TimerModule_Run(timerModule))
      {
         Interrupts_WaitForInterrupt();
      }
   }
}
