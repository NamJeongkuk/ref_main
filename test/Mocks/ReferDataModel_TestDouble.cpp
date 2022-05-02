/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DataModelErdPointerAccess.h"
#include "Action_Null.h"
#include "Action_Context.h"
#include "SystemData.h"
}

#include "ReferDataModel_TestDouble.h"

static void RunTimerModule(void *context)
{
   REINTERPRET(timerModule, context, TimerModule_TestDouble_t *);
   TimerModule_TestDouble_ElapseTime(timerModule, 1);
}

void ReferDataModel_TestDouble_Init(ReferDataModel_TestDouble_t *instance)
{
   TimerModule_TestDouble_Init(&instance->_private.timerModuleTestDouble);
   Action_Context_Init(&instance->_private.runTimerModuleAction, &instance->_private.timerModuleTestDouble.timerModule, RunTimerModule);

   AsyncDataSource_Eeprom_TestDouble_Init(
      &instance->_private.asyncEepromTestDouble,
      &instance->_private.timerModuleTestDouble.timerModule,
      Crc16Calculator_Table);

   SystemData_Init(
      &instance->_private.systemData,
      &instance->_private.timerModuleTestDouble.timerModule,
      AsyncDataSource_Eeprom_TestDouble_GetAsyncDataSource(&instance->_private.asyncEepromTestDouble),
      Crc16Calculator_Table,
      &instance->_private.runTimerModuleAction.interface,
      Action_Null_GetInstance());

   instance->dataModel = SystemData_DataModel(&instance->_private.systemData);
   instance->externalDataSource = SystemData_ExternalDataSource(&instance->_private.systemData);

   DataModelErdPointerAccess_Write(instance->dataModel, Erd_TimerModule, &instance->_private.timerModuleTestDouble.timerModule);
}

TimerModule_TestDouble_t *ReferDataModel_TestDouble_GetTimerModuleTestDouble(ReferDataModel_TestDouble_t *instance)
{
   return &instance->_private.timerModuleTestDouble;
}
