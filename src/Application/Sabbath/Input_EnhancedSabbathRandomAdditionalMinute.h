/*!
 * @file
 * @brief Input used to generate a pseudorandom time between 0-60 seconds converted to msec
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef INPUT_ENHANCEDSABBATHRANDOMADDITIONALMINUTE_H
#define INPUT_ENHANCEDSABBATHRANDOMADDITIONALMINUTE_H

#include <stdint.h>
#include "I_Input.h"

typedef struct
{
   I_Input_t interface;

   struct
   {
      uint32_t seed;
   } _private;
} Input_EnhancedSabbathRandomAdditionalMinute_t;

/*!
 * @param instance
 */
void Input_EnhancedSabbathRandomAdditionalMinute_Init(
   Input_EnhancedSabbathRandomAdditionalMinute_t *instance);

#endif
