/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NOTIFICATIONSBITMAP_H
#define NOTIFICATIONSBITMAP_H

#include <stdint.h>
#include "utils.h"

#define GET_NOTIFICATION_BYTE_INDEX(x) ((x) >> 3)
#define GET_NOTIFICATION_BYTE_MASK(x) (BIT_MASK((x) % 8))

enum
{
   Notifications_SupportedFreshFoodDoorAlert = 0,
   Notifications_SupportedFreezerDoorAlert,
   Notifications_SupportedFilterOrderAlert,
   Notifications_SupportedFilterReplaceAlert,
   Notifications_SupportedFreshFoodHighTemperatureAlert,
   Notifications_SupportedFreezerTemperatureAlert,
   Notifications_SupportedFreshFoodIceMakerFullAlert,
   Notifications_SupportedFreezerIceMakerFullAlert,
   Notifications_SupportedLeakDetectedAlert,
   Notifications_SupportedPotentialPitcherLeakAlert,
   Notifications_SupportedPitcherLowFlowAlert,
   Notifications_SupportedDrawerTemperatureMetAlert,
   Notifications_SupportedFreezerDoorOpenedAlert,
   Notifications_TriggeredFreshFoodDoorAlert = 128,
   Notifications_TriggeredFreezerDoorAlert,
   Notifications_TriggeredFilterOrderAlert,
   Notifications_TriggeredFilterReplaceAlert,
   Notifications_TriggeredFreshFoodHighTemperatureAlert,
   Notifications_TriggeredFreezerTemperatureAlert,
   Notifications_TriggeredFreshFoodIceMakerFullAlert,
   Notifications_TriggeredFreezerIceMakerFullAlert,
   Notifications_TriggeredLeakDetectedAlert,
   Notifications_TriggeredPotentialPitcherLeakAlert,
   Notifications_TriggeredPitcherLowFlowAlert,
   Notifications_TriggeredDrawerTemperatureMetAlert,
   Notifications_TriggeredFreezerDoorOpenedAlert,
};
typedef uint8_t Notifications_t;

typedef struct
{
   uint8_t bitmap[32];
} NotificationsBitmap_t;

#endif
