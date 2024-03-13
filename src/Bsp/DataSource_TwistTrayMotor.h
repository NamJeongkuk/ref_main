
/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_TWISTTRAYMOTOR_H
#define DATASOURCE_TWISTTRAYMOTOR_H

#include "BspErdRanges.h"
#include "I_DataSource.h"
#include "XMacroUtils.h"
#include "TwistTrayMotorConfiguration.h"
#include "I_GpioGroup.h"

//clang-format off
#define TWIST_TRAY_MOTOR_EXPAND_AS_ERD_NAMES(name, clockwiseGpio, counterClockwiseGpio) \
   name,

//clang-format on

enum
{
   Erd_BspTwistTrayMotor_Start = (BspErdTwistTrayMotorStart - 1),
   TWIST_TRAY_MOTOR_TABLE(TWIST_TRAY_MOTOR_EXPAND_AS_ERD_NAMES)
      Erd_BspTwistTrayMotor_End
};

I_DataSource_t *DataSource_TwistTrayMotor_Init(I_GpioGroup_t *gpioGroup);

#endif
