/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "NotificationsPlugin.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   // 0x01 Fresh_Food_Door_Alert
   // 0x02 Freezer_Door_Alert
   // 0x04 Filter_Order_Alert
   // 0x08 Filter_Replace_Alert
   // 0x10 Fresh_Food_High_Temperature_Alert
   // 0x20 Freezer_Temperature_Alert
   // 0x80 Freezer_Ice_Maker_Full_Alert
   SupportedNotificationsByte0 = 0xBF,
   // 0x01 Leak_Detected_Alert
   // 0x10 Freezer_Door_Opened_Alert
   SupportedNotificationsByte1 = 0x11,
};

static const NotificationsBitmap_t notificationsConfiguration = {
   .bitmap = { SupportedNotificationsByte0, SupportedNotificationsByte1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

TEST_GROUP(NotificationsPlugin)
{
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      NotificationsPlugin_Init(dataModel);
   }

   void TheNotificationsErdShouldBe(const NotificationsBitmap_t *notificationsBitmap)
   {
      NotificationsBitmap_t actualNotificationsBitmap;

      DataModel_Read(dataModel, Erd_NotificationsBitmap, &actualNotificationsBitmap);

      MEMCMP_EQUAL(notificationsBitmap, &actualNotificationsBitmap, sizeof(NotificationsBitmap_t));
   }
};

TEST(NotificationsPlugin, ShouldUpdateNotificationErdOnInit)
{
   GivenTheModuleIsInitialized();
   TheNotificationsErdShouldBe(&notificationsConfiguration);
}
