/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "AndroidSignOfLifeMonitor.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   TimeToSetFaultForConsecutiveMissesInMsec = 5 * MSEC_PER_SEC,
   MoreThanEnoughTimeToSetFaultInFaultSettingConditions = 2 * TimeToSetFaultForConsecutiveMissesInMsec,
   DiscoveryTimeMsec = 3 * MSEC_PER_SEC,
   SignOfLifeTimeBetweenIncrementsInMsec = 1 * MSEC_PER_SEC
};

TEST_GROUP(AndroidSignOfLifeMonitor)
{
   AndroidSignOfLifeMonitor_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks);
   }

   void GivenInitialization()
   {
      AndroidSignOfLifeMonitor_Init(&instance, dataModel);
   }

   void GivenAndroidSbcIsDiscoveredInTheSystem()
   {
      DataModel_Write(dataModel, Erd_AndroidSbcBoardInSystem, set);
   }

   void WhenAndroidSbcIsDiscoveredInTheSystem()
   {
      DataModel_Write(dataModel, Erd_AndroidSbcBoardInSystem, set);
   }

   void WhenAndroidSbcIsRandomlyUndiscoveredInTheSystem()
   {
      DataModel_Write(dataModel, Erd_AndroidSbcBoardInSystem, clear);
   }

   void AndroidSignOfLifeFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AndroidSignOfLifeDebugFault, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheSignOfLifeIncrements()
   {
      Signal_t value;
      DataModel_Read(dataModel, Erd_PrimaryUiSignOfLife, &value);
      value++;
      DataModel_Write(dataModel, Erd_PrimaryUiSignOfLife, &value);
   }

   void WhenSignOfLifeIncrements()
   {
      TheSignOfLifeIncrements();
   }

   void WhenSignOfLifeIncrementsEverySecondForTwiceTheFaultSettingTime()
   {
      for(uint8_t i = 1; i <= ((TimeToSetFaultForConsecutiveMissesInMsec / 1000) * 2); i++)
      {
         After(SignOfLifeTimeBetweenIncrementsInMsec);
         TheSignOfLifeIncrements();
      }
   }
};

TEST(AndroidSignOfLifeMonitor, ShouldNeverSetFaultIfAndroidIsNotDiscovered)
{
   GivenInitialization();
   After(MoreThanEnoughTimeToSetFaultInFaultSettingConditions);
   AndroidSignOfLifeFaultShouldBe(false);
}

TEST(AndroidSignOfLifeMonitor, ShouldSetAndroidFaultWhenTheSignOfLifeDoesNotChangeInTheSpecifiedDurationWhenBoardIsDiscoveredPriorToInitialization)
{
   GivenAndroidSbcIsDiscoveredInTheSystem();
   GivenInitialization();
   After(TimeToSetFaultForConsecutiveMissesInMsec - 1);
   AndroidSignOfLifeFaultShouldBe(false);

   After(1);
   AndroidSignOfLifeFaultShouldBe(true);
}

TEST(AndroidSignOfLifeMonitor, ShouldSetAndroidFaultWhenTheSignOfLifeDoesNotChangeInTheSpecifiedDurationWhenBoardIsDiscoveredAfterInitialization)
{
   GivenInitialization();

   After(DiscoveryTimeMsec);
   WhenAndroidSbcIsDiscoveredInTheSystem();

   After(TimeToSetFaultForConsecutiveMissesInMsec - 1);
   AndroidSignOfLifeFaultShouldBe(false);

   After(1);
   AndroidSignOfLifeFaultShouldBe(true);
}

TEST(AndroidSignOfLifeMonitor, ShouldNotSetFaultIfSignOfLifeIsProperlyIncrementingEverySecond)
{
   GivenAndroidSbcIsDiscoveredInTheSystem();
   GivenInitialization();

   WhenSignOfLifeIncrementsEverySecondForTwiceTheFaultSettingTime();
   AndroidSignOfLifeFaultShouldBe(false);
}

TEST(AndroidSignOfLifeMonitor, ShouldClearFaultIfPreviouslySetWhenSignOfLifeIncrements)
{
   GivenAndroidSbcIsDiscoveredInTheSystem();
   GivenInitialization();

   After(TimeToSetFaultForConsecutiveMissesInMsec);
   AndroidSignOfLifeFaultShouldBe(true);

   WhenSignOfLifeIncrements();
   AndroidSignOfLifeFaultShouldBe(false);
}

TEST(AndroidSignOfLifeMonitor, ShouldNotPayAttentionToAndroidSbcInSystemChangingAndStillSetFaultCommaOnceDiscoveredAlwaysDiscovered)
{
   GivenAndroidSbcIsDiscoveredInTheSystem();
   GivenInitialization();

   After(TimeToSetFaultForConsecutiveMissesInMsec);
   AndroidSignOfLifeFaultShouldBe(true);

   WhenSignOfLifeIncrements();
   AndroidSignOfLifeFaultShouldBe(false);

   WhenAndroidSbcIsRandomlyUndiscoveredInTheSystem();

   After(TimeToSetFaultForConsecutiveMissesInMsec);
   AndroidSignOfLifeFaultShouldBe(true);
}
