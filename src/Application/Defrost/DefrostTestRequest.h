/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTESTREQUEST_H
#define DEFROSTTESTREQUEST_H

#include "Signal.h"

enum
{
   DefrostTestRequest_None,
   DefrostTestRequest_Idle,
   DefrostTestRequest_SecondaryOnlyDefrost,
   DefrostTestRequest_Defrost,
   DefrostTestRequest_SecondaryOnlyPrechill,
   DefrostTestRequest_Prechill,
   DefrostTestRequest_AhamSecondaryOnlyPrechill,
   DefrostTestRequest_AhamPrechill,
   DefrostTestRequest_Enable,
   DefrostTestRequest_Disable,
   DefrostTestRequest_ExitDefrostHeaterOnState
};
typedef uint8_t DefrostTestRequest_t;

#endif
