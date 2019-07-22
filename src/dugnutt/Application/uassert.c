/*!
 * @file
 * @brief Implementation of uassert
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "uassert.h"

void __uassert_func(
   const char * fileName,
   const int lineNumber,
   const bool condition,
   const char *conditionString)
{
   (void)conditionString;
   (void)fileName;
   (void)lineNumber;

   if(!condition)
   {
      while(1)
      {

      }
   }
}
