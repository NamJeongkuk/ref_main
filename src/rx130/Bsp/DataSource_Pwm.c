/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_Pwm.h"
#include "I_Pwm.h"
#include "iodefine.h"
#include "utils.h"
#include "uassert.h"
#include "PwmDutyCycle.h"
#include "DataSource_Pwm.h"

#define ERD_IS_IN_RANGE(erd) (BETWEEN(Erd_BspPwm_Start, erd, Erd_BspPwm_End))

PWM_TABLE(EXPAND_AS_UPDATE_PWM_FUNCTION)

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   IGNORE(erd);
   IGNORE(data);
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   IGNORE(erd);
   IGNORE(data);
}

static bool Has(const I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return ERD_IS_IN_RANGE(erd);
}

static uint8_t SizeOf(const I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));
   return sizeof(PwmDutyCycle_t);
}

static struct
{
   I_DataSource_t interface;
} instance;

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

I_DataSource_t *DataSource_Pwm_Init(void)
{
   instance.interface.api = &api;
   return &instance.interface;
}
