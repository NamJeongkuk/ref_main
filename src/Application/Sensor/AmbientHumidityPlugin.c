/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AmbientHumidityPlugin.h"
#include "SystemErds.h"

static const Erd_t ambientHumiditySensorValidOverrideArbiterRequestErdList[] = {
   Erd_AmbientHumiditySensor_IsValidOverrideRequest
};

static const Erd_t ambientHumiditySensorValidValueErdList[] = {
   Erd_Ambient_HumiditySensorIsValid,
   Erd_AmbientHumiditySensor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t ambientHumiditySensorValidArbiterConfiguration = {
   ambientHumiditySensorValidOverrideArbiterRequestErdList,
   ambientHumiditySensorValidValueErdList,
   Erd_AmbientHumiditySensor_IsValidResolved,
   NUM_ELEMENTS(ambientHumiditySensorValidOverrideArbiterRequestErdList)
};

void AmbientHumidityPlugin_Init(
   AmbientHumidityPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->ambientHumiditySensorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &ambientHumiditySensorValidArbiterConfiguration);
}
