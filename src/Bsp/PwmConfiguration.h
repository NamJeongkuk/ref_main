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

#ifdef OLD_HW
// name, pwm, initialValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig
#define PWM_TABLE(ENTRY) \
   ENTRY(Erd_BspPwm_PWM_0,         0,         1, 2, 6, 0x01, 1,   2, B, SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_1,         1,         1, A, 3, 0x01, 2,   0, D, SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_2,         2,         1, A, 1, 0x01, 2,   0, B, DontSetTimeConfig) \
   ENTRY(Erd_BspPwm_PWM_4,         4,         1, B, 5, 0x02, 2,   1, B, SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_5,         5,         1, B, 3, 0x01, 2,   0, A, DontSetTimeConfig) \
   ENTRY(Erd_BspPwm_FZ_LED_PWM, FZ_LED_PWM,   0, 1, 6, 0x01, 1,   3, D, DontSetTimeConfig) \
   ENTRY(Erd_BspPwm_FF1_LED_PWM, FF1_LED_PWM, 0, 1, 7, 0x01, 1,   3, B, DontSetTimeConfig)
#else
// name,                          pwm,             initialValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig
#define PWM_TABLE(ENTRY) \
   ENTRY(Erd_BspPwm_PWM_25K_00,    25K_00,         1,            3,    4,   0x01,         2,    0,           A,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_25K_01,    25K_01,         1,            A,    1,   0x01,         2,    0,           B,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_25K_02,    25K_02,         1,            3,    2,   0x01,         2,    0,           C,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_25K_03,    25K_03,         1,            E,    4,   0x01,         2,    1,           A,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_25K_04,    25K_04,         1,            B,    5,   0x01,         2,    1,           B,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_200_00,    200_00,         0,            H,    1,   0x05,         tmo,  0,           B,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_200_01,    200_01,         0,            1,    7,   0x05,         tmo,  1,           B,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_200_02,    200_02,         0,            C,    7,   0x05,         tmo,  2,           B,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_200_03,    200_03,         0,            5,    5,   0x05,         tmo,  3,           B,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_VAR_00,    VAR_00,         1,            C,    1,   0x01,         1,    3,           A,                      SetTimerConfig) \
   ENTRY(Erd_BspPwm_PWM_VAR_01,    VAR_01,         1,            C,    6,   0x01,         1,    3,           C,                      SetTimerConfig)
#endif

// clang-format on

#endif
