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
#include "Uart_Channel12.h"
#include "BufferedUart_Channel1.h"
#include "BufferedUart_Channel5.h"
#include "BufferedUart_Channel6.h"
#include "BufferedUart_Channel9.h"
#include "SystemClock.h"
#include "Dtc.h"
#include "r_dataflash.h"
#include "HardwareTimer.h"

void Hardware_InitializeStage1(void)
{
   SystemClock_Init();
   __builtin_rx_setpsw('I');
}

void Hardware_InitializeStage2(I_DataModel_t *dataModel)
{
   R_DF_Enable_DataFlashAccess(); // This is here for the boot-loader parametric items stored in data flash to be able to be read.

   Dtc_Init();

   I_BufferedUart_t *bufferedUartA = BufferedUart_Channel1_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_BufferedUartA, bufferedUartA);

   I_BufferedUart_t *bufferedUartC = BufferedUart_Channel5_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_BufferedUartC, bufferedUartC);

   I_BufferedUart_t *bufferedUartD = BufferedUart_Channel9_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_BufferedUartD, bufferedUartD);

   I_BufferedUart_t *bufferedUartE = BufferedUart_Channel6_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_BufferedUartE, bufferedUartE);

   I_Uart_t *gea2Uart = Uart_Channel12_GetInstance(U32_PCLKB);
   DataModelErdPointerAccess_Write(dataModel, Erd_Gea2Uart, gea2Uart);

   I_Crc16Calculator_t *crcCalcTable = Crc16Calculator_Table;
   DataModelErdPointerAccess_Write(dataModel, Erd_CrcCalcTable, crcCalcTable);

   DataModelErdPointerAccess_Write(dataModel, Erd_JumpToBootLoaderAction, Header_GetEnterBootLoaderAction());
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemResetAction, Action_Rx2xxSystemReset_Init());
}
