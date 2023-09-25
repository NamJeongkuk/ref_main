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
#include "Interrupt_TestDouble.h"
#include "GpioGroup_TestDouble.h"
#include "Gea2MessageEndpoint_TestDouble.h"

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
      Interrupt_TestDouble_t systemTickInterruptTestDouble;
      Gea2MessageEndpoint_TestDouble_t messageEndpointTestDouble;
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
 * @brief Refer DataModel TestDouble
 * @param instance of refer datamodel testdouble
 * @param instance of action to invoke on reset
 * @param instance of personality id for test. Matching Lua file will be loaded into personality parametric ERD
 */
void ReferDataModel_TestDouble_Init(ReferDataModel_TestDouble_t *instance, I_Action_t *resetAction, PersonalityId_t personalityIdForTest);

/*!
 * @brief TimerModule TestDouble
 * @param instance of timermodule testdouble
 * @return pointer to the TimerModule TestDouble
 */
TimerModule_TestDouble_t *ReferDataModel_TestDouble_GetTimerModuleTestDouble(ReferDataModel_TestDouble_t *instance);

/*!
 * @brief Simulate a reset - resets everything except for NV ERDs
 * @param instance of refer datamodel testdouble
 * @param instance of personality id for test. Matching Lua file will be loaded into personality parametric ERD
 */
void ReferDataModel_TestDouble_Reset(ReferDataModel_TestDouble_t *instance);

/*!
 * @brief Simulate a reset - resets everything except for NV ERDs
 * @param instance of refer datamodel testdouble
 * @param instance of personality id for test. Matching Lua file will be loaded into personality parametric ERD
 */
void ReferDataModel_TestDouble_Reset(ReferDataModel_TestDouble_t *instance, PersonalityId_t personalityIdForTest);

#endif
