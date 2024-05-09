/*!
 * @file
 * @brief Feature Pan Grid
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "ConstArrayMap_FeaturePan.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "FeaturePanMode.h"
#include "Grid.h"
#include "Grid_FeaturePan.h"
#include "PlatformData.h"
#include "SystemErds.h"
#include "utils.h"

static bool HasConvertibleCompartment(const PersonalityParametricData_t *personalityData)
{
   return BITMAP_STATE(&personalityData->platformData->compartmentBitmap, Compartment_Convertible);
}

static bool GetDelayConvertibleCompartmentCooling(I_DataModel_t *dataModel)
{
   bool state;
   DataModel_Read(dataModel, Erd_DelayConvertibleCompartmentCooling, &state);
   return state;
}

static bool GetCoolConvertibleCompartmentBeforeOff(I_DataModel_t *dataModel)
{
   bool state;
   DataModel_Read(dataModel, Erd_CoolConvertibleCompartmentBeforeOff, &state);
   return state;
}

static void SetFeaturePanCoolingMode(I_DataModel_t *dataModel, FeaturePanCoolingMode_t coolingMode)
{
   DataModel_Write(dataModel, Erd_FeaturePanCoolingMode, &coolingMode);
}

static void SetFeaturePanForcedHeatingMode(I_DataModel_t *dataModel, FeaturePanForcedHeatingMode_t forcedHeatingMode)
{
   DataModel_Write(dataModel, Erd_FeaturePanForcedHeatingMode, &forcedHeatingMode);
}

static FeaturePanCoolingMode_t GetFeaturePanCoolingMode(I_DataModel_t *dataModel)
{
   FeaturePanCoolingMode_t currentFeaturePanCoolingMode;
   DataModel_Read(dataModel, Erd_FeaturePanCoolingMode, &currentFeaturePanCoolingMode);
   return currentFeaturePanCoolingMode;
}

static void Vote(I_DataModel_t *dataModel, FeaturePanVotes_t votes)
{
   DamperVotedPosition_t damperVotedPosition = {
      .position = votes.damperPosition,
      .care = Vote_Care
   };
   FanVotedSpeed_t fanVotedSpeed = {
      .speed = votes.fanSpeed,
      .care = Vote_Care
   };
   PercentageDutyCycleVote_t heaterVotedDutyCycle = {
      .percentageDutyCycle = votes.heaterDutyCycle,
      .care = Vote_Care
   };

   DataModel_Write(dataModel, Erd_ConvertibleCompartmentDamperPosition_GridVote, &damperVotedPosition);
   DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvapFanSpeed_GridVote, &fanVotedSpeed);
   DataModel_Write(dataModel, Erd_ConvertibleCompartmentHeater_GridVote, &heaterVotedDutyCycle);
   DataModel_Write(dataModel, Erd_DeliDamperPosition_GridVote, &damperVotedPosition);
   DataModel_Write(dataModel, Erd_DeliFanSpeed_GridVote, &fanVotedSpeed);
   DataModel_Write(dataModel, Erd_DeliPanHeater_GridVote, &heaterVotedDutyCycle);
}

static void SearchFeaturePanTableAndPublishGridVotes(I_DataModel_t *dataModel)
{
   FeaturePanStatesTable_t foundTableEntry;
   memset(&foundTableEntry, 0, sizeof(FeaturePanStatesTable_t));

   FeaturePanCoolingMode_t featurePanCoolingModeKey = GetFeaturePanCoolingMode(dataModel);

   uint16_t searchIndex;
   I_ConstArrayMap_t *coolingStateMap = DataModelErdPointerAccess_GetPointer(dataModel, Erd_FeaturePanCoolingStatesGridVotesConstArrayMapInterface);
   ConstArrayMap_Find(coolingStateMap, &featurePanCoolingModeKey, &searchIndex, &foundTableEntry);

   Vote(dataModel, foundTableEntry.votes);
}

static bool CoolingCondition1(I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *personalityData = PersonalityParametricData_Get(dataModel);

   if(HasConvertibleCompartment(personalityData))
   {
      CoolingMode_t coolingMode;
      DataModel_Read(dataModel, Erd_CoolingMode, &coolingMode);

      return ((GetCoolConvertibleCompartmentBeforeOff(dataModel) == true) ||
         ((coolingMode == CoolingMode_Freezer) && GetDelayConvertibleCompartmentCooling(dataModel) == false));
   }
   else
   {
      return true;
   }
}

static bool CoolingCondition2(I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *personalityData = PersonalityParametricData_Get(dataModel);

   if(HasConvertibleCompartment(personalityData))
   {
      CoolingMode_t coolingMode;
      DataModel_Read(dataModel, Erd_CoolingMode, &coolingMode);

      return ((GetCoolConvertibleCompartmentBeforeOff(dataModel) == true) ||
         ((coolingMode != CoolingMode_Off) && GetDelayConvertibleCompartmentCooling(dataModel) == false));
   }
   else
   {
      return true;
   }
}

void Grid_FeaturePan(void *context)
{
   I_DataModel_t *dataModel = context;

   GridBlockNumber_t blockNumber;
   DataModel_Read(dataModel, Erd_FeaturePanGrid_BlockNumber, &blockNumber);

   switch(blockNumber)
   {
      case 0:
         SetFeaturePanCoolingMode(dataModel, FeaturePanCoolingMode_ActiveCooling);
         SetFeaturePanForcedHeatingMode(dataModel, FeaturePanForcedHeatingMode_Inactive);
         break;

      case 1:
         SetFeaturePanCoolingMode(dataModel, CoolingCondition2(dataModel) ? FeaturePanCoolingMode_ActiveCooling : FeaturePanCoolingMode_Neutral);
         SetFeaturePanForcedHeatingMode(dataModel, FeaturePanForcedHeatingMode_Inactive);
         break;

      case 2:
         SetFeaturePanCoolingMode(dataModel, CoolingCondition1(dataModel) ? FeaturePanCoolingMode_ActiveCooling : FeaturePanCoolingMode_Neutral);
         SetFeaturePanForcedHeatingMode(dataModel, FeaturePanForcedHeatingMode_Inactive);
         break;

      case 3:
         if(FeaturePanCoolingMode_ActiveHeating == GetFeaturePanCoolingMode(dataModel))
         {
            SetFeaturePanCoolingMode(dataModel, FeaturePanCoolingMode_Neutral);
         }
         SetFeaturePanForcedHeatingMode(dataModel, FeaturePanForcedHeatingMode_Inactive);
         break;

      case 4:
         SetFeaturePanForcedHeatingMode(dataModel, FeaturePanForcedHeatingMode_Inactive);
         break;

      case 5:
         if(FeaturePanCoolingMode_ActiveCooling == GetFeaturePanCoolingMode(dataModel))
         {
            SetFeaturePanCoolingMode(dataModel, FeaturePanCoolingMode_Neutral);
         }
         break;

      case 6:
         SetFeaturePanCoolingMode(dataModel, FeaturePanCoolingMode_ActiveHeating);
         break;

      case 7:
         SetFeaturePanCoolingMode(dataModel, FeaturePanCoolingMode_ActiveHeating);
         SetFeaturePanForcedHeatingMode(dataModel, FeaturePanForcedHeatingMode_Active);
         break;

      default:
         break;
   }

   SearchFeaturePanTableAndPublishGridVotes(dataModel);
}
