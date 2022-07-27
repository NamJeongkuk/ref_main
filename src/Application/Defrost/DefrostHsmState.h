/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTHSMSTATE_H
#define DEFROSTHSMSTATE_H

#include <stdint.h>

enum
{
   DefrostHsmState_PowerUp,
   DefrostHsmState_Idle,
   DefrostHsmState_PrechillPrep,
   DefrostHsmState_Prechill,
   DefrostHsmState_HeaterOnEntry,
   DefrostHsmState_HeaterOnHeat,
   DefrostHsmState_Dwell,
   DefrostHsmState_PostDwell,
   DefrostHsmState_Disabled
};
typedef uint8_t DefrostHsmState_t;

#endif
