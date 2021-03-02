/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>

#include "Hardware.h"
#include "Gpio_Ra.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "Header.h"
#include "I_Interrupt.h"
#include "SystemErds.h"
#include "BufferedUart_Sci0.h"
#include "BufferedUart_Sci7.h"
#include "Action_RaSystemReset.h"

void Hardware_InitializeStage1(void)
{
   Gpio_Ra_Init();
}

void Hardware_InitializeStage2(I_DataModel_t *dataModel)
{
   I_BufferedUart_t *internalUart = BufferedUart_Sci7_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_InternalBufferedUart, internalUart);

   I_BufferedUart_t *externalUart = BufferedUart_Sci0_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_ExternalBufferedUart, externalUart);

   I_Crc16Calculator_t *crcCalcTable = Crc16Calculator_Table;
   DataModelErdPointerAccess_Write(dataModel, Erd_CrcCalcTable, crcCalcTable);

   DataModelErdPointerAccess_Write(dataModel, Erd_JumpToBootLoaderAction, Header_GetEnterBootLoaderAction());
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemResetAction, Action_RaSystemReset_Init());
}
