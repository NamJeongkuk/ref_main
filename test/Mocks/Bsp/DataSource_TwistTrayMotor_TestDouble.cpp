/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SystemErds.h"
#include "DataSource_TwistTrayMotor.h"
#include "utils.h"
}

#include "DataSource_NonMockTestDouble.h"
#include "TwistTrayMotorOutput_TestDouble.h"

#define EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS(name, clockwiseChannel, counterClockwiseChannel) \
   {                                                                                                 \
      .erd = name, .sizeOfEntity = sizeof(I_Output_t *)                                              \
   },

static const DataSource_NonMockTestDoubleItem_t twistTrayMotorOutputItems[] = {
   TWIST_TRAY_MOTOR_TABLE(EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS)
};

static struct
{
   DataSource_NonMockTestDouble_t dataSourceNonMockTestDouble;
   TwistTrayMotorOutput_TestDouble_t outputTestDouble[NUM_ELEMENTS(twistTrayMotorOutputItems)];
} instance;

extern "C" I_DataSource_t *DataSource_TwistTrayMotor_Init(I_GpioGroup_t *gpioGroup)
{
   (void)gpioGroup;

   DataSource_NonMockTestDouble_Init(
      &instance.dataSourceNonMockTestDouble,
      twistTrayMotorOutputItems,
      NUM_ELEMENTS(twistTrayMotorOutputItems));

   for(uint8_t index = 0; index < NUM_ELEMENTS(instance.outputTestDouble); index++)
   {
      TwistTrayMotorOutput_TestDouble_Init(&instance.outputTestDouble[index]);

      I_Output_t *output = &instance.outputTestDouble[index].interface;

      DataSource_Write(
         &instance.dataSourceNonMockTestDouble.interface,
         twistTrayMotorOutputItems[index].erd,
         &output);
   }

   return &instance.dataSourceNonMockTestDouble.interface;
}
