/*!
 * @file
 * @brief Ramps from current PWM duty cycle to target PWM duty cycle (ERD write data) using
 * the ramping up or ramping down count per msec (ERD write data) within a 1 msec interrupt
 * Uses DTC to update TCORB on the 8-bit timer's (TMO) compare match A (this is the compare
 * match that resets TCNT)
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_RAMPINGLEDPWM_H
#define DATASOURCE_RAMPINGLEDPWM_H

#include "I_DataSource.h"
#include "BspErdRanges.h"
#include "RampingLedPwmConfiguration.h"
#include "I_Interrupt.h"
#include "Dtc.h"

#define RAMPING_LED_PWM_EXPAND_AS_ERD_NAMES(name, pwm, initalValue, portNumber, port, bit, timerNumber) \
   name,

enum
{
   Erd_BspRampingLedPwm_Start = (BspErdRampingLedPwmStart - 1),
   RAMPING_LED_PWM_TABLE(RAMPING_LED_PWM_EXPAND_AS_ERD_NAMES)
      Erd_BspRampingLedPwm_End
};

I_DataSource_t *DataSource_RampingLedPwm_Init(I_Interrupt_t *interrupt);

extern volatile struct st_dtc_full tmr0CompareMatchA;
extern volatile struct st_dtc_full tmr1CompareMatchA;
extern volatile struct st_dtc_full tmr2CompareMatchA;
extern volatile struct st_dtc_full tmr3CompareMatchA;

#endif
