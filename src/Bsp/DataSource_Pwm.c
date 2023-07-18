/*!
 * @file
 * @brief MTU0 and MTU1 are synchronous to allow for MTU1.TGRA to be used as the cycle (frequency) register
 *        and allow all 3 of MTIOC0A, MTIOC0B, MTIOC0C on MTU0 and MTIOC1A, MTIOC1B on MTU1
 *        to be used on PWM Mode 2. MTIOC3A and MTIOC3C are configured on PWM Mode 1.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "DataSource_Pwm.h"
#include "I_Pwm.h"
#include "iodefine.h"
#include "uassert.h"
#include "Event_Null.h"
#include "InterruptPriorityLevel.h"
#include "utils.h"
#include "PwmDutyCycle.h"
#include "SystemClock.h"
#include "PwmFrequency.h"

#define IncludeTimerConfig_SetTimerConfig(_x) _x
#define IncludeTimerConfig_DontSetTimeConfig(_x)

#define IncludeMode1_1(_x) _x
#define IncludeMode1_2(_x)

#define IncludeMode2_1(_x)
#define IncludeMode2_2(_x) _x

#define IncludeTimer0_0(_x) _x
#define IncludeTimer0_1(_x)
#define IncludeTimer0_2(_x)
#define IncludeTimer0_3(_x)

#define IncludeTimer1_0(_x)
#define IncludeTimer1_1(_x) _x
#define IncludeTimer1_2(_x)
#define IncludeTimer1_3(_x)

#define IncludeTimer2_0(_x)
#define IncludeTimer2_1(_x)
#define IncludeTimer2_2(_x) _x
#define IncludeTimer2_3(_x)

#define IncludeTimer3_0(_x)
#define IncludeTimer3_1(_x)
#define IncludeTimer3_2(_x)
#define IncludeTimer3_3(_x) _x

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspPwm_Start - 1)
#define ERD_IS_IN_RANGE(erd) (BETWEEN(Erd_BspPwm_Start, erd, Erd_BspPwm_End))

#define PCLOCK_FREQUENCY (U32_PCLKB)

enum
{
   Mtu0Mtu1SynchronousFrequencyInHz = 25000,
   Mtu0Mtu1SynchronousFrequencyCount = PCLOCK_FREQUENCY / Mtu0Mtu1SynchronousFrequencyInHz,
   Mtu2FrequencyInHz = 25000,
   Mtu2FrequencyCount = PCLOCK_FREQUENCY / Mtu2FrequencyInHz,
};

#define IN_RANGE_GAP(left_gap, middle, right_gap, value) (IN_RANGE(middle - left_gap, value, middle + right_gap))

#define EXPAND_AS_SET_PIN_MODES(name, pwm, initialValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   PORT##port.PMR.BIT.B##bit = 0;

#define EXPAND_AS_SET_PIN_DIRECTIONS(name, pwm, initialValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   PORT##port.PDR.BIT.B##bit = 1;

#define EXPAND_AS_SET_PIN_DEFAULT_OUTPUTS(name, pwm, initialValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   PORT##port.PODR.BIT.B##bit = initialValue;

#define EXPAND_AS_SET_FUNCTIONS(name, pwm, initialValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   MPC.P##port##bit##PFS.BIT.PSEL = pinSelection;

#define EXPAND_AS_STOP_TIMER(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   CONCAT(IncludeMode2_, mode)                                                                                                        \
   (CONCAT(IncludeTimerConfig_, timerConfig)(MTU.TSTR.BIT.CST##timerNumber = 0;))

#define EXPAND_AS_SET_CLK_PRESCALER(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   CONCAT(IncludeMode2_, mode)                                                                                                               \
   (CONCAT(IncludeTimerConfig_, timerConfig)(MTU##timerNumber.TCR.BIT.TPSC = 0;))

#define EXPAND_AS_SET_COUNT_RISING_EDGE(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   CONCAT(IncludeMode2_, mode)                                                                                                                   \
   (CONCAT(IncludeTimerConfig_, timerConfig)(MTU##timerNumber.TCR.BIT.CKEG = 0;))

#define EXPAND_AS_SET_PWM_MODE2(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   CONCAT(IncludeMode2_, mode)                                                                                                           \
   (CONCAT(IncludeTimerConfig_, timerConfig)(MTU##timerNumber.TMDR.BIT.MD = 0x03;))

#define EXPAND_AS_INTERRUPT_NOT_USED(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   CONCAT(IncludeMode2_, mode)                                                                                                                \
   (CONCAT(IncludeTimerConfig_, timerConfig)(MTU##timerNumber.TIER.BYTE = 0;))

#define EXPAND_AS_SET_SYNC_OPERATION(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   CONCAT(IncludeMode2_, mode)                                                                                                                \
   (CONCAT(IncludeTimerConfig_, timerConfig)(MTU.TSYR.BIT.SYNC##timerNumber = 1;))

// clang-format off
#define EXPAND_AS_UPDATE_PWM_FUNCTION(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   void UpdatePwm##pwm(PwmDutyCycle_t dutyCycle)                                                                                               \
   {                                                                                                                                           \
      if(dutyCycle >= PwmDutyCycle_Max)                                                                                                        \
      {                                                                                                                                        \
         PORT##port.PODR.BIT.B##bit = 1;                                                                                                       \
         PORT##port.PMR.BIT.B##bit = 0;                                                                                                        \
      }                                                                                                                                        \
      else if(dutyCycle == PwmDutyCycle_Min)                                                                                                   \
      {                                                                                                                                        \
         PORT##port.PODR.BIT.B##bit = 0;                                                                                                       \
         PORT##port.PMR.BIT.B##bit = 0;                                                                                                        \
      }                                                                                                                                        \
      else                                                                                                                                     \
      {                                                                                                                                        \
         CONCAT(IncludeTimer2_, timerNumber)                                                                                                   \
         (                                                                                                                                     \
            MTU.TSTR.BIT.CST##timerNumber = 0;                                                                                                 \
            MTU##timerNumber.TGR##timerCarryFreqRegister = ConvertDutyCycleToCounts(Mtu2FrequencyCount, dutyCycle);                            \
         )                                                                                                                                     \
         CONCAT(IncludeMode2_, mode)                                                                                                           \
         (                                                                                                                                     \
            MTU##timerNumber.TGR##timerCarryFreqRegister = ConvertDutyCycleToCounts(Mtu0Mtu1SynchronousFrequencyCount, dutyCycle);             \
         )                                                                                                                                     \
         PORT##port.PMR.BIT.B##bit = 1;                                                                                                        \
      }                                                                                                                                        \
   }

// clang-format on

static PwmDutyCycle_t dutyCycles[OutputChannel_Pwm_Max];
static PwmFrequency_t pwmFrequency;

static void UpdatePwmVar_00(PwmFrequency_t frequency)
{
   if(frequency >= PwmFrequency_Max)
   {
      PORTC.PODR.BIT.B1 = 1;
      PORTC.PMR.BIT.B1 = 0;
   }
   else if(frequency == PwmFrequency_Min)
   {
      PORTC.PODR.BIT.B1 = 0;
      PORTC.PMR.BIT.B1 = 0;
   }
   else
   {
      // Frequency counts
      MTU3.TGRA = (PCLOCK_FREQUENCY / 64) / (frequency);
      // 50% duty cycle
      MTU3.TGRB = MTU3.TGRA / 2;

      // Set pin mode for peripheral function
      PORTC.PMR.BIT.B1 = 1;

      // Start counting
      MTU.TSTR.BIT.CST3 = 1;
   }
}

static void ConfigurePins(void)
{
   // Set up pins
   PWM_TABLE(EXPAND_AS_SET_PIN_MODES)
   PWM_TABLE(EXPAND_AS_SET_PIN_DEFAULT_OUTPUTS)
   PWM_TABLE(EXPAND_AS_SET_PIN_DIRECTIONS)

   // Enable writing to the PFSWE bit
   MPC.PWPR.BIT.B0WI = 0;

   // Enable writing to the PFS register
   MPC.PWPR.BIT.PFSWE = 1;

   PWM_TABLE(EXPAND_AS_SET_FUNCTIONS)

   // Disable writing to the PFS register
   MPC.PWPR.BIT.PFSWE = 0;

   // Disable writing to the PFSWE bit
   MPC.PWPR.BIT.B0WI = 1;
}

static void ConfigurePwmMode2(void)
{
   // PRCR write enabled
   SYSTEM.PRCR.WORD = 0xA50F;

   // Enable module clock MSTP(MTU)
   MSTP(MTU) = 0;

   // PRCR write disabled
   SYSTEM.PRCR.WORD = 0xA500;

   // Stop timers
   PWM_TABLE(EXPAND_AS_STOP_TIMER)

   // Timer based on PCLK
   PWM_TABLE(EXPAND_AS_SET_CLK_PRESCALER)

   // Count on rising edge
   PWM_TABLE(EXPAND_AS_SET_COUNT_RISING_EDGE)

   // TCNT cleared by TGRA compare match
   MTU1.TCR.BIT.CCLR = 0x01;

   // TCNT synchronous clearing
   MTU0.TCR.BIT.CCLR = 0x03;

   // Configure PWM 2 mode
   PWM_TABLE(EXPAND_AS_SET_PWM_MODE2)

   // Init fan polarity
   // Initially low - Low at TRGA compare match
   MTU1.TIOR.BIT.IOA = 0x01;

   // Initially low - High at TRGB compare match
   MTU1.TIOR.BIT.IOB = 0x02;

   // Configure frequency
   MTU1.TGRA = Mtu0Mtu1SynchronousFrequencyCount;

   // Initially low - High at TGRA compare match
   MTU0.TIORH.BIT.IOA = 0x02;

   // Initially low - High at TRGB compare match
   MTU0.TIORH.BIT.IOB = 0x02;

   // Initially low - High at TRGC compare match
   MTU0.TIORL.BIT.IOC = 0x02;

   // Initially low - High at TRGD compare match
   MTU0.TIORL.BIT.IOD = 0x02;

   // No interrupts used
   PWM_TABLE(EXPAND_AS_INTERRUPT_NOT_USED)

   // Perform synchronous operation
   PWM_TABLE(EXPAND_AS_SET_SYNC_OPERATION)
}

static void ConfigurePwmMode1ForMtu3(void)
{
   // PRCR write enabled
   SYSTEM.PRCR.WORD = 0xA50F;

   // Enable module clock MSTP(MTU)
   MSTP(MTU) = 0;

   // PRCR write disabled
   SYSTEM.PRCR.WORD = 0xA500;

   // Stop timers
   MTU.TSTR.BIT.CST3 = 0;

   // Timer based on PCLK/64
   MTU3.TCR.BIT.TPSC = 0x03;

   // Count on rising edge
   MTU3.TCR.BIT.CKEG = 0;

   // TCNT cleared by TGRA compare match
   MTU3.TCR.BIT.CCLR = 0x01;

   // Clear the counter
   MTU3.TCNT = 0;

   // Configure PWM 1 mode
   MTU3.TMDR.BIT.MD = 0x02;

   // Initially high - Low at TGRA compare match
   MTU3.TIORH.BIT.IOA = 0x05;

   // Initially high - High at TGRB compare match
   MTU3.TIORH.BIT.IOB = 0x06;

   // Init to off
   MTU3.TGRA = 0;
   MTU3.TGRB = 0;

   // Don't use interrupts
   MTU3.TIER.BYTE = 0x00;
}

/*!
 * Convert the desired duty cycle to counts for the hardware register
 * @param dutyCycle The scaled duty cycle percent 0xFFFF is equivalent to 100%
 */
static uint16_t ConvertDutyCycleToCounts(uint16_t frequencyCount, uint16_t dutyCycle)
{
   if(dutyCycle >= PwmDutyCycle_Max)
   {
      dutyCycle = PwmDutyCycle_Max;
   }

   dutyCycle = ((PwmDutyCycle_Max - dutyCycle) * frequencyCount) / PwmDutyCycle_Max;

   // Transit from OFF to ON without ramping.
   if(dutyCycle >= frequencyCount)
   {
      dutyCycle = frequencyCount + 1;
   }

   return dutyCycle;
}

static bool TimerNeedsToBeOnForChannel(uint8_t channel)
{
   return !((PwmDutyCycle_Min == dutyCycles[channel]) || (PwmDutyCycle_Max <= dutyCycles[channel]));
}

static bool TimerNeedsToBeOnForPwm_VAR_00()
{
   return !((PwmFrequency_Min == pwmFrequency) || (PwmFrequency_Max <= pwmFrequency));
}

static void StopTimersWhenPwmUnused(void)
{
   if(TimerNeedsToBeOnForChannel(OutputChannel_Pwm_25K_00) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_25K_01) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_25K_02) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_25K_03) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_25K_04) ||
      TimerNeedsToBeOnForPwm_VAR_00())
   {
      MTU.TSTR.BIT.CST0 = 1;
      MTU.TSTR.BIT.CST1 = 1;
      MTU.TSTR.BIT.CST2 = 1;
      MTU.TSTR.BIT.CST3 = 1;
   }
   else
   {
      MTU.TSTR.BIT.CST0 = 0;
      MTU.TSTR.BIT.CST1 = 0;
      MTU.TSTR.BIT.CST2 = 0;
      MTU.TSTR.BIT.CST3 = 0;
   }
}

PWM_TABLE(EXPAND_AS_UPDATE_PWM_FUNCTION)

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   PwmDutyCycle_t invertedDutyCycle;

   uassert(ERD_IS_IN_RANGE(erd));

   uint16_t channel = CHANNEL_FROM_ERD(erd);

   if(erd == Erd_BspPwm_PWM_VAR_00)
   {
      const PwmFrequency_t *frequency = data;
      pwmFrequency = *frequency;
      UpdatePwmVar_00(*frequency);
   }
   else
   {
      const PwmDutyCycle_t *dutyCycle = data;

      dutyCycles[channel] = *dutyCycle;
      invertedDutyCycle = PwmDutyCycle_Max - *dutyCycle;

      switch(erd)
      {
         case Erd_BspPwm_PWM_25K_00:
            UpdatePwm25K_00(invertedDutyCycle);
            break;

         case Erd_BspPwm_PWM_25K_01:
            UpdatePwm25K_01(invertedDutyCycle);
            break;

         case Erd_BspPwm_PWM_25K_02:
            UpdatePwm25K_02(invertedDutyCycle);
            break;

         case Erd_BspPwm_PWM_25K_03:
            UpdatePwm25K_03(invertedDutyCycle);
            break;

         case Erd_BspPwm_PWM_25K_04:
            UpdatePwm25K_04(invertedDutyCycle);
            break;
      }
   }
   StopTimersWhenPwmUnused();
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   if(erd == Erd_BspPwm_PWM_VAR_00)
   {
      PwmFrequency_t *frequency = data;
      *frequency = pwmFrequency;
      memcpy(data, frequency, sizeof(PwmFrequency_t));
   }
   else
   {
      PwmDutyCycle_t *dutyCycle = data;
      uint16_t channel = CHANNEL_FROM_ERD(erd);
      *dutyCycle = dutyCycles[channel];
      memcpy(data, dutyCycle, sizeof(PwmDutyCycle_t));
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
   uassert(ERD_IS_IN_RANGE(erd));

   if(erd == Erd_BspPwm_PWM_VAR_00)
   {
      return sizeof(PwmFrequency_t);
   }
   return sizeof(PwmDutyCycle_t);
}

static struct
{
   I_DataSource_t interface;
} instance;

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

I_DataSource_t *DataSource_Pwm_Init(void)
{
   instance.interface.api = &api;

   memset(dutyCycles, 0, sizeof(dutyCycles));

   ConfigurePins();
   ConfigurePwmMode2();
   ConfigurePwmMode1ForMtu3();

   return &instance.interface;
}
