/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef KEYSTREAMPLUGIN_H
#define KEYSTREAMPLUGIN_H

#include "ErdStreamReceiver.h"
#include "Timer.h"
#include "I_DataModel.h"

enum
{
   DataSize = sizeof(uint8_t),
   ErdStreamEntryCount = 5
};

typedef struct
{
   uint8_t data[DataSize + sizeof(Erd_t)];
} KeyErdStreamData_t;

typedef struct
{
   ErdStreamHeader_t header;
   KeyErdStreamData_t streamEntries[ErdStreamEntryCount];
} KeyErdStream_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      ErdStreamReceiver_t erdStreamReceiver;
      Timer_t heartbeatTimer;
      EventSubscription_t streamStateRequestOnChangeSubscription;
   } _private;
} KeyStreamPlugin_t;

void KeyStreamPlugin_Init(KeyStreamPlugin_t *instance, I_DataModel_t *dataModel);

#endif
