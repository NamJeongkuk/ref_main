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
#include "Header.h"
#include "I_Interrupt.h"
#include "SystemClock.h"
#include "SystemErds.h"

void Hardware_InitializeStage1(void)
{
   __builtin_rx_setpsw('I');
   SystemClock_Init();
}

void Hardware_InitializeStage2(I_DataModel_t *dataModel)
{
   Dtc_Init();

   I_BufferedUart_t *internalUart = BufferedUart_Channel5_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_InternalBufferedUart, internalUart);

   I_BufferedUart_t *externalUart = BufferedUart_Channel6_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_ExternalBufferedUart, externalUart);

   I_Crc16Calculator_t *crcCalcTable = Crc16Calculator_Table;
   DataModelErdPointerAccess_Write(dataModel, Erd_CrcCalcTable, crcCalcTable);

   DataModelErdPointerAccess_Write(dataModel, Erd_JumpToBootLoaderAction, Header_GetEnterBootLoaderAction());
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemResetAction, Action_Rx2xxSystemReset_Init());
}
