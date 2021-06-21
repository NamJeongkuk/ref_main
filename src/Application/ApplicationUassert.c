/*!
 * @file
 * @brief Implementation of uassert
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplicationUassert.h"
#include "uassert.h"
#include "ProgramCounterAddress.h"

enum
{
   DelayBeforeResetInMsec = 250
};

static struct
{
   I_Action_t *resetAction;
   I_Output_t *programCounterAddressOutput;
   TimerModule_t *timerModule;
   Timer_t delayedRestartTimer;
} instance;

static void Reset(void *context)
{
   IGNORE(context);
   Action_Invoke(instance.resetAction);
}

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
      if(instance.programCounterAddressOutput)
      {
         ProgramCounterAddress_t programCounterAddress = (ProgramCounterAddress_t)programCounter;
         Output_Write(instance.programCounterAddressOutput, &programCounterAddress);
      }

      if(instance.timerModule)
      {
         TimerModule_StartOneShot(instance.timerModule, &instance.delayedRestartTimer, DelayBeforeResetInMsec, Reset, NULL);
      }

      while(1)
      {
         if(instance.timerModule)
         {
            TimerModule_Run(instance.timerModule);
         }
      }
   }
}

void ApplicationUassert_Init(
   I_Action_t *resetAction,
   I_Output_t *programCounterAddressOutput,
   TimerModule_t *timerModule)
{
   instance.resetAction = resetAction;
   instance.programCounterAddressOutput = programCounterAddressOutput;
   instance.timerModule = timerModule;
}
