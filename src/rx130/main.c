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
#include "HardwareEeprom_I2c.h"

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
   Gea2Address_DoorBoard
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

   ContextProtector_Protect(ContextProtector_Rx2xx_GetInstance());
   {
      const StackConfiguration_t *stackConfig = StackConfigurator_GetConfiguration();
      INIT_STACK_MEMORY(stackConfig->stackStartAddress, stackConfig->stackSize, stackConfig->stackDirection, stackConfig->pattern, stackConfig->patternSize);
   }
   ContextProtector_Unprotect(ContextProtector_Rx2xx_GetInstance());

   I_Action_t *watchdogKickAction = Action_RxWatchdog_InitWithConfiguration(Action_RxWatchdogConfiguration_2Seconds);
   I_Action_t *resetAction = Action_Rx2xxSystemReset_Init();
   TimerModule_t *timerModule = TimerModuleStack_Init(&timerModuleStack, Interrupt_Cmt0_Init());

   EepromStack_Init(watchdogKickAction, timerModule);

   SystemData_Init(
      &systemData,
      timerModule,
      EepromStack_GetEeprom(),
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
      Gea2Address_Mainboard,
      staticRoutingTable,
      ELEMENT_COUNT(staticRoutingTable));

   Application_Init(
      &application,
      dataModel,
      StackConfigurator_GetConfiguration(),
      Rx2xxResetSource_GetResetReason());

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

   // init hardware eeprom i2c
   // volatile uint8_t counter = 0;
   // HardwareEeprom_I2c_t *hardwareEepromI2c;
   // hardwareEepromI2c = HardwareEeprom_I2c_Init(watchdogKickAction);
   // // uint8_t writeBuffer[] = {0xA5};
   // uint8_t writeBuffer[] = { 0xA5, 0xB5, 0xC3, 0xB8 };
   // // // hardwareeeprom_write(..., writeBuffer, sizeof(writeBuffer))
   // // HardwareEeprom_Write(&hardwareEepromI2c->interface, 12, 4, writeBuffer);
   // // uint8_t readBuffer[sizeof(writeBuffer)];
   // uint8_t readBuffer[sizeof(writeBuffer)];
   // // hardwareeeprom_read(..., readBuffer, sizeof(readBuffer))
   // HardwareEeprom_Read(&hardwareEepromI2c->interface, 12, 4, readBuffer);
   // // check whether what you wrote is what you read back
   // if(readBuffer[0] == writeBuffer[0])
   // {
   //    counter += 1;
   // }

   while(1)
   {
      GeaStack_Run(&geaStack);

      if(!TimerModule_Run(timerModule))
      {
         Interrupts_WaitForInterrupt();
      }
   }
}
