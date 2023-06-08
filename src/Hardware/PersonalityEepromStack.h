/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PERSONALITYEEPROMSTACK_H
#define PERSONALITYEEPROMSTACK_H

#include "Timer.h"
#include "Eeprom_AT21CS11.h"

void PersonalityEepromStack_Init(TimerModule_t *timerModule);

I_Eeprom_t *PersonalityEepromStack_GetEeprom(void);

#endif
