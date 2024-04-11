/*!
 * @file
 * @brief Implementation of an asynchronous HardwareEeprom interface that will access a MicroChip EEPROM 24XX128
 * via I2C
 *
 * This is a brief description of the bus traffic expected by the MicroChip EEPROM 24XX128
 *
 * Items in <> are transmitted to the chip
 * Items in :: are received from the chip
 *
 * Write a byte
 * <start> <chip select write> :ack: <msb> :ack: <lsb> :ack: <data> :ack: <stop>
 *
 * Write more than one byte
 * <start> <chip select write> :ack: <msb> :ack: <lsb> :ack: <data> :ack: <data> :ack:
 * <data> :ack: <data> :ack: <data> :ack: <data> :ack: <stop>
 *
 * Read a byte
 * <start> <chip select write> :ack: <msb> :ack: <lsb> :ack: <start> <chip select read> :ack: :data: <stop>
 *
 * Read more then one byte
 * <start> <chip select write> :ack: <msb> :ack: <lsb> :ack:
 * <start> <chip select read> :ack: :data: <ack> :data: <ack> :data: <ack> :data: <ack> :data: <ack> :data: <stop>
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stdint.h>
#include "HardwareEeprom_I2cMt24xxmmm.h"
#include "uassert.h"
#include "utils.h"

enum
{
   Signal_InterruptContext_ChipSelectComplete = FSM_USER_SIGNAL_START,
   Signal_InterruptContext_I2CObjectError,
   Signal_InterruptContext_ReadComplete,
   Signal_ReadStart,
   Signal_InterruptContext_StartComplete,
   Signal_InterruptContext_StopComplete,
   Signal_InterruptContext_CancelComplete,
   Signal_Tick,
   Signal_BusAvailable,
   Signal_BusBusy,
   Signal_BusArbitrationTimeout,
   Signal_CoolDownTimeOut,
   Signal_InterruptContext_WriteAddressLSBComplete,
   Signal_InterruptContext_WriteAddressMSBComplete,
   Signal_InterruptContext_WriteComplete,
   Signal_WriteStart,
};

typedef struct
{
   bool publishResponseEvent;
   HardwareEepromMode_t mode; // Defines mode that caused the error.
   HardwareEepromErrorSource_t source; // Detailed information for debug indicating the source of the error. Defined above and by the implementation
} PublishResponseEventData_t;

typedef struct
{
   PublishResponseEventData_t *eventData;
} TickContext_t;

static void State_Idle(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_WaitingForBus(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_ChipSelect(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_Address(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_Writing(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_WriteComplete(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_PollForHardwareWriteComplete(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_Reading(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_ReadComplete(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_CoolDown(Fsm_t *fsm, FsmSignal_t signal, const void *data);

static void OnStartComplete(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_InterruptContext_StartComplete, NULL);
}

static void PublishResponseEvent(
   HardwareEeprom_I2cMt24xxmmm_t *instance,
   HardwareEepromMode_t currentMode,
   HardwareEepromErrorSource_t source)
{
   HardwareEepromEventArgs_t eventArgs = {
      .mode = currentMode,
      .source = source
   };

   switch(currentMode)
   {
      case HardwareEepromMode_Read:
         Event_Synchronous_Publish(&instance->_private.onReadEvent, &eventArgs);
         break;
      case HardwareEepromMode_Write:
         Event_Synchronous_Publish(&instance->_private.onWrittenEvent, &eventArgs);
         break;
      case HardwareEepromMode_Erase:
         Event_Synchronous_Publish(&instance->_private.onErasedEvent, &eventArgs);
         break;
   }
}

static void OnTick(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   PublishResponseEventData_t publishResponseEventData = {
      .publishResponseEvent = false,
      .mode = instance->_private.currentMode,
      .source = HardwareEepromErrorSource_None,
   };

   TickContext_t tickContext = { &publishResponseEventData };

   ContextProtector_Protect(instance->_private.contextProtector);
   Fsm_SendSignal(&instance->_private.fsm, Signal_Tick, &tickContext);
   ContextProtector_Unprotect(instance->_private.contextProtector);

   if(publishResponseEventData.publishResponseEvent)
   {
      PublishResponseEvent(
         instance,
         publishResponseEventData.mode,
         publishResponseEventData.source);
   }
}

static void CheckI2cBusStatus(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);

   if(I2c_Busy(instance->_private.i2c))
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_BusBusy, NULL);
   }
   else
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_BusAvailable, NULL);
   }
}

static void I2cBusArbitrationTimerCallback(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);

   PublishResponseEventData_t publishResponseEventData = {
      .publishResponseEvent = false,
      .mode = instance->_private.currentMode,
      .source = HardwareEepromErrorSource_BusAlwaysBusy,
   };

   Fsm_SendSignal(&instance->_private.fsm, Signal_BusArbitrationTimeout, &publishResponseEventData);
}

static void CoolDownTimerCallback(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);

   Fsm_SendSignal(&instance->_private.fsm, Signal_CoolDownTimeOut, NULL);
}

static void OnStopComplete(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_InterruptContext_StopComplete, NULL);
}

static void OnCancelComplete(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_InterruptContext_CancelComplete, NULL);
}

static void OnWriteComplete(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_InterruptContext_WriteComplete, NULL);
}

static void OnChipSelectComplete(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_InterruptContext_ChipSelectComplete, NULL);
}

static void OnAddressMSBWriteComplete(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_InterruptContext_WriteAddressMSBComplete, NULL);
}

static void OnAddressLSBWriteComplete(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_InterruptContext_WriteAddressLSBComplete, NULL);
}

static void OnReadComplete(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_InterruptContext_ReadComplete, NULL);
}

static void OnErrorOccurred(void *context)
{
   REINTERPRET(instance, context, HardwareEeprom_I2cMt24xxmmm_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_InterruptContext_I2CObjectError, NULL);
}

static bool ThereAreMoreBytesToWrite(HardwareEeprom_I2cMt24xxmmm_t *instance)
{
   return (instance->_private.byteIndex + 1) < instance->_private.holdParameters.numBytes;
}

static bool TheNextByteIsOnADifferentPage(HardwareEeprom_I2cMt24xxmmm_t *instance)
{
   return (((instance->_private.currentAddress + 1) % instance->_private.configuration->bytesPerPage) == 0);
}

static uint8_t GetByteToWrite(HardwareEeprom_I2cMt24xxmmm_t *instance, uint32_t index)
{
   uint8_t returnData = 0;

   switch(instance->_private.currentMode)
   {
      case HardwareEepromMode_Write:
         returnData = instance->_private.holdParameters.writeBuffer[index];
         break;

      case HardwareEepromMode_Erase:
         returnData = instance->_private.configuration->valueToWriteWhenErasing;
         break;
   }
   return returnData;
}

static void ProcessTickSignal(HardwareEeprom_I2cMt24xxmmm_t *instance, PublishResponseEventData_t *publishResponseEventData)
{
   if(++instance->_private.ticksSinceLastResponse >= instance->_private.configuration->maxTicksAllowedSinceLastResponse)
   {
      // It is necessary to publish from Idle so that the client can retry.
      Fsm_Transition(&instance->_private.fsm, State_CoolDown);
      I2c_Cancel(instance->_private.i2c, OnCancelComplete, instance);

      publishResponseEventData->publishResponseEvent = true;
      publishResponseEventData->source = HardwareEepromErrorSource_OperationTimeOut;
   }
   else if(instance->_private.numberOfI2cObjectErrors > 0)
   {
      // It is necessary to publish from Idle so that the client can retry.
      Fsm_Transition(&instance->_private.fsm, State_CoolDown);
      I2c_Cancel(instance->_private.i2c, OnCancelComplete, instance);

      publishResponseEventData->publishResponseEvent = true;
      publishResponseEventData->source = HardwareEepromErrorSource_DeviceError;
   }
}

static void ResetTicksSinceLastResponse(HardwareEeprom_I2cMt24xxmmm_t *instance)
{
   instance->_private.ticksSinceLastResponse = 0;
}

static void ResetNumberOfI2cObjectErrors(HardwareEeprom_I2cMt24xxmmm_t *instance)
{
   instance->_private.numberOfI2cObjectErrors = 0;
}

static void StartTheBusArbitrationTimer(HardwareEeprom_I2cMt24xxmmm_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.busAccessTimer,
      instance->_private.configuration->i2cBusAccessTimeOut,
      I2cBusArbitrationTimerCallback,
      instance);
}

static void StartBusAccessCheckTimer(HardwareEeprom_I2cMt24xxmmm_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.tickTimer,
      0,
      CheckI2cBusStatus,
      instance);
}

static void StartTheTickTimer(HardwareEeprom_I2cMt24xxmmm_t *instance)
{
   TimerModule_StartPeriodic(
      instance->_private.timerModule,
      &instance->_private.tickTimer,
      instance->_private.configuration->pollingPeriodMilliseconds,
      OnTick,
      instance);
}

static void StartTheCoolDownTimer(HardwareEeprom_I2cMt24xxmmm_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.tickTimer,
      instance->_private.configuration->coolDownTimeOutAfterTransactionCompletes,
      CoolDownTimerCallback,
      instance);
}

static void State_Idle(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE_ARG(data);

   switch(signal)
   {
      case FSM_ENTRY:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.tickTimer);
         instance->_private.currentMode = HardwareEepromMode_None;
         break;

      case Signal_ReadStart:
      case Signal_WriteStart:
         Fsm_Transition(&instance->_private.fsm, State_WaitingForBus);
         break;

      case FSM_EXIT:
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

static void State_WaitingForBus(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE_ARG(data);

   switch(signal)
   {
      case FSM_ENTRY:
         StartTheBusArbitrationTimer(instance);
         CheckI2cBusStatus(instance);
         break;

      case Signal_BusAvailable:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.busAccessTimer);
         StartTheTickTimer(instance);
         Fsm_Transition(&instance->_private.fsm, State_ChipSelect);
         break;

      case Signal_BusBusy:
         StartBusAccessCheckTimer(instance);
         break;

      case Signal_BusArbitrationTimeout:
      {
         REINTERPRET(publishResponseEventData, data, const PublishResponseEventData_t *);

         TimerModule_Stop(instance->_private.timerModule, &instance->_private.tickTimer);

         Fsm_Transition(&instance->_private.fsm, State_Idle);

         PublishResponseEvent(
            instance,
            publishResponseEventData->mode,
            publishResponseEventData->source);
      }
      break;

      case FSM_EXIT:
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

static void State_ChipSelect(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE_ARG(data);

   switch(signal)
   {
      case FSM_ENTRY:
         I2c_Start(instance->_private.i2c, OnStartComplete, instance);
         break;

      case Signal_InterruptContext_StartComplete:
         ResetTicksSinceLastResponse(instance);
         I2c_WriteByte(
            instance->_private.i2c,
            instance->_private.configuration->peripheralAddress_Write,
            OnChipSelectComplete,
            OnErrorOccurred,
            instance);
         break;

      case Signal_InterruptContext_ChipSelectComplete:
         ResetTicksSinceLastResponse(instance);
         Fsm_Transition(&instance->_private.fsm, State_Address);
         break;

      case Signal_Tick:
      {
         REINTERPRET(context, data, const TickContext_t *);
         ProcessTickSignal(instance, context->eventData);
      }
      break;

      case Signal_InterruptContext_I2CObjectError:
         instance->_private.numberOfI2cObjectErrors++;
         break;

      case FSM_EXIT:
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

static void State_Address(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE_ARG(data);

   switch(signal)
   {
      case FSM_ENTRY:
         // InterruptContext
         {
            uint8_t addressMSB = (uint8_t)(instance->_private.currentAddress >> 8);

            I2c_WriteByte(
               instance->_private.i2c,
               addressMSB,
               OnAddressMSBWriteComplete,
               OnErrorOccurred,
               instance);
         }
         break;

      case Signal_InterruptContext_WriteAddressMSBComplete:
      {
         ResetTicksSinceLastResponse(instance);

         uint8_t addressLSB = (uint8_t)(instance->_private.currentAddress);

         I2c_WriteByte(
            instance->_private.i2c,
            addressLSB,
            OnAddressLSBWriteComplete,
            OnErrorOccurred,
            instance);
      }
      break;

      case Signal_InterruptContext_WriteAddressLSBComplete:
      {
         ResetTicksSinceLastResponse(instance);
         switch(instance->_private.currentMode)
         {
            case HardwareEepromMode_Erase:
            case HardwareEepromMode_Write:
               Fsm_Transition(&instance->_private.fsm, State_Writing);
               break;
            case HardwareEepromMode_Read:
               Fsm_Transition(&instance->_private.fsm, State_Reading);
               break;
         }
      }
      break;

      case Signal_Tick:
      {
         REINTERPRET(context, data, const TickContext_t *);
         ProcessTickSignal(instance, context->eventData);
      }
      break;

      case Signal_InterruptContext_I2CObjectError:
         instance->_private.numberOfI2cObjectErrors++;
         break;

      case FSM_EXIT:
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

static void State_Writing(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE_ARG(data);

   switch(signal)
   {
      case FSM_ENTRY:
         // InterruptContext
         I2c_WriteByte(
            instance->_private.i2c,
            GetByteToWrite(instance, instance->_private.byteIndex),
            OnWriteComplete,
            OnErrorOccurred,
            instance);
         break;

      case Signal_InterruptContext_WriteComplete:
         ResetTicksSinceLastResponse(instance);
         if(TheNextByteIsOnADifferentPage(instance))
         {
            I2c_Stop(instance->_private.i2c, OnStopComplete, instance);
         }
         else if(ThereAreMoreBytesToWrite(instance))
         {
            instance->_private.byteIndex++;
            instance->_private.currentAddress++;

            I2c_WriteByte(
               instance->_private.i2c,
               GetByteToWrite(instance, instance->_private.byteIndex),
               OnWriteComplete,
               OnErrorOccurred,
               instance);
         }
         else
         {
            I2c_Stop(instance->_private.i2c, OnStopComplete, instance);
         }
         break;

      case Signal_InterruptContext_StopComplete:
         ResetTicksSinceLastResponse(instance);
         Fsm_Transition(&instance->_private.fsm, State_PollForHardwareWriteComplete);
         break;

      case Signal_Tick:
      {
         REINTERPRET(context, data, const TickContext_t *);
         ProcessTickSignal(instance, context->eventData);
      }
      break;

      case Signal_InterruptContext_I2CObjectError:
         instance->_private.numberOfI2cObjectErrors++;
         break;

      case FSM_EXIT:
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

/*!
 * All of the data to be written to the current page has been sent.  Prior to the <stop> that was sent above the
 * data was stored in a buffer on the chip.  When the <stop> is sent the data in the buffer for the current
 * page is written to the NV memory on the chip.  The chip goes silent during this process.
 * When the <start> and the <chip select> are sent to the chip they are ignored during the write process.
 * In order to continue writing additional data or to tell the calling process that the write operation
 * is complete, this state will send the <start> and <chip select> until the chip responds.
 *
 * Since the <start> and the <chip select> will be ignored by the EEPROM chip, the I2C module will
 * call the OnErrorOccurred call back.  Since we are in this mode where we know the EEPROM chip
 * is not going to respond we must accept or tolerate a certain number of errors in this state.
 *
 * The max buffer to NV write time is 5 ms.  If this object receives more than 10
 * HardwareEepromI2cMT24XXMMMSignal_InterruptContext_I2CObjectError signals the object will
 * publish the error event.
 */
static void State_PollForHardwareWriteComplete(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE_ARG(data);

   switch(signal)
   {
      case FSM_ENTRY:
         // InterruptContext
         /*!
          * We must restart the tick timer here because the timer has been running
          * and we do not know how long it is until the next Signal_Tick.  We want to be sure
          * that the stop from above has time to complete before we start polling.
          * This is in the interrupt context, so we will wait for the next tick then have
          * that tick restart the timer.
          */
         instance->_private.timerRestartRequested = true;
         instance->_private.i2cBusAcquired = false;
         break;

      case Signal_InterruptContext_StartComplete:
         ResetTicksSinceLastResponse(instance);
         I2c_WriteByte(
            instance->_private.i2c,
            instance->_private.configuration->peripheralAddress_Write,
            OnWriteComplete,
            OnErrorOccurred,
            instance);
         break;

      case Signal_InterruptContext_WriteComplete:
         ResetTicksSinceLastResponse(instance);
         if(ThereAreMoreBytesToWrite(instance))
         {
            instance->_private.byteIndex++;
            instance->_private.currentAddress++;
            Fsm_Transition(&instance->_private.fsm, State_Address);
         }
         else
         {
            Fsm_Transition(&instance->_private.fsm, State_WriteComplete);
         }
         break;

      case Signal_Tick:
      {
         REINTERPRET(context, data, const TickContext_t *);

         if(instance->_private.timerRestartRequested)
         {
            instance->_private.timerRestartRequested = false;
            StartTheTickTimer(instance);
         }
         else if(++instance->_private.ticksSinceLastResponse >= instance->_private.configuration->maxTicksAllowedSinceLastResponse)
         {
            Fsm_Transition(&instance->_private.fsm, State_CoolDown);
            I2c_Cancel(instance->_private.i2c, OnCancelComplete, instance);

            context->eventData->publishResponseEvent = true;
            context->eventData->source = HardwareEepromErrorSource_OperationTimeOut;
         }
         else if(instance->_private.numberOfI2cObjectErrors > instance->_private.configuration->maxObjectErrorsAllowedWhenPolling)
         {
            Fsm_Transition(&instance->_private.fsm, State_CoolDown);
            I2c_Cancel(instance->_private.i2c, OnCancelComplete, instance);

            context->eventData->publishResponseEvent = true;
            context->eventData->source = HardwareEepromErrorSource_DeviceError;
         }
         else
         {
            /*!
             * If the bus has not been acquired, then perform a check to see if it is busy and call I2c_Start if it's available.
             * If it has been acquired, that means the chip is not responding and we must send a start again. However the state
             * of the I2C object is unclear so we will send a cancel and send the start after receiving the cancel callback.
             */
            if(!instance->_private.i2cBusAcquired)
            {
               CheckI2cBusStatus(instance);
            }
            else
            {
               I2c_Cancel(instance->_private.i2c, OnCancelComplete, instance);
            }
         }
      }
      break;

      case Signal_BusAvailable:
         instance->_private.i2cBusAcquired = true;
         I2c_Start(instance->_private.i2c, OnStartComplete, instance);
         break;

      case Signal_BusBusy:
         instance->_private.i2cBusAcquired = false;
         break;

      case Signal_InterruptContext_CancelComplete:
         CheckI2cBusStatus(instance);
         break;

      case Signal_InterruptContext_I2CObjectError:
         instance->_private.numberOfI2cObjectErrors++;
         break;

      case FSM_EXIT:
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

/*!
 * The write/erase is complete. This state needs to send the Stop Command and publish
 * the write/erase complete event in the normal run context.
 * Stop is sent and a quick response is expected.
 * There is no expectation that State_WriteComplete would ever receive a Signal_InterruptContext_I2CObjectError.
 * There is no expectation that State_WriteComplete would timeout.
 */
static void State_WriteComplete(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE_ARG(data);

   switch(signal)
   {
      case FSM_ENTRY:
         // InterruptContext
         I2c_Stop(instance->_private.i2c, OnStopComplete, instance);
         break;

      case Signal_InterruptContext_StopComplete:
         ResetTicksSinceLastResponse(instance);
         break;

      case Signal_Tick:
      {
         REINTERPRET(context, data, const TickContext_t *);

         // It is necessary to publish from Idle so that the client can retry.
         Fsm_Transition(&instance->_private.fsm, State_CoolDown);

         context->eventData->publishResponseEvent = true;
         context->eventData->source = HardwareEepromErrorSource_None;
      }
      break;

      case FSM_EXIT:
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

static void State_Reading(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE_ARG(data);

   switch(signal)
   {
      case FSM_ENTRY:
         // InterruptContext
         I2c_Start(instance->_private.i2c, OnStartComplete, instance);
         break;

      case Signal_InterruptContext_StartComplete:
         ResetTicksSinceLastResponse(instance);
         I2c_Read(
            instance->_private.i2c,
            instance->_private.configuration->peripheralAddress_Read,
            instance->_private.holdParameters.readBuffer,
            instance->_private.holdParameters.numBytes,
            OnReadComplete,
            OnErrorOccurred,
            instance);
         break;

      case Signal_InterruptContext_ReadComplete:
         ResetTicksSinceLastResponse(instance);
         Fsm_Transition(&instance->_private.fsm, State_ReadComplete);
         break;

      case Signal_Tick:
      {
         REINTERPRET(context, data, const TickContext_t *);
         ProcessTickSignal(instance, context->eventData);
      }
      break;

      case Signal_InterruptContext_I2CObjectError:
         instance->_private.numberOfI2cObjectErrors++;
         break;

      case FSM_EXIT:
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

/*!
 * The read is complete. This state needs to send the Stop Command and publish
 * the read complete event in the normal run context.
 * Stop is sent and a quick response is expected.
 * There is no expectation that State_ReadComplete would ever receive a Signal_InterruptContext_I2CObjectError.
 * There is no expectation that State_ReadComplete would timeout.
 */
static void State_ReadComplete(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case FSM_ENTRY:
         // InterruptContext
         I2c_Stop(instance->_private.i2c, OnStopComplete, instance);
         break;

      case Signal_InterruptContext_StopComplete:
         ResetTicksSinceLastResponse(instance);
         break;

      case Signal_Tick:
      {
         REINTERPRET(context, data, const TickContext_t *);

         // It is necessary to publish from Idle so that the client can retry.
         Fsm_Transition(&instance->_private.fsm, State_CoolDown);

         context->eventData->publishResponseEvent = true;
         context->eventData->source = HardwareEepromErrorSource_None;
      }
      break;

      case FSM_EXIT:
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

static bool EepromRegionIsWithinRange(uint32_t offset, uint32_t numBytes, uint32_t dataSize)
{
   return (offset + numBytes) > dataSize;
}

static bool ParametersAreValid(
   HardwareEeprom_I2cMt24xxmmm_t *instance,
   uint32_t offset,
   uint32_t numBytes,
   const void *buffer,
   HardwareEepromErrorSource_t *source)
{
   bool parametersAreValid = true;
   *source = HardwareEepromErrorSource_None;

   if(offset >= instance->_private.configuration->numberOfStorageBytes)
   {
      *source = HardwareEepromErrorSource_OffsetOutOfRange;
      parametersAreValid = false;
   }
   else if(numBytes > instance->_private.configuration->numberOfStorageBytes)
   {
      *source = HardwareEepromErrorSource_NumberOfBytesOutOfRange;
      parametersAreValid = false;
   }
   else if(buffer == NULL)
   {
      *source = HardwareEepromErrorSource_NullBuffer;
      parametersAreValid = false;
   }
   else if(EepromRegionIsWithinRange(offset, numBytes, instance->_private.configuration->numberOfStorageBytes))
   {
      *source = HardwareEepromErrorSource_OffsetPlusNumberOfBytesOutOfRange;
      parametersAreValid = false;
   }

   return parametersAreValid;
}

static void State_CoolDown(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   HardwareEeprom_I2cMt24xxmmm_t *instance = CONTAINER_OF(HardwareEeprom_I2cMt24xxmmm_t, _private.fsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case FSM_ENTRY:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.tickTimer);
         instance->_private.requestQueueBuffer.coolDownProcessInProgress = true;
         instance->_private.currentMode = HardwareEepromMode_None;
         StartTheCoolDownTimer(instance);
         break;

      case Signal_CoolDownTimeOut:
      {
         Fsm_Transition(&instance->_private.fsm, State_Idle);

         if(instance->_private.requestQueueBuffer.requestPending)
         {
            instance->_private.requestQueueBuffer.requestPending = false;
            HardwareEepromErrorSource_t source;
            switch(instance->_private.requestQueueBuffer.requestedMode)
            {
               case HardwareEepromMode_Read:
                  if(ParametersAreValid(
                        instance,
                        instance->_private.requestQueueBuffer.offset,
                        instance->_private.requestQueueBuffer.numBytes,
                        instance->_private.requestQueueBuffer.readBuffer,
                        &source))
                  {
                     instance->_private.currentMode = HardwareEepromMode_Read;
                     instance->_private.holdParameters.offset = instance->_private.requestQueueBuffer.offset;
                     instance->_private.holdParameters.numBytes = instance->_private.requestQueueBuffer.numBytes;
                     instance->_private.holdParameters.readBuffer = instance->_private.requestQueueBuffer.readBuffer;
                     instance->_private.byteIndex = 0;
                     instance->_private.currentAddress = instance->_private.requestQueueBuffer.offset;

                     Fsm_SendSignal(&instance->_private.fsm, Signal_ReadStart, NULL);
                  }
                  else
                  {
                     // It is necessary to publish from Idle so that the client can retry.
                     Fsm_Transition(&instance->_private.fsm, State_Idle);
                     PublishResponseEvent(instance, HardwareEepromMode_Read, source);
                  }
                  break;

               case HardwareEepromMode_Write:
                  if(ParametersAreValid(
                        instance,
                        instance->_private.requestQueueBuffer.offset,
                        instance->_private.requestQueueBuffer.numBytes,
                        instance->_private.requestQueueBuffer.writeBuffer,
                        &source))
                  {
                     instance->_private.currentMode = HardwareEepromMode_Write;
                     instance->_private.holdParameters.offset = instance->_private.requestQueueBuffer.offset;
                     instance->_private.holdParameters.numBytes = instance->_private.requestQueueBuffer.numBytes;
                     instance->_private.holdParameters.writeBuffer = instance->_private.requestQueueBuffer.writeBuffer;
                     instance->_private.byteIndex = 0;
                     instance->_private.currentAddress = instance->_private.requestQueueBuffer.offset;

                     Fsm_SendSignal(&instance->_private.fsm, Signal_WriteStart, NULL);
                  }
                  else
                  {
                     // It is necessary to publish from Idle so that the client can retry.
                     Fsm_Transition(&instance->_private.fsm, State_Idle);
                     PublishResponseEvent(instance, HardwareEepromMode_Write, source);
                  }
                  break;

               case HardwareEepromMode_Erase:
               {
                  instance->_private.currentMode = HardwareEepromMode_Erase;

                  instance->_private.holdParameters.offset = 0;
                  instance->_private.holdParameters.numBytes = instance->_private.configuration->numberOfStorageBytes;
                  instance->_private.holdParameters.writeBuffer = NULL;

                  instance->_private.byteIndex = 0;
                  instance->_private.currentAddress = 0;

                  Fsm_SendSignal(&instance->_private.fsm, Signal_WriteStart, NULL);
               }
               break;

               default:
                  break;
            }
         }
      }
      break;

      case FSM_EXIT:
         instance->_private.requestQueueBuffer.coolDownProcessInProgress = false;
         ResetTicksSinceLastResponse(instance);
         ResetNumberOfI2cObjectErrors(instance);
         break;
   }
}

static void Read(I_HardwareEeprom_t *_instance, uint32_t offset, uint32_t numBytes, void *readBuffer)
{
   REINTERPRET(instance, _instance, HardwareEeprom_I2cMt24xxmmm_t *);
   uassert(instance->_private.currentMode == HardwareEepromMode_None);

   HardwareEepromErrorSource_t source;

   if(instance->_private.requestQueueBuffer.coolDownProcessInProgress)
   {
      instance->_private.requestQueueBuffer.requestedMode = HardwareEepromMode_Read;

      instance->_private.requestQueueBuffer.numBytes = numBytes;
      instance->_private.requestQueueBuffer.offset = offset;
      instance->_private.requestQueueBuffer.readBuffer = readBuffer;

      instance->_private.requestQueueBuffer.requestPending = true;
   }
   else
   {
      if(ParametersAreValid(instance, offset, numBytes, readBuffer, &source))
      {
         instance->_private.currentMode = HardwareEepromMode_Read;
         instance->_private.holdParameters.offset = offset;
         instance->_private.holdParameters.numBytes = numBytes;
         instance->_private.holdParameters.readBuffer = readBuffer;
         instance->_private.byteIndex = 0;
         instance->_private.currentAddress = offset;

         Fsm_SendSignal(&instance->_private.fsm, Signal_ReadStart, NULL);
      }
      else
      {
         // It is necessary to publish from Idle so that the client can retry.
         Fsm_Transition(&instance->_private.fsm, State_Idle);
         PublishResponseEvent(instance, HardwareEepromMode_Read, source);
      }
   }
}

static void Write(I_HardwareEeprom_t *_instance, uint32_t offset, uint32_t numBytes, const void *writeBuffer)
{
   REINTERPRET(instance, _instance, HardwareEeprom_I2cMt24xxmmm_t *);
   uassert(instance->_private.currentMode == HardwareEepromMode_None);

   HardwareEepromErrorSource_t source;

   if(instance->_private.requestQueueBuffer.coolDownProcessInProgress)
   {
      instance->_private.requestQueueBuffer.requestedMode = HardwareEepromMode_Write;

      instance->_private.requestQueueBuffer.numBytes = numBytes;
      instance->_private.requestQueueBuffer.offset = offset;
      instance->_private.requestQueueBuffer.writeBuffer = writeBuffer;

      instance->_private.requestQueueBuffer.requestPending = true;
   }
   else
   {
      if(ParametersAreValid(instance, offset, numBytes, writeBuffer, &source))
      {
         instance->_private.currentMode = HardwareEepromMode_Write;
         instance->_private.holdParameters.offset = offset;
         instance->_private.holdParameters.numBytes = numBytes;
         instance->_private.holdParameters.writeBuffer = writeBuffer;
         instance->_private.byteIndex = 0;
         instance->_private.currentAddress = offset;

         Fsm_SendSignal(&instance->_private.fsm, Signal_WriteStart, NULL);
      }
      else
      {
         // It is necessary to publish from Idle so that the client can retry.
         Fsm_Transition(&instance->_private.fsm, State_Idle);
         PublishResponseEvent(instance, HardwareEepromMode_Write, source);
      }
   }
}

static void Erase(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, HardwareEeprom_I2cMt24xxmmm_t *);
   uassert(instance->_private.currentMode == HardwareEepromMode_None);

   if(instance->_private.requestQueueBuffer.coolDownProcessInProgress)
   {
      instance->_private.requestQueueBuffer.requestedMode = HardwareEepromMode_Erase;

      instance->_private.requestQueueBuffer.numBytes = instance->_private.configuration->numberOfStorageBytes;
      instance->_private.requestQueueBuffer.offset = 0;
      instance->_private.requestQueueBuffer.writeBuffer = NULL;

      instance->_private.requestQueueBuffer.requestPending = true;
   }
   else
   {
      instance->_private.currentMode = HardwareEepromMode_Erase;

      instance->_private.holdParameters.offset = 0;
      instance->_private.holdParameters.numBytes = instance->_private.configuration->numberOfStorageBytes;
      instance->_private.holdParameters.writeBuffer = NULL;

      instance->_private.byteIndex = 0;
      instance->_private.currentAddress = 0;

      Fsm_SendSignal(&instance->_private.fsm, Signal_WriteStart, NULL);
   }
}

static uint16_t GetByteAlignment(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, HardwareEeprom_I2cMt24xxmmm_t *);
   return instance->_private.configuration->byteAlignment;
}

static uint32_t GetSize(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, HardwareEeprom_I2cMt24xxmmm_t *);
   return instance->_private.configuration->numberOfStorageBytes;
}

static I_Event_t *GetOnWrittenEvent(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, HardwareEeprom_I2cMt24xxmmm_t *);
   return &instance->_private.onWrittenEvent.interface;
}

static I_Event_t *GetOnReadEvent(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, HardwareEeprom_I2cMt24xxmmm_t *);
   return &instance->_private.onReadEvent.interface;
}

static I_Event_t *GetOnErasedEvent(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, HardwareEeprom_I2cMt24xxmmm_t *);
   return &instance->_private.onErasedEvent.interface;
}

static const I_HardwareEeprom_Api_t api = {
   .Read = Read,
   .Write = Write,
   .Erase = Erase,
   .GetByteAlignment = GetByteAlignment,
   .GetSize = GetSize,
   .GetOnWrittenEvent = GetOnWrittenEvent,
   .GetOnReadEvent = GetOnReadEvent,
   .GetOnErasedEvent = GetOnErasedEvent,
};

void HardwareEeprom_I2cMt24xxmmm_Init(
   HardwareEeprom_I2cMt24xxmmm_t *instance,
   I_I2c_t *i2c,
   const HardwareEeprom_I2cMt24xxmmmConfiguration_t *configuration,
   TimerModule_t *timerModule,
   I_ContextProtector_t *contextProtector)
{
   instance->_private.timerModule = timerModule;
   instance->_private.i2c = i2c;
   instance->_private.configuration = configuration;
   instance->_private.contextProtector = contextProtector;

   Event_Synchronous_Init(&instance->_private.onReadEvent);
   Event_Synchronous_Init(&instance->_private.onWrittenEvent);
   Event_Synchronous_Init(&instance->_private.onErasedEvent);

   instance->interface.api = &api;

   Fsm_Init(&instance->_private.fsm, State_Idle);
}
