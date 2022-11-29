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
#include "Uart_Channel12.h"
#include "BufferedUart_Channel1.h"
#include "BufferedUart_Channel5.h"
#include "BufferedUart_Channel6.h"
#include "BufferedUart_Channel9.h"
#include "SystemClock.h"
#include "Dtc.h"

void Hardware_InitializeStage1(void)
{
   SystemClock_Init();
   __builtin_rx_setpsw('I');
}

void Hardware_InitializeStage2(I_DataModel_t *dataModel)
{
#ifdef OLD_HW
   I_Uart_t *internalUart = Uart_Channel6_GetInstance(U32_PCLKB);
   DataModelErdPointerAccess_Write(dataModel, Erd_InternalUart, internalUart);

   I_Uart_t *externalUart = Uart_Channel0_GetInstance(U32_PCLKB);
   DataModelErdPointerAccess_Write(dataModel, Erd_ExternalUart, externalUart);
#else
   Dtc_Init();

   I_BufferedUart_t *wifiBufferedUart = BufferedUart_Channel1_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_WifiBufferedUart, wifiBufferedUart);

   I_BufferedUart_t *factoryBufferedUart = BufferedUart_Channel5_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_FactoryBufferedUart, factoryBufferedUart);

   I_BufferedUart_t *doorBufferedUart = BufferedUart_Channel6_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_DoorBufferedUart, doorBufferedUart);

   I_BufferedUart_t *caseBufferedUart = BufferedUart_Channel9_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_CaseBufferedUart, caseBufferedUart);

   I_Uart_t *gea2Uart = Uart_Channel12_GetInstance(U32_PCLKB);
   DataModelErdPointerAccess_Write(dataModel, Erd_Gea2Uart, gea2Uart);
#endif

   I_Crc16Calculator_t *crcCalcTable = Crc16Calculator_Table;
   DataModelErdPointerAccess_Write(dataModel, Erd_CrcCalcTable, crcCalcTable);

   DataModelErdPointerAccess_Write(dataModel, Erd_JumpToBootLoaderAction, Header_GetEnterBootLoaderAction());
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemResetAction, Action_Rx2xxSystemReset_Init());
}