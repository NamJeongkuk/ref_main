/*!
 * @file
 * @brief The hardware initialization implementation.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>

#include "Hardware.h"
#include "Action_Rx2xxSystemReset.h"
#include "BufferedUart_Channel5.h"
#include "BufferedUart_Channel6.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "Dtc.h"
#include "PlatformGpio.h"
#include "Header.h"
#include "I_Interrupt.h"
#include "InitializeStackMemory.h"
#include "StackConfigurator.h"
#include "SystemClock.h"
#include "SystemErds.h"

static void InitializeStack(void)
{
   StackConfiguration_t stackConfiguration;
   uint8_t *stackStartAddress;
   uint32_t stackSize;
   StackDirection_t stackDirection;
   const uint8_t *pattern;
   uint8_t patternSize;

   StackConfigurator_GetConfiguration(&stackConfiguration);
   stackStartAddress = stackConfiguration.stackStartAddress;
   stackSize = stackConfiguration.stackSize;
   stackDirection = stackConfiguration.stackDirection;
   pattern = stackConfiguration.pattern;
   patternSize = stackConfiguration.patternSize;

   INIT_STACK_MEMORY(stackStartAddress, stackSize, stackDirection, pattern, patternSize);
}

void Hardware_InitializeStage1(void)
{
   SystemClock_Init();
   InitializeStack();

   __builtin_rx_setpsw('I');
}

void Hardware_InitializeStage2(I_DataModel_t *dataModel)
{
   Dtc_Init();

   PlatformGpio_Init();

   I_BufferedUart_t *internalUart = BufferedUart_Channel5_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_InternalBufferedUart, internalUart);

   I_BufferedUart_t *externalUart = BufferedUart_Channel6_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_ExternalBufferedUart, externalUart);

   I_Crc16Calculator_t *crcCalcTable = Crc16Calculator_Table;
   DataModelErdPointerAccess_Write(dataModel, Erd_CrcCalcTable, crcCalcTable);

   DataModelErdPointerAccess_Write(dataModel, Erd_JumpToBootLoaderAction, Header_GetEnterBootLoaderAction());
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemResetAction, Action_Rx2xxSystemReset_Init());
}
