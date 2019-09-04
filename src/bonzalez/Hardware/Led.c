/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include <stdbool.h>
#include "Led.h"
#include "stm8s.h"
#include "utils.h"

typedef struct
{
   Erd_t ledErd;
   TinyEventSubscription_t dataSourceOnChangeSubscription;
} Instance_t;

static Instance_t instance;

enum
{
   Pin3 = (1 << 3)
};

static void SetLedState(bool state)
{
   if(!state)
   {
      GPIOA->ODR |= Pin3;
   }
   else
   {
      GPIOA->ODR &= ~Pin3;
   }
}

static void DataSourceChanged(void *context, const void *_args)
{
   IGNORE(context);
   REINTERPRET(args, _args, const TinyDataSourceOnDataChangeArgs_t *);

   if(args->erd == instance.ledErd)
   {
      SetLedState(*(const bool*)args->data);
   }
}

void Led_Init(I_TinyDataSource_t *dataSource, Erd_t ledErd)
{
   instance.ledErd = ledErd;

   GPIOA->CR1 |= Pin3;
   GPIOA->DDR |= Pin3;

   bool state;
   TinyDataSource_Read(dataSource, ledErd, &state);
   SetLedState(state);

   TinyEventSubscription_Init(&instance.dataSourceOnChangeSubscription, NULL, DataSourceChanged);
   TinyEvent_Subscribe(TinyDataSource_GetOnChangeEvent(dataSource), &instance.dataSourceOnChangeSubscription);
}
