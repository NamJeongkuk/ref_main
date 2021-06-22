/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Action_JumpToBootLoader.h"
#include "MemoryMap.h"
#include "utils.h"

typedef void (*Function_t)(void);

static void JumpToBootLoader(I_Action_t *_instance)
{
   IGNORE(_instance);
   ((Function_t)(BootLoaderImageHeader)->imageExecutionStart.pointer)();
}

static const I_Action_Api_t api = { JumpToBootLoader };

I_Action_t *Action_JumpToBootLoader_Init(void)
{
   static I_Action_t action = { &api };
   return &action;
}
