/*!
 * @file
 * @brief Implementation of an asynchronous HardwareEeprom interface that will access a MicroChip EEPROM 24XX128
 * via I2C
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef HARDWAREEEPROM_I2cMT24XXMMM_H
#define HARDWAREEEPROM_I2cMT24XXMMM_H

#include <stdbool.h>
#include "Event_Synchronous.h"
#include "Fsm.h"
#include "I_ContextProtector.h"
#include "I_HardwareEeprom.h"
#include "I_I2c.h"
#include "Timer.h"

typedef struct
{
   uint8_t peripheralAddress_Write;
   uint8_t peripheralAddress_Read;
   uint32_t numberOfStorageBytes;
   uint8_t bytesPerPage;
   uint8_t byteAlignment;
   uint8_t pollingPeriodMilliseconds;
   uint8_t valueToWriteWhenErasing;
   uint8_t maxTicksAllowedSinceLastResponse;
   uint8_t maxObjectErrorsAllowedWhenPolling;
   uint8_t i2cBusAccessTimeOut;
   uint8_t coolDownTimeOutAfterTransactionCompletes;
} HardwareEeprom_I2cMt24xxmmmConfiguration_t;

typedef struct
{
   bool coolDownProcessInProgress;
   bool requestPending;
   HardwareEepromMode_t requestedMode;
   uint32_t offset;
   uint32_t numBytes;
   union
   {
      uint8_t *readBuffer;
      const uint8_t *writeBuffer;
   };
} HardwareEeprom_I2cRequestBuffer_t;

typedef struct
{
   I_HardwareEeprom_t interface;

   struct
   {
      Fsm_t fsm;

      Event_Synchronous_t onReadEvent;
      Event_Synchronous_t onWrittenEvent;
      Event_Synchronous_t onErasedEvent;
      I_I2c_t *i2c;
      const HardwareEeprom_I2cMt24xxmmmConfiguration_t *configuration;
      TimerModule_t *timerModule;
      I_ContextProtector_t *contextProtector;
      struct
      {
         uint32_t offset;
         uint32_t numBytes;
         union
         {
            uint8_t *readBuffer;
            const uint8_t *writeBuffer;
         };
      } holdParameters;
      Timer_t tickTimer;
      Timer_t busAccessTimer;
      uint32_t byteIndex;
      uint32_t currentAddress;
      uint8_t ticksSinceLastResponse;
      uint8_t numberOfI2cObjectErrors;
      uint8_t currentMode;
      HardwareEeprom_I2cRequestBuffer_t requestQueueBuffer;
      bool timerRestartRequested;
      bool i2cBusAcquired;
   } _private;
} HardwareEeprom_I2cMt24xxmmm_t;

/*!
 *
 * @param instance This object
 * @param i2c The I2C to use to communicate with the device
 * @param timerModule
 */
void HardwareEeprom_I2cMt24xxmmm_Init(
   HardwareEeprom_I2cMt24xxmmm_t *instance,
   I_I2c_t *i2c,
   const HardwareEeprom_I2cMt24xxmmmConfiguration_t *configuration,
   TimerModule_t *timerModule,
   I_ContextProtector_t *contextProtector);

#endif
