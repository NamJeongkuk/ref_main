/*!
 * @file
 * @brief Count how long the system has met (stayed in) the prechill condition, defined as:
 * - Compressor is on
 * - Single/Dual evap: cooling mode is freezer
 * - Triple evap: cooling mode is freezer (convertible compartment is acting as a fresh food) or
 * cooling mode is convertible compartment (convertible compartment is acting as a freezer)
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TIMETHATPRECHILLCONDITIONSAREMET_H
#define TIMETHATPRECHILLCONDITIONSAREMET_H

#include "Timer.h"
#include "Fsm.h"
#include "I_DataModel.h"
#include "PlatformData.h"

typedef struct
{
   Erd_t compressorIsOnErd; // bool
   Erd_t coolingModeErd; // CoolingMode_t
   Erd_t timeThatPrechillConditionsAreMetInMinutesErd; // uint32_t
   Erd_t convertibleCompartmentStateErd; // ConvertibleCompartmentStateType_t
   Erd_t timerModuleErd; // TimerModule_t *
} TimeThatPrechillConditionsAreMetConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Fsm_t fsm;
      Timer_t periodicTimer;
      EventSubscription_t dataModelSubscription;
      const TimeThatPrechillConditionsAreMetConfiguration_t *config;
      uint8_t numberOfEvaporators;
   } _private;
} TimeThatPrechillConditionsAreMet_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void TimeThatPrechillConditionsAreMet_Init(
   TimeThatPrechillConditionsAreMet_t *instance,
   I_DataModel_t *dataModel,
   const TimeThatPrechillConditionsAreMetConfiguration_t *config);

#endif
