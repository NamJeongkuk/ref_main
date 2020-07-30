/*!
 * @file
 * @brief Implementation of uassert
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "uassert.h"
#include "iodefine.h"
#include "ProgramCounterAddress.h"

static I_Output_t *programCounterAddressOutput;

void __uassert_func(
   const char *fileName,
   const int lineNumber,
   const bool condition,
   const char *conditionString,
   const void *programCounter)
{
   (void)conditionString;
   (void)fileName;
   (void)lineNumber;

   if(!condition)
   {
      if(programCounterAddressOutput)
      {
         ProgramCounterAddress_t programCounterAddress = (ProgramCounterAddress_t)programCounter;
         Output_Write(programCounterAddressOutput, &programCounterAddress);
      }

      while(1)
      {
      }
   }
}

void Uassert_Init(I_Output_t *_programCounterAddressOutput)
{
   programCounterAddressOutput = _programCounterAddressOutput;
}
