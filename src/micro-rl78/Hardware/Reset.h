/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RESET_H
#define RESET_H

#include <stdint.h>

/*!
 */
static inline void Reset(void)
{
   static const unsigned char illegalInstruction = 0xFF;
   void (*reset)(void) = (void (*)(void)) & illegalInstruction;
   reset();
}

#endif
