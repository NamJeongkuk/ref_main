/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RAMPINGLEDPWMCONFIGURATION_H
#define RAMPINGLEDPWMCONFIGURATION_H

// clang-format off

// name,                                     pwm,            initialValue, portNumber, port, pin,   timerNumber
#define RAMPING_LED_PWM_TABLE(ENTRY) \
   ENTRY(Erd_BspRampingLedPwm_PWM_200_00,    200_00,         0,            H,        PortH,    1,     0) \
   ENTRY(Erd_BspRampingLedPwm_PWM_200_01,    200_01,         0,            1,        Port1,    7,     1) \
   ENTRY(Erd_BspRampingLedPwm_PWM_200_02,    200_02,         0,            C,        PortC,    7,     2) \
   ENTRY(Erd_BspRampingLedPwm_PWM_200_03,    200_03,         0,            5,        Port5,    5,     3)

// clang-format on

#endif
