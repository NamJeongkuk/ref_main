/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef REFERDATAMODEL_TESTDOUBLE_H
#define REFERDATAMODEL_TESTDOUBLE_H

#include "ReferDataModel_TestDouble.h"
#include "I_DataModel.h"
#include "Timer.h"
#include "SystemData.h"
#include "TimerModule_TestDouble.h"
#include "Action_Context.h"
#include "Eeprom_Model.h"
#include "AsyncDataSource_Eeprom_TestDouble.h"
#include "PersonalityId.h"

typedef struct
{
   I_DataModel_t *dataModel;
   I_DataSource_t *externalDataSource;

   struct
   {
      SystemData_t systemData;
      PersonalityParametricData_t *personalityParametricData;
      TimerModule_TestDouble_t timerModuleTestDouble;
      Action_Context_t runTimerModuleAction;
      Action_Context_t resetAction;
      AsyncDataSource_Eeprom_TestDouble_t asyncEepromTestDouble;
   } _private;
} ReferDataModel_TestDouble_t;

/*!
 * @brief Refer DataModel TestDouble
 * Uses default personality for test file
 * @param instance of refer datamodel testdouble
 */
void ReferDataModel_TestDouble_Init(ReferDataModel_TestDouble_t *instance);

/*!
 * @brief Refer DataModel TestDouble
 * @param instance of refer datamodel testdouble
 * @param instance of personality id for test. Matching Lua file will be loaded into personality parametric ERD
 */
void ReferDataModel_TestDouble_Init(ReferDataModel_TestDouble_t *instance, PersonalityId_t personalityIdForTest);

/*!
 * @brief TimerModule TestDouble
 * @param instance of timermodule testdouble
 * @return pointer to the TimerModule TestDouble
 */
TimerModule_TestDouble_t *ReferDataModel_TestDouble_GetTimerModuleTestDouble(ReferDataModel_TestDouble_t *instance);

#endif
