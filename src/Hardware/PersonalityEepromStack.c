/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PersonalityEepromStack.h"
#include "Eeprom_AT21CS11.h"
#include "OneWire_AT21CS11.h"
#include "HardwareTimer.h"
#include "DataSourceErdPointerAccess.h"

static Eeprom_AT21CS11_t eeprom;

void PersonalityEepromStack_Init(TimerModule_t *timerModule)
{
   HardwareTimer_Init();
   Eeprom_AT21CS11_Init(&eeprom, timerModule, OneWire_AT21CS11_Init());
}

I_Eeprom_t *PersonalityEepromStack_GetEeprom(void)
{
   return &eeprom.interface;
}
