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

#define ERD_IS_IN_RANGE(erd) (BETWEEN(Erd_BspInputCapture_Start, erd, Erd_BspInputCapture_End))

#define CHANNEL_FROM_INPUT_ERD(erd) (erd - Erd_BspInputCapture_InputErd_Start - 1)
#define ERD_IS_IN_INPUT_ERD_RANGE(erd) \
   (BETWEEN(Erd_BspInputCapture_InputErd_Start, erd, Erd_BspInputCapture_InputErd_End))

#define ERD_IS_IN_INPUT_MOTOR_ERD_RANGE(erd) \
   (BETWEEN(Erd_BspInputCapture_InputErd_Motor_Start, erd, Erd_BspInputCapture_InputErd_Motor_End))

#define ERD_IS_IN_INPUT_FAN_ERD_RANGE(erd) \
   (BETWEEN(Erd_BspInputCapture_InputErd_Fan_Start, erd, Erd_BspInputCapture_InputErd_Fan_End))

#define CHANNEL_FROM_OUTPUT_ERD(erd) (erd - Erd_BspInputCapture_OutputErd_Start - 1)
#define ERD_IS_IN_OUTPUT_ERD_RANGE(erd) \
   (BETWEEN(Erd_BspInputCapture_OutputErd_Start, erd, Erd_BspInputCapture_OutputErd_End))

#define EXPAND_AS_CHANNEL_ENUM(inputName, port, bit, timer, channel, type) \
   Channel_##inputName,

#define EXPAND_AS_INTERRUPT_PENDING_REGISTER(inputName, port, bit, timer, channel, type) \
   &ICU.IR[IR_MTU##timer##_TGI##channel##timer].BYTE,

#define EXPAND_AS_INPUT_CAPTURE_COUNTS_REGISTER(inputName, port, bit, timer, channel, type) \
   &MTU##timer.TGR##channel,

typedef uint32_t MotorAngularDegreesx100_t;

enum
{
   Mtu4_RisingEdgeCapture = 0x08,
   Mtu4_FallingEdgeCapture = 0x09,
   Mtu4_BothEdgesCapture = 0x0B,
   Mtu4_MaxTgrCount = 0xFFFF,
   OverflowCounts = Mtu4_MaxTgrCount + 1,
   CountCutoffCloseToOverflow = 0xD999,
};

enum
{
   Mtu5_RisingEdgeCapture = 0x11
};

enum
{
   MaxOverflows = 17, // this is a made up number, need a real one at some point
   CountWhenMaxOverflowsReached = MaxOverflows * UINT16_MAX,
   InputCaptureOffMicroseconds = (0xFFFFFFFF / 2),
   InputCapturePollPeriodInMsec = 10,
   InputCapturePollSize = 1,
   PollAngularDegreesInMsec = 10,
   FilterWindowSize = 5
};

enum
{
   DegreesPerCountx10000 = 1636
};

static volatile unsigned char const *interruptPendingRegister[] = {
   INPUT_CAPTURE_TABLE(EXPAND_AS_INTERRUPT_PENDING_REGISTER)
};

static volatile unsigned short const *inputCaptureCountsRegister[] = {
   INPUT_CAPTURE_TABLE(EXPAND_AS_INPUT_CAPTURE_COUNTS_REGISTER)
};

typedef struct
{
   Filter_SimpleMovingAverage_t filter;

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
   volatile uint16_t motorFeedbackCounts;
   FeedbackData_t feedbackData[Channel_InputCapture_Max];
   Timer_t timer;
   Timer_t pollAngularDegreesTimer;
   MotorAngularDegreesx100_t previousDegrees;
   Event_Synchronous_t *onChangeEvent;
} instance;

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

static inline bool InterruptPending(uint8_t channel)
{
   return !!*interruptPendingRegister[channel];
}

static inline bool LastCaptureWasJustBeforeAnOverflow(uint8_t channel)
{
   return *inputCaptureCountsRegister[channel] >= CountCutoffCloseToOverflow;
}

void MTU4_TGIC4(void) __attribute__((interrupt));
void MTU4_TGIC4(void)
{
   if(instance.motorFeedbackCounts != UINT16_MAX)
   {
      instance.motorFeedbackCounts += 1;
   }
}

void MTU4_TGIB4(void) __attribute__((interrupt));
void MTU4_TGIB4(void)
{
   InputCaptureCompareMatch(Channel_Erd_BspFanInputCapture_CAPT_1, MTU4.TGRB);
}

void MTU4_TGIA4(void) __attribute__((interrupt));
void MTU4_TGIA4(void)
{
   InputCaptureCompareMatch(Channel_Erd_BspFanInputCapture_CAPT_2, MTU4.TGRA);
}

void MTU4_TCIV4(void) __attribute__((interrupt));
void MTU4_TCIV4(void)
{
   // Update if it overflowed
   for(uint8_t channel = 0; channel < Channel_InputCapture_Max; channel++)
   {
      if(InterruptPending(channel) && LastCaptureWasJustBeforeAnOverflow(channel))
      {
         // This lets us know that we ignored an overflow that occurred after the capture
         // completed and that we should account for that on the next capture
         instance.feedbackData[channel].isr.pendingOverflow = true;
      }
      else if(instance.feedbackData[channel].isr.overflows <= MaxOverflows)
      {
         instance.feedbackData[channel].isr.overflows++;
      }

      if(!instance.feedbackData[channel].shared.dataReady)
      {
         if(instance.feedbackData[channel].isr.overflows >= MaxOverflows)
         {
            instance.feedbackData[channel].isr.overflows = 0;
            instance.feedbackData[channel].isr.pendingOverflow = false;
            instance.feedbackData[channel].shared.feedbackCounts = MaxOverflows * (InputCaptureCounts_t)OverflowCounts;
            instance.feedbackData[channel].shared.dataReady = true;
         }
      }
   }
}

void MTU5_TGIU5(void) __attribute__((interrupt));
void MTU5_TGIU5(void)
{
   InputCaptureCompareMatch(Channel_Erd_BspFanInputCapture_CAPT_5, MTU5.TGRU);
}

void MTU5_TGIV5(void) __attribute__((interrupt));
void MTU5_TGIV5(void)
{
   InputCaptureCompareMatch(Channel_Erd_BspFanInputCapture_CAPT_4, MTU5.TGRV);
}

#define EXPAND_AS_GENERAL_INPUT_PIN(inputName, port, bit, timer, channel, type) \
   PORT##port.PDR.BIT.B##bit = 0;                                               \
   PORT##port.PMR.BIT.B##bit = 0;

#define EXPAND_AS_FUNCTION_SELECTION(inputName, port, bit, timer, channel, type) \
   MPC.P##port##bit##PFS.BIT.PSEL = 0x01;

#define EXPAND_AS_PERIPHERAL_PIN(inputName, port, bit, timer, channel, type) \
   PORT##port.PMR.BIT.B##bit = 1;

static void ConfigureInputCapture(void)
{
   SYSTEM.PRCR.WORD = U16_PRCR_ENABLE;
   MSTP(MTU) = 0; // enable MTU
   SYSTEM.PRCR.WORD = U16_PRCR_DISABLE;

   // Stop the timers
   MTU.TSTR.BIT.CST4 = DISABLED;
   MTU5.TSTR.BIT.CSTU5 = DISABLED;
   MTU5.TSTR.BIT.CSTV5 = DISABLED;

   // Disable interrupts
   IEN(MTU4, TGIA4) = 0;
   IEN(MTU4, TGIB4) = 0;
   IEN(MTU4, TGIC4) = 0;
   IEN(MTU4, TCIV4) = 0;
   IEN(MTU5, TGIU5) = 0;
   IEN(MTU5, TGIV5) = 0;

   MTU4.TIER.BIT.TGIEA = 0;
   MTU4.TIER.BIT.TGIEB = 0;
   MTU4.TIER.BIT.TGIEC = 0;
   MTU4.TIER.BIT.TCIEV = 0;
   MTU5.TIER.BIT.TGIE5U = 0;
   MTU5.TIER.BIT.TGIE5V = 0;

   // Timer based on PCLK (32 MHz) / 16
   MTU4.TCR.BIT.TPSC = 2;
   MTU5.TCRU.BIT.TPSC = 2;
   MTU5.TCRV.BIT.TPSC = 2;

   // Set Timer I/O control register to rising edge capture
   MTU4.TIORH.BIT.IOA = Mtu4_RisingEdgeCapture;
   MTU4.TIORH.BIT.IOB = Mtu4_RisingEdgeCapture;
   MTU4.TIORL.BIT.IOC = Mtu4_RisingEdgeCapture;
   MTU5.TIORU.BIT.IOC = Mtu5_RisingEdgeCapture;
   MTU5.TIORV.BIT.IOC = Mtu5_RisingEdgeCapture;

   // Count on Rising Edge
   MTU4.TCR.BIT.CKEG = 0;

   // TCNT clearing disabled
   MTU4.TCR.BIT.CCLR = 0x00;
   MTU5.TCNTCMPCLR.BIT.CMPCLR5U = 0x00;
   MTU5.TCNTCMPCLR.BIT.CMPCLR5V = 0x00;

   // Clear counters
   MTU4.TCNT = 0;
   MTU5.TCNTU = 0;
   MTU5.TCNTV = 0;
   MTU4.TGRA = 0;
   MTU4.TGRB = 0;
   MTU4.TGRC = 0;
   MTU5.TGRU = 0;
   MTU5.TGRV = 0;

   // Normal Operating Mode
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
   IPR(MTU4, TGIA4) = 13;
   IPR(MTU4, TGIB4) = 13;
   IPR(MTU4, TGIC4) = 13;
   IPR(MTU4, TCIV4) = 14; // Must have higher priority to handle when overflow and capture interrupts fire at the same time
   IPR(MTU5, TGIU5) = 13;
   IPR(MTU5, TGIV5) = 13;

   // Clear interrupt requests
   IR(MTU4, TGIA4) = 0;
   IR(MTU4, TGIB4) = 0;
   IR(MTU4, TGIC4) = 0;
   IR(MTU4, TCIV4) = 0;
   IR(MTU5, TGIU5) = 0;
   IR(MTU5, TGIV5) = 0;

   // Enable interrupts
   MTU4.TIER.BIT.TGIEA = 1;
   MTU4.TIER.BIT.TGIEB = 1;
   MTU4.TIER.BIT.TGIEC = 1;
   MTU4.TIER.BIT.TCIEV = 1;
   MTU5.TIER.BIT.TGIE5U = 1;
   MTU5.TIER.BIT.TGIE5V = 1;

   IEN(MTU4, TGIA4) = 1;
   IEN(MTU4, TGIB4) = 1;
   IEN(MTU4, TGIC4) = 1;
   IEN(MTU4, TCIV4) = 1;
   IEN(MTU5, TGIU5) = 1;
   IEN(MTU5, TGIV5) = 1;

   // Start the timer
   MTU.TSTR.BIT.CST4 = ENABLED;
   MTU5.TSTR.BIT.CSTU5 = ENABLED;
   MTU5.TSTR.BIT.CSTV5 = ENABLED;
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);

   if(ERD_IS_IN_INPUT_FAN_ERD_RANGE(erd))
   {
      // PCLK is 32 MHz, input capture clock is 32/16 = 2 MHz
      // 1 count = 0.5 × 10^-6
      // 1 micro second = 2 counts
      REINTERPRET(totalTimeUs, data, InputCaptureMicroSeconds_t *);
      FanInputCaptureChannel_t channel = CHANNEL_FROM_INPUT_ERD(erd);

      bool filterReady;
      Input_Read(Filter_GetReadyInput(&instance.feedbackData[channel].filter.interface), &filterReady);

      if(filterReady)
      {
         InputCaptureCounts_t filteredCounts;
         Filter_Read(&instance.feedbackData[channel].filter.interface, &filteredCounts);
         // Convert counts into time depending on clock speed (/2 because PCLK is 32 MHz)
         *totalTimeUs = (InputCaptureMicroSeconds_t)(filteredCounts / 2);
      }
   }
   else if(ERD_IS_IN_INPUT_MOTOR_ERD_RANGE(erd))
   {
      REINTERPRET(degreesx100, data, MotorAngularDegreesx100_t *);

      *degreesx100 = instance.previousDegrees;
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   IGNORE(erd);

   if(ERD_IS_IN_INPUT_MOTOR_ERD_RANGE(erd))
   {
      REINTERPRET(degreesx100, data, const MotorAngularDegreesx100_t *);

      uassert(*degreesx100 == 0);
      uint16_t newMotorFeedbackCounts = *degreesx100;

      do
      {
         instance.motorFeedbackCounts = newMotorFeedbackCounts;
      } while(instance.motorFeedbackCounts != newMotorFeedbackCounts);

      instance.previousDegrees = 0;
   }
}

static bool Has(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return ERD_IS_IN_RANGE(erd);
}

static uint8_t SizeOf(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);

   if(ERD_IS_IN_INPUT_FAN_ERD_RANGE(erd))
   {
      return sizeof(InputCaptureMicroSeconds_t);
   }
   else if(ERD_IS_IN_INPUT_MOTOR_ERD_RANGE(erd))
   {
      return sizeof(MotorAngularDegreesx100_t);
   }

   return sizeof(InputCaptureMode_t);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

static MotorAngularDegreesx100_t ReadMotorAngularDegrees(void)
{
   uint16_t current;
   uint16_t last;

   current = instance.motorFeedbackCounts;

   do
   {
      last = current;
      current = instance.motorFeedbackCounts;
   } while(last != current);

   return ((uint32_t)current * DegreesPerCountx10000) / 100;
}

static void PollAngularDegrees(void *context)
{
   IGNORE(context);

   MotorAngularDegreesx100_t data = ReadMotorAngularDegrees();

   if(data != instance.previousDegrees)
   {
      DataModelOnDataChangeArgs_t args = { Erd_BspFanInputCapture_CAPT_0, &data };
      Event_Synchronous_Publish(instance.onChangeEvent, &args);
      instance.previousDegrees = data;
   };
}

I_DataSource_t *DataSource_InputCapture_Init(
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent)
{
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

      Filter_SimpleMovingAverage_Init(
         &instance.feedbackData[channel].filter,
         FilterWindowSize,
         IS_SIGNED(InputCaptureCounts_t),
         sizeof(InputCaptureCounts_t));

      InputCaptureMicroSeconds_t seed = (InputCaptureMicroSeconds_t)InputCaptureOffMicroseconds;
      Filter_Seed(&instance.feedbackData[channel].filter.interface, &seed);
   }

   TimerModule_StartPeriodic(
      timerModule,
      &instance.timer,
      InputCapturePollPeriodInMsec,
      PollInputCapture,
      NULL);

   TimerModule_StartPeriodic(
      timerModule,
      &instance.pollAngularDegreesTimer,
      PollAngularDegreesInMsec,
      PollAngularDegrees,
      &instance);

   return &instance.interface;
}
