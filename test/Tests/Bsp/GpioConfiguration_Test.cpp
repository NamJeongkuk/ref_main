/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "GpioConfiguration.h"
#include "I_Gpio.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"

#define EXPAND_PORTS_AND_PINS(channel, direction, offToOnDebounceCount, onToOffDebounceCount, pullUp, driveCapacity, port, pin, inverted) direction,

static const uint8_t gpioDirection[] = {
   GPIO_TABLE(EXPAND_PORTS_AND_PINS)
};

TEST_GROUP(GpioConfiguration){

};

TEST(GpioConfiguration, DirectionInputShouldBeConfiguredBeforeDirectionOuput)
{
   bool outputFounded = false;

   for(uint8_t i = 0; i < NUM_ELEMENTS(gpioDirection); i++)
   {
      if(gpioDirection[i] == GpioDirection_Output)
      {
         outputFounded = true;
      }
      else if(outputFounded)
      {
         FAIL("GpioDirection_Input should be configured before GpioDirection_Output");
         break;
      }
   }
}
