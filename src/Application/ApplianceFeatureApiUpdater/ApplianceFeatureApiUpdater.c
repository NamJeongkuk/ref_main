/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplianceFeatureApiUpdater.h"
#include "SystemErds.h"

void ApplianceFeatureApiUpdater_Init(I_DataModel_t *dataModel)
{
   ApplianceFeatureApiManifest_t applianceFeatureApiManifest;
   applianceFeatureApiManifest.type = ApplianceFeatureApiType_Refrigerator;
   applianceFeatureApiManifest.version = 2;
   applianceFeatureApiManifest.features =
      ApplianceFeatureApi_Refrigerator_Version2_Foundation |
      ApplianceFeatureApi_Refrigerator_Version2_FreshFoodCabinetSetpoint |
      ApplianceFeatureApi_Refrigerator_Version2_FreezerCabinetSetpoint |
      ApplianceFeatureApi_Refrigerator_Version2_DispenserSelection |
      ApplianceFeatureApi_Refrigerator_Version2_ColdWaterDispensable |
      ApplianceFeatureApi_Refrigerator_Version2_CrushedIceDispensable |
      ApplianceFeatureApi_Refrigerator_Version2_CubedIceDispensable |
      ApplianceFeatureApi_Refrigerator_Version2_ColdWaterDispenseRequest |
      ApplianceFeatureApi_Refrigerator_Version2_CrushedIceDispenseRequest |
      ApplianceFeatureApi_Refrigerator_Version2_CubedIceDispenseRequest |
      ApplianceFeatureApi_Refrigerator_Version2_CoolingOnOffRequest |
      ApplianceFeatureApi_Refrigerator_Version2_LeftSideFreezerDoorStatus |
      ApplianceFeatureApi_Refrigerator_Version2_RightSideFreshFoodDoorStatus |
      ApplianceFeatureApi_Refrigerator_Version2_WaterFilterStatus |
      ApplianceFeatureApi_Refrigerator_Version2_IceMaker0 |
      ApplianceFeatureApi_Refrigerator_Version2_IceMaker1 |
      ApplianceFeatureApi_Refrigerator_Version2_TurboCool |
      ApplianceFeatureApi_Refrigerator_Version2_TurboFreeze |
      ApplianceFeatureApi_Refrigerator_Version2_PresenceSensingEnabled |
      ApplianceFeatureApi_Refrigerator_Version2_PresenceSensedActivatesRecessLight |
      ApplianceFeatureApi_Refrigerator_Version2_DimmableLight0PercentLevel |
      ApplianceFeatureApi_Refrigerator_Version2_NightSnackModeLighting;

   DataModel_Write(dataModel, Erd_ApplianceApiApplianceTypeManifest1, &applianceFeatureApiManifest);

   applianceFeatureApiManifest.version = 3;
   applianceFeatureApiManifest.features = ApplianceFeatureApi_Refrigerator_Version3_IceMaker2;

   DataModel_Write(dataModel, Erd_ApplianceApiApplianceTypeManifest2, &applianceFeatureApiManifest);
}
