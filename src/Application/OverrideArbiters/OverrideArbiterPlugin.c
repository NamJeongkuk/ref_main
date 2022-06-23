/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "OverrideArbiter.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "uassert.h"

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

static const Erd_t freshFoodCabinetFilteredTemperatureOverrideRequestErdList[] = {
   Erd_FreshFood_FilteredTemperatureOverrideRequest
};

static const Erd_t freshFoodCabinetFilteredTemperatureValueErdList[] = {
   Erd_FreshFood_FilteredTemperature,
   Erd_FreshFood_FilteredTemperatureOverrideValue
};

static const OverrideArbiterConfiguration_t freshFoodCabinetFilteredTemperatureArbiterConfiguration = {
   freshFoodCabinetFilteredTemperatureOverrideRequestErdList,
   freshFoodCabinetFilteredTemperatureValueErdList,
   Erd_FreshFood_FilteredTemperatureResolved,
   NUM_ELEMENTS(freshFoodCabinetFilteredTemperatureOverrideRequestErdList)
};

static struct
{
   OverrideArbiter_t freezerEvaporatorFilteredTemperatureArbiter;
   OverrideArbiter_t freezerCabinetFilteredTemperatureArbiter;
   OverrideArbiter_t freshFoodCabinetFilteredTemperatureArbiter;
} instance;

void OverrideArbiterPlugin_Init(I_DataModel_t *dataModel)
{
   bool sensorsReadyToBeRead;
   DataModel_Read(
      dataModel,
      Erd_SensorsReadyToBeRead,
      &sensorsReadyToBeRead);

   uassert(sensorsReadyToBeRead);

   OverrideArbiter_Init(
      &instance.freezerEvaporatorFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerEvaporatorFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.freezerCabinetFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerCabinetFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.freshFoodCabinetFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodCabinetFilteredTemperatureArbiterConfiguration);

   DataModel_Write(
      dataModel,
      Erd_OverrideArbiterReady,
      set);
}
