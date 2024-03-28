/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "NotificationsPlugin.h"
#include "SystemErds.h"

enum
{
   Notifications_SupportedByte0 =
      GET_NOTIFICATION_BYTE_MASK(Notifications_SupportedFreshFoodDoorAlert) |
      GET_NOTIFICATION_BYTE_MASK(Notifications_SupportedFreezerDoorAlert) |
      GET_NOTIFICATION_BYTE_MASK(Notifications_SupportedFilterOrderAlert) |
      GET_NOTIFICATION_BYTE_MASK(Notifications_SupportedFilterReplaceAlert) |
      GET_NOTIFICATION_BYTE_MASK(Notifications_SupportedFreshFoodHighTemperatureAlert) |
      GET_NOTIFICATION_BYTE_MASK(Notifications_SupportedFreezerTemperatureAlert) |
      GET_NOTIFICATION_BYTE_MASK(Notifications_SupportedFreezerIceMakerFullAlert),
   Notifications_SupportedByte1 =
      GET_NOTIFICATION_BYTE_MASK(Notifications_SupportedLeakDetectedAlert) |
      GET_NOTIFICATION_BYTE_MASK(Notifications_SupportedFreezerDoorOpenedAlert)
};

void NotificationsPlugin_Init(I_DataModel_t *dataModel)
{
   NotificationsBitmap_t notificationsBitmap;

   memset(&notificationsBitmap, 0, sizeof(notificationsBitmap));

   notificationsBitmap.bitmap[0] = Notifications_SupportedByte0;
   notificationsBitmap.bitmap[1] = Notifications_SupportedByte1;

   DataModel_Write(dataModel, Erd_NotificationsBitmap, &notificationsBitmap);
}
