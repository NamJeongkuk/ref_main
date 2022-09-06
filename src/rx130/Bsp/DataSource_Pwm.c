/*!
 * @file
 * @brief MTU0 and MTU1 are synchronous to allow for MTU1.TGRA to be used as the cycle (frequency) register
 *        and allow all 4 of MTIOC0A, MTIOC0B, MTIOC0C, MTIOC0D on MTU0 (and MTIOC1B on MTU1)
 *        to be used on PWM Mode 2. Other timers are unavailable to separate MTU0 on its own.
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

#define IncludeTimerConfig_SetTimerConfig(_x) _x
#define IncludeTimerConfig_DontSetTimeConfig(_x)

#define IncludeMode1_1(_x) _x
#define IncludeMode1_2(_x)

#define IncludeMode2_1(_x)
#define IncludeMode2_2(_x) _x

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

#define PCLOCK_FREQUENCY (32000000)

enum
{
   Mtu0Mtu1SynchronousFrequencyInHz = 25000,
   Mtu0Mtu1SynchronousFrequencyCount = PCLOCK_FREQUENCY / Mtu0Mtu1SynchronousFrequencyInHz,
   Mtu2FrequencyInHz = 25000,
   Mtu2FrequencyCount = PCLOCK_FREQUENCY / Mtu2FrequencyInHz,
   Mtu3FrequencyInHz = 2000,
   Mtu3FrequencyCount = PCLOCK_FREQUENCY / Mtu3FrequencyInHz,
   Mtu3RaceConditionLeftGap = 2, /* Heuristics value for detect race-condition that only for Mtu3FrequencyCount */
   Mtu3RaceConditionRightGap = 35, /* Heuristics value for detect race-condition that only for Mtu3FrequencyCount */
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

#define EXPAND_AS_UPDATE_PWM_BUFFER(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   CONCAT(IncludeTimer3_, timerNumber)                                                                                                       \
   (static uint16_t dutyBufferMtu##timerNumber##timerCarryFreqRegister = 0;)

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
         CONCAT(IncludeTimer3_, timerNumber)                                                                                                   \
         (  /* Copy to buffer, and interrupt function will update pwm */                                                                       \
            dutyBufferMtu##timerNumber##timerCarryFreqRegister = ConvertDutyCycleToCounts(Mtu3FrequencyCount, dutyCycle);                      \
            /* Enable interrupt temporarily */                                                                                                 \
            IEN(MTU##timerNumber, TGI##timerCarryFreqRegister##timerNumber) = 1;                                                               \
         )                                                                                                                                     \
         PORT##port.PMR.BIT.B##bit = 1;                                                                                                        \
      }                                                                                                                                        \
   }

#define EXPAND_AS_UPDATE_PWM_INTERRUPT_FUNCTION(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   CONCAT(IncludeTimer3_, timerNumber)                                                                                                                   \
   (                                                                                                                                                     \
      void CONCAT(MTU3_TGI##timerCarryFreqRegister, 3)(void) __attribute__((interrupt));                                                                 \
      void CONCAT(MTU3_TGI##timerCarryFreqRegister, 3)(void)                                                                                             \
      {                                                                                                                                                  \
         /* Need to store peripheral register to CPU register temporary. Because MTUx.TCNT is changing everytime. */                                     \
         uint16_t tcntBackup = MTU##timerNumber.TCNT;                                                                                                    \
         /* Detect race-condition between CPU core and peripheral */                                                                                     \
         /* By following equation (TCNT – 2 ≤ New TGR ≤ TCNT + 35), if the equation is true. Race condition can be occured. */                           \
         /* In details, While increasing TCNT is near to New TGR value. Peripheral (timer and match) cannot catch the updated value. */                  \
         /* Then failed to compare match the TCNT and TGR register. As a result, one period of PWM is dead. */                                           \
         /* Thus following branch state only allow to update PWM it's not under the race-condition situation. */                                         \
         if(!IN_RANGE_GAP(                                                                                                                               \
               Mtu3RaceConditionLeftGap,                                                                                                                 \
               dutyBufferMtu##timerNumber##timerCarryFreqRegister,                                                                                       \
               Mtu3RaceConditionRightGap,                                                                                                                \
               tcntBackup))                                                                                                                              \
         {                                                                                                                                               \
            /* Update PWM to MTUx.TGRy when it's not under the race-condition */                                                                         \
            MTU##timerNumber.TGR##timerCarryFreqRegister = dutyBufferMtu##timerNumber##timerCarryFreqRegister;                                           \
            /* Disable interrupt */                                                                                                                      \
            IEN(MTU##timerNumber, TGI##timerCarryFreqRegister##timerNumber) = 0;                                                                         \
         }                                                                                                                                               \
            /* When race-condition detected. Interrupt function don't execute above code in if state */                                                  \
            /* Don't update pwm to MTUx.TGRy when it's under the race-condition */                                                                       \
            /* Interrupt flag will keep enabled state. */                                                                                                \
            /* So interrupt function will retry to update pwm to MTUx.TGRy */                                                                            \
         }                                                                                                                                               \
   )
// clang-format on

static PwmDutyCycle_t dutyCycles[OutputChannel_Pwm_Max];

PWM_TABLE(EXPAND_AS_UPDATE_PWM_BUFFER)

PWM_TABLE(EXPAND_AS_UPDATE_PWM_INTERRUPT_FUNCTION)

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

static void ConfigurePwmMode1ForMtu2(void)
{
   // PRCR write enabled
   SYSTEM.PRCR.WORD = 0xA50F;

   // Enable module clock MSTP(MTU)
   MSTP(MTU) = 0;

   // PRCR write disabled
   SYSTEM.PRCR.WORD = 0xA500;

   // Stop timers
   MTU.TSTR.BIT.CST2 = 0;

   // Timer based on PCLK
   MTU2.TCR.BIT.TPSC = 0;

   // Count on rising edge
   MTU2.TCR.BIT.CKEG = 0;

   // TCNT cleared by TGRA compare match
   MTU2.TCR.BIT.CCLR = 0x01;

   // Configure PWM 1 mode
   MTU2.TMDR.BIT.MD = 0x02;

   // Init fan polarity
   // Initially low - Low at TRGA compare match
   MTU2.TIOR.BIT.IOA = 0x01;

   // Initially low - High at TRGB compare match
   MTU2.TIOR.BIT.IOB = 0x02;

   // Configure frequency
   MTU2.TGRA = Mtu2FrequencyCount;

   // No interrupts used
   MTU2.TIER.BYTE = 0;
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

   // Disable interrupts
   IEN(MTU3, TGIB3) = 0;
   IEN(MTU3, TGID3) = 0;
   MTU3.TIER.BYTE = 0x00;

   // Timer based on PCLK
   MTU3.TCR.BIT.TPSC = 0;

   // Count on rising edge
   MTU3.TCR.BIT.CKEG = 0;

   // TCNT cleared by TGRA compare match
   MTU3.TCR.BIT.CCLR = 0x01;

   // Clear the counter
   MTU3.TCNT = 0;

   // Configure PWM 1 mode
   MTU3.TMDR.BIT.MD = 0x02;

   // Initially low - Low at TGRA compare match
   MTU3.TIORH.BIT.IOA = 0x01;

   // Initially low - High at TGRB compare match
   MTU3.TIORH.BIT.IOB = 0x02;

   // Initially low - low at TGRC compare match
   MTU3.TIORL.BIT.IOC = 0x01;

   // Initially low - high at TGRD compare match
   MTU3.TIORL.BIT.IOD = 0x02;

   // Configure frequency
   MTU3.TGRA = Mtu3FrequencyCount;

   // Init to off
   MTU3.TGRB = Mtu3FrequencyCount;

   // Configure frequency
   MTU3.TGRC = Mtu3FrequencyCount;

   // Init to off
   MTU3.TGRD = Mtu3FrequencyCount;

   // Set interrupts priority
   IPR(MTU3, TGIB3) = 15;
   IPR(MTU3, TGID3) = 15;

   // Clear interrupt requests
   IR(MTU3, TGIB3) = 0;
   IR(MTU3, TGID3) = 0;

   // Configure interrupts
   MTU3.TIER.BIT.TGIEB = 1;
   MTU3.TIER.BIT.TGIED = 1;

   // Start counting
   MTU.TSTR.BIT.CST3 = 1;
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

static void StopTimersWhenPwmUnused(void)
{
   if(TimerNeedsToBeOnForChannel(OutputChannel_Pwm_0) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_1) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_2) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_4) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_5) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_FZ_LED_PWM) ||
      TimerNeedsToBeOnForChannel(OutputChannel_Pwm_FF1_LED_PWM))
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
   REINTERPRET(dutyCycle, data, const PwmDutyCycle_t *);
   PwmDutyCycle_t invertedDutyCycle;

   uassert(ERD_IS_IN_RANGE(erd));

   uint16_t channel = CHANNEL_FROM_ERD(erd);

   if(erd < Erd_BspPwm_End)
   {
      dutyCycles[channel] = *dutyCycle;
      invertedDutyCycle = PwmDutyCycle_Max - *dutyCycle;

      switch(erd)
      {
         case Erd_BspPwm_PWM_0:
            UpdatePwm0(invertedDutyCycle);
            break;

         case Erd_BspPwm_PWM_1:
            UpdatePwm1(invertedDutyCycle);
            break;

         case Erd_BspPwm_PWM_2:
            UpdatePwm2(invertedDutyCycle);
            break;

         case Erd_BspPwm_PWM_4:
            UpdatePwm4(invertedDutyCycle);
            break;

         case Erd_BspPwm_PWM_5:
            UpdatePwm5(invertedDutyCycle);
            break;

         case Erd_BspPwm_FZ_LED_PWM:
            UpdatePwmFZ_LED_PWM(*dutyCycle);
            break;

         case Erd_BspPwm_FF1_LED_PWM:
            UpdatePwmFF1_LED_PWM(*dutyCycle);
            break;
      }
      StopTimersWhenPwmUnused();
   }
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   REINTERPRET(dutyCycle, data, PwmDutyCycle_t *);

   uassert(ERD_IS_IN_RANGE(erd));

   uint16_t channel = CHANNEL_FROM_ERD(erd);
   *dutyCycle = dutyCycles[channel];
   memcpy(data, dutyCycle, sizeof(PwmDutyCycle_t));
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

   ConfigurePins();
   ConfigurePwmMode2();
   ConfigurePwmMode1ForMtu2();
   ConfigurePwmMode1ForMtu3();

   return &instance.interface;
}
