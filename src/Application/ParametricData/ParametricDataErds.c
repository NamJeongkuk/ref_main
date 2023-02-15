/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ParametricData.h"
#include "ParametricDataErds.h"
#include "DataModelErdPointerAccess.h"
#include "ParametricDataTableOfContents.h"
#include "AppliancePersonality.h"
#include "PersonalityId.h"
#include "Constants_Binary.h"

void ParametricDataErds_Init(
   I_DataModel_t *dataModel,
   Erd_t parametricCrcErd,
   Erd_t personalityIdErd,
   Erd_t personalityParametricErd,
   Erd_t personalityIdOutOfRangeErd)
{
   REINTERPRET(parametricData, ParametricData_GetParametricTableOfContents(), const ParametricDataTableOfContents_t *);
   REINTERPRET(parametricHeader, ParametricData_GetParametricHeader(), const ImageHeader_t *);

   AppliancePersonality_t appliancePersonality;
   DataModel_Read(dataModel, personalityIdErd, &appliancePersonality);

   if(appliancePersonality < parametricData->numberOfPersonalities)
   {
      DataModel_Write(
         dataModel,
         personalityIdOutOfRangeErd,
         clear);
      DataModelErdPointerAccess_Write(
         dataModel,
         personalityParametricErd,
         parametricData->personalities[appliancePersonality]);
   }
   else
   {
      DataModel_Write(
         dataModel,
         personalityIdOutOfRangeErd,
         set);
      DataModelErdPointerAccess_Write(
         dataModel,
         personalityParametricErd,
         parametricData->personalities[PersonalityId_Default]);
   }

   DataModel_Write(
      dataModel,
      parametricCrcErd,
      &parametricHeader->imageCrc);
}
