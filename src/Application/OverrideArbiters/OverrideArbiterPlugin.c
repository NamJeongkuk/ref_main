/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "OverrideArbiter.h"
#include "SystemErds.h"

static const Erd_t freezerEvaporatorFilteredTemperatureOverrideRequestErdList[] = {
   Erd_FreezerEvap_FilteredTemperatureOverrideRequest
};

static const Erd_t freezerEvaporatorFilteredTemperatureValueErdList[] = {
   Erd_FreezerEvap_FilteredTemperature,
   Erd_FreezerEvap_FilteredTemperatureOverrideValue
};

static const OverrideArbiterConfiguration_t freezerEvaporatorFilteredTemperatureArbiterConfiguration = {
   freezerEvaporatorFilteredTemperatureOverrideRequestErdList,
   freezerEvaporatorFilteredTemperatureValueErdList,
   Erd_FreezerEvap_FilteredTemperatureResolved,
   NUM_ELEMENTS(freezerEvaporatorFilteredTemperatureValueErdList)
};

static struct
{
   OverrideArbiter_t freezerEvaporatorFilteredTemperatureArbiter;
} instance;

void OverrideArbiterPlugin_Init(I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance.freezerEvaporatorFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerEvaporatorFilteredTemperatureArbiterConfiguration);
}
