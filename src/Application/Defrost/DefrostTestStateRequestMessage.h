/*!
 * @file
 * @brief Defrost Test State Request Type Definition
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTESTSTATEREQUESTMESSAGE_H
#define DEFROSTTESTSTATEREQUESTMESSAGE_H

#include "Signal.h"

enum
{
   DefrostTestStateRequest_Idle,
   DefrostTestStateRequest_Defrost,
   DefrostTestStateRequest_Prechill,
   DefrostTestStateRequest_None
};
typedef uint8_t DefrostTestStateRequest_t;

typedef struct
{
   DefrostTestStateRequest_t request;
   Signal_t requestId;
} DefrostTestStateRequestMessage_t;

#endif
