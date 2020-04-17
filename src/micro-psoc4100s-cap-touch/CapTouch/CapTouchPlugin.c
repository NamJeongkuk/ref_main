/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include "CapTouchPlugin.h"
#include "CapTouchPluginConfiguration.h"
#include "CapTouch.h"
#include "Reset.h"
#include "utils.h"

#define EXPAND_AS_SIZE_ENUM(Channel, Key) \
   derp##Channel,

enum
{
   CAP_TOUCH_KEYS(EXPAND_AS_SIZE_ENUM)
   CapTouchKeyCount
};

enum
{
   KeyWatchdogTimeoutMsec = 5 * 60 * 1000,
   KeyWatchdogCounterLimit = KeyWatchdogTimeoutMsec / CapTouchPluginPollPeriodMsec,

   SensorResetNoMansLandTimeoutMsec = 5000,
   SensorResetNoMansLandCounterLimit = SensorResetNoMansLandTimeoutMsec / CapTouchPluginPollPeriodMsec,

   SensorResetAboveNoiseThresholdTimeoutMsec = 35000,
   SensorResetAboveNoiseThresholdCounterLimit = SensorResetAboveNoiseThresholdTimeoutMsec / CapTouchPluginPollPeriodMsec,

   AllowedBaselineDriftPercentageFromTarget = 30,

   CapTouchKeysPollPeriodMsec = 0
};

#define EXPAND_AS_CAP_SENSE_RAW_KEYS(Channel, Key) \
   CapTouch_dsRam.snsList.u##Key[0].raw,

#define EXPAND_AS_CAP_SENSE_ID(Channel, Key) \
   CapTouch_U##Key##_WDGT_ID,

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

typedef struct
{
   bool valid;
   uint16_t word;
} CapTouchStatus_t;

static I_TinyDataSource_t *dataSource;
static CapTouchStatus_t capTouchStatus;
static KeyWatchdogState_t watchdogStateKeys[CapTouchKeyCount];
static SensorResetState_t sensorResetState[CapTouch_TOTAL_CSD_WIDGETS];
static TinyTimer_t pollTimer;
static uint16_t lastWord;

static bool baselineHasDriftedOutOfBounds = false;

static const uint16_t *const capTouchRawKeys[] =
   {
      CAP_TOUCH_KEYS(EXPAND_AS_CAP_SENSE_RAW_KEYS)
   };

static const uint32_t capTouchId[] =
   {
      CAP_TOUCH_KEYS(EXPAND_AS_CAP_SENSE_ID)
   };

static void StartTimer(TinyTimerModule_t *timerModule);

static void KeyWatchdogUpdate(void)
{
   for(uint8_t key = 0; key < CapTouchKeyCount; key++)
   {
      if(*capTouchRawKeys[key] != watchdogStateKeys[key].lastRaw)
      {
         watchdogStateKeys[key].counter = 0;
         watchdogStateKeys[key].lastRaw = *capTouchRawKeys[key];
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

static void PollCapTouch(void)
{
   capTouchStatus.word = 0;

   for(int8_t key = 0; key < CapTouchKeyCount; key++)
   {
      uint32_t capTouchState = CapTouch_IsWidgetActive(capTouchId[key]);
      if(capTouchState)
      {
         BIT_SET(capTouchStatus.word, key);
      }
   }
}

static void SensorResetUpdate(void)
{
   baselineHasDriftedOutOfBounds = false;

   for(uint8_t i = 0; i < CapTouch_TOTAL_CSX_WIDGETS; i++)
   {
      CapTouch_THRESHOLD_TYPE diff = CapTouch_dsFlash.wdgtArray[i].ptr2SnsRam->diff;
      CapTouch_RAM_WD_BUTTON_STRUCT *wdgt = CapTouch_dsFlash.wdgtArray[i].ptr2WdgtRam;
      uint8 noiseTh = wdgt->noiseTh;
      CapTouch_THRESHOLD_TYPE fingerTh = wdgt->fingerTh;
      uint8 hysteresis = wdgt->hysteresis;
      uint16 baseline = CapTouch_dsFlash.wdgtArray[i].ptr2SnsRam->bsln[0];
      uint16 rawCounts = CapTouch_dsFlash.wdgtArray[i].ptr2SnsRam->raw[0];
      uint32_t maximumCounts = ((1 << wdgt->resolution) - 1);
      uint16_t targetBaseline = (uint16_t)(maximumCounts * CapTouch_CSD_RAWCOUNT_CAL_LEVEL / 100);
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
            CapTouch_InitializeSensorBaseline(i, 0);
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
            CapTouch_InitializeSensorBaseline(i, 0);
         }
      }
      else if(sensorResetState[i].aboveNoiseThresholdCounter > 0)
      {
         sensorResetState[i].aboveNoiseThresholdCounter--;
      }
   }
}

// fixme do this internally
// bool CapTouchKeys_BaselineHasDriftedOutOfBounds(void)
// {
//    return baselineHasDriftedOutOfBounds;
// }

static void ReadCapTouchKeys(void)
{
   if(CapTouch_NOT_BUSY == CapTouch_IsBusy())
   {
      capTouchStatus.valid = true;

      CapTouch_ProcessAllWidgets();

      PollCapTouch();
      KeyWatchdogUpdate();
      SensorResetUpdate();

      CapTouch_ScanAllWidgets();
   }
   else
   {
      capTouchStatus.valid = false;
   }
}

static void PollCapTouchKeys(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);

   ReadCapTouchKeys();

   if(capTouchStatus.valid && (capTouchStatus.word != lastWord))
   {
      // fixme write to data source or something
      lastWord = capTouchStatus.word;
   }

   StartTimer(timerModule);
}

static void StartTimer(TinyTimerModule_t *timerModule)
{
   TinyTimerModule_Start(timerModule, &pollTimer, CapTouchKeysPollPeriodMsec, PollCapTouchKeys, NULL);
}

void CapTouchPlugin_Init(I_TinyDataSource_t *_dataSource)
{
   dataSource = _dataSource;
   capTouchStatus.valid = false;
   capTouchStatus.word = 0;

   CapTouch_Start();
   CapTouch_ScanAllWidgets();

   TinyTimerModule_t *timerModule;
   TinyDataSource_Read(dataSource, CapTouchTimerModuleErd, &timerModule);
   StartTimer(timerModule);
}
