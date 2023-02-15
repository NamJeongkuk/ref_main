/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EEPROMERASER_H
#define EEPROMERASER_H

#include "I_DataModel.h"
#include "I_Eeprom.h"
#include "EventSubscription.h"
#include "I_Action.h"

typedef struct
{
   struct
   {
      EventSubscription_t eraseSignalErdSubscription;
      EventSubscription_t eepromOnEraseSubscription;
      I_Action_t *resetAction;
   } _private;
} EepromEraser_t;

/*!
 * Initialize eeprom eraser
 * @param instance
 * @param dataModel
 * @param eeprom
 * @param eepromEraseRequestSignalErd
 * @param resetActionErd
 */
void EepromEraser_Init(
   EepromEraser_t *instance,
   I_DataModel_t *dataModel,
   I_Eeprom_t *eeprom,
   Erd_t eepromEraseRequestSignalErd,
   Erd_t resetActionErd);

#endif
