/*!
 * @file
 * @brief Pwm driver implementation for rockhopper rx130 mainboard.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_PWM_H
#define DATASOURCE_PWM_H

#include "I_DataSource.h"
#include "BspErdRanges.h"
#include "Event_Synchronous.h"
#include "PwmConfiguration.h"
#include "Timer.h"

#define PWM_EXPAND_AS_ERD_NAMES(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   name,

#define EXPAND_AS_ENUM(name, pwm, initalValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   OutputChannel_Pwm_##pwm,

enum
{
   Erd_BspPwm_Start = (BspErdPwmStart - 1),
   PWM_TABLE(PWM_EXPAND_AS_ERD_NAMES)
      Erd_BspPwm_End
};

enum
{
   PWM_TABLE(EXPAND_AS_ENUM)
      OutputChannel_Pwm_Max
};

/*!
 * @return
 */
I_DataSource_t *DataSource_Pwm_Init(void);

#endif
