/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHPLUGINCONFIGURATION_H
#define CAPTOUCHPLUGINCONFIGURATION_H

#include "SystemErds.h"
#include "Reset.h"

// Configuration generated by E2 Studio
#include "qe_config01.h"

#define CapTouchConfigName CONFIG01

enum
{
   CapTouchPollPeriodMsec = 20,
   CapTouchTimerModuleErd = Erd_TimerModule,
   CapTouchResultErd = Erd_KeyState
};

typedef KeyState_t CapTouchState_t;

#define CapTouchSystemReset() Reset()

// ENTRY(keyId)
#define CAP_TOUCH_KEYS(ENTRY) \
   ENTRY(KEY1)                \
   ENTRY(KEY2)

#endif
