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
#include "Eeprom_AT21CS11.h"
}

#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricDataTestDouble.h"
#include "TddPersonality.h"

enum
{
   DefaultPersonalityForTest = TddPersonality_DevelopmentSingleEvaporator
};

static void RunTimerModule(void *context)
{
   REINTERPRET(timerModule, context, TimerModule_TestDouble_t *);
   TimerModule_TestDouble_ElapseTime(timerModule, 1);
}

static void Init(ReferDataModel_TestDouble_t *instance, PersonalityId_t personalityIdForTest, bool clearEeprom)
{
   TimerModule_TestDouble_Init(&instance->_private.timerModuleTestDouble);
   Action_Context_Init(&instance->_private.runTimerModuleAction, &instance->_private.timerModuleTestDouble.timerModule, RunTimerModule);

   AsyncDataSource_Eeprom_TestDouble_Init(
      &instance->_private.asyncEepromTestDouble,
      &instance->_private.timerModuleTestDouble.timerModule,
      Crc16Calculator_Table,
      clearEeprom);

   SystemData_Init(
      &instance->_private.systemData,
      &instance->_private.timerModuleTestDouble.timerModule,
      AsyncDataSource_Eeprom_TestDouble_GetAsyncDataSource(&instance->_private.asyncEepromTestDouble),
      Crc16Calculator_Table,
      &instance->_private.runTimerModuleAction.interface,
      Action_Null_GetInstance());

   instance->dataModel = SystemData_DataModel(&instance->_private.systemData);
   instance->externalDataSource = SystemData_ExternalDataSource(&instance->_private.systemData);

   Interrupt_TestDouble_Init(&instance->_private.systemTickInterruptTestDouble);
   DataModelErdPointerAccess_Write(instance->dataModel, Erd_SystemTickInterrupt, &instance->_private.systemTickInterruptTestDouble.interface);

   DataModelErdPointerAccess_Write(instance->dataModel, Erd_TimerModule, &instance->_private.timerModuleTestDouble.timerModule);

   instance->_private.personalityParametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(personalityIdForTest);
   DataModelErdPointerAccess_Write(instance->dataModel, Erd_PersonalityParametricData, instance->_private.personalityParametricData);

   SystemData_AddBspDataSource(
      &instance->_private.systemData,
      instance->dataModel,
      NULL,
      NULL);
}

void ReferDataModel_TestDouble_Init(ReferDataModel_TestDouble_t *instance)
{
   ReferDataModel_TestDouble_Init(instance, DefaultPersonalityForTest);
}

void ReferDataModel_TestDouble_Reset(ReferDataModel_TestDouble_t *instance)
{
   ReferDataModel_TestDouble_Reset(instance, DefaultPersonalityForTest);
}

void ReferDataModel_TestDouble_Init(ReferDataModel_TestDouble_t *instance, PersonalityId_t personalityIdForTest)
{
   Init(instance, personalityIdForTest, true);
}

void ReferDataModel_TestDouble_Reset(ReferDataModel_TestDouble_t *instance, PersonalityId_t personalityIdForTest)
{
   Init(instance, personalityIdForTest, false);
}

TimerModule_TestDouble_t *ReferDataModel_TestDouble_GetTimerModuleTestDouble(ReferDataModel_TestDouble_t *instance)
{
   return &instance->_private.timerModuleTestDouble;
}
