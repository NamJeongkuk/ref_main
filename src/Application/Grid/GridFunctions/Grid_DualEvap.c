/*!
 * @file
 * @brief Main grid run function for dual evap systems
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid_DualEvap.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"
#include "ConstArrayMap_FourDoorDualEvap.h"
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

static CoolingSpeed_t GetCoolingSpeed(I_DataModel_t *dataModel)
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

static FanSpeed_t GetFreshFoodEvapFanSpeed(I_DataModel_t *dataModel)
{
   FanVotedSpeed_t currentFanVotedSpeed;
   DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_ResolvedVote, &currentFanVotedSpeed);
   return currentFanVotedSpeed.speed;
}

static FanSpeed_t GetFreezerEvapFanSpeed(I_DataModel_t *dataModel)
{
   FanVotedSpeed_t currentFanVotedSpeed;
   DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote, &currentFanVotedSpeed);
   return currentFanVotedSpeed.speed;
}

static void SetCondenserFanAntiSweatBehavior(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledByGrid, &state);
}

static void SetLowAmbientValveBehavior(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_LowAmbientValveBehaviorEnabledByGrid, &state);
}

static void SetFreshFoodPulldownOffset(I_DataModel_t *dataModel, TemperatureDegFx100_t pulldownOffsetInDegFx100)
{
   DataModel_Write(dataModel, Erd_FreshFoodPulldownOffsetFromGrid, &pulldownOffsetInDegFx100);
}

static void SetPulldownFanBehavior(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_PulldownFanBehaviorEnabledByGrid, &state);
}

static void SetPulldownValveBehavior(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_PulldownValveBehaviorEnabledByGrid, &state);
}

static void SetFreshFoodAndFreezerIceMakers(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, &state);
}

static void IceCabinetHighSpeedOverride(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_IceCabinetHighSpeedOverrideEnabledByGrid, &state);
}

static void MaxTimeInValveA(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_MaxValveTimeInPosAEnabled, &state);
}

static void DelayConvertibleCompartmentCooling(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_DelayConvertibleCompartmentCooling, &state);
}

static bool GetDelayConvertibleCompartmentCooling(I_DataModel_t *dataModel)
{
   bool state;
   DataModel_Read(dataModel, Erd_DelayConvertibleCompartmentCooling, &state);
   return state;
}

static void CoolConvertibleCompartmentBeforeOff(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_CoolConvertibleCompartmentBeforeOff, &state);
}

static bool GetCoolConvertibleCompartmentBeforeOff(I_DataModel_t *dataModel)
{
   bool state;
   DataModel_Read(dataModel, Erd_CoolConvertibleCompartmentBeforeOff, &state);
   return state;
}

static void UseDelayedConvertibleCompartmentCoolingSpeed(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_UseDelayedConvertibleCompartmentCoolingSpeedEnabledByGrid, &state);
}

static GridBlockNumber_t GetFeaturePanGridBlockNumber(I_DataModel_t *dataModel)
{
   GridBlockNumber_t gridBlockNumber;
   DataModel_Read(dataModel, Erd_FeaturePanGridBlockNumber, &gridBlockNumber);
   return gridBlockNumber;
}

static void ExecuteGridVote(I_DataModel_t *dataModel, FourDoorDualEvaporatorVotes_t votes)
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
   FanVotedSpeed_t freshFoodEvapFanVotedSpeed = {
      .speed = votes.freshFoodEvapFanSpeed,
      .care = Vote_Care
   };
   SealedSystemValveVotedPosition_t sealedSystemValveVotedPosition = {
      .position = votes.sealedSystemValvePosition,
      .care = Vote_Care
   };

   if(!freezerThermistorIsValid && !freshFoodThermistorIsValid)
   {
      compressorVotedSpeed.care = Vote_DontCare;
      condenserFanVotedSpeed.care = Vote_DontCare;
      freezerEvapFanVotedSpeed.care = Vote_DontCare;
      freshFoodEvapFanVotedSpeed.care = Vote_DontCare;
      sealedSystemValveVotedPosition.care = Vote_DontCare;
   }

   DataModel_Write(dataModel, Erd_CompressorSpeed_GridVote, &compressorVotedSpeed);
   DataModel_Write(dataModel, Erd_CondenserFanSpeed_GridVote, &condenserFanVotedSpeed);
   DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &freezerEvapFanVotedSpeed);
   DataModel_Write(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &freshFoodEvapFanVotedSpeed);
   DataModel_Write(dataModel, Erd_SealedSystemValvePosition_GridVote, &sealedSystemValveVotedPosition);
}

static void ApplyGridBlockOverrides(I_DataModel_t *dataModel, GridBlockNumber_t blockNumber, FourDoorDualEvaporatorVotes_t *votes)
{
   FanSpeed_t currentFreshFoodEvapFanSpeed = GetFreshFoodEvapFanSpeed(dataModel);
   FanSpeed_t currentFreezerEvapFanSpeed = GetFreezerEvapFanSpeed(dataModel);
   CoolingMode_t currentCoolingMode = GetCoolingMode(dataModel);
   CoolingSpeed_t currentCoolingSpeed = GetCoolingSpeed(dataModel);
   bool coolConvertibleCompartmentBeforeOff = GetCoolConvertibleCompartmentBeforeOff(dataModel);
   GridBlockNumber_t featurePanGridBlockNumber = GetFeaturePanGridBlockNumber(dataModel);

   switch(blockNumber)
   {
      case 13:
         votes->condenserFanSpeed = FanSpeed_SuperHigh;
         break;

      case 20:
         if((currentFreshFoodEvapFanSpeed == FanSpeed_Off) || (currentFreshFoodEvapFanSpeed == FanSpeed_Low))
         {
            votes->freshFoodEvapFanSpeed = FanSpeed_Low;
         }
         else
         {
            votes->freshFoodEvapFanSpeed = FanSpeed_High;
         }
         break;

      case 24:
      case 25:
      case 32:
         if((currentCoolingMode == CoolingMode_FreshFood) && (currentCoolingSpeed == CoolingSpeed_Low))
         {
            votes->freezerEvapFanSpeed = FanSpeed_Off;
         }
         break;

      case 26:
      case 27:
         if(currentFreezerEvapFanSpeed == FanSpeed_High)
         {
            votes->freezerEvapFanSpeed = FanSpeed_High;
         }
         else
         {
            votes->freezerEvapFanSpeed = FanSpeed_Medium;
         }
         break;

      case 31:
         votes->freezerEvapFanSpeed = currentFreezerEvapFanSpeed;
         break;

      case 33:
      case 34:
         if((currentFreezerEvapFanSpeed == FanSpeed_Off) || (currentFreezerEvapFanSpeed == FanSpeed_Low))
         {
            votes->freezerEvapFanSpeed = FanSpeed_Low;
         }
         else
         {
            votes->freezerEvapFanSpeed = FanSpeed_Medium;
         }
         break;

      case 38:
         if(currentCoolingMode == CoolingMode_FreshFood)
         {
            if(currentFreezerEvapFanSpeed != FanSpeed_Off)
            {
               votes->freezerEvapFanSpeed = FanSpeed_Low;
            }
            else
            {
               votes->freezerEvapFanSpeed = currentFreezerEvapFanSpeed;
            }
         }

         if(coolConvertibleCompartmentBeforeOff && (featurePanGridBlockNumber < 4))
         {
            votes->freezerEvapFanSpeed = FanSpeed_High;
            votes->freshFoodEvapFanSpeed = FanSpeed_Off;
         }
         break;

      case 39:
         if((currentCoolingSpeed == CoolingSpeed_Low) || (currentCoolingSpeed == CoolingSpeed_Off))
         {
            votes->freezerEvapFanSpeed = FanSpeed_Off;
         }
         else
         {
            votes->freezerEvapFanSpeed = currentFreezerEvapFanSpeed;
         }
         break;

      case 40:
      case 41:
         if(currentFreezerEvapFanSpeed == FanSpeed_Off)
         {
            votes->freezerEvapFanSpeed = FanSpeed_Off;
         }
         else
         {
            votes->freezerEvapFanSpeed = FanSpeed_Low;
         }
         break;

      case 42:
      case 43:
      case 44:
         if(coolConvertibleCompartmentBeforeOff && (featurePanGridBlockNumber < 4))
         {
            votes->freezerEvapFanSpeed = FanSpeed_High;
            votes->freshFoodEvapFanSpeed = FanSpeed_Off;
         }
         break;

      case 45:
         if(coolConvertibleCompartmentBeforeOff && (featurePanGridBlockNumber < 4))
         {
            votes->freezerEvapFanSpeed = FanSpeed_High;
            votes->freshFoodEvapFanSpeed = FanSpeed_Off;
         }
         else
         {
            votes->freezerEvapFanSpeed = FanSpeed_Off;
         }
         break;

      case 46:
      case 47:
      case 48:
         votes->freezerEvapFanSpeed = FanSpeed_Off;
         break;

      default:
         break;
   }
}

static void SearchFourDoorDualEvapTableAndPublishGridVotes(I_DataModel_t *dataModel, GridBlockNumber_t blockNumber)
{
   FourDoorDualEvaporatorStatesTable_t foundTableEntry;
   memset(&foundTableEntry, 0, sizeof(FourDoorDualEvaporatorStatesTable_t));

   CoolingMapKey_t coolingKey;
   DataModel_Read(dataModel, Erd_CoolingMode, &coolingKey.mode);
   DataModel_Read(dataModel, Erd_CoolingSpeed, &coolingKey.speed);

   uint16_t searchIndex;
   I_ConstArrayMap_t *coolingStateMap = DataModelErdPointerAccess_GetPointer(dataModel, Erd_CoolingStatesGridVotesConstArrayMapInterface);
   ConstArrayMap_Find(coolingStateMap, &coolingKey, &searchIndex, &foundTableEntry);

   ApplyGridBlockOverrides(dataModel, blockNumber, &foundTableEntry.votes);
   ExecuteGridVote(dataModel, foundTableEntry.votes);
}

void Grid_DualEvap(void *context)
{
   I_DataModel_t *dataModel = context;

   GridBlockNumber_t blockNumber;
   DataModel_Read(dataModel, Erd_GridBlockNumberOverrideResolved, &blockNumber);

   CoolingMode_t currentCoolingMode = GetCoolingMode(dataModel);
   CoolingSpeed_t currentCoolingSpeed = GetCoolingSpeed(dataModel);
   bool coolConvertibleCompartmentBeforeOff = GetCoolConvertibleCompartmentBeforeOff(dataModel);
   bool delayConvertibleCompartmentCooling = GetDelayConvertibleCompartmentCooling(dataModel);
   GridBlockNumber_t featurePanGridBlockNumber = GetFeaturePanGridBlockNumber(dataModel);

   switch(blockNumber)
   {
      case 0:
      case 1:
      case 2:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetFreshFoodPulldownOffset(dataModel, 0);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, DISABLED);
         SetFreshFoodAndFreezerIceMakers(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 3:
      case 4:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, ENABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, DISABLED);
         SetFreshFoodAndFreezerIceMakers(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 5:
      case 6:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetFreshFoodPulldownOffset(dataModel, PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->freshFoodAdjustedSetpointData->pulldownOffsetInDegFx100);
         SetPulldownFanBehavior(dataModel, ENABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, DISABLED);
         SetFreshFoodAndFreezerIceMakers(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 7:
      case 8:
      case 9:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetFreshFoodPulldownOffset(dataModel, 0);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetFreshFoodAndFreezerIceMakers(dataModel, ENABLED);
         IceCabinetHighSpeedOverride(dataModel, ENABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 10:
      case 11:
         SetCoolingMode(dataModel, (currentCoolingMode == CoolingMode_Freezer) ? currentCoolingMode : CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, ENABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 12:
      case 19:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, ENABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 13:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, ENABLED);
         IceCabinetHighSpeedOverride(dataModel, ENABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 14:
      case 15:
      case 16:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_High) ? currentCoolingSpeed : CoolingSpeed_Mid);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetFreshFoodPulldownOffset(dataModel, 0);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetFreshFoodAndFreezerIceMakers(dataModel, ENABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, SET);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 17:
      case 18:
         SetCoolingMode(dataModel, (currentCoolingMode == CoolingMode_Freezer) ? currentCoolingMode : CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_High) ? currentCoolingSpeed : CoolingSpeed_Mid);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, SET);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 20:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, ENABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 21:
      case 22:
      case 23:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingSpeed_Low : currentCoolingSpeed);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetFreshFoodPulldownOffset(dataModel, 0);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetFreshFoodAndFreezerIceMakers(dataModel, ENABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, SET);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 24:
      case 25:
      case 32:
         SetCoolingMode(dataModel, (currentCoolingMode == CoolingMode_Freezer) ? currentCoolingMode : CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingSpeed_Low : currentCoolingSpeed);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, SET);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 26:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_High) ? currentCoolingSpeed : CoolingSpeed_Mid);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 27:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, ENABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 28:
      case 29:
      case 30:
         SetCoolingMode(dataModel, (currentCoolingMode == CoolingMode_Off) ? currentCoolingMode : CoolingMode_Freezer);
         SetLowAmbientValveBehavior(dataModel, ENABLED);
         SetFreshFoodPulldownOffset(dataModel, 0);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         SetFreshFoodAndFreezerIceMakers(dataModel, ENABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, SET);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 31:
         SetLowAmbientValveBehavior(dataModel, ENABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, SET);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 33:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_High) ? currentCoolingSpeed : CoolingSpeed_Mid);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 34:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, ENABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 35:
      case 36:
      case 37:
         SetCoolingMode(dataModel, (currentCoolingMode == CoolingMode_Off) ? currentCoolingMode : CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, (currentCoolingMode == CoolingMode_Off) ? CoolingSpeed_Off : CoolingSpeed_Low);
         SetLowAmbientValveBehavior(dataModel, ENABLED);
         SetFreshFoodPulldownOffset(dataModel, 0);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         SetFreshFoodAndFreezerIceMakers(dataModel, ENABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, SET);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 38:
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_Off) ? CoolingSpeed_Low : currentCoolingSpeed);
         if(coolConvertibleCompartmentBeforeOff)
         {
            if(featurePanGridBlockNumber >= 4)
            {
               CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
               UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
            }
            else
            {
               UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, SET);
            }
         }
         SetLowAmbientValveBehavior(dataModel, ENABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, SET);
         break;

      case 39:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         if((currentCoolingSpeed == CoolingSpeed_Low) || (currentCoolingSpeed == CoolingSpeed_Off))
         {
            SetCoolingSpeed(dataModel, CoolingSpeed_Low);
         }
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 40:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_High) ? currentCoolingSpeed : CoolingSpeed_Mid);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 41:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, ENABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 42:
      case 43:
      case 44:
         if((coolConvertibleCompartmentBeforeOff || (delayConvertibleCompartmentCooling && (featurePanGridBlockNumber < 2))) &&
            (featurePanGridBlockNumber < 4))
         {
            SetCoolingMode(dataModel, CoolingMode_Freezer);
            SetCoolingSpeed(dataModel, CoolingSpeed_Low);
            CoolConvertibleCompartmentBeforeOff(dataModel, SET);
            UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, SET);
         }
         else
         {
            SetCoolingMode(dataModel, CoolingMode_Off);
            SetCoolingSpeed(dataModel, CoolingSpeed_Off);
            CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
            UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         }

         SetLowAmbientValveBehavior(dataModel, ENABLED);
         SetFreshFoodPulldownOffset(dataModel, 0);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         SetFreshFoodAndFreezerIceMakers(dataModel, ENABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         break;

      case 45:
         if((coolConvertibleCompartmentBeforeOff || (delayConvertibleCompartmentCooling && (featurePanGridBlockNumber < 2))) &&
            (featurePanGridBlockNumber < 4))
         {
            SetCoolingMode(dataModel, CoolingMode_Freezer);
            SetCoolingSpeed(dataModel, CoolingSpeed_Low);
            CoolConvertibleCompartmentBeforeOff(dataModel, SET);
            UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, SET);
         }
         else
         {
            if(currentCoolingMode == CoolingMode_FreshFood)
            {
               SetCoolingMode(dataModel, CoolingMode_FreshFood);
               SetCoolingSpeed(dataModel, CoolingSpeed_Low);
            }
            else
            {
               SetCoolingMode(dataModel, CoolingMode_Off);
               SetCoolingSpeed(dataModel, CoolingSpeed_Off);
            }
            CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
            UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         }

         SetLowAmbientValveBehavior(dataModel, ENABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         SetCondenserFanAntiSweatBehavior(dataModel, ENABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         break;

      case 46:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingSpeed_Low : currentCoolingSpeed);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 47:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed == CoolingSpeed_High) ? currentCoolingSpeed : CoolingSpeed_Mid);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, ENABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      case 48:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetLowAmbientValveBehavior(dataModel, DISABLED);
         SetPulldownFanBehavior(dataModel, DISABLED);
         SetPulldownValveBehavior(dataModel, DISABLED);
         IceCabinetHighSpeedOverride(dataModel, DISABLED);
         MaxTimeInValveA(dataModel, DISABLED);
         DelayConvertibleCompartmentCooling(dataModel, CLEAR);
         CoolConvertibleCompartmentBeforeOff(dataModel, CLEAR);
         UseDelayedConvertibleCompartmentCoolingSpeed(dataModel, CLEAR);
         break;

      default:
         break;
   }
   SearchFourDoorDualEvapTableAndPublishGridVotes(dataModel, blockNumber);
}
