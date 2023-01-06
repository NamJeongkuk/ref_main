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
   Erd_FreezerEvap_FilteredTemperatureInDegFx100,
   Erd_FreezerEvap_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t freezerEvaporatorFilteredTemperatureArbiterConfiguration = {
   freezerEvaporatorFilteredTemperatureOverrideRequestErdList,
   freezerEvaporatorFilteredTemperatureValueErdList,
   Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(freezerEvaporatorFilteredTemperatureOverrideRequestErdList)
};

static const Erd_t freezerCabinetFilteredTemperatureOverrideRequestErdList[] = {
   Erd_Freezer_FilteredTemperatureOverrideRequest
};

static const Erd_t freezerCabinetFilteredTemperatureValueErdList[] = {
   Erd_Freezer_FilteredTemperatureInDegFx100,
   Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t freezerCabinetFilteredTemperatureArbiterConfiguration = {
   freezerCabinetFilteredTemperatureOverrideRequestErdList,
   freezerCabinetFilteredTemperatureValueErdList,
   Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(freezerCabinetFilteredTemperatureOverrideRequestErdList)
};

static const Erd_t freshFoodCabinetFilteredTemperatureOverrideRequestErdList[] = {
   Erd_FreshFood_FilteredTemperatureOverrideRequest
};

static const Erd_t freshFoodCabinetFilteredTemperatureValueErdList[] = {
   Erd_FreshFood_FilteredTemperatureInDegFx100,
   Erd_FreshFood_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t freshFoodCabinetFilteredTemperatureArbiterConfiguration = {
   freshFoodCabinetFilteredTemperatureOverrideRequestErdList,
   freshFoodCabinetFilteredTemperatureValueErdList,
   Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(freshFoodCabinetFilteredTemperatureOverrideRequestErdList)
};

static const Erd_t ambientFilteredTemperatureOverrideRequestErdList[] = {
   Erd_Ambient_FilteredTemperatureOverrideRequest
};

static const Erd_t ambientFilteredTemperatureValueErdList[] = {
   Erd_Ambient_FilteredTemperatureInDegFx100,
   Erd_Ambient_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t ambientFilteredTemperatureArbiterConfiguration = {
   ambientFilteredTemperatureOverrideRequestErdList,
   ambientFilteredTemperatureValueErdList,
   Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(ambientFilteredTemperatureValueErdList)
};

static const Erd_t freshFoodThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_FreshFoodThermistor_IsValidOverrideRequest
};

static const Erd_t freshFoodThermistorValidValueErdList[] = {
   Erd_FreshFood_ThermistorIsValid,
   Erd_FreshFoodThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freshFoodThermistorValidArbiterConfiguration = {
   freshFoodThermistorValidOverrideArbiterRequestErdList,
   freshFoodThermistorValidValueErdList,
   Erd_FreshFoodThermistor_IsValidResolved,
   NUM_ELEMENTS(freshFoodThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t freezerThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_FreezerThermistor_IsValidOverrideRequest
};

static const Erd_t freezerThermistorValidValueErdList[] = {
   Erd_Freezer_ThermistorIsValid,
   Erd_FreezerThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freezerThermistorValidArbiterConfiguration = {
   freezerThermistorValidOverrideArbiterRequestErdList,
   freezerThermistorValidValueErdList,
   Erd_FreezerThermistor_IsValidResolved,
   NUM_ELEMENTS(freezerThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t freshFoodEvapThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_FreshFoodEvapThermistor_IsValidOverrideRequest
};

static const Erd_t freshFoodEvapThermistorValidValueErdList[] = {
   Erd_FreshFoodEvaporatorThermistorIsValid,
   Erd_FreshFoodEvapThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freshFoodEvapThermistorValidArbiterConfiguration = {
   freshFoodEvapThermistorValidOverrideArbiterRequestErdList,
   freshFoodEvapThermistorValidValueErdList,
   Erd_FreshFoodEvapThermistor_IsValidResolved,
   NUM_ELEMENTS(freshFoodEvapThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t freezerEvapThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_FreezerEvapThermistor_IsValidOverrideRequest
};

static const Erd_t freezerEvapThermistorValidValueErdList[] = {
   Erd_FreezerEvaporatorThermistorIsValid,
   Erd_FreezerEvapThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freezerEvapThermistorValidArbiterConfiguration = {
   freezerEvapThermistorValidOverrideArbiterRequestErdList,
   freezerEvapThermistorValidValueErdList,
   Erd_FreezerEvapThermistor_IsValidResolved,
   NUM_ELEMENTS(freezerEvapThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t convertibleCompartmentCabinetThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideRequest
};

static const Erd_t convertibleCompartmentCabinetThermistorValidValueErdList[] = {
   Erd_ConvertibleCompartment_ThermistorIsValid,
   Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t convertibleCompartmentCabinetThermistorValidArbiterConfiguration = {
   convertibleCompartmentCabinetThermistorValidOverrideArbiterRequestErdList,
   convertibleCompartmentCabinetThermistorValidValueErdList,
   Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved,
   NUM_ELEMENTS(convertibleCompartmentCabinetThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t ambientThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_AmbientThermistor_IsValidOverrideRequest
};

static const Erd_t ambientThermistorValidValueErdList[] = {
   Erd_Ambient_ThermistorIsValid,
   Erd_AmbientThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t ambientThermistorValidArbiterConfiguration = {
   ambientThermistorValidOverrideArbiterRequestErdList,
   ambientThermistorValidValueErdList,
   Erd_AmbientThermistor_IsValidResolved,
   NUM_ELEMENTS(ambientThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t convertibleCompartmentEvapThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideRequest
};

static const Erd_t convertibleCompartmentEvapThermistorValidValueErdList[] = {
   Erd_ConvertibleCompartmentEvaporatorThermistorIsValid,
   Erd_ConvertibleCompartmentEvapThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t convertibleCompartmentEvapThermistorValidArbiterConfiguration = {
   convertibleCompartmentEvapThermistorValidOverrideArbiterRequestErdList,
   convertibleCompartmentEvapThermistorValidValueErdList,
   Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,
   NUM_ELEMENTS(convertibleCompartmentEvapThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t aluminumMoldIceMakerFilteredTemperatureOverrideRequestErdList[] = {
   Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideRequest
};

static const Erd_t aluminumMoldIceMakerFilteredTemperatureValueErdList[] = {
   Erd_AluminumMoldIceMaker_FilteredTemperatureInDegFx100,
   Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t aluminumMoldIceMakerFilteredTemperatureArbiterConfiguration = {
   aluminumMoldIceMakerFilteredTemperatureOverrideRequestErdList,
   aluminumMoldIceMakerFilteredTemperatureValueErdList,
   Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(aluminumMoldIceMakerFilteredTemperatureOverrideRequestErdList)
};

static const Erd_t aluminumMoldIceMakerThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_AluminumMoldIceMakerThermistorIsValidOverrideRequest
};

static const Erd_t aluminumMoldIceMakerThermistorValidValueErdList[] = {
   Erd_AluminumMoldIceMakerThermistorIsValid,
   Erd_AluminumMoldIceMakerThermistorIsValidOverrideValue
};

static const OverrideArbiterConfiguration_t aluminumMoldIceMakerThermistorValidArbiterConfiguration = {
   aluminumMoldIceMakerThermistorValidOverrideArbiterRequestErdList,
   aluminumMoldIceMakerThermistorValidValueErdList,
   Erd_AluminumMoldIceMakerThermistorIsValidResolved,
   NUM_ELEMENTS(aluminumMoldIceMakerThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t iceMakerEnabledOverrideRequestErdList[] = {
   Erd_IceMakerEnabledOverrideRequest
};

static const Erd_t iceMakerEnabledOverrideValueErdList[] = {
   Erd_IceMakerEnabledResolved,
   Erd_IceMakerEnabledOverrideValue
};

static const OverrideArbiterConfiguration_t iceMakerEnabledOverrideConfiguration = {
   iceMakerEnabledOverrideRequestErdList,
   iceMakerEnabledOverrideValueErdList,
   Erd_IceMakerEnabledOverrideResolved,
   NUM_ELEMENTS(iceMakerEnabledOverrideRequestErdList)
};

static struct
{
   OverrideArbiter_t freezerEvaporatorFilteredTemperatureArbiter;
   OverrideArbiter_t freezerCabinetFilteredTemperatureArbiter;
   OverrideArbiter_t freshFoodCabinetFilteredTemperatureArbiter;
   OverrideArbiter_t ambientFilteredTemperatureArbiter;
   OverrideArbiter_t freshFoodThermistorValidArbiter;
   OverrideArbiter_t freezerThermistorValidArbiter;
   OverrideArbiter_t freshFoodEvapThermistorValidArbiter;
   OverrideArbiter_t freezerEvapThermistorValidArbiter;
   OverrideArbiter_t convertibleCompartmentCabinetThermistorValidArbiter;
   OverrideArbiter_t ambientThermistorValidArbiter;
   OverrideArbiter_t convertibleCompartmentEvapThermistorValidArbiter;
   OverrideArbiter_t aluminumMoldIceMakerFilteredTemperatureArbiter;
   OverrideArbiter_t aluminumMoldIceMakerThermistorValidArbiter;
   OverrideArbiter_t iceMakerEnabledArbiter;
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

   OverrideArbiter_Init(
      &instance.ambientFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &ambientFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance.aluminumMoldIceMakerFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &aluminumMoldIceMakerFilteredTemperatureArbiterConfiguration);
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

   OverrideArbiter_Init(
      &instance.aluminumMoldIceMakerThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &aluminumMoldIceMakerThermistorValidArbiterConfiguration);
}

void OverrideArbiterPlugin_Init(I_DataModel_t *dataModel)
{
   InitializeFilteredTemperatureArbiters(dataModel);
   InitializeThermistorIsValidArbiters(dataModel);

   OverrideArbiter_Init(
      &instance.iceMakerEnabledArbiter,
      DataModel_AsDataSource(dataModel),
      &iceMakerEnabledOverrideConfiguration);

   DataModel_Write(
      dataModel,
      Erd_OverrideArbiterReady,
      set);
}
