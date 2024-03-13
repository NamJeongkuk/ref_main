/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_TwistTrayMotor.h"
#include "Output_TwistTrayIceMakerMotorState.h"
#include "utils.h"
#include "DataSource_Gpio.h"
#include "uassert.h"

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspTwistTrayMotor_Start - 1)
#define ERD_IS_IN_RANGE(erd) (BETWEEN(Erd_BspTwistTrayMotor_Start, erd, Erd_BspTwistTrayMotor_End))
#define TWIST_TRAY_MOTOR_TABLE_EXPAND_AS_TWIST_TRAY_MOTOR_COUNT(name, clockwiseGpio, counterClockwiseGpio) +1

enum
{
   TwistTrayMotorCount = 0 TWIST_TRAY_MOTOR_TABLE(TWIST_TRAY_MOTOR_TABLE_EXPAND_AS_TWIST_TRAY_MOTOR_COUNT)
};

static struct
{
   I_DataSource_t interface;
   Output_TwistTrayIceMakerMotorState_t motorStateOutput[TwistTrayMotorCount];
} instance;

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   (void)(_instance);
   uassert(ERD_IS_IN_RANGE(erd));
   uint16_t channel = CHANNEL_FROM_ERD(erd);
   *((I_Output_t **)data) = &instance.motorStateOutput[channel].interface;
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   (void)(_instance);
   (void)(erd);
   (void)(data);
}

static bool Has(I_DataSource_t *_instance, const Erd_t erd)
{
   (void)(_instance);
   return (ERD_IS_IN_RANGE(erd));
}

static uint8_t SizeOf(I_DataSource_t *_instance, const Erd_t erd)
{
   (void)(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   return sizeof(I_Output_t *);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

#define TWIST_TRAY_MOTOR_TABLE_EXPAND_AS_TWIST_TRAY_MOTOR_COUNT_EXPAND_AS_OUTPUT_MOTOR(name, clockwiseGpio, counterClockWiseGpio) \
   static const Output_TwistTrayIceMakerMotorStateConfig_t config##name = { clockwiseGpio, counterClockWiseGpio };                \
   Output_TwistTrayIceMakerMotorState_Init(                                                                                       \
      &instance.motorStateOutput[CHANNEL_FROM_ERD(name)],                                                                         \
      gpioGroup,                                                                                                                  \
      &config##name);

I_DataSource_t *DataSource_TwistTrayMotor_Init(I_GpioGroup_t *gpioGroup)
{
   instance.interface.api = &api;

   TWIST_TRAY_MOTOR_TABLE(TWIST_TRAY_MOTOR_TABLE_EXPAND_AS_TWIST_TRAY_MOTOR_COUNT_EXPAND_AS_OUTPUT_MOTOR);

   return &instance.interface;
}
