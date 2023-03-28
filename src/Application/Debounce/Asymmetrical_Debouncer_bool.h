/*!
 * @file
 * @brief wrapper for debouncer_bool type that handles asymmetrical debounces for going from off to on, and from on to off
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ASYMMETRICAL_DEBOUNCER_BOOL_H
#define ASYMMETRICAL_DEBOUNCER_BOOL_H

#include "Debouncer_bool.h"

typedef struct
{
   uint8_t debounceCountOffToOn;
   uint8_t debounceCountOnToOff;
} Asymmetrical_Debouncer_bool_Config_t;

typedef struct
{
   struct
   {
      const Asymmetrical_Debouncer_bool_Config_t *config;
      Debouncer_bool_t offToOnDebouncer;
      Debouncer_bool_t onToOffDebouncer;
      bool filteredOutputValue;
   } _private;
} Asymmetrical_Debouncer_bool_t;

bool Asymmetrical_Debouncer_bool_GetDebounced(
   Asymmetrical_Debouncer_bool_t *instance);

bool Asymmetrical_Debouncer_bool_Process(
   Asymmetrical_Debouncer_bool_t *instance,
   bool value);

void Asymmetrical_Debouncer_bool_Init(
   Asymmetrical_Debouncer_bool_t *instance,
   const Asymmetrical_Debouncer_bool_Config_t *config,
   bool initialSeedValue);

#endif
