/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "uassert.h"
#include "utils.h"

void TinyUassert(const bool condition)
{
   if(!condition)
   {
      while(1);
   }
}
