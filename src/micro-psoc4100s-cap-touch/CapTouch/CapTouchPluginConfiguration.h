/*!
 * @file
 * @brief Defines cap sense keys for a given target.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHPLUGINCONFIGURATION_H
#define CAPTOUCHPLUGINCONFIGURATION_H

#include "SystemErds.h"

enum
{
   CapTouchPluginPollPeriodMsec = 5,
   CapTouchTimerModuleErd = Erd_TimerModule,
   CapTouchResultErd = Erd_KeyState
};

#define CAP_TOUCH_KEYS(ENTRY) \
   ENTRY(CapSense_U102, 102)           \
   ENTRY(CapSense_U103, 103)           \
   ENTRY(CapSense_U104, 104)           \
   ENTRY(CapSense_U105, 105)           \
   ENTRY(CapSense_U106, 106)           \
   ENTRY(CapSense_U107, 107)           \
   ENTRY(CapSense_U108, 108)           \
   ENTRY(CapSense_U109, 109)           \
   ENTRY(CapSense_U110, 110)           \
   ENTRY(CapSense_U111, 111)           \
   ENTRY(CapSense_U112, 112)           \
   ENTRY(CapSense_U113, 113)           \
   ENTRY(CapSense_U114, 114)

#endif
