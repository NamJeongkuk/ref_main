/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Asymmetrical_Debouncer_bool.h"
#include "Constants_Binary.h"

bool Asymmetrical_Debouncer_bool_GetDebounced(
   Asymmetrical_Debouncer_bool_t *instance)
{
   return instance->_private.filteredOutputValue;
}

bool Asymmetrical_Debouncer_bool_Process(
   Asymmetrical_Debouncer_bool_t *instance,
   bool value)
{
   bool valueChanged = false;

   if(instance->_private.filteredOutputValue == HIGH)
   {
      if(value == LOW)
      {
         valueChanged = Debouncer_bool_Process(&instance->_private.onToOffDebouncer, value);
      }
      else
      {
         Debouncer_bool_Reset(&instance->_private.onToOffDebouncer, HIGH);
      }
   }
   else
   {
      if(value == HIGH)
      {
         valueChanged = Debouncer_bool_Process(&instance->_private.offToOnDebouncer, value);
      }
      else
      {
         Debouncer_bool_Reset(&instance->_private.offToOnDebouncer, LOW);
      }
   }

   if(valueChanged)
   {
      instance->_private.filteredOutputValue = !instance->_private.filteredOutputValue;
   }
   return valueChanged;
}

void Asymmetrical_Debouncer_bool_Init(
   Asymmetrical_Debouncer_bool_t *instance,
   const Asymmetrical_Debouncer_bool_Config_t *config)
{
   instance->_private.config = config;
   instance->_private.filteredOutputValue = config->initialValue;

   Debouncer_bool_Init(
      &instance->_private.onToOffDebouncer,
      SET,
      instance->_private.config->debounceCountOnToOff);

   Debouncer_bool_Init(
      &instance->_private.offToOnDebouncer,
      CLEAR,
      instance->_private.config->debounceCountOffToOn);
}
