/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stdint.h>
#include "ContextProtector_Psoc6.h"
#include "utils.h"
#include "uassert.h"
#include "cy_pdl.h"

static int8_t contextCounter = 0;
static uint32_t preservedPsw;

static void Protect(I_ContextProtector_t *instance)
{
   uassert(contextCounter < INT8_MAX);

   IGNORE(instance);

   if(contextCounter == 0)
   {
      preservedPsw = Cy_SysLib_EnterCriticalSection();
   }

   contextCounter++;
}

static void Unprotect(I_ContextProtector_t *instance)
{
   IGNORE(instance);

   // This should never be called before we've saved the context
   uassert(contextCounter > 0);

   contextCounter--;

   // Are we out of the nested calls?
   if(contextCounter == 0)
   {
      Cy_SysLib_ExitCriticalSection(preservedPsw);
   }
}

static const I_ContextProtector_Api_t api = { Protect, Unprotect };

static I_ContextProtector_t instance = { .api = &api };

I_ContextProtector_t *ContextProtector_Psoc6_GetInstance(void)
{
   return &instance;
}
