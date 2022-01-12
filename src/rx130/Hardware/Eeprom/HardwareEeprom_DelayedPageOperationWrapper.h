/*!
 * @file
 * @brief Eeprom Wrapper that slices requested Read/Write bytes in units of pages
 * and provides delays between Read/Write operations using the timer module.
 * It also retries page read/write operations if they fail
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef HARDWAREEEPROM_DELAYEDPAGEOPERATIONWRAPPER_H
#define HARDWAREEEPROM_DELAYEDPAGEOPERATIONWRAPPER_H

#include "I_HardwareEeprom.h"
#include "I_Action.h"
#include "Timer.h"
#include "Fsm.h"
#include "Event_SafeSynchronous.h"

#define EEPROM_WRITE_ACCESS_DELAY_MSEC (TimerTicks_t)(5)
#define EEPROM_READ_ACCESS_DELAY_MSEC (TimerTicks_t)(1)

typedef struct
{
   uint16_t offset;
   uint16_t numBytes;
   union
   {
      uint8_t *readBuffer;
      const uint8_t *writeBuffer;
   };
} OperationInfo_t;

typedef struct
{
   I_HardwareEeprom_t interface;

   struct
   {
      Fsm_t fsm;
      I_HardwareEeprom_t *eeprom;
      TimerModule_t *timerModule;
      Timer_t delayTimer;
      OperationInfo_t operationInfo;
      uint16_t endOffset;
      uint8_t tries;
      Event_SafeSynchronous_t readCompleteEvent;
      Event_SafeSynchronous_t writeCompleteEvent;
      Event_SafeSynchronous_t eraseCompleteEvent;
      HardwareEepromErrorSource_t lastErrorSource;

      struct
      {
         EventSubscription_t eepromRead;
         EventSubscription_t eepromWritten;
         EventSubscription_t eepromErased;
      } subscription;
   } _private;
} HardwareEeprom_DelayedPageOperationWrapper_t;

/*!
 * @param instance
 * @param eeprom
 * @param timerModule
 */
void HardwareEeprom_DelayedPageOperationWrapper_Init(
   HardwareEeprom_DelayedPageOperationWrapper_t *instance,
   I_HardwareEeprom_t *eeprom,
   TimerModule_t *timerModule);

#endif
