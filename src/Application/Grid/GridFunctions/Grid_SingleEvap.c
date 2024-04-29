/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "Grid_SingleEvap.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"
#include "ConstArrayMap_SideBySideSingleEvap.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"

static void SetCoolingMode(I_DataModel_t *dataModel, CoolingMode_t currentCoolingMode)
{
   DataModel_Write(dataModel, Erd_CoolingMode, &currentCoolingMode);
}

static void SetCoolingSpeed(I_DataModel_t *dataModel, CoolingSpeed_t currentCoolingSpeed)
{
   DataModel_Write(dataModel, Erd_CoolingSpeed, &currentCoolingSpeed);
}

static CoolingSpeed_t CoolingSpeed(I_DataModel_t *dataModel)
{
   CoolingSpeed_t currentCoolingSpeed;
   DataModel_Read(dataModel, Erd_CoolingSpeed, &currentCoolingSpeed);
   return currentCoolingSpeed;
}

static CoolingMode_t GetCoolingMode(I_DataModel_t *dataModel)
{
   CoolingMode_t currentCoolingMode;
   DataModel_Read(dataModel, Erd_CoolingMode, &currentCoolingMode);
   return currentCoolingMode;
}

static bool GetCompressorTripMitigationActive(I_DataModel_t *dataModel)
{
   bool active;
   DataModel_Read(dataModel, Erd_CompressorTripMitigationActive, &active);
   return active;
}

static void SetGridArea(I_DataModel_t *dataModel, GridArea_t currentGridArea)
{
   DataModel_Write(dataModel, Erd_GridArea, &currentGridArea);
}

static void SetPulldownActive(I_DataModel_t *dataModel, bool pulldownActive)
{
   DataModel_Write(dataModel, Erd_SingleEvaporatorPulldownActive, &pulldownActive);
}

static void SetIceMakerEnable(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, &state);
}

static void SetCondenserFanAntiSweatBehavior(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledByGrid, &state);
}

static GridArea_t GridArea(I_DataModel_t *dataModel)
{
   GridArea_t currentGridArea;
   DataModel_Read(dataModel, Erd_GridArea, &currentGridArea);
   return currentGridArea;
}

static bool GetPulldownActive(I_DataModel_t *dataModel)
{
   bool active;
   DataModel_Read(dataModel, Erd_SingleEvaporatorPulldownActive, &active);
   return active;
}

static void ExecuteGridVote(I_DataModel_t *dataModel, SideBySideSingleEvaporatorVotes_t votes)
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
   FanVotedSpeed_t freezerEvapFanVotedSpeed = {
      .speed = votes.freezerEvapFanSpeed,
      .care = Vote_Care
   };
   DamperVotedPosition_t freshFoodDamperPositionVotedPosition = {
      .position = votes.freshFoodDamperPosition,
      .care = Vote_Care
   };

   if(!freezerThermistorIsValid && !freshFoodThermistorIsValid)
   {
      compressorVotedSpeed.care = Vote_DontCare;
      condenserFanVotedSpeed.care = Vote_DontCare;
      freezerEvapFanVotedSpeed.care = Vote_DontCare;
      freshFoodDamperPositionVotedPosition.care = Vote_DontCare;
   }

   DataModel_Write(dataModel, Erd_CompressorSpeed_GridVote, &compressorVotedSpeed);
   DataModel_Write(dataModel, Erd_CondenserFanSpeed_GridVote, &condenserFanVotedSpeed);
   DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &freezerEvapFanVotedSpeed);
   DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_GridVote, &freshFoodDamperPositionVotedPosition);
}

static void ApplyGridBlockOverrides(I_DataModel_t *dataModel, GridBlockNumber_t blockNumber, SideBySideSingleEvaporatorVotes_t *votes)
{
   bool pulldownInMediumCompressorSpeedEnabled;
   DataModel_Read(dataModel, Erd_PulldownInMediumCompressorSpeedEnabled, &pulldownInMediumCompressorSpeedEnabled);

   switch(blockNumber)
   {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
         votes->compressorSpeed = pulldownInMediumCompressorSpeedEnabled ? CompressorSpeed_Medium : CompressorSpeed_Low;
         break;

      case 11:
      case 12:
      case 13:
         votes->condenserFanSpeed = FanSpeed_High;
         break;

      default:
         break;
   }
}

static void SearchSideBySideTableAndPublishGridVotes(I_DataModel_t *dataModel, GridBlockNumber_t blockNumber)
{
   SideBySideSingleEvaporatorStatesTable_t foundTableEntry;
   memset(&foundTableEntry, 0, sizeof(SideBySideSingleEvaporatorStatesTable_t));

   CoolingMapKey_t coolingKey;
   DataModel_Read(dataModel, Erd_CoolingMode, &coolingKey.mode);
   DataModel_Read(dataModel, Erd_CoolingSpeed, &coolingKey.speed);

   uint16_t searchIndex;
   I_ConstArrayMap_t *coolingStateMap = DataModelErdPointerAccess_GetPointer(dataModel, Erd_FreshFoodAndFreezerCoolingStatesGridVotesConstArrayMapInterface);
   ConstArrayMap_Find(coolingStateMap, &coolingKey, &searchIndex, &foundTableEntry);

   ApplyGridBlockOverrides(dataModel, blockNumber, &foundTableEntry.votes);
   ExecuteGridVote(dataModel, foundTableEntry.votes);
}

void Grid_SingleEvap(void *context)
{
   I_DataModel_t *dataModel = context;

   GridBlockNumber_t blockNumber;
   DataModel_Read(dataModel, Erd_GridBlockNumberOverrideResolved, &blockNumber);

   CoolingMode_t currentCoolingMode = GetCoolingMode(dataModel);
   CoolingSpeed_t currentCoolingSpeed = CoolingSpeed(dataModel);
   GridArea_t currentGridArea = GridArea(dataModel);
   bool pullDownActive = GetPulldownActive(dataModel);
   bool compressorTripMitigationActive = GetCompressorTripMitigationActive(dataModel);

   switch(blockNumber)
   {
      case 0:
      case 1:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetGridArea(dataModel, GridArea_1);
         SetPulldownActive(dataModel, SET);
         SetIceMakerEnable(dataModel, CLEAR);
         SetCondenserFanAntiSweatBehavior(dataModel, DISABLED);
         break;

      case 2:
      case 3:
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetPulldownActive(dataModel, SET);
         SetIceMakerEnable(dataModel, CLEAR);
         SetCondenserFanAntiSweatBehavior(dataModel, DISABLED);
         break;

      case 4:
      case 5:
      case 6:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetGridArea(dataModel, GridArea_2);
         SetPulldownActive(dataModel, SET);
         SetIceMakerEnable(dataModel, CLEAR);
         SetCondenserFanAntiSweatBehavior(dataModel, DISABLED);
         break;

      case 7:
      case 8:
      case 14:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetGridArea(dataModel, GridArea_1);
         SetPulldownActive(dataModel, CLEAR);
         SetIceMakerEnable(dataModel, SET);
         break;

      case 9:
      case 10:
         SetCoolingMode(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingMode_Freezer : currentCoolingMode);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         break;

      case 11:
      case 12:
      case 13:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (pullDownActive && compressorTripMitigationActive) ? CoolingSpeed_Mid : CoolingSpeed_High);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 15:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_High) ? CoolingSpeed_Mid : currentCoolingSpeed);
         SetGridArea(dataModel, GridArea_1);
         SetPulldownActive(dataModel, CLEAR);
         SetIceMakerEnable(dataModel, SET);
         break;

      case 16:
      case 17:
         SetCoolingMode(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingMode_Freezer : currentCoolingMode);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_High) ? CoolingSpeed_Mid : currentCoolingSpeed);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 18:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel,
            (pullDownActive && compressorTripMitigationActive) ? CoolingSpeed_High
               : (currentCoolingSpeed != CoolingSpeed_High)    ? CoolingSpeed_Mid
                                                               : CoolingSpeed_High);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 19:
      case 20:
      case 27:
      case 34:
      case 41:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetGridArea(dataModel, GridArea_2);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 21:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_Low);
         SetGridArea(dataModel, GridArea_1);
         SetPulldownActive(dataModel, CLEAR);
         SetIceMakerEnable(dataModel, SET);
         break;

      case 22:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingSpeed_Low : currentCoolingSpeed);
         SetGridArea(dataModel, GridArea_1);
         SetPulldownActive(dataModel, CLEAR);
         SetIceMakerEnable(dataModel, SET);
         break;

      case 23:
      case 24:
         SetCoolingMode(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingMode_Freezer : currentCoolingMode);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingSpeed_Low : currentCoolingSpeed);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 25:
      case 32:
      case 39:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingSpeed_Low : currentCoolingSpeed);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 26:
      case 33:
      case 40:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_High) ? CoolingSpeed_Mid : currentCoolingSpeed);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 28:
      case 29:
      case 35:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_1);
         SetIceMakerEnable(dataModel, SET);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         break;

      case 30:
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 31:
         SetCoolingMode(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingMode_Freezer : CoolingMode_FreshFood);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 36:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_Off) ? CoolingSpeed_Low : CoolingSpeed_Off);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_1);
         SetIceMakerEnable(dataModel, SET);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         break;

      case 37:
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_Off) ? CoolingSpeed_Low : CoolingSpeed_Off);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 38:
         SetCoolingMode(dataModel, (currentCoolingSpeed != CoolingSpeed_Off) ? CoolingMode_FreshFood : currentCoolingMode);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_Off) ? CoolingSpeed_Low : CoolingSpeed_Off);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 42:
      case 43:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_Off);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_1);
         SetIceMakerEnable(dataModel, SET);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         break;

      case 44:
      case 45:
         SetCoolingMode(dataModel, (currentGridArea == GridArea_1) ? CoolingMode_Freezer : CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_Off);
         SetPulldownActive(dataModel, CLEAR);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         break;

      case 46:
      case 47:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_Low);
         SetGridArea(dataModel, GridArea_2);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 48:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_Mid);
         SetGridArea(dataModel, GridArea_2);
         SetPulldownActive(dataModel, CLEAR);
         break;

      default:
         break;
   }
   SearchSideBySideTableAndPublishGridVotes(dataModel, blockNumber);
}
