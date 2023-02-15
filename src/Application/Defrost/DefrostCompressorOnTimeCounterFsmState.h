/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTCOMPRESSORONTIMECOUNTERFSMSTATE_H
#define DEFROSTCOMPRESSORONTIMECOUNTERFSMSTATE_H

#include <stdint.h>

enum
{
   DefrostCompressorOnTimeCounterFsmState_Stop,
   DefrostCompressorOnTimeCounterFsmState_Pause,
   DefrostCompressorOnTimeCounterFsmState_Run
};
typedef uint8_t DefrostCompressorOnTimeCounterFsmState_t;

#endif
