/*!
 * @file
 * @brief Output that will drive the twist tray ice maker motor in the requested state
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef OUTPUT_TWISTTRAYICEMAKERMOTORSTATE_H
#define OUTPUT_TWISTTRAYICEMAKERMOTORSTATE_H

#include "I_Output.h"
#include "I_GpioGroup.h"
#include "Erd.h"

typedef struct
{
   Erd_t motorDrivePositiveClockwiseChannel;
   Erd_t motorDriveNegativeCounterClockwiseChannel;
} Output_TwistTrayIceMakerMotorStateConfig_t;

typedef struct
{
   I_Output_t interface;

   struct
   {
      I_GpioGroup_t *gpioGroup;
      const Output_TwistTrayIceMakerMotorStateConfig_t *config;
   } _private;
} Output_TwistTrayIceMakerMotorState_t;

I_Output_t *Output_TwistTrayIceMakerMotorState_Init(
   Output_TwistTrayIceMakerMotorState_t *instance,
   I_GpioGroup_t *gpioGroup,
   const Output_TwistTrayIceMakerMotorStateConfig_t *config);

#endif
