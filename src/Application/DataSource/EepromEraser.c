/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "EepromEraser.h"
#include "utils.h"

static void OnEepromEraseSignalChanged(void *context, const void *_args)
{
   REINTERPRET(eeprom, context, I_Eeprom_t *);
   IGNORE(_args);

   Eeprom_Erase(eeprom);
}

void EepromEraser_Init(
   EepromEraser_t *instance,
   I_DataModel_t *dataModel,
   I_Eeprom_t *eeprom,
   Erd_t eepromEraseRequestSignalErd)
{
   EventSubscription_Init(
      &instance->_private.eraseSignalErdSubscription,
      eeprom,
      OnEepromEraseSignalChanged);

   DataModel_Subscribe(
      dataModel,
      eepromEraseRequestSignalErd,
      &instance->_private.eraseSignalErdSubscription);
}
