/*!
 * @file
 * @brief The module to read and write Eeprom data using I2C.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef HARDWAREEEPROM_I2C_H
#define HARDWAREEEPROM_I2C_H

#include "I_HardwareEeprom.h"
#include "I_Action.h"
#include "Event_SafeSynchronous.h"

#define HardwareEeprom_I2c_Size_In_Bytes          (16384)
#define HardwareEeprom_I2c_Page_Size_In_Bytes     (64)

typedef struct
{
   I_HardwareEeprom_t interface;

   struct
   {
      uint8_t receiveDataCount;
      uint8_t totalReadBytes;
      uint8_t* readBuffer;
      I_Action_t *watchdogKickAction;
      Event_SafeSynchronous_t readCompleteEvent;
      Event_SafeSynchronous_t writeCompleteEvent;
      Event_SafeSynchronous_t eraseCompleteEvent;
   }_private;
} HardwareEeprom_I2c_t;

/*!
 * Initialize an Eeprom I2c instance
 * @param Watchdog refresh action
 * @return Eeprom I2c instance pointer
 */
HardwareEeprom_I2c_t *HardwareEeprom_I2c_Init(I_Action_t* watchdogKickAction);

#endif
