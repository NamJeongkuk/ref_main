/*!
 * @file
 * @brief AT21CS11 One-wire Eeprom driver
 * Adapts the one wire low level driver into an async Eeprom driver
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EEPROM_AT21CS11_H
#define EEPROM_AT21CS11_H

#include "I_DataSource.h"
#include "I_HardwareEeprom.h"
#include "I_OneWire.h"
#include "Timer.h"
#include "Fsm.h"
#include "Event_Synchronous.h"

typedef struct
{
   I_HardwareEeprom_t interface;
   
   struct
   {
      Event_Synchronous_t OnEraseEvent;
      Event_Synchronous_t OnWrittenEvent;
      Event_Synchronous_t OnReadEvent;
      const uint8_t *writeData;
      uint8_t *readData;
      uint8_t dataSize;
      uint8_t offset;
      Fsm_t fsm;
      TimerModule_t *timerModule_1ms;
      Timer_t timer;
      I_OneWire_t *at21cs11;
      bool busy;
      bool devicePresent;
   } _private;
} Eeprom_AT21CS11_t;

/*!
 * @param instance
 * @param timerModule
 * @param oneWire
 */
void Eeprom_AT21CS11_Init(
   Eeprom_AT21CS11_t *instance,
   TimerModule_t *timerModule,
   I_OneWire_t *oneWire);

uint8_t Eeprom_AT21CS11_GetUniqueId(Eeprom_AT21CS11_t *instance);

I_Eeprom_t *Eeprom_AT21CS11_GetInterface(Eeprom_AT21CS11_t *instance);

#endif
