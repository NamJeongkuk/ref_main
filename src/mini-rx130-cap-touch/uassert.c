/*!
 * @file
 * @brief Function called to check assertions and respond to failed assertions.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include "Reset.h"

void __uassert_func(const bool condition)
{
   if(!condition)
   {
      Reset();
   }
}
