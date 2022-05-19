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
   NUM_ELEMENTS(freezerEvaporatorFilteredTemperatureOverrideRequestErdList)
};

static const Erd_t freezerCabinetFilteredTemperatureOverrideRequestErdList[] = {
   Erd_Freezer_FilteredTemperatureOverrideRequest
};

static const Erd_t freezerCabinetFilteredTemperatureValueErdList[] = {
   Erd_Freezer_FilteredTemperature,
   Erd_Freezer_FilteredTemperatureOverrideValue
};

static const OverrideArbiterConfiguration_t freezerCabinetFilteredTemperatureArbiterConfiguration = {
   freezerCabinetFilteredTemperatureOverrideRequestErdList,
   freezerCabinetFilteredTemperatureValueErdList,
   Erd_Freezer_FilteredTemperatureResolved,
   NUM_ELEMENTS(freezerCabinetFilteredTemperatureOverrideRequestErdList)
};
static struct
{
   OverrideArbiter_t freezerEvaporatorFilteredTemperatureArbiter;
   OverrideArbiter_t freezerCabinetFilteredTemperatureArbiter;
} instance;

void OverrideArbiterPlugin_Init(I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance.freezerEvaporatorFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerEvaporatorFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.freezerCabinetFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerCabinetFilteredTemperatureArbiterConfiguration);
}
