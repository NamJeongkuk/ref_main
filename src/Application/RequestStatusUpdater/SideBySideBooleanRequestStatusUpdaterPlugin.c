/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideBooleanRequestStatusUpdaterPlugin.h"
#include "SystemErds.h"

// clang-format off
// ENTRY(_requestErd, _statusErd)
#define BOOLEAN_REQUEST_STATUS_ERD_PAIR_TABLE(ENTRY) \
   ENTRY(Erd_PresenceSensingEnableRequest, Erd_PresenceSensingEnableStatus)         \
   ENTRY(Erd_TurboFreezeRequest, Erd_TurboFreezeStatus)                             \
   ENTRY(Erd_DoorAlarmEnableRequest, Erd_DoorAlarmEnableStatus)

#define EXPAND_AS_REQUEST_STATUS_ERD_PAIRS( _requestErd, _statusErd) \
{ .requestErd = _requestErd, .statusErd = _statusErd },

static const BooleanRequestStatusErdPair_t erdPairs[] = {
   BOOLEAN_REQUEST_STATUS_ERD_PAIR_TABLE(EXPAND_AS_REQUEST_STATUS_ERD_PAIRS)
};

static const BooleanRequestStatusUpdaterConfig_t requestStatusUpdaterPluginConfig ={
   .requestStatusErdPairs = erdPairs,
   .numberOfRequestStatusErdPairs = NUM_ELEMENTS(erdPairs)
};

// clang-format on
void SideBySideBooleanRequestStatusUpdaterPlugin_Init(
   SideBySideBooleanRequestStatusUpdaterPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   BooleanRequestStatusUpdater_Init(
      &instance->_private.booleanRequestStatusUpdater,
      &requestStatusUpdaterPluginConfig,
      dataModel);
}
