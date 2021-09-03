/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>

#include "Hardware.h"
#include "Action_Rx62tReset.h"
#include "BufferedUart_Uart0.h"
#include "BufferedUart_Null.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "Dtc.h"
#include "Header.h"
#include "I_Interrupt.h"
#include "Clock.h"
#include "SystemErds.h"
#include "Interrupts.h"

void Hardware_InitializeStage1(void)
{
   Clock_Init();
   Interrupts_Enable();
}

void Hardware_InitializeStage2(I_DataModel_t *dataModel)
{
   Dtc_Init();

   I_BufferedUart_t *internalUart = BufferedUart_Null_GetInstance();
   DataModelErdPointerAccess_Write(dataModel, Erd_InternalBufferedUart, internalUart);

   I_BufferedUart_t *externalUart = BufferedUart_Uart0_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_ExternalBufferedUart, externalUart);

   I_Crc16Calculator_t *crcCalcTable = Crc16Calculator_Table;
   DataModelErdPointerAccess_Write(dataModel, Erd_CrcCalcTable, crcCalcTable);

   DataModelErdPointerAccess_Write(dataModel, Erd_JumpToBootLoaderAction, Header_GetEnterBootLoaderAction());
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemResetAction, Action_Rx62tReset_Init());
}
