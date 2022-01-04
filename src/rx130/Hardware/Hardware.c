/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>

#include "Hardware.h"
#include "Action_Rx2xxSystemReset.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "Header.h"
#include "I_Interrupt.h"
#include "SystemClock.h"
#include "SystemErds.h"
#include "Uart_Channel0.h"
#include "Uart_Channel6.h"
#include "SystemClock.h"

void Hardware_InitializeStage1(void)
{
   SystemClock_Init();
   __builtin_rx_setpsw('I');
}

void Hardware_InitializeStage2(I_DataModel_t *dataModel)
{
   I_Uart_t *internalUart = Uart_Channel6_GetInstance(U32_PCLKB);
   DataModelErdPointerAccess_Write(dataModel, Erd_InternalUart, internalUart);

   I_Uart_t *externalUart = Uart_Channel0_GetInstance(U32_PCLKB);
   DataModelErdPointerAccess_Write(dataModel, Erd_ExternalUart, externalUart);

   I_Crc16Calculator_t *crcCalcTable = Crc16Calculator_Table;
   DataModelErdPointerAccess_Write(dataModel, Erd_CrcCalcTable, crcCalcTable);

   DataModelErdPointerAccess_Write(dataModel, Erd_JumpToBootLoaderAction, Header_GetEnterBootLoaderAction());
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemResetAction, Action_Rx2xxSystemReset_Init());
}
