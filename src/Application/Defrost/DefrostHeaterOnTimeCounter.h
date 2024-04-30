/*!
 * @file
 * @brief Counts the amount of time a defrost heater has been on continuously
 * in the Heater On defrost state
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTHEATERONTIMECOUNTER_H
#define DEFROSTHEATERONTIMECOUNTER_H

#include "I_DataModel.h"
#include "Timer.h"

typedef struct
{
   Erd_t defrostHeaterOnTimeErd; // uint8_t
   Erd_t defrostHeaterStateErd; // HeaterVotedState_t
   Erd_t defrostStateErd; // DefrostState_t
} DefrostHeaterOnTimeCounterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t periodicTimer;
      EventSubscription_t dataModelSubscription;
      const DefrostHeaterOnTimeCounterConfig_t *config;
   } _private;
} DefrostHeaterOnTimeCounter_t;

void DefrostHeaterOnTimeCounter_Init(
   DefrostHeaterOnTimeCounter_t *instance,
   I_DataModel_t *dataModel,
   const DefrostHeaterOnTimeCounterConfig_t *config);

#endif
