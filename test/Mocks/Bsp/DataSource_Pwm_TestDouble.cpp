/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SystemErds.h"
#include "DataSource_Pwm.h"
}

#include "DataSource_NonMockTestDouble.h"

#define EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS(name, pwm, initialValue, port, bit, pinSelection, mode, timerNumber, timerCarryFreqRegister, timerConfig) \
   {                                                                                                                                                          \
      .erd = name, .sizeOfEntity = sizeof(PwmDutyCycle_t)                                                                                                     \
   },

static struct
{
   DataSource_NonMockTestDouble_t dataSourceNonMockTestDouble;
} instance;

static const DataSource_NonMockTestDoubleItem_t pwmItems[] = {
   PWM_TABLE(EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS)
};

extern "C" I_DataSource_t *DataSource_Pwm_Init(void)
{
   DataSource_NonMockTestDouble_Init(
      &instance.dataSourceNonMockTestDouble,
      pwmItems,
      NUM_ELEMENTS(pwmItems));

   return &instance.dataSourceNonMockTestDouble.interface;
}
