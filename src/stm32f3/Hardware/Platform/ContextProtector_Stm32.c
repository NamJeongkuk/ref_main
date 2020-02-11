/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stdint.h>
#include "ContextProtector_Stm32.h"
#include "utils.h"
#include "uassert.h"
#include "stm32f3xx.h"

static int8_t contextCounter = 0;
static uint32_t preservedPsw;

static void Protect(I_ContextProtector_t *instance)
{
   volatile uint32_t psw;

   uassert(contextCounter < INT8_MAX);

   IGNORE_ARG(instance);

   if(contextCounter == 0)
   {
      psw = __get_PRIMASK();
      __disable_irq();
      preservedPsw = psw;
   }

   contextCounter++;
}

static void Unprotect(I_ContextProtector_t *instance)
{
   IGNORE_ARG(instance);

   // This should never be called before we've saved the context
   uassert(contextCounter > 0);

   contextCounter--;

   // Are we out of the nested calls?
   if(contextCounter == 0)
   {
      __set_PRIMASK(preservedPsw);
   }
}

static const I_ContextProtector_Api_t api =
   { Protect, Unprotect };

static const I_ContextProtector_t instance =
   { .api = &api };

I_ContextProtector_t *ContextProtector_Stm32_GetInstance(void)
{
   return (I_ContextProtector_t *)&instance;
}
