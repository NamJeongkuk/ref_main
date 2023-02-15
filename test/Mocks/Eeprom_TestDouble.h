/*!
 * @file
 * @brief Test double implementation of an EEPROM
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef EEPROM_TESTDOUBLE_H
#define EEPROM_TESTDOUBLE_H

extern "C"
{
#include "I_Eeprom.h"
#include "Event_Synchronous.h"
}

typedef struct
{
   I_Eeprom_t interface;
   struct
   {
      Event_Synchronous_t OnWrittenEvent;
      Event_Synchronous_t OnReadEvent;
      Event_Synchronous_t OnErasedEvent;
   } _private;
} Eeprom_TestDouble_t;

void Eeprom_TestDouble_Init(Eeprom_TestDouble_t *instance);

void Eeprom_TestDouble_TriggerReceiveWritenEvent(Eeprom_TestDouble_t *instance);

void Eeprom_TestDouble_TriggerReceiveReadEvent(Eeprom_TestDouble_t *instance);

void Eeprom_TestDouble_TriggerReceiveErasedEvent(Eeprom_TestDouble_t *instance);

#endif
