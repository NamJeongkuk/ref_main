/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "Hardware.h"
#include "Clock.h"
#include "ContextProtector_Psoc6.h"
#include "Crc16Calculator_Table.h"
#include "DataModelErdPointerAccess.h"
#include "Header.h"
#include "I_Interrupt.h"
#include "SystemErds.h"
#include "Uart_Scb2.h"
#include "BufferedUart_Scb0.h"
#include "BufferedUart_Scb5.h"
#include "Action_Psoc6SystemReset.h"
#include "cy_pdl.h"

void Hardware_InitializeStage1(void)
{
   Clock_Init();

   __enable_irq();
}

void Hardware_InitializeStage2(I_DataModel_t *dataModel)
{
   I_Uart_t *singleWireUart = Uart_Scb2_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_SingleWireUart, singleWireUart);

   I_BufferedUart_t *internalUart = BufferedUart_Scb0_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_InternalBufferedUart, internalUart);

   I_BufferedUart_t *externalUart = BufferedUart_Scb5_Init();
   DataModelErdPointerAccess_Write(dataModel, Erd_ExternalBufferedUart, externalUart);

   I_ContextProtector_t *contextProtector = ContextProtector_Psoc6_GetInstance();
   DataModelErdPointerAccess_Write(dataModel, Erd_ContextProtector, contextProtector);

   I_Crc16Calculator_t *crcCalcTable = Crc16Calculator_Table;
   DataModelErdPointerAccess_Write(dataModel, Erd_CrcCalcTable, crcCalcTable);

   DataModelErdPointerAccess_Write(dataModel, Erd_JumpToBootLoaderAction, Header_GetEnterBootLoaderAction());
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemResetAction, Action_Psoc6SystemReset_Init());
}
