/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CapTouchKeys.h"
#include "CapSenseStatus.h"
#include "CapSense.h"
#include "CapSensePollPeriodMsec.h"
#include "Reset.h"
#include "CapSenseConfiguration.h"
#include "utils.h"

#define EXPAND_AS_SIZE_ENUM(Channel, Key) \
   derp##Channel,

enum
{
   CAP_SENSE_CONFIGURATION(EXPAND_AS_SIZE_ENUM)
   WatchdogTotalKeys
};

enum
{
   KeyWatchdogTimeoutMsec = 5 * 60 * 1000,
   KeyWatchdogCounterLimit = KeyWatchdogTimeoutMsec / CapSensePollPeriodMsec,

   SensorResetNoMansLandTimeoutMsec = 5000,
   SensorResetNoMansLandCounterLimit = SensorResetNoMansLandTimeoutMsec / CapSensePollPeriodMsec,

   SensorResetAboveNoiseThresholdTimeoutMsec = 35000,
   SensorResetAboveNoiseThresholdCounterLimit = SensorResetAboveNoiseThresholdTimeoutMsec / CapSensePollPeriodMsec,

   AllowedBaselineDriftPercentageFromTarget = 30,

   CapTouchKeysPollPeriodMsec = 0
};

#define EXPAND_AS_CAP_SENSE_RAW_KEYS(Channel, Key) \
   CapSense_dsRam.snsList.u##Key[0].raw,

#define EXPAND_AS_CAP_SENSE_ID(Channel, Key) \
   CapSense_U##Key##_WDGT_ID,

typedef struct
{
   uint16_t lastRaw;
   uint16_t counter;
} KeyWatchdogState_t;

typedef struct
{
   uint16_t noMansLandCounter;
   uint16_t aboveNoiseThresholdCounter;
} SensorResetState_t;

static CapSenseStatus_t statusCapSense;
static KeyWatchdogState_t watchdogStateKeys[WatchdogTotalKeys];
static SensorResetState_t sensorResetState[CapSense_TOTAL_CSD_WIDGETS];
static TinyTimer_t pollTimer;
static uint16_t lastWord;

static bool baselineHasDriftedOutOfBounds = false;

static const uint16_t *const capSenseRawKeys[] =
   {
      CAP_SENSE_CONFIGURATION(EXPAND_AS_CAP_SENSE_RAW_KEYS)
   };

static const uint32_t capSenseId[] =
   {
      CAP_SENSE_CONFIGURATION(EXPAND_AS_CAP_SENSE_ID)
   };

static void StartTimer(TinyTimerModule_t *timerModule);

static void KeyWatchdogUpdate(void)
{
   for(uint8_t key = 0; key < WatchdogTotalKeys; key++)
   {
      if(*capSenseRawKeys[key] != watchdogStateKeys[key].lastRaw)
      {
         watchdogStateKeys[key].counter = 0;
         watchdogStateKeys[key].lastRaw = *capSenseRawKeys[key];
      }
      else
      {
         watchdogStateKeys[key].counter++;

         if(watchdogStateKeys[key].counter >= KeyWatchdogCounterLimit)
         {
            Reset();
         }
      }
   }
}

static void PollCapSense(void)
{
   statusCapSense.word = 0;

   for(int8_t key = 0; key < WatchdogTotalKeys; key++)
   {
      uint32_t capSenseState = CapSense_IsWidgetActive(capSenseId[key]);
      if(capSenseState)
      {
         BIT_SET(statusCapSense.word, key);
      }
   }
}

static void SensorResetUpdate(void)
{
   baselineHasDriftedOutOfBounds = false;

   for(uint8_t i = 0; i < CapSense_TOTAL_CSX_WIDGETS; i++)
   {
      CapSense_THRESHOLD_TYPE diff = CapSense_dsFlash.wdgtArray[i].ptr2SnsRam->diff;
      CapSense_RAM_WD_BUTTON_STRUCT *wdgt = CapSense_dsFlash.wdgtArray[i].ptr2WdgtRam;
      uint8 noiseTh = wdgt->noiseTh;
      CapSense_THRESHOLD_TYPE fingerTh = wdgt->fingerTh;
      uint8 hysteresis = wdgt->hysteresis;
      uint16 baseline = CapSense_dsFlash.wdgtArray[i].ptr2SnsRam->bsln[0];
      uint16 rawCounts = CapSense_dsFlash.wdgtArray[i].ptr2SnsRam->raw[0];
      uint32_t maximumCounts = ((1 << wdgt->resolution) - 1);
      uint16_t targetBaseline = (uint16_t)(maximumCounts * CapSense_CSD_RAWCOUNT_CAL_LEVEL / 100);
      uint16_t minBaseline = (uint16_t)(targetBaseline - ((uint32_t)targetBaseline * AllowedBaselineDriftPercentageFromTarget / 100));
      uint16_t maxBaseline = (uint16_t)(targetBaseline + ((uint32_t)targetBaseline * AllowedBaselineDriftPercentageFromTarget / 100));

      if((baseline < minBaseline) || (baseline > maxBaseline))
      {
         baselineHasDriftedOutOfBounds = true;
         break;
      }

      if((diff > noiseTh) && (diff < (fingerTh + hysteresis)))
      {
         if(sensorResetState[i].noMansLandCounter++ >= SensorResetNoMansLandCounterLimit)
         {
            sensorResetState[i].noMansLandCounter = 0;
            CapSense_InitializeSensorBaseline(i, 0);
         }
      }
      else if(sensorResetState[i].noMansLandCounter > 0)
      {
         sensorResetState[i].noMansLandCounter--;
      }

      if(diff > noiseTh)
      {
         if(sensorResetState[i].aboveNoiseThresholdCounter++ >= SensorResetAboveNoiseThresholdCounterLimit)
         {
            sensorResetState[i].aboveNoiseThresholdCounter = 0;
            CapSense_InitializeSensorBaseline(i, 0);
         }
      }
      else if(sensorResetState[i].aboveNoiseThresholdCounter > 0)
      {
         sensorResetState[i].aboveNoiseThresholdCounter--;
      }
   }
}

bool CapTouchKeys_BaselineHasDriftedOutOfBounds(void)
{
   return baselineHasDriftedOutOfBounds;
}

static void ReadCapTouchKeys(void)
{
   if(CapSense_NOT_BUSY == CapSense_IsBusy())
   {
      statusCapSense.valid = true;

      CapSense_ProcessAllWidgets();

      PollCapSense();
      KeyWatchdogUpdate();
      SensorResetUpdate();

      CapSense_ScanAllWidgets();
   }
   else
   {
      statusCapSense.valid = false;
   }
}

static void PollCapTouchKeys(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);

   ReadCapTouchKeys();

   if(statusCapSense.valid && (statusCapSense.word != lastWord))
   {
      // fixme write to data source or something
      lastWord = statusCapSense.word;
   }

   StartTimer(timerModule);
}

static void StartTimer(TinyTimerModule_t *timerModule)
{
   TinyTimerModule_Start(timerModule, &pollTimer, CapTouchKeysPollPeriodMsec, PollCapTouchKeys, NULL);
}

void CapTouchKeys_Init(TinyTimerModule_t *timerModule)
{
   statusCapSense.valid = false;
   statusCapSense.word = 0;
   CapSense_Start();
   CapSense_ScanAllWidgets();
   StartTimer(timerModule);
}
