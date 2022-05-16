/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "EepromEraser.h"
#include "utils.h"
#include "DataModelErdPointerAccess.h"

static void OnEepromEraseComplete(void *context, const void *_args)
{
   REINTERPRET(instance, context, EepromEraser_t *);
   IGNORE(_args);

   Action_Invoke(instance->_private.resetAction);
}

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
   Erd_t eepromEraseRequestSignalErd,
   Erd_t resetActionErd)
{
   instance->_private.resetAction = DataModelErdPointerAccess_GetAction(dataModel, resetActionErd);

   EventSubscription_Init(
      &instance->_private.eraseSignalErdSubscription,
      eeprom,
      OnEepromEraseSignalChanged);

   EventSubscription_Init(
      &instance->_private.eepromOnEraseSubscription,
      instance,
      OnEepromEraseComplete);

   DataModel_Subscribe(
      dataModel,
      eepromEraseRequestSignalErd,
      &instance->_private.eraseSignalErdSubscription);

   Event_Subscribe(
      Eeprom_GetOnErasedEvent(eeprom),
      &instance->_private.eepromOnEraseSubscription);
}
