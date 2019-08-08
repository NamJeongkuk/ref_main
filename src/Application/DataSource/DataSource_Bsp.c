/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_Bsp.h"
#include "PlatformGpio.h"
#include "SystemErds.h"
#include "uassert.h"
#include <string.h>

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);
   uint16_t index;
   GpioChannel_t channel;

   if(ConstArrayMap_Find(instance->_private.erdGpioChannelPairMap, &erd, &index, &channel))
   {
      bool value = PlatformGpio_Read(channel);
      memcpy(data, &value, sizeof(bool));
   }
   else
   {
      uassert(!"ERD not supported");
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);
   uint16_t index;
   ErdGpioChannelPair_t entry;

   if(ConstArrayMap_Find(instance->_private.erdGpioChannelPairMap, &erd, &index, &entry))
   {
      REINTERPRET(state, data, const bool *);

      if(PlatformGpio_Read(entry.channel) != *state)
      {
         PlatformGpio_Write(entry.channel, *state);

         DataSourceOnDataChangeArgs_t args =
         { erd, data };
         Event_Synchronous_Publish(&instance->_private.OnDataChange, &args);
      }
   }
   else
   {
      uassert(!"ERD not supported");
   }
}

static bool Has(const I_DataSource_t *_instance, const Erd_t erd)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);
   uint16_t unusedIndex;
   ErdGpioChannelPair_t unusedEntry;

   bool has = ConstArrayMap_Find(instance->_private.erdGpioChannelPairMap, &erd, &unusedIndex, &unusedEntry);
   return has;
}

static uint8_t SizeOf(const I_DataSource_t *_instance, const Erd_t erd)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);
   uint16_t unusedIndex;
   GpioChannel_t unusedEntry;

   if(ConstArrayMap_Find(instance->_private.erdGpioChannelPairMap, &erd, &unusedIndex, &unusedEntry))
   {
      return sizeof(bool);
   }
   else
   {
      uassert(!"ERD not supported");
   }

   return 0;
}

static const I_DataSource_Api_t api =
   { Read, Write, Has, SizeOf };

void DataSource_Bsp_Init(
   DataSource_Bsp_t *instance,
   I_ConstArrayMap_t *erdGpioChannelPairMap)
{
   instance->interface.api = &api;
   instance->interface.OnDataChange = &instance->_private.OnDataChange.interface;
   instance->_private.erdGpioChannelPairMap = erdGpioChannelPairMap;

   Event_Synchronous_Init(&instance->_private.OnDataChange);
}
