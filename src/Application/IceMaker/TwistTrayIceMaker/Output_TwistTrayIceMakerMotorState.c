/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Output_TwistTrayIceMakerMotorState.h"
#include "TwistTrayIceMakerMotorState.h"
#include "Constants_Binary.h"
#include "utils.h"

static void Write(I_Output_t *_instance, const void *value)
{
   REINTERPRET(instance, _instance, Output_TwistTrayIceMakerMotorState_t *);
   const TwistTrayIceMakerMotorState_t *newState = value;

   switch(*newState)
   {
      case TwistTrayIceMakerMotorState_Coasting:
         GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->motorDrivePositiveClockwiseChannel, CLEAR);
         GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->motorDriveNegativeCounterClockwiseChannel, CLEAR);
         break;

      case TwistTrayIceMakerMotorState_Untwisting:
         GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->motorDrivePositiveClockwiseChannel, CLEAR);
         GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->motorDriveNegativeCounterClockwiseChannel, SET);
         break;

      case TwistTrayIceMakerMotorState_Twisting:
         GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->motorDrivePositiveClockwiseChannel, SET);
         GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->motorDriveNegativeCounterClockwiseChannel, CLEAR);
         break;

      case TwistTrayIceMakerMotorState_Braking:
         GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->motorDrivePositiveClockwiseChannel, SET);
         GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->motorDriveNegativeCounterClockwiseChannel, SET);
         break;
   }
}

static const I_Output_Api_t api = { Write };

I_Output_t *Output_TwistTrayIceMakerMotorState_Init(
   Output_TwistTrayIceMakerMotorState_t *instance,
   I_GpioGroup_t *gpioGroup,
   const Output_TwistTrayIceMakerMotorStateConfig_t *config)
{
   instance->interface.api = &api;
   instance->_private.gpioGroup = gpioGroup;
   instance->_private.config = config;

   return &instance->interface;
}
