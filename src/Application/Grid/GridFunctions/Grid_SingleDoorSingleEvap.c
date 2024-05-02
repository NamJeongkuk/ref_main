/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"
#include "Grid_SingleDoorSingleEvap.h"
#include "ConstArrayMap_SingleDoorSingleEvap.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"

static void SetCoolingSpeed(I_DataModel_t *dataModel, CoolingSpeed_t coolingSpeed)
{
   DataModel_Write(dataModel, Erd_CoolingSpeed, &coolingSpeed);
}

static CoolingSpeed_t CoolingSpeed(I_DataModel_t *dataModel)
{
   CoolingSpeed_t coolingSpeed;
   DataModel_Read(dataModel, Erd_CoolingSpeed, &coolingSpeed);
   return coolingSpeed;
}

static void SetGridArea(I_DataModel_t *dataModel, GridArea_t gridArea)
{
   DataModel_Write(dataModel, Erd_GridArea, &gridArea);
}

static GridArea_t GridArea(I_DataModel_t *dataModel)
{
   GridArea_t gridArea;
   DataModel_Read(dataModel, Erd_GridArea, &gridArea);
   return gridArea;
}

static void SetIceMakerEnable(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, &state);
}

static void SetCondenserFanAntiSweatBehavior(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledByGrid, &state);
}

static void ExecuteGridVote(I_DataModel_t *dataModel, SingleDoorSingleEvaporatorVotes_t votes)
{
   bool freezerThermistorIsValid;
   bool freshFoodThermistorIsValid;

   DataModel_Read(dataModel, Erd_FreezerThermistor_IsValidResolved, &freezerThermistorIsValid);
   DataModel_Read(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &freshFoodThermistorIsValid);

   CompressorVotedSpeed_t compressorVotedSpeed = {
      .speed = votes.compressorSpeed,
      .care = Vote_Care
   };
   FanVotedSpeed_t condenserFanVotedSpeed = {
      .speed = votes.condenserFanSpeed,
      .care = Vote_Care
   };
   FanVotedSpeed_t primaryEvapFanSpeed = {
      .speed = votes.primaryEvapFanSpeed,
      .care = Vote_Care
   };

   if(!freezerThermistorIsValid && !freshFoodThermistorIsValid)
   {
      compressorVotedSpeed.care = Vote_DontCare;
      condenserFanVotedSpeed.care = Vote_DontCare;
      primaryEvapFanSpeed.care = Vote_DontCare;
   }

   DataModel_Write(dataModel, Erd_CompressorSpeed_GridVote, &compressorVotedSpeed);
   DataModel_Write(dataModel, Erd_CondenserFanSpeed_GridVote, &condenserFanVotedSpeed);
   DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &primaryEvapFanSpeed);
   DataModel_Write(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &primaryEvapFanSpeed);
}

static void SearchSingleDoorTableAndPublishGridVotes(I_DataModel_t *dataModel, GridBlockNumber_t blockNumber)
{
   (void)blockNumber;
   SingleDoorSingleEvaporatorStatesTable_t foundTableEntry;
   memset(&foundTableEntry, 0, sizeof(SingleDoorSingleEvaporatorStatesTable_t));

   CoolingSpeed_t coolingSpeed = CoolingSpeed(dataModel);

   uint16_t searchIndex;
   I_ConstArrayMap_t *coolingStateMap = DataModelErdPointerAccess_GetPointer(dataModel, Erd_FreshFoodAndFreezerCoolingStatesGridVotesConstArrayMapInterface);
   ConstArrayMap_Find(coolingStateMap, &coolingSpeed, &searchIndex, &foundTableEntry);

   ExecuteGridVote(dataModel, foundTableEntry.votes);
}

void Grid_SingleDoorSingleEvap(void *context)
{
   I_DataModel_t *dataModel = context;

   GridBlockNumber_t blockNumber;
   DataModel_Read(dataModel, Erd_GridBlockNumberOverrideResolved, &blockNumber);

   CoolingSpeed_t coolingSpeed = CoolingSpeed(dataModel);
   GridArea_t gridArea = GridArea(dataModel);

   switch(blockNumber)
   {
      case 1:
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetIceMakerEnable(dataModel, CLEAR);
         SetCondenserFanAntiSweatBehavior(dataModel, DISABLED);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 8:
         SetCoolingSpeed(dataModel,
            (gridArea == GridArea_1)
               ? CoolingSpeed_Mid
               : CoolingSpeed_High);
         break;

      case 15:
         SetCoolingSpeed(dataModel,
            (gridArea == GridArea_1)
               ? CoolingSpeed_High
               : ((coolingSpeed == CoolingSpeed_High)
                       ? CoolingSpeed_High
                       : CoolingSpeed_Mid));
         break;

      case 22:
      case 46:
         SetCoolingSpeed(dataModel,
            (coolingSpeed == CoolingSpeed_Off)
               ? CoolingSpeed_Low
               : coolingSpeed);
         SetIceMakerEnable(dataModel, ENABLED);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 29:
      case 45:
         SetCoolingSpeed(dataModel,
            (coolingSpeed == CoolingSpeed_High)
               ? CoolingSpeed_Mid
               : coolingSpeed);
         SetIceMakerEnable(dataModel, ENABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 36:
      case 44:
         SetCoolingSpeed(dataModel,
            (coolingSpeed == CoolingSpeed_Off)
               ? CoolingSpeed_Off
               : CoolingSpeed_Low);
         SetIceMakerEnable(dataModel, ENABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 42:
      case 43:
         SetCoolingSpeed(dataModel, CoolingSpeed_Off);
         SetIceMakerEnable(dataModel, ENABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 47:
         SetCoolingSpeed(dataModel,
            (gridArea == GridArea_1)
               ? CoolingSpeed_High
               : ((coolingSpeed == CoolingSpeed_High)
                       ? CoolingSpeed_High
                       : CoolingSpeed_Mid));
         break;
      case 48:
         // TODO: Implement pulldown: https://geappliances.atlassian.net/wiki/spaces/RDR/pages/3272540450/Pulldown#Single-Door-Fresh-Food-Pulldown--
         // if(FilteredFFTemp > FF Single Door Extreme High)
         // {
         //    SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         // }
         // else
         // {
         SetCoolingSpeed(dataModel,
            (gridArea == GridArea_1)
               ? ((coolingSpeed != CoolingSpeed_High)
                       ? CoolingSpeed_Mid
                       : CoolingSpeed_High)
               : CoolingSpeed_High);
         // }
         SetIceMakerEnable(dataModel, DISABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, DISABLED);
         // if(FilteredFFTemp > FF Single Door Extreme High)
         // {
         //    SetGridArea(dataModel, GridArea_1);
         // }

      default:
         break;
   }
   SearchSingleDoorTableAndPublishGridVotes(dataModel, blockNumber);
}
