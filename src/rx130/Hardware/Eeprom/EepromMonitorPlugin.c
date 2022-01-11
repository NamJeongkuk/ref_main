/*!
 * @file
 * @brief Implementation of the EepromMonitorPlugin.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "EepromMonitorPlugin.h"
#include "EepromStack.h"
#include "InputOutputConnector.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static struct
{
   InputOutputConnector_t readFaultErdConnector;
   InputOutputConnector_t writeFaultErdConnector;
   InputOutputConnector_t eraseFaultErdConnector;
} instance;

void EepromMonitorPlugin_Init(I_DataModel_t *dataModel)
{
   InputOutputConnector_Init(&instance.readFaultErdConnector);
   InputOutputConnector_Init(&instance.writeFaultErdConnector);
   InputOutputConnector_Init(&instance.eraseFaultErdConnector);

   InputOutputConnector_Connect(
      &instance.readFaultErdConnector,
      EepromStack_GetReadFaultInput(),
      DataModel_GetOutput(dataModel, Erd_Fault_EepromReadFailure),
      sizeof(bool));
   InputOutputConnector_Connect(
      &instance.writeFaultErdConnector,
      EepromStack_GetWriteFaultInput(),
      DataModel_GetOutput(dataModel, Erd_Fault_EepromWriteFailure),
      sizeof(bool));
   InputOutputConnector_Connect(
      &instance.eraseFaultErdConnector,
      EepromStack_GetEraseFaultInput(),
      DataModel_GetOutput(dataModel, Erd_Fault_EepromEraseFailure),
      sizeof(bool));
}
