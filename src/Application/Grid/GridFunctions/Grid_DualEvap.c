/*!
 * @file
 * @brief Main grid run function for dual evap systems with the Convertible compartment
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid_DualEvap.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"
#include "Constants_Binary.h"

static void VoteDontCareForAllVotes(I_DataModel_t *dataModel)
{
   ValveVotedPosition_t valveActual = { .care = false };
   DataModel_Write(dataModel, Erd_ValvePosition_GridVote, &valveActual);

   FanVotedSpeed_t fanSpeedVote = { .care = false };
   DataModel_Write(dataModel, Erd_CondenserFanSpeed_GridVote, &fanSpeedVote);
   DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &fanSpeedVote);
   DataModel_Write(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &fanSpeedVote);

   CompressorVotedSpeed_t actualCompressor = { .care = false };
   DataModel_Write(dataModel, Erd_CompressorSpeed_GridVote, &actualCompressor);
}

static void VoteFanSpeed(I_DataModel_t *dataModel, Erd_t erd, FanSpeed_t fanSpeed)
{
   FanVotedSpeed_t fanSpeedVote = { .speed = fanSpeed, .care = true };
   DataModel_Write(dataModel, erd, &fanSpeedVote);
}

static void VoteValvePosition(I_DataModel_t *dataModel, ValvePosition_t position)
{
   ValveVotedPosition_t positionVote = { .care = true, .position = position };
   DataModel_Write(dataModel, Erd_ValvePosition_GridVote, &positionVote);
}
static void SetDelayConvertibleCompartmentCooling(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_DelayConvertibleCompartmentCooling, &state);
}

static void SetCoolConvertibleCompartmentBeforeOff(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_CoolConvertibleCompartmentBeforeOff, &state);
}

static void SetDelayConvertibleCompartmentCoolingLowSpeedTo(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_DelayedConvertibleCompartmentCoolingLowSpeed, &state);
}

static void SetIceMakerEnabled(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, &state);
}

static void SetFreshFoodPulldownOffsetEnabled(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_FreshFoodPulldownOffsetEnabled, &state);
}

static void VoteCompressorSpeed(I_DataModel_t *dataModel, CompressorSpeed_t speed)
{
   CompressorVotedSpeed_t speedVote = { .care = true, .speed = speed };
   DataModel_Write(dataModel, Erd_CompressorSpeed_GridVote, &speedVote);
}

static void SetMaxValveTimeInPosAEnabled(I_DataModel_t *dataModel, bool state)
{
   DataModel_Write(dataModel, Erd_MaxValveTimeInPosAEnabled, &state);
}

static ValvePosition_t GetValvePosition(I_DataModel_t *dataModel)
{
   ValveVotedPosition_t actual;
   DataModel_Read(dataModel, Erd_ValvePosition_ResolvedVote, &actual);
   return actual.position;
}

static CompressorSpeed_t GetCompressorSpeed(I_DataModel_t *dataModel)
{
   CompressorVotedSpeed_t actual;
   DataModel_Read(dataModel, Erd_CompressorSpeed_ResolvedVote, &actual);
   return actual.speed;
}

static FanSpeed_t GetFanSpeed(I_DataModel_t *dataModel, Erd_t erd)
{
   FanVotedSpeed_t actual;
   DataModel_Read(dataModel, erd, &actual);
   return actual.speed;
}

static bool GetCoolConvertibleCompartmentBeforeOff(I_DataModel_t *dataModel)
{
   bool actual;
   DataModel_Read(dataModel, Erd_CoolConvertibleCompartmentBeforeOff, &actual);
   return actual;
}

static GridBlockNumber_t GetConvertibleCompartmentGridBlockNumber(I_DataModel_t *dataModel)
{
   GridBlockNumber_t actual;
   DataModel_Read(dataModel, Erd_ConvertibleCompartmentGridBlockNumber, &actual);
   return actual;
}

static bool GetDelayConvertibleCompartmentCooling(I_DataModel_t *dataModel)
{
   bool delayConvertibleCompartmentCoolingData;
   DataModel_Read(dataModel, Erd_DelayConvertibleCompartmentCooling, &delayConvertibleCompartmentCoolingData);
   return delayConvertibleCompartmentCoolingData;
}

void Grid_DualEvap(void *context)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);
   GridBlockNumber_t blockNumber;
   DataModel_Read(dataModel, Erd_Grid_BlockNumber, &blockNumber);

   VoteDontCareForAllVotes(dataModel);

   switch(blockNumber)
   {
      case 0:
      case 1:
      case 2:
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Low);
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_SuperHigh);
         VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
         VoteValvePosition(dataModel, ValvePosition_B);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         SetIceMakerEnabled(dataModel, DISABLED);
         SetFreshFoodPulldownOffsetEnabled(dataModel, DISABLED);
         break;

      case 3:
      case 4:
      case 5:
      case 6:
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_SuperHigh);
         VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
         VoteValvePosition(dataModel, ValvePosition_A);
         SetFreshFoodPulldownOffsetEnabled(dataModel, ENABLED);
         SetIceMakerEnabled(dataModel, DISABLED);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         break;

      case 7:
      case 8:
      case 9:
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
         VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         VoteValvePosition(dataModel, ValvePosition_B);
         SetFreshFoodPulldownOffsetEnabled(dataModel, DISABLED);
         SetIceMakerEnabled(dataModel, ENABLED);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         break;

      case 10:
      case 11:
         if(GetValvePosition(dataModel) == ValvePosition_B)
         {
            VoteValvePosition(dataModel, ValvePosition_B);
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
         }
         else
         {
            VoteValvePosition(dataModel, ValvePosition_A);
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
         }

         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
         VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         break;

      case 12:
      case 19:
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
         VoteValvePosition(dataModel, ValvePosition_A);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         break;

      case 13:
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_SuperHigh);
         VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         break;

      case 14:
      case 15:
      case 16:
         if(GetCompressorSpeed(dataModel) == CompressorSpeed_High)
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
            VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         }
         else
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Medium);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
         }

         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
         SetDelayConvertibleCompartmentCooling(dataModel, ENABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         SetIceMakerEnabled(dataModel, ENABLED);
         VoteValvePosition(dataModel, ValvePosition_B);
         SetFreshFoodPulldownOffsetEnabled(dataModel, DISABLED);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         break;

      case 17:
      case 18:
         SetDelayConvertibleCompartmentCooling(dataModel, ENABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         SetIceMakerEnabled(dataModel, ENABLED);

         if(GetValvePosition(dataModel) == ValvePosition_B)
         {
            VoteValvePosition(dataModel, ValvePosition_B);
         }
         else
         {
            VoteValvePosition(dataModel, ValvePosition_A);
         }

         if(GetCompressorSpeed(dataModel) == CompressorSpeed_High)
         {
            if(GetValvePosition(dataModel) == ValvePosition_B)
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
            }
            else
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
            }
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
            VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         }
         else
         {
            if(GetValvePosition(dataModel) == ValvePosition_B)
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
            }
            else
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Medium);
            }
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Medium);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
         }
         break;

      case 20:
         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
         VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);

         if(GetFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_ResolvedVote) < FanSpeed_Medium)
         {
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Low);
         }
         else
         {
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
         }

         VoteValvePosition(dataModel, ValvePosition_A);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         break;

      case 21:
      case 22:
      case 23:
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
         VoteValvePosition(dataModel, ValvePosition_B);
         SetFreshFoodPulldownOffsetEnabled(dataModel, DISABLED);
         SetIceMakerEnabled(dataModel, ENABLED);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, ENABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         switch(GetCompressorSpeed(dataModel))
         {
            case CompressorSpeed_High:
               VoteCompressorSpeed(dataModel, CompressorSpeed_High);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
               break;

            case CompressorSpeed_Medium:
               VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Medium);
               break;

            default:
               VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Low);
               break;
         }
         break;

      case 24:
      case 25:
      case 32:
         SetMaxValveTimeInPosAEnabled(dataModel, ENABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, ENABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);

         if(GetValvePosition(dataModel) == ValvePosition_B)
         {
            VoteValvePosition(dataModel, ValvePosition_B);
         }
         else
         {
            VoteValvePosition(dataModel, ValvePosition_A);
         }

         if(GetCompressorSpeed(dataModel) == CompressorSpeed_High)
         {
            if(GetValvePosition(dataModel) == ValvePosition_B)
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
            }
            else
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
            }

            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
            VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         }
         else if(GetCompressorSpeed(dataModel) == CompressorSpeed_Medium)
         {
            if(GetValvePosition(dataModel) == ValvePosition_B)
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
            }
            else
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Medium);
            }

            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Medium);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
         }
         else
         {
            if(GetValvePosition(dataModel) == ValvePosition_B)
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Low);
            }
            else
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Low);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
            }

            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
         }
         break;

      case 26:
         VoteValvePosition(dataModel, ValvePosition_A);
         SetMaxValveTimeInPosAEnabled(dataModel, ENABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);

         if(GetFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_ResolvedVote) < FanSpeed_Medium)
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Medium);
         }
         if(GetCompressorSpeed(dataModel) == CompressorSpeed_High)
         {
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
            VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         }
         else
         {
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Medium);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
         }
         break;

      case 27:
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
         VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
         VoteValvePosition(dataModel, ValvePosition_A);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);

         if(GetFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote) < FanSpeed_Medium)
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Medium);
         }
         break;

      case 28:
      case 29:
      case 30:
         SetIceMakerEnabled(dataModel, ENABLED);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, ENABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
         SetFreshFoodPulldownOffsetEnabled(dataModel, DISABLED);

         switch(GetCompressorSpeed(dataModel))
         {
            case CompressorSpeed_High:
               VoteValvePosition(dataModel, ValvePosition_B);
               VoteCompressorSpeed(dataModel, CompressorSpeed_High);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
               break;

            case CompressorSpeed_Medium:
               VoteValvePosition(dataModel, ValvePosition_B);
               VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Medium);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
               break;

            case CompressorSpeed_Low:
               VoteValvePosition(dataModel, ValvePosition_B);
               VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Low);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
               break;

            case CompressorSpeed_Off:
               VoteValvePosition(dataModel, ValvePosition_D);
               VoteCompressorSpeed(dataModel, CompressorSpeed_Off);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Off);
               break;

            default:
               VoteValvePosition(dataModel, ValvePosition_B);
               VoteCompressorSpeed(dataModel, CompressorSpeed_Off);
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Off);
               break;
         }
         break;

      case 31:
         SetMaxValveTimeInPosAEnabled(dataModel, ENABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, ENABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);

         if(GetCompressorSpeed(dataModel) != CompressorSpeed_Off)
         {
            if(GetValvePosition(dataModel) == ValvePosition_B)
            {
               VoteValvePosition(dataModel, ValvePosition_B);
            }
            else
            {
               VoteValvePosition(dataModel, ValvePosition_A);
            }

            if(GetFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_ResolvedVote) == FanSpeed_Off && GetValvePosition(dataModel) == ValvePosition_A)
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Low);
            }

            if(GetFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote) == FanSpeed_Off && GetValvePosition(dataModel) == ValvePosition_B)
            {
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Low);
            }

            if(GetFanSpeed(dataModel, Erd_CondenserFanSpeed_ResolvedVote) < FanSpeed_Low)
            {
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
            }
         }
         else
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Off);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Off);
            VoteValvePosition(dataModel, ValvePosition_D);
         }
         break;

      case 33:
      case 40:
         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Low);
         VoteValvePosition(dataModel, ValvePosition_A);
         SetMaxValveTimeInPosAEnabled(dataModel, ENABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);

         if(GetCompressorSpeed(dataModel) == CompressorSpeed_High)
         {
            VoteCompressorSpeed(dataModel, CompressorSpeed_High);
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
         }
         else
         {
            VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Medium);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
         }
         break;

      case 34:
      case 41:
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Low);
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
         VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         VoteValvePosition(dataModel, ValvePosition_A);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         break;

      case 35:
      case 36:
      case 37:
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
         SetFreshFoodPulldownOffsetEnabled(dataModel, DISABLED);
         SetIceMakerEnabled(dataModel, ENABLED);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);

         if(GetCompressorSpeed(dataModel) != CompressorSpeed_Off)
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Low);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
            VoteValvePosition(dataModel, ValvePosition_B);
         }
         else
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Off);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Off);
            VoteValvePosition(dataModel, ValvePosition_D);
         }
         break;

      case 38:
         SetMaxValveTimeInPosAEnabled(dataModel, ENABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, ENABLED);

         if(GetCompressorSpeed(dataModel) == CompressorSpeed_Off)
         {
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Off);
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Off);
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
            VoteValvePosition(dataModel, ValvePosition_D);
            SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
            SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         }
         else
         {
            if(GetValvePosition(dataModel) == ValvePosition_B)
            {
               VoteValvePosition(dataModel, ValvePosition_B);
            }
            else
            {
               VoteValvePosition(dataModel, ValvePosition_A);
            }

            if(GetFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_ResolvedVote) > FanSpeed_Low && GetValvePosition(dataModel) == ValvePosition_A)
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Low);
            }

            if(GetFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote) > FanSpeed_Low)
            {
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Low);
            }
            VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);

            if(GetFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_ResolvedVote) == FanSpeed_Off && GetFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote) == FanSpeed_Off)
            {
               VoteCompressorSpeed(dataModel, CompressorSpeed_Off);
            }

            if(GetCoolConvertibleCompartmentBeforeOff(dataModel) == ENABLED && GetConvertibleCompartmentGridBlockNumber(dataModel) >= 4)
            {
               SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
               SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
            }
         }
         break;

      case 39:
         SetMaxValveTimeInPosAEnabled(dataModel, ENABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);

         if(GetCompressorSpeed(dataModel) < CompressorSpeed_Medium)
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
         }

         if(GetCompressorSpeed(dataModel) == CompressorSpeed_High)
         {
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
            VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         }
         else if(GetCompressorSpeed(dataModel) == CompressorSpeed_Medium)
         {
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Medium);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
         }
         else
         {
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Low);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
         }
         break;

      case 42:
      case 43:
      case 44:
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
         SetFreshFoodPulldownOffsetEnabled(dataModel, DISABLED);
         SetIceMakerEnabled(dataModel, ENABLED);
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);

         if(GetCoolConvertibleCompartmentBeforeOff(dataModel))
         {
            if(GetConvertibleCompartmentGridBlockNumber(dataModel) < 4) // ConvertibleCompartment is still Enabledly cooling
            {
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
               VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
               VoteValvePosition(dataModel, ValvePosition_B);
               SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, ENABLED);
            }
            else
            {
               SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
               SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
            }
         }

         if(!GetCoolConvertibleCompartmentBeforeOff(dataModel))
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Off);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Off);
            VoteValvePosition(dataModel, ValvePosition_D);
            SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         }
         break;

      case 45:
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);

         if(GetDelayConvertibleCompartmentCooling(dataModel) == ENABLED && GetConvertibleCompartmentGridBlockNumber(dataModel) < 2)
         {
            SetCoolConvertibleCompartmentBeforeOff(dataModel, ENABLED);
         }
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);

         if(GetCoolConvertibleCompartmentBeforeOff(dataModel))
         {
            if(GetConvertibleCompartmentGridBlockNumber(dataModel) < 4)
            {
               VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_High);
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
               VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
               VoteValvePosition(dataModel, ValvePosition_B);
               SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, ENABLED);
            }
            else
            {
               SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
               SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
            }
         }
         else
         {
            VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
            if(GetCompressorSpeed(dataModel) != CompressorSpeed_Off)
            {
               if(GetValvePosition(dataModel) == ValvePosition_A)
               {
                  VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Low);
                  VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
                  VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
                  VoteValvePosition(dataModel, ValvePosition_A);
               }
               else
               {
                  VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
                  VoteCompressorSpeed(dataModel, CompressorSpeed_Off);
                  VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Off);
                  VoteValvePosition(dataModel, ValvePosition_D);
               }
            }
            else
            {
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Off);
               VoteCompressorSpeed(dataModel, CompressorSpeed_Off);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Off);
               VoteValvePosition(dataModel, ValvePosition_D);
            }
            SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         }
         break;

      case 46:
         SetMaxValveTimeInPosAEnabled(dataModel, ENABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);

         switch(GetCompressorSpeed(dataModel))
         {
            case CompressorSpeed_High:
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
               VoteCompressorSpeed(dataModel, CompressorSpeed_High);
               break;

            case CompressorSpeed_Medium:
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Medium);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
               VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
               break;

            default:
               VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Low);
               VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Low);
               VoteCompressorSpeed(dataModel, CompressorSpeed_Low);
               break;
         }
         break;

      case 47:
         SetMaxValveTimeInPosAEnabled(dataModel, ENABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
         VoteValvePosition(dataModel, ValvePosition_A);

         if(GetCompressorSpeed(dataModel) == CompressorSpeed_High)
         {
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
            VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         }
         else
         {
            VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_Medium);
            VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_Medium);
            VoteCompressorSpeed(dataModel, CompressorSpeed_Medium);
         }
         break;

      case 48:
         SetMaxValveTimeInPosAEnabled(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCooling(dataModel, DISABLED);
         SetCoolConvertibleCompartmentBeforeOff(dataModel, DISABLED);
         SetDelayConvertibleCompartmentCoolingLowSpeedTo(dataModel, DISABLED);
         VoteFanSpeed(dataModel, Erd_FreezerEvapFanSpeed_GridVote, FanSpeed_Off);
         VoteCompressorSpeed(dataModel, CompressorSpeed_High);
         VoteFanSpeed(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, FanSpeed_High);
         VoteFanSpeed(dataModel, Erd_CondenserFanSpeed_GridVote, FanSpeed_High);
         VoteValvePosition(dataModel, ValvePosition_A);
         break;

      default:
         break;
   }
}
