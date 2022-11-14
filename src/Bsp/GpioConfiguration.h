/*!
 * @file
 * @brief GPIO Configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIOCONFIGURATION_H
#define GPIOCONFIGURATION_H

// clang-format off
#define Inverted 1
#define NonInverted 0

#ifdef OLD_HW
// Name, Direction, PullUp, DriveCapacity, Port, Bit, Inverted
#define GPIO_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_HeartbeatLed,        GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_09,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_OtherLed,            GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_Io1,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 7,  NonInverted) \
   ENTRY(Erd_BspGpio_Io2,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_Io3,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_Io4,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_Io5,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port3, 1,  NonInverted) \
   ENTRY(Erd_BspGpio_Io6,                 GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 3,  NonInverted) \
   ENTRY(Erd_BspGpio_PushButtonSwitch,    GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_00,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 2,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_01,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_02,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_03,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_EN_00,       GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 0,  NonInverted)

#else
// Name, Direction, PullUp, DriveCapacity, Port, Bit, Inverted
#define GPIO_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_PWM_VAR_01,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_GPIO_OUT_00,         GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 2,  NonInverted) \
   ENTRY(Erd_BspGpio_GPIO_OUT_01,         GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_GPIO_OUT_02,         GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 7,  NonInverted) \
   ENTRY(Erd_BspGpio_HTR_00,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_HTR_01,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 7,  NonInverted) \
   ENTRY(Erd_BspGpio_HTR_02,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port4, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_HTR_03,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_HTR_04,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port0, 7,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_EN_00,       GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port0, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_EN_01,       GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port0, 3,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_00,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port0, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_01,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 3,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_02,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 1,  NonInverted) \
   ENTRY(Erd_BspGpio_MTR_DRV_03,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port3, 7,  NonInverted) \
   ENTRY(Erd_BspGpio_REF_VAL_00,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port3, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_REF_VAL_01,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port3, 3,  NonInverted) \
   ENTRY(Erd_BspGpio_REF_VAL_02,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port3, 1,  NonInverted) \
   ENTRY(Erd_BspGpio_REF_VAL_03,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port3, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_00,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port2, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_01,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortH, 3,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_02,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortH, 2,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_03,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortH, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_04,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port2, 2,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_05,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_06,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port5, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_07,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port5, 2,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_08,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port5, 1,  NonInverted) \
   ENTRY(Erd_BspGpio_RLY_09,              GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port2, 3,  NonInverted) \
   ENTRY(Erd_BspGpio_AUGER_PWR,           GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port2, 7,  NonInverted) \
   ENTRY(Erd_BspGpio_AUGER_DIRECTION,     GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port5, 3,  NonInverted) \
   ENTRY(Erd_BspGpio_HEARTBEAT,           GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port5, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_SBC_RESET,           GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_PERSONALITY,         GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_DOOR_INT,            GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_DOOR_01,             GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_DOOR_02,             GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 7,  NonInverted) \
   ENTRY(Erd_BspGpio_DOOR_03,             GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 1,  NonInverted) \
   ENTRY(Erd_BspGpio_DOOR_04,             GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_GPIO_IN_00,          GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_GPIO_IN_01,          GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 2,  NonInverted) \
   ENTRY(Erd_BspGpio_GPIO_IN_02,          GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_GPIO_IN_03,          GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_SABBATH,             GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 3,  NonInverted) \

#endif

// clang-format on

#endif
