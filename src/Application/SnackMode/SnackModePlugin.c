/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SnackModePlugin.h"
#include "SnackModeEnableRequest.h"
#include "SystemErds.h"

typedef struct
{
   EventSubscription_t snackModeSubscription;
} SnackModePlugin_t;

static SnackModePlugin_t instance;

static void BeReadyForNewSnackModeEnableRequest(I_DataModel_t *dataModel)
{
   SnackModeEnableRequest_t readyForNewSnackModeEnableRequest = SnackModeEnableRequest_Ready;
   DataModel_Write(dataModel, Erd_NightTimeSnackModeEnableRequest, &readyForNewSnackModeEnableRequest);
}

static void OnSnackModeRequest(void *context, const void *args)
{
   I_DataModel_t *dataModel = context;
   const SnackModeEnableRequest_t *snackModeEnableRequest = args;

   switch(*snackModeEnableRequest)
   {
      case SnackModeEnableRequest_On:
      case SnackModeEnableRequest_Off:
         DataModel_Write(dataModel, Erd_NightTimeSnackModeEnableStatus, snackModeEnableRequest);
         break;

      default:
         break;
   }

   BeReadyForNewSnackModeEnableRequest(dataModel);
}

void SnackModePlugin_Init(I_DataModel_t *dataModel)
{
   BeReadyForNewSnackModeEnableRequest(dataModel);

   EventSubscription_Init(
      &instance.snackModeSubscription,
      dataModel,
      OnSnackModeRequest);

   DataModel_Subscribe(
      dataModel,
      Erd_NightTimeSnackModeEnableRequest,
      &instance.snackModeSubscription);
}
