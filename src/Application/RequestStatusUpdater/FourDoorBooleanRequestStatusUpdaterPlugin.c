/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorBooleanRequestStatusUpdaterPlugin.h"
#include "SystemErds.h"

// ENTRY(_requestErd, _statusErd)
#define BOOLEAN_REQUEST_STATUS_ERD_PAIR_TABLE(ENTRY)                                                  \
   ENTRY(Erd_PresenceSensingEnableRequest, Erd_PresenceSensingEnableStatus)                           \
   ENTRY(Erd_TurboFreezeOnOffRequest, Erd_TurboFreezeOnOffStatus)                                     \
   ENTRY(Erd_DoorAlarmEnableRequest, Erd_DoorAlarmEnableStatus)                                       \
   ENTRY(Erd_TurboCoolOnOffRequest, Erd_TurboCoolOnOffStatus)                                         \
   ENTRY(Erd_IceMaker0_EnableRequest, Erd_IceMaker0_EnableStatus)                                     \
   ENTRY(Erd_IceMaker1_EnableRequest, Erd_IceMaker1_EnableStatus)                                     \
   ENTRY(Erd_IceMaker2_EnableRequest, Erd_IceMaker2_EnableStatus)                                     \
   ENTRY(Erd_CoolingOffRequest, Erd_CoolingOffStatus)                                                 \
   ENTRY(Erd_PresenceSensedActivatesRecessLightRequest, Erd_PresenceSensedActivatesRecessLightStatus) \
   ENTRY(Erd_RecessLightOnRequest, Erd_RecessLightOnStatus)

#define EXPAND_AS_REQUEST_STATUS_ERD_PAIRS(_requestErd, _statusErd) \
   { .requestErd = _requestErd, .statusErd = _statusErd },

static const BooleanRequestStatusErdPair_t erdPairs[] = {
   BOOLEAN_REQUEST_STATUS_ERD_PAIR_TABLE(EXPAND_AS_REQUEST_STATUS_ERD_PAIRS)
};

static const BooleanRequestStatusUpdaterConfig_t requestStatusUpdaterPluginConfig = {
   .requestStatusErdPairs = erdPairs,
   .numberOfRequestStatusErdPairs = NUM_ELEMENTS(erdPairs)
};

void FourDoorBooleanRequestStatusUpdaterPlugin_Init(
   FourDoorBooleanRequestStatusUpdaterPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   BooleanRequestStatusUpdater_Init(
      &instance->_private.booleanRequestStatusUpdater,
      &requestStatusUpdaterPluginConfig,
      dataModel);
}
