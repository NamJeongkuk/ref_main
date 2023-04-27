/*!
 * @file
 * @brief HSM state for Ready to Defrost state machine
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef READYTODEFROSTHSMSTATE_H
#define READYTODEFROSTHSMSTATE_H

#include <stdint.h>

enum
{
   ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts,
   ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts,
   ReadyToDefrostHsmState_ReadyAndDefrosting
};
typedef uint8_t ReadyToDefrostHsmState_t;

#endif
