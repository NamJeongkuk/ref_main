/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "DataSource_RampingLedPwm.h"
#include "iodefine.h"
#include "PwmDutyCycle.h"
#include "RampingPwmDutyCycle.h"
#include "SystemClock.h"
#include "utils.h"
#include "uassert.h"
#include "EventSubscription.h"
#include "ContextProtector_Rx2xx.h"

volatile struct st_dtc_full tmr0CompareMatchA __attribute__((section(".dtcTransferInformation")));
volatile struct st_dtc_full tmr1CompareMatchA __attribute__((section(".dtcTransferInformation")));
volatile struct st_dtc_full tmr2CompareMatchA __attribute__((section(".dtcTransferInformation")));
volatile struct st_dtc_full tmr3CompareMatchA __attribute__((section(".dtcTransferInformation")));

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspRampingLedPwm_Start - 1)
#define ERD_IS_IN_RANGE(erd) (BETWEEN(Erd_BspRampingLedPwm_Start, erd, Erd_BspRampingLedPwm_End))

#define PCLOCK_FREQUENCY (U32_PCLKB)

#define CSS_MASK 0x18
#define CSS_OFFSET 3
#define CKS_MASK 0x07
#define CKS_OFFSET 0

#define OSA_MASK 0x03
#define OSA_OFFSET 0
#define OSB_MASK 0x0C
#define OSB_OFFSET 2

#define CCLR_MASK 0x18
#define CCLR_OFFSET 3
#define CMIEA_MASK 0x40
#define CMIEA_OFFSET 6

enum
{
   Port0,
   Port1,
   Port2,
   Port3,
   Port4,
   Port5,
   PortA,
   PortB,
   PortC,
   PortD,
   PortE,
   PortH,
   PortJ
};

enum
{
   PwmTmoClockSourceFrequency = PCLOCK_FREQUENCY / 64,
   PwmTmoFrequencyInHz = PwmTmoClockSourceFrequency / UINT8_MAX,
   PwmTmoFrequencyCount = PwmTmoClockSourceFrequency / PwmTmoFrequencyInHz
};

typedef struct
{
   uint8_t *outputData;
   uint8_t *direction;
   uint8_t *mode;
} RampingLedPortAddresses_t;

#define RAMPING_LED_PORT_ADDRESS_ENTRY(__port) \
   (uint8_t *)&__port.PODR, (uint8_t *)&__port.PDR, (uint8_t *)&__port.PMR

static const RampingLedPortAddresses_t rampingLedPortAddresses[] = {
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORT0) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORT1) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORT2) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORT3) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORT4) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORT5) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORTA) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORTB) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORTC) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORTD) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORTE) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORTH) },
   { RAMPING_LED_PORT_ADDRESS_ENTRY(PORTJ) },
};

typedef struct
{
   bool initialValue;
   uint8_t port;
   uint8_t pin;
   uint8_t timerNumber;
} RampingLedPortsAndPins_t;

#define EXPAND_PORTS_AND_PINS(name, pwm, initialValue, portNumber, port, pin, timerNumber) { initialValue, port, pin, timerNumber },

static const RampingLedPortsAndPins_t rampingLedPortsAndPins[] = {
   RAMPING_LED_PWM_TABLE(EXPAND_PORTS_AND_PINS)
};

static struct
{
   I_DataSource_t interface;

   EventSubscription_t interruptSubscription;

   volatile PwmDutyCycle_t targetPwmDutyCycle[NUM_ELEMENTS(rampingLedPortsAndPins)];
   volatile PwmDutyCycle_t currentPwmDutyCycle[NUM_ELEMENTS(rampingLedPortsAndPins)];
   volatile uint8_t dtcTcorbValueBuffer[NUM_ELEMENTS(rampingLedPortsAndPins)];
   volatile uint16_t rampingUpCountPerMsec[NUM_ELEMENTS(rampingLedPortsAndPins)];
   volatile uint16_t rampingDownCountPerMsec[NUM_ELEMENTS(rampingLedPortsAndPins)];
} instance __attribute__((section(".dtcTransferInformation")));

static void SetGpioMode(const uint16_t channel, bool mode)
{
   if(rampingLedPortAddresses[rampingLedPortsAndPins[channel].port].mode)
   {
      uint8_t modeRegister = *rampingLedPortAddresses[rampingLedPortsAndPins[channel].port].mode;
      BIT_WRITE(modeRegister, rampingLedPortsAndPins[channel].pin, mode);
      *rampingLedPortAddresses[rampingLedPortsAndPins[channel].port].mode = modeRegister;
   }
}

static void SetDirection(const uint16_t channel, const uint8_t direction)
{
   if(rampingLedPortAddresses[rampingLedPortsAndPins[channel].port].direction)
   {
      uint8_t directionRegister = *rampingLedPortAddresses[rampingLedPortsAndPins[channel].port].direction;
      BIT_WRITE(directionRegister, rampingLedPortsAndPins[channel].pin, direction);
      *rampingLedPortAddresses[rampingLedPortsAndPins[channel].port].direction = directionRegister;
   }
}

static void SetOutput(const uint16_t channel, const bool state)
{
   if(rampingLedPortAddresses[rampingLedPortsAndPins[channel].port].outputData)
   {
      uint8_t outputRegister = *rampingLedPortAddresses[rampingLedPortsAndPins[channel].port].outputData;
      BIT_WRITE(outputRegister, rampingLedPortsAndPins[channel].pin, state);
      *rampingLedPortAddresses[rampingLedPortsAndPins[channel].port].outputData = outputRegister;
   }
}

#define EXPAND_AS_SET_FUNCTIONS(name, pwm, initialValue, portNumber, port, pin, timerNumber) \
   MPC.P##portNumber##pin##PFS.BIT.PSEL = 0x05;

static void ConfigurePins(void)
{
   for(uint16_t channel = 0; channel < NUM_ELEMENTS(rampingLedPortsAndPins); channel++)
   {
      SetGpioMode(channel, 0);
      SetOutput(channel, rampingLedPortsAndPins[channel].initialValue);
      SetDirection(channel, 1);
   }

   // Enable writing to the PFSWE bit
   MPC.PWPR.BIT.B0WI = 0;

   // Enable writing to the PFS register
   MPC.PWPR.BIT.PFSWE = 1;

   // Set peripheral function
   RAMPING_LED_PWM_TABLE(EXPAND_AS_SET_FUNCTIONS)

   // Disable writing to the PFS register
   MPC.PWPR.BIT.PFSWE = 0;

   // Disable writing to the PFSWE bit
   MPC.PWPR.BIT.B0WI = 1;
}

typedef struct
{
   volatile uint8_t *timeConstantRegisterA;
   volatile uint8_t *timeConstantRegisterB;
   volatile uint8_t *timeCounterControlRegister;
   volatile uint8_t *timeCounterStatusRegister;
   volatile uint8_t *timeControlRegister;
   volatile uint8_t *timeCounter;
   volatile struct st_dtc_full *dtcControlBlock;
   volatile uint8_t *dtcTransferRequestEnableRegister;
} RampingLedTmoPortAddresses_t;

#define RAMPING_LED_TMO_PORT_ADDRESSES(__timer, __number, __dtcControlBlock) \
   (volatile uint8_t *)&__timer.TCORA, (volatile uint8_t *)&__timer.TCORB, (volatile uint8_t *)&__timer.TCCR, (volatile uint8_t *)&__timer.TCSR, (volatile uint8_t *)&__timer.TCR, (volatile uint8_t *)&__timer.TCNT, __dtcControlBlock, (volatile uint8_t *)&ICU.DTCER[DTCE_TMR##__number##_CMIA##__number]

static const RampingLedTmoPortAddresses_t rampingLedTmoPortAddresses[] = {
   { RAMPING_LED_TMO_PORT_ADDRESSES(TMR0, 0, &tmr0CompareMatchA) },
   { RAMPING_LED_TMO_PORT_ADDRESSES(TMR1, 1, &tmr1CompareMatchA) },
   { RAMPING_LED_TMO_PORT_ADDRESSES(TMR2, 2, &tmr2CompareMatchA) },
   { RAMPING_LED_TMO_PORT_ADDRESSES(TMR3, 3, &tmr3CompareMatchA) }
};

static void SetFrequenciesForTimeConstantRegisterA(const uint16_t channel, uint16_t frequency)
{
   if(rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeConstantRegisterA)
   {
      *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeConstantRegisterA = frequency;
   }
}

static void SetFrequenciesForTimeConstantRegisterB(const uint16_t channel, uint16_t frequency)
{
   if(rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeConstantRegisterB)
   {
      *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeConstantRegisterB = frequency;
   }
}

static void SetCountSource(const uint16_t channel)
{
   if(rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeCounterControlRegister)
   {
      // Count source is PCLK/64
      uint8_t timeCounterControlRegister = *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeCounterControlRegister;
      timeCounterControlRegister = (timeCounterControlRegister & ~CSS_MASK) | (0x01 << CSS_OFFSET);
      timeCounterControlRegister = (timeCounterControlRegister & ~CKS_MASK) | (0x04 << CKS_OFFSET);

      *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeCounterControlRegister = timeCounterControlRegister;
   }
}

static void SetHighOutputAtCompareMatchAAndLowOutputAtCompareMatchB(const uint16_t channel)
{
   if(rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeCounterStatusRegister)
   {
      uint8_t timeCounterStatusRegister = *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeCounterStatusRegister;
      timeCounterStatusRegister = (timeCounterStatusRegister & ~OSA_MASK) | (0x02 << OSA_OFFSET);
      timeCounterStatusRegister = (timeCounterStatusRegister & ~OSB_MASK) | (0x01 << OSB_OFFSET);

      *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeCounterStatusRegister = timeCounterStatusRegister;
   }
}

static void SetTimerResetsAtCompareMatchA(const uint16_t channel)
{
   if(rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeControlRegister)
   {
      uint8_t timeControlRegister = *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeControlRegister;
      timeControlRegister = (timeControlRegister & ~CCLR_MASK) | (0x01 << CCLR_OFFSET);

      *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeControlRegister = timeControlRegister;
   }
}

static void ClearCounts(const uint16_t channel)
{
   if(rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeCounter)
   {
      *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeCounter = 0;
   }
}

static void EnableCompareMatchInterruptA(const uint16_t channel)
{
   if(rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeControlRegister)
   {
      uint8_t timeControlRegister = *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeControlRegister;
      timeControlRegister = (timeControlRegister & ~CMIEA_MASK) | (0x01 << CMIEA_OFFSET);

      *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeControlRegister = timeControlRegister;
   }
}

#define EXPAND_AS_SET_INTERRUPT_PRIORITY(name, pwm, initialValue, portNumber, port, pin, timerNumber) \
   IPR(TMR##timerNumber, CMIA##timerNumber) = 6;

#define EXPAND_AS_ENABLE_INTERRUPT_REQUEST(name, pwm, initialValue, portNumber, port, pin, timerNumber) \
   IEN(TMR##timerNumber, CMIA##timerNumber) = 1;

static void SetUpDtcToCopyDutyCycleToTcorbOnCompareMatchA(uint16_t channel)
{
   if(rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcControlBlock)
   {
      rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcControlBlock->MRA = 0x00;
      rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcControlBlock->MRB = 0x00;
      rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcControlBlock->SAR = (void *)&instance.dtcTcorbValueBuffer[channel];
      rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcControlBlock->DAR = (void *)rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].timeConstantRegisterB;
      rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcControlBlock->CR.CRB = 0x0000;
   }
}

static void ConfigurePwmTmo(void)
{
   // PRCR write enabled
   SYSTEM.PRCR.WORD = 0xA50F;

   // Enable module clock
   MSTP(TMR0) = 0;
   MSTP(TMR1) = 0;
   MSTP(TMR2) = 0;
   MSTP(TMR3) = 0;

   // PRCR write disabled
   SYSTEM.PRCR.WORD = 0xA500;

   for(uint16_t channel = 0; channel < NUM_ELEMENTS(rampingLedPortsAndPins); channel++)
   {
      // Set off duty cycle
      SetFrequenciesForTimeConstantRegisterA(channel, PwmTmoFrequencyCount);
      SetFrequenciesForTimeConstantRegisterB(channel, PwmTmoFrequencyCount);
      SetCountSource(channel);
      SetHighOutputAtCompareMatchAAndLowOutputAtCompareMatchB(channel);
      SetTimerResetsAtCompareMatchA(channel);
      EnableCompareMatchInterruptA(channel);
      ClearCounts(channel);
      SetUpDtcToCopyDutyCycleToTcorbOnCompareMatchA(channel);
   }

   RAMPING_LED_PWM_TABLE(EXPAND_AS_SET_INTERRUPT_PRIORITY)
   RAMPING_LED_PWM_TABLE(EXPAND_AS_ENABLE_INTERRUPT_REQUEST)
}

static void UpdateDutyCycle(uint16_t channel)
{
   int32_t newDutyCycle = instance.currentPwmDutyCycle[channel];

   if(newDutyCycle < instance.targetPwmDutyCycle[channel])
   {
      newDutyCycle += instance.rampingUpCountPerMsec[channel];
      newDutyCycle = MIN(newDutyCycle, instance.targetPwmDutyCycle[channel]);

      instance.currentPwmDutyCycle[channel] = (uint16_t)newDutyCycle;
      instance.dtcTcorbValueBuffer[channel] = newDutyCycle >> 8;

      *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcTransferRequestEnableRegister = 1;
      rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcControlBlock->CR.CRB = 1;
   }

   if(newDutyCycle > instance.targetPwmDutyCycle[channel])
   {
      newDutyCycle -= instance.rampingDownCountPerMsec[channel];
      newDutyCycle = MAX(newDutyCycle, instance.targetPwmDutyCycle[channel]);

      instance.currentPwmDutyCycle[channel] = (uint16_t)newDutyCycle;
      instance.dtcTcorbValueBuffer[channel] = newDutyCycle >> 8;

      *rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcTransferRequestEnableRegister = 1;
      rampingLedTmoPortAddresses[rampingLedPortsAndPins[channel].timerNumber].dtcControlBlock->CR.CRB = 1;
   }
}

static void UpdateGpioModeDependingOnCurrentPwmDutyCycle(uint16_t channel)
{
   if(instance.currentPwmDutyCycle[channel] == PwmDutyCycle_Min)
   {
      SetOutput(channel, 0);
      SetGpioMode(channel, 0);
   }
   else
   {
      SetGpioMode(channel, 1);
   }
}

static void InterruptCallback(void *context, const void *args)
{
   IGNORE(context);
   IGNORE(args);

   for(uint16_t channel = 0; channel < NUM_ELEMENTS(rampingLedPortsAndPins); channel++)
   {
      UpdateDutyCycle(channel);
      UpdateGpioModeDependingOnCurrentPwmDutyCycle(channel);
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   uint16_t channel = CHANNEL_FROM_ERD(erd);
   const RampingPwmDutyCycle_t *rampingPwmDutyCycle = data;

   ContextProtector_Protect(ContextProtector_Rx2xx_GetInstance());
   instance.targetPwmDutyCycle[channel] = rampingPwmDutyCycle->pwmDutyCycle;
   instance.rampingUpCountPerMsec[channel] = rampingPwmDutyCycle->rampingUpCountPerMsec;
   instance.rampingDownCountPerMsec[channel] = rampingPwmDutyCycle->rampingDownCountPerMsec;
   ContextProtector_Unprotect(ContextProtector_Rx2xx_GetInstance());
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   uint16_t channel = CHANNEL_FROM_ERD(erd);
   RampingPwmDutyCycle_t rampingPwmDutyCycle;

   ContextProtector_Protect(ContextProtector_Rx2xx_GetInstance());
   rampingPwmDutyCycle.pwmDutyCycle = instance.currentPwmDutyCycle[channel];
   rampingPwmDutyCycle.rampingUpCountPerMsec = instance.rampingUpCountPerMsec[channel];
   rampingPwmDutyCycle.rampingDownCountPerMsec = instance.rampingDownCountPerMsec[channel];
   ContextProtector_Unprotect(ContextProtector_Rx2xx_GetInstance());

   memcpy(data, &rampingPwmDutyCycle, sizeof(rampingPwmDutyCycle));
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

   return sizeof(RampingPwmDutyCycle_t);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

I_DataSource_t *DataSource_RampingLedPwm_Init(
   Event_Synchronous_t *onChangeEvent,
   I_Interrupt_t *interrupt)
{
   (void)onChangeEvent;

   instance.interface.api = &api;

   for(uint16_t channel = 0; channel < NUM_ELEMENTS(rampingLedPortsAndPins); channel++)
   {
      instance.targetPwmDutyCycle[channel] = 0;
      instance.currentPwmDutyCycle[channel] = 0;
      instance.rampingUpCountPerMsec[channel] = 0xFFFF;
      instance.rampingDownCountPerMsec[channel] = 0xFFFF;
   }

   ConfigurePins();
   ConfigurePwmTmo();

   EventSubscription_Init(&instance.interruptSubscription, NULL, InterruptCallback);
   Event_Subscribe(interrupt->OnInterrupt, &instance.interruptSubscription);

   return &instance.interface;
}
