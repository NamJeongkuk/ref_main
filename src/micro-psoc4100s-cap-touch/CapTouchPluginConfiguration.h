/*!
 * @file
 * @brief Defines cap touch keys and configuration for a given target.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHPLUGINCONFIGURATION_H
#define CAPTOUCHPLUGINCONFIGURATION_H

#include "SystemErds.h"
#include "Reset.h"

enum
{
   CapTouchPollPeriodMsec = 5,
   CapTouchTimerModuleErd = Erd_TimerModule,
   CapTouchResultErd = Erd_KeyState
};

typedef uint32_t CapTouchState_t;

#define CapTouchSystemReset() Reset()

// ENTRY(Widget ID)
#define CAP_TOUCH_KEYS(ENTRY) \
   ENTRY(102)                 \
   ENTRY(103)                 \
   ENTRY(104)                 \
   ENTRY(105)                 \
   ENTRY(106)                 \
   ENTRY(107)                 \
   ENTRY(108)                 \
   ENTRY(109)                 \
   ENTRY(110)                 \
   ENTRY(111)                 \
   ENTRY(112)                 \
   ENTRY(113)                 \
   ENTRY(114)

#endif
