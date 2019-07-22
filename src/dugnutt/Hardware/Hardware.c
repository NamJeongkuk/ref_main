/*!
 * @file
 * @brief The hardware initialization implementation.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "Hardware.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "I_Interrupt.h"
#include "GpioGroup_Rx130.h"
#include "Dtc.h"
#include "BufferedUart_Channel5.h"
#include "Crc16Calculator_Table.h"
#include "Header.h"
#include "Action_Rx2xxSystemReset.h"

void Hardware_Init(I_DataModel_t *internalDataModel)
{
   I_GpioGroup_t *gpioGroup = GpioGroup_Init();
   DataModelErdPointerAccess_Write(internalDataModel, Erd_GpioGroup, gpioGroup);

   Dtc_Init();

   I_BufferedUart_t *uart = BufferedUart_Channel5_Init();
   DataModelErdPointerAccess_Write(internalDataModel, Erd_BufferedUart, uart);

   I_Crc16Calculator_t *crcCalcTable = Crc16Calculator_Table;
   DataModelErdPointerAccess_Write(internalDataModel, Erd_CrcCalcTable, crcCalcTable);

   DataModelErdPointerAccess_Write(internalDataModel, Erd_JumpToBootLoaderAction, Header_GetEnterBootLoaderAction());
   DataModelErdPointerAccess_Write(internalDataModel, Erd_SystemResetAction, Action_Rx2xxSystemReset_Init());
}
