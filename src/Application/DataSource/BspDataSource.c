/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "BspDataSource.h"
#include "GpioBspConfiguration.h"
#include "ErdGpioChannelPair.h"

#define EXPAND_AS_ERD_GPIO_CHANNEL_PAIR(Erd, Channel) \
   { Erd, Channel },

static const ErdGpioChannelPair_t erdGpioChannelPairs[] =
   {
      ERD_GPIO_CHANNEL_TABLE(EXPAND_AS_ERD_GPIO_CHANNEL_PAIR)
   };

static const ConstArrayMap_BinarySearchConfiguration_t erdGpioChannelPairMapConfig =
   {
      .array = erdGpioChannelPairs,
      .elementCount = NUM_ELEMENTS(erdGpioChannelPairs),
      .elementSize = ELEMENT_SIZE(erdGpioChannelPairs),
      .keySize = MEMBER_SIZE(ErdGpioChannelPair_t, erd),
      .keyOffset = OFFSET_OF(ErdGpioChannelPair_t, erd),
      .keyIsSigned = IS_SIGNED(Erd_t)
   };

void BspDataSource_Init(BspDataSource_t *instance)
{
   ConstArrayMap_BinarySearch_Init(
      &instance->_private.erdGpioChannelMap,
      &erdGpioChannelPairMapConfig);

   DataSource_Bsp_Init(
      &instance->_private.dataSource,
      &instance->_private.erdGpioChannelMap.interface);
}

I_DataSource_t *BspDataSource_DataSource(BspDataSource_t *instance)
{
   return &instance->_private.dataSource.interface;
}
