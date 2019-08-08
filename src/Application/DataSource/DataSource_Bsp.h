/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_BSP_H
#define DATASOURCE_BSP_H

#include "I_DataSource.h"
#include "ErdGpioChannelPair.h"
#include "ConstArrayMap_BinarySearch.h"
#include "Event_Synchronous.h"

typedef struct
{
   const ErdGpioChannelPair_t *erdGpioChannelPairs;
   uint8_t erdGpioChannelPairCount;
} DataSource_BspConfiguration_t;

typedef struct
{
   I_DataSource_t interface;

   struct
   {
      const DataSource_BspConfiguration_t *configuration;
      I_ConstArrayMap_t *erdGpioChannelPairMap;

      Event_Synchronous_t OnDataChange;
   } _private;
} DataSource_Bsp_t;

void DataSource_Bsp_Init(
   DataSource_Bsp_t *instance,
   I_ConstArrayMap_t *erdGpioChannelPairMap);

#endif
