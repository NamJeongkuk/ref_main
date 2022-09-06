/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PWMCONFIGURATION_H
#define PWMCONFIGURATION_H

/*
 *
 */

// clang-format off

// name, pwm, initialValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig
#define PWM_TABLE(ENTRY) \
   ENTRY(Erd_BspPwm_PWM_0,         0,         1, 2, 6, 0x01, 1,   2, B, SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_1,         1,         1, A, 3, 0x01, 2,   0, D, SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_2,         2,         1, A, 1, 0x01, 2,   0, B, DontSetTimeConfig) \
   ENTRY(Erd_BspPwm_PWM_4,         4,         1, B, 5, 0x02, 2,   1, B, SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_5,         5,         1, B, 3, 0x01, 2,   0, A, DontSetTimeConfig) \
   ENTRY(Erd_BspPwm_FZ_LED_PWM, FZ_LED_PWM,   0, 1, 6, 0x01, 1,   3, D, DontSetTimeConfig) \
   ENTRY(Erd_BspPwm_FF1_LED_PWM, FF1_LED_PWM, 0, 1, 7, 0x01, 1,   3, B, DontSetTimeConfig)
#endif

// clang-format on
