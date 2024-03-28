/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideSingleEvaporatorCabinetTemperatureExceededMonitorPlugin.h"
#include "SystemErds.h"

static const Erd_t freshFoodDoors[] = {
   Erd_RightSideFreshFoodDoorStatusResolved
};

static const CabinetTemperatureExceededMonitorConfiguration_t freshFoodCabinetTemperatureExceededMonitorConfig = {
   .doorErdList = {
      .erds = freshFoodDoors,
      .numberOfErds = NUM_ELEMENTS(freshFoodDoors),
   },
   .cabinetFilteredTemperatureInDegFX100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .cabinetThermistorIsValidErd = Erd_FreshFoodThermistor_IsValidResolved,
   .cabinetSetpointTemperatureInDegFErd = Erd_FreshFoodSetpointStatus,
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .faultErd = Erd_FreshFoodTemperatureExceededFault
};

static const Erd_t freezerDoors[] = {
   Erd_LeftSideFreezerDoorStatusResolved
};

static const CabinetTemperatureExceededMonitorConfiguration_t freezerCabinetTemperatureExceededMonitorConfig = {
   .doorErdList = {
      .erds = freezerDoors,
      .numberOfErds = NUM_ELEMENTS(freezerDoors),
   },
   .cabinetFilteredTemperatureInDegFX100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .cabinetThermistorIsValidErd = Erd_FreezerThermistor_IsValidResolved,
   .cabinetSetpointTemperatureInDegFErd = Erd_FreezerSetpointStatus,
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .faultErd = Erd_FreezerTemperatureExceededFault
};

void SideBySideSingleEvaporatorCabinetTemperatureExceededMonitorPlugin_Init(
   SideBySideSingleEvaporatorCabinetTemperatureExceededMonitorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const CabinetTemperatureExceededData_t *cabinetTemperatureExceededData = PersonalityParametricData_Get(dataModel)->cabinetTemperatureExceeded;

   CabinetTemperatureExceededMonitor_Init(
      &instance->_private.freshFoodCabinetTemperatureExceededMonitor,
      dataModel,
      &freshFoodCabinetTemperatureExceededMonitorConfig,
      cabinetTemperatureExceededData);

   CabinetTemperatureExceededMonitor_Init(
      &instance->_private.freezerCabinetTemperatureExceededMonitor,
      dataModel,
      &freezerCabinetTemperatureExceededMonitorConfig,
      cabinetTemperatureExceededData);
}
