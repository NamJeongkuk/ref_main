/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "HeartbeatLedPlugin.h"
#include "GpioGroup.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

enum
{
   HeartbeatBlinkPeriodMsec = 250
};

void HeartbeatLedPlugin_Init(
   HeartbeatLedPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   DigitalOutput_GpioGroupAdapter_Init(
      &instance->_private.heartbeatLed,
      DataModelErdPointerAccess_GetGpioGroup(dataModel, Erd_GpioGroup),
      Gpio_HeartbeatLed);

   DigitalOutputBlinker_Init(
      &instance->_private.blinkHeartbeat,
      &instance->_private.heartbeatLed.interface,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      HeartbeatBlinkPeriodMsec);
}
