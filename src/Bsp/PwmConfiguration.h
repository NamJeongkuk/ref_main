/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PWMCONFIGURATION_H
#define PWMCONFIGURATION_H

// clang-format off

// name,                          pwm,             initialValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig
#define PWM_TABLE(ENTRY) \
   ENTRY(Erd_BspPwm_PWM_25K_00,    25K_00,         1,            3,    4,   0x01,         2,    0,           A,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_25K_01,    25K_01,         1,            A,    1,   0x01,         2,    0,           B,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_25K_02,    25K_02,         1,            3,    2,   0x01,         2,    0,           C,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_25K_03,    25K_03,         1,            E,    4,   0x02,         2,    1,           A,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_25K_04,    25K_04,         1,            B,    5,   0x02,         2,    1,           B,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_VAR_00,    VAR_00,         0,            C,    1,   0x01,         1,    3,           A,                      SetTimerConfig)

// clang-format on

#endif
