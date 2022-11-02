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

static const Erd_t freshFoodThermistorValidOverrideArbiterRequestErdLis[] = {
   Erd_FreshFoodThermistor_IsValidOverrideRequest
};

static const Erd_t freshFoodThermistorValidValueErdList[] = {
   Erd_FreshFood_ThermistorIsValid,
   Erd_FreshFoodThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freshFoodThermistorValidArbiterConfiguration = {
   freshFoodThermistorValidOverrideArbiterRequestErdLis,
   freshFoodThermistorValidValueErdList,
   Erd_FreshFoodThermistor_IsValidResolved,
   NUM_ELEMENTS(freshFoodThermistorValidOverrideArbiterRequestErdLis)
};

static const Erd_t freezerThermistorValidOverrideArbiterRequestErdLis[] = {
   Erd_FreezerThermistor_IsValidOverrideRequest
};

static const Erd_t freezerThermistorValidValueErdList[] = {
   Erd_Freezer_ThermistorIsValid,
   Erd_FreezerThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freezerThermistorValidArbiterConfiguration = {
   freezerThermistorValidOverrideArbiterRequestErdLis,
   freezerThermistorValidValueErdList,
   Erd_FreezerThermistor_IsValidResolved,
   NUM_ELEMENTS(freezerThermistorValidOverrideArbiterRequestErdLis)
};

static const Erd_t freshFoodEvapThermistorValidOverrideArbiterRequestErdLis[] = {
   Erd_FreshFoodEvapThermistor_IsValidOverrideRequest
};

static const Erd_t freshFoodEvapThermistorValidValueErdList[] = {
   Erd_FreshFoodEvaporatorThermistorIsValid,
   Erd_FreshFoodEvapThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freshFoodEvapThermistorValidArbiterConfiguration = {
   freshFoodEvapThermistorValidOverrideArbiterRequestErdLis,
   freshFoodEvapThermistorValidValueErdList,
   Erd_FreshFoodEvapThermistor_IsValidResolved,
   NUM_ELEMENTS(freshFoodEvapThermistorValidOverrideArbiterRequestErdLis)
};

static const Erd_t freezerEvapThermistorValidOverrideArbiterRequestErdLis[] = {
   Erd_FreezerEvapThermistor_IsValidOverrideRequest
};

static const Erd_t freezerEvapThermistorValidValueErdList[] = {
   Erd_FreezerEvaporatorThermistorIsValid,
   Erd_FreezerEvapThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freezerEvapThermistorValidArbiterConfiguration = {
   freezerEvapThermistorValidOverrideArbiterRequestErdLis,
   freezerEvapThermistorValidValueErdList,
   Erd_FreezerEvapThermistor_IsValidResolved,
   NUM_ELEMENTS(freezerEvapThermistorValidOverrideArbiterRequestErdLis)
};

static const Erd_t convertibleCompartmentCabinetThermistorValidOverrideArbiterRequestErdLis[] = {
   Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideRequest
};

static const Erd_t convertibleCompartmentCabinetThermistorValidValueErdList[] = {
   Erd_ConvertibleCompartment_ThermistorIsValid,
   Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t convertibleCompartmentCabinetThermistorValidArbiterConfiguration = {
   convertibleCompartmentCabinetThermistorValidOverrideArbiterRequestErdLis,
   convertibleCompartmentCabinetThermistorValidValueErdList,
   Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved,
   NUM_ELEMENTS(convertibleCompartmentCabinetThermistorValidOverrideArbiterRequestErdLis)
};

static const Erd_t ambientThermistorValidOverrideArbiterRequestErdLis[] = {
   Erd_AmbientThermistor_IsValidOverrideRequest
};

static const Erd_t ambientThermistorValidValueErdList[] = {
   Erd_Ambient_ThermistorIsValid,
   Erd_AmbientThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t ambientThermistorValidArbiterConfiguration = {
   ambientThermistorValidOverrideArbiterRequestErdLis,
   ambientThermistorValidValueErdList,
   Erd_AmbientThermistor_IsValidResolved,
   NUM_ELEMENTS(ambientThermistorValidOverrideArbiterRequestErdLis)
};

static const Erd_t convertibleCompartmentEvapThermistorValidOverrideArbiterRequestErdLis[] = {
   Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideRequest
};

static const Erd_t convertibleCompartmentEvapThermistorValidValueErdList[] = {
   Erd_ConvertibleCompartmentEvaporatorThermistorIsValid,
   Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t convertibleCompartmentEvapThermistorValidArbiterConfiguration = {
   convertibleCompartmentEvapThermistorValidOverrideArbiterRequestErdLis,
   convertibleCompartmentEvapThermistorValidValueErdList,
   Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,
   NUM_ELEMENTS(convertibleCompartmentEvapThermistorValidOverrideArbiterRequestErdLis)
};

static struct
{
   OverrideArbiter_t freezerEvaporatorFilteredTemperatureArbiter;
   OverrideArbiter_t freezerCabinetFilteredTemperatureArbiter;
   OverrideArbiter_t freshFoodCabinetFilteredTemperatureArbiter;
   OverrideArbiter_t freshFoodThermistorValidArbiter;
   OverrideArbiter_t freezerThermistorValidArbiter;
   OverrideArbiter_t freshFoodEvapThermistorValidArbiter;
   OverrideArbiter_t freezerEvapThermistorValidArbiter;
   OverrideArbiter_t convertibleCompartmentCabinetThermistorValidArbiter;
   OverrideArbiter_t ambientThermistorValidArbiter;
   OverrideArbiter_t convertibleCompartmentEvapThermistorValidArbiter;
} instance;

static void InitializeFilteredTemperatureArbiters(I_DataModel_t *dataModel)
{
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
}

static void InitializeThermistorIsValidArbiters(I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance.freshFoodThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.freezerThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.freshFoodEvapThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodEvapThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.freezerEvapThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerEvapThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.convertibleCompartmentCabinetThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &convertibleCompartmentCabinetThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.ambientThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &ambientThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.convertibleCompartmentEvapThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &convertibleCompartmentEvapThermistorValidArbiterConfiguration);
}

void OverrideArbiterPlugin_Init(I_DataModel_t *dataModel)
{
   InitializeFilteredTemperatureArbiters(dataModel);
   InitializeThermistorIsValidArbiters(dataModel);

   DataModel_Write(
      dataModel,
      Erd_OverrideArbiterReady,
      set);
}
