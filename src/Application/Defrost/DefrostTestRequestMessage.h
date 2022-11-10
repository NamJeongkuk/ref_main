/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTESTREQUESTMESSAGE_H
#define DEFROSTTESTREQUESTMESSAGE_H

#include "Signal.h"

enum
{
   DefrostTestRequest_Idle,
   DefrostTestRequest_FreshFoodOnlyDefrost,
   DefrostTestRequest_Defrost,
   DefrostTestRequest_FreshFoodOnlyPrechill,
   DefrostTestRequest_Prechill,
   DefrostTestRequest_AhamFreshFoodOnlyPrechill,
   DefrostTestRequest_AhamPrechill,
   DefrostTestRequest_Enable,
   DefrostTestRequest_Disable
};
typedef uint8_t DefrostTestRequest_t;

typedef struct
{
   DefrostTestRequest_t request;
   Signal_t requestId;
} DefrostTestRequestMessage_t;

#endif
