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
      ApplianceFeatureApi_Refrigerator_Version2_FreezerCabinetSetpoint;

   DataModel_Write(dataModel, Erd_ApplianceApiApplianceTypeManifest1, &applianceFeatureApiManifest);
}
