/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_InputCapture.h"
#include "Event_Null.h"
#include "Hardware.h"
#include "iodefine.h"
#include "Rx130Defines.h"
#include "utils.h"
#include "uassert.h"
#include "InputCaptureType.h"
#include "Constants_Binary.h"
#include "I_InputCapture.h"
#include "ContextProtector_Rx2xx.h"
#include "SystemErds.h"
#include "InputCapture_FeedbackCountCalculation.h"
#include "Filter_SimpleMovingAverage.h"

#ifndef OLD_HW

#define ERD_IS_IN_RANGE(erd) (BETWEEN(Erd_BspInputCapture_Start, erd, Erd_BspInputCapture_End))

#define ERD_IS_IN_PULSE_WIDTH_ERD_RANGE(erd) \
   (BETWEEN(Erd_BspInputCapture_Start, erd, Erd_BspInputCapture_End) && !(erd == Erd_BspInputCapture_CAPTURE_03))

#define ERD_IS_IN_PULSE_COUNT_ERD_RANGE(erd) \
   (erd == Erd_BspInputCapture_CAPTURE_03)

#define CHANNEL_FROM_INPUT_ERD(erd) (erd - Erd_BspInputCapture_InputErd_Start - 1)

#define CHANNEL_FROM_OUTPUT_ERD(erd) (erd - Erd_BspInputCapture_OutputErd_Start - 1)
#define ERD_IS_IN_OUTPUT_ERD_RANGE(erd) \
   (BETWEEN(Erd_BspInputCapture_OutputErd_Start, erd, Erd_BspInputCapture_OutputErd_End))

#define EXPAND_AS_CHANNEL_ENUM(inputName, port, bit, timer, channel, peripheralFunction) \
   Channel_##inputName,

#define EXPAND_AS_INTERRUPT_PENDING_REGISTER(inputName, port, bit, timer, channel, peripheralFunction) \
   &ICU.IR[IR_MTU##timer##_TGI##channel##timer].BYTE,

#define EXPAND_AS_INPUT_CAPTURE_COUNTS_REGISTER(inputName, port, bit, timer, channel, peripheralFunction) \
   &MTU##timer.TGR##channel,

enum
{
   Mtu2_RisingEdgeCapture = 0x08,
   Mtu4_RisingEdgeCapture = 0x08,
   Mtu4_FallingEdgeCapture = 0x09,
   Mtu4_BothEdgesCapture = 0x0B,
   Mtu4_MaxTgrCount = 0xFFFF,
   OverflowCounts = Mtu4_MaxTgrCount + 1,
   CountCutoffCloseToOverflow = 0xD999,
   Mtu5_RisingEdgeCapture = 0x11,

   InputCaptureOffMicroseconds = UINT32_MAX,
   InputCapturePollPeriodInMsec = 10,
   InputCapturePollCountToSetToOff = 10, // Maximum pulse length we can measure without overflow is 65ms, set this to 100ms second for margin.
   FilterWindowSize = 5
};

typedef struct
{
   Filter_SimpleMovingAverage_t filter;
   uint8_t pollCountWithoutCaptures;

   struct
   {
      uint16_t previousCaptureCounts;
      uint8_t overflows;
      bool firstCaptureCompleted;
      bool pendingOverflow;
   } isr;

   struct
   {
      volatile bool dataReady;
      volatile InputCaptureCounts_t feedbackCounts;
   } shared;
} FeedbackData_t;

// clang-format off
enum
{
   INPUT_CAPTURE_TABLE(EXPAND_AS_CHANNEL_ENUM)
   Channel_InputCapture_Max
};
typedef uint8_t FanInputCaptureChannel_t;
// clang-format on

typedef struct
{
   InputCaptureCounts_t counts;
} FanInputCaptureCache_t;

static struct
{
   I_DataSource_t interface;
   volatile uint16_t flowMeterFeedbackCounts;
   FeedbackData_t feedbackData[Channel_InputCapture_Max];
   Timer_t timer;
   Event_Synchronous_t *onChangeEvent;
} instance;

static void SeedInputCaptureFilterWithOffCounts(uint8_t channel)
{
   InputCaptureMicroSeconds_t seed = (InputCaptureMicroSeconds_t)InputCaptureOffMicroseconds;
   Filter_Seed(&instance.feedbackData[channel].filter.interface, &seed);
}

static void PollInputCapture(void *context)
{
   IGNORE(context);

   for(uint8_t channel = 0; channel < Channel_InputCapture_Max; channel++)
   {
      if(instance.feedbackData[channel].shared.dataReady)
      {
         InputCaptureCounts_t feedbackCounts = instance.feedbackData[channel].shared.feedbackCounts;

         Filter_Feed(&instance.feedbackData[channel].filter.interface, &feedbackCounts);

         instance.feedbackData[channel].shared.dataReady = false;
         instance.feedbackData[channel].pollCountWithoutCaptures = 0;
      }
      else
      {
         instance.feedbackData[channel].pollCountWithoutCaptures =
            TRUNCATE_UNSIGNED_ADDITION(instance.feedbackData[channel].pollCountWithoutCaptures, 1, UINT8_MAX);

         if(instance.feedbackData[channel].pollCountWithoutCaptures > InputCapturePollCountToSetToOff)
         {
            SeedInputCaptureFilterWithOffCounts(channel);
         }
      }
   }
}

static void InputCaptureCompareMatch(FanInputCaptureChannel_t channel, uint16_t currentCounts)
{
   InputCaptureCounts_t feedbackCounts = CalculateFeedbackCounts(
      instance.feedbackData[channel].isr.previousCaptureCounts,
      currentCounts,
      instance.feedbackData[channel].isr.overflows);

   instance.feedbackData[channel].isr.previousCaptureCounts = currentCounts;
   instance.feedbackData[channel].isr.overflows = instance.feedbackData[channel].isr.pendingOverflow;
   instance.feedbackData[channel].isr.pendingOverflow = false;

   if(instance.feedbackData[channel].isr.firstCaptureCompleted)
   {
      if(!instance.feedbackData[channel].shared.dataReady)
      {
         instance.feedbackData[channel].shared.feedbackCounts = feedbackCounts;
         instance.feedbackData[channel].shared.dataReady = true;
      }
   }
   else
   {
      instance.feedbackData[channel].isr.firstCaptureCompleted = true;
   }
}

void MTU4_TGIC4(void) __attribute__((interrupt));
void MTU4_TGIC4(void)
{
   InputCaptureCompareMatch(Channel_Erd_BspInputCapture_CAPTURE_04, MTU4.TGRC);
}

void MTU4_TGIA4(void) __attribute__((interrupt));
void MTU4_TGIA4(void)
{
   instance.flowMeterFeedbackCounts += 1;
}

void MTU5_TGIU5(void) __attribute__((interrupt));
void MTU5_TGIU5(void)
{
   InputCaptureCompareMatch(Channel_Erd_BspInputCapture_CAPTURE_00, MTU5.TGRU);
}

void MTU5_TGIV5(void) __attribute__((interrupt));
void MTU5_TGIV5(void)
{
   InputCaptureCompareMatch(Channel_Erd_BspInputCapture_CAPTURE_01, MTU5.TGRV);
}

void MTU5_TGIW5(void) __attribute__((interrupt));
void MTU5_TGIW5(void)
{
   InputCaptureCompareMatch(Channel_Erd_BspInputCapture_CAPTURE_02, MTU5.TGRW);
}

#define EXPAND_AS_GENERAL_INPUT_PIN(inputname, port, bit, timer, channel, peripheralFunction) \
   PORT##port.PDR.BIT.B##bit = 0;                                                             \
   PORT##port.PMR.BIT.B##bit = 0;

#define EXPAND_AS_FUNCTION_SELECTION(inputname, port, bit, timer, channel, peripheralFunction) \
   MPC.P##port##bit##PFS.BIT.PSEL = peripheralFunction;

#define EXPAND_AS_PERIPHERAL_PIN(inputname, port, bit, timer, channel, peripheralFunction) \
   PORT##port.PMR.BIT.B##bit = 1;

static void ConfigureInputCapture(void)
{
   SYSTEM.PRCR.WORD = U16_PRCR_ENABLE;
   MSTP(MTU) = 0; // enable MTU
   SYSTEM.PRCR.WORD = U16_PRCR_DISABLE;

   // Stop the timers
   MTU.TSTR.BIT.CST2 = DISABLED;
   MTU.TSTR.BIT.CST4 = DISABLED;
   MTU5.TSTR.BIT.CSTU5 = DISABLED;
   MTU5.TSTR.BIT.CSTV5 = DISABLED;
   MTU5.TSTR.BIT.CSTW5 = DISABLED;

   // Disable interrupts
   IEN(MTU2, TGIA2) = 0;
   IEN(MTU4, TGIA4) = 0;
   IEN(MTU4, TGIC4) = 0;
   IEN(MTU5, TGIU5) = 0;
   IEN(MTU5, TGIV5) = 0;
   IEN(MTU5, TGIW5) = 0;

   MTU2.TIER.BIT.TGIEA = 0;
   MTU4.TIER.BIT.TGIEA = 0;
   MTU4.TIER.BIT.TGIEC = 0;
   MTU5.TIER.BIT.TGIE5U = 0;
   MTU5.TIER.BIT.TGIE5V = 0;
   MTU5.TIER.BIT.TGIE5W = 0;

   // Timer based on PCLK (16 MHz) / 16
   MTU2.TCR.BIT.TPSC = 2;
   MTU4.TCR.BIT.TPSC = 2;
   MTU5.TCRU.BIT.TPSC = 2;
   MTU5.TCRV.BIT.TPSC = 2;
   MTU5.TCRW.BIT.TPSC = 2;

   // Set Timer I/O control register to rising edge capture
   MTU2.TIOR.BIT.IOA = Mtu2_RisingEdgeCapture;
   MTU4.TIORH.BIT.IOA = Mtu4_RisingEdgeCapture;
   MTU4.TIORH.BIT.IOB = Mtu4_RisingEdgeCapture;
   MTU4.TIORL.BIT.IOC = Mtu4_RisingEdgeCapture;
   MTU5.TIORU.BIT.IOC = Mtu5_RisingEdgeCapture;
   MTU5.TIORV.BIT.IOC = Mtu5_RisingEdgeCapture;
   MTU5.TIORW.BIT.IOC = Mtu5_RisingEdgeCapture;

   // Count on Rising Edge
   MTU2.TCR.BIT.CKEG = 0;
   MTU4.TCR.BIT.CKEG = 0;

   // TCNT clearing disabled
   MTU2.TCR.BIT.CCLR = 0x00;
   MTU4.TCR.BIT.CCLR = 0x00;
   MTU5.TCNTCMPCLR.BIT.CMPCLR5U = 0x00;
   MTU5.TCNTCMPCLR.BIT.CMPCLR5V = 0x00;
   MTU5.TCNTCMPCLR.BIT.CMPCLR5W = 0x00;

   // Clear counters
   MTU2.TCNT = 0;
   MTU4.TCNT = 0;
   MTU5.TCNTU = 0;
   MTU5.TCNTV = 0;
   MTU5.TCNTW = 0;
   MTU4.TGRA = 0;
   MTU4.TGRC = 0;
   MTU5.TGRU = 0;
   MTU5.TGRV = 0;
   MTU5.TGRW = 0;

   // Normal Operating Mode
   MTU2.TMDR.BIT.MD = 0x00;
   MTU4.TMDR.BIT.MD = 0x00;

   INPUT_CAPTURE_TABLE(EXPAND_AS_GENERAL_INPUT_PIN)

   // Set pin function
   MPC.PWPR.BIT.B0WI = 0; // enable writing PFSWE bit
   MPC.PWPR.BIT.PFSWE = 1; // enable writing PFS register

   INPUT_CAPTURE_TABLE(EXPAND_AS_FUNCTION_SELECTION)

   MPC.PWPR.BIT.PFSWE = 0; // disable writing PFS register
   MPC.PWPR.BIT.B0WI = 1; // disable writing PFSWE bit

   INPUT_CAPTURE_TABLE(EXPAND_AS_PERIPHERAL_PIN)

   // Set Priorities
   IPR(MTU2, TGIA2) = 13;
   IPR(MTU4, TGIA4) = 13;
   IPR(MTU4, TGIC4) = 13;
   IPR(MTU5, TGIU5) = 13;
   IPR(MTU5, TGIV5) = 13;
   IPR(MTU5, TGIW5) = 13;

   // Clear interrupt requests
   IR(MTU2, TGIA2) = 0;
   IR(MTU4, TGIA4) = 0;
   IR(MTU4, TGIC4) = 0;
   IR(MTU5, TGIU5) = 0;
   IR(MTU5, TGIV5) = 0;
   IR(MTU5, TGIW5) = 0;

   // Enable interrupts
   MTU2.TIER.BIT.TGIEA = 1;
   MTU4.TIER.BIT.TGIEA = 1;
   MTU4.TIER.BIT.TGIEC = 1;
   MTU5.TIER.BIT.TGIE5U = 1;
   MTU5.TIER.BIT.TGIE5V = 1;
   MTU5.TIER.BIT.TGIE5W = 1;

   IEN(MTU2, TGIA2) = 1;
   IEN(MTU4, TGIA4) = 1;
   IEN(MTU4, TGIC4) = 1;
   IEN(MTU5, TGIU5) = 1;
   IEN(MTU5, TGIV5) = 1;
   IEN(MTU5, TGIW5) = 1;

   // Start the timer
   MTU.TSTR.BIT.CST4 = ENABLED;
   MTU.TSTR.BIT.CST2 = ENABLED;
   MTU5.TSTR.BIT.CSTU5 = ENABLED;
   MTU5.TSTR.BIT.CSTV5 = ENABLED;
   MTU5.TSTR.BIT.CSTW5 = ENABLED;
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);

   if(ERD_IS_IN_PULSE_WIDTH_ERD_RANGE(erd))
   {
      InputCaptureMicroSeconds_t *totalTimeUs = data;
      FanInputCaptureChannel_t channel = CHANNEL_FROM_INPUT_ERD(erd);

      bool filterReady;
      Input_Read(Filter_GetReadyInput(&instance.feedbackData[channel].filter.interface), &filterReady);

      if(filterReady)
      {
         InputCaptureCounts_t filteredCounts;
         Filter_Read(&instance.feedbackData[channel].filter.interface, &filteredCounts);
         // PCLK is 16 MHz, input capture clock is 16/16 = 1 MHz
         // 1 count = 1 × 10^-6
         // 1 micro second = 1 count
         *totalTimeUs = (InputCaptureMicroSeconds_t)(filteredCounts);
      }
   }
   else if(ERD_IS_IN_PULSE_COUNT_ERD_RANGE(erd))
   {
      uint32_t *counts = data;
      uint16_t current;
      uint16_t last;

      current = instance.flowMeterFeedbackCounts;

      do
      {
         last = current;
         current = instance.flowMeterFeedbackCounts;
      } while(last != current);

      *counts = ((uint32_t)current);
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   IGNORE(erd);
   IGNORE(data);
}

static bool Has(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return ERD_IS_IN_RANGE(erd);
}

static uint8_t SizeOf(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);

   if(ERD_IS_IN_PULSE_WIDTH_ERD_RANGE(erd))
   {
      return sizeof(InputCaptureMicroSeconds_t);
   }
   else if(ERD_IS_IN_PULSE_COUNT_ERD_RANGE(erd))
   {
      return sizeof(InputCaptureCounts_t);
   }

   return sizeof(InputCaptureMode_t);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

#endif

I_DataSource_t *DataSource_InputCapture_Init(
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent)
{
#ifndef OLD_HW
   instance.interface.api = &api;
   instance.interface.OnDataChange = &onChangeEvent->interface;
   instance.onChangeEvent = onChangeEvent;

   ConfigureInputCapture();

   for(uint8_t channel = 0; channel < Channel_InputCapture_Max; channel++)
   {
      instance.feedbackData[channel].isr.previousCaptureCounts = 0;
      instance.feedbackData[channel].shared.feedbackCounts = 0;
      instance.feedbackData[channel].isr.overflows = 0;
      instance.feedbackData[channel].isr.firstCaptureCompleted = false;
      instance.feedbackData[channel].isr.pendingOverflow = false;
      instance.feedbackData[channel].shared.dataReady = false;
      instance.feedbackData[channel].pollCountWithoutCaptures = 0;

      Filter_SimpleMovingAverage_Init(
         &instance.feedbackData[channel].filter,
         FilterWindowSize,
         IS_SIGNED(InputCaptureCounts_t),
         sizeof(InputCaptureCounts_t));

      SeedInputCaptureFilterWithOffCounts(channel);
   }

   TimerModule_StartPeriodic(
      timerModule,
      &instance.timer,
      InputCapturePollPeriodInMsec,
      PollInputCapture,
      NULL);

   return &instance.interface;
#else
   IGNORE(timerModule);
   IGNORE(onChangeEvent);
   return NULL;
#endif
}
