/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Input_EnhancedSabbathRandomAdditionalMinute.h"
#include "Event_Null.h"
#include "utils.h"
#include "Constants_Time.h"

enum
{
   Max2BytesPlusOne = 65536,
   Rand_Max = 32768,
   RandMultiplier = 1103515245,
   RandAdder = 12345,
   MaxRandomAdditionTimeInSeconds = 60,
};

static void Read(I_Input_t *_instance, void *value)
{
   REINTERPRET(instance, _instance, Input_EnhancedSabbathRandomAdditionalMinute_t *);
   uint32_t *result = value;

   instance->_private.seed = (instance->_private.seed * RandMultiplier) + RandAdder;
   uint32_t randomNumber = (instance->_private.seed / Max2BytesPlusOne) % Rand_Max;

   *result = (randomNumber * MaxRandomAdditionTimeInSeconds / Rand_Max) * MSEC_PER_SEC;
}

static I_Event_t *GetOnChangeEvent(I_Input_t *_instance)
{
   IGNORE(_instance);
   return Event_Null_GetInstance();
}

static const I_Input_Api_t api = { .Read = Read, .GetOnChangeEvent = GetOnChangeEvent };

void Input_EnhancedSabbathRandomAdditionalMinute_Init(Input_EnhancedSabbathRandomAdditionalMinute_t *instance)
{
   instance->interface.api = &api;
   instance->_private.seed = 0;
}
