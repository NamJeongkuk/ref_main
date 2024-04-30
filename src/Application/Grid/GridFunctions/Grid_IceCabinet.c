/*!
 * @file
 * @brief Ice Cabinet Grid
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "IceCabinetCoolingMode.h"
#include "Grid.h"
#include "Grid_IceCabinet.h"
#include "SystemErds.h"
#include "utils.h"
#include "uassert.h"

static void Vote(I_DataModel_t *dataModel, FanSpeed_t iceCabinetFanSpeed)
{
   FanVotedSpeed_t fanVotedSpeed = {
      .speed = iceCabinetFanSpeed,
      .care = Vote_Care
   };

   DataModel_Write(dataModel, Erd_IceCabinetFanSpeed_IceCabinetGridVote, &fanVotedSpeed);
}

static void PublishIceCabinetModeAndVote(I_DataModel_t *dataModel, IceCabinetCoolingMode_t iceCabinetCoolingMode)
{
   FanSpeed_t iceCabinetFanSpeed = FanSpeed_Off;
   const GammaTable_t *gammaTable = PersonalityParametricData_Get(dataModel)->iceCabinetData->gammaTable;

   switch(iceCabinetCoolingMode)
   {
      case IceCabinetCoolingMode_Neutral:
         iceCabinetFanSpeed = FanSpeed_Off;
         break;

      case IceCabinetCoolingMode_ForcedCooling:
         iceCabinetFanSpeed = FanSpeed_SuperHigh;
         break;

      case IceCabinetCoolingMode_ActiveCooling: {
         Gamma_t gamma;
         DataModel_Read(dataModel, Erd_IceCabinetGamma, &gamma);
         if(gamma < gammaTable->iceCabinetMedSpeedLimit)
         {
            iceCabinetFanSpeed = FanSpeed_Low;
         }
         else if(gamma < gammaTable->iceCabinetHighSpeedLimit)
         {
            iceCabinetFanSpeed = FanSpeed_Medium;
         }
         else if(gamma < gammaTable->iceCabinetSuperHighSpeedLimit)
         {
            iceCabinetFanSpeed = FanSpeed_High;
         }
         else
         {
            iceCabinetFanSpeed = FanSpeed_SuperHigh;
         }
         break;
      }

      case IceCabinetCoolingMode_FallbackCooling:
         iceCabinetFanSpeed = FanSpeed_High;
         break;

      default:
         uassert(!"Unreachable");
         break;
   }

   bool iceCabinetFanDisabledByFreshFoodAndFreezerGrid;
   DataModel_Read(
      dataModel,
      Erd_IceCabinetFanDisabledByGrid,
      &iceCabinetFanDisabledByFreshFoodAndFreezerGrid);

   bool highSpeedOverrideEnabledByFreshFoodGrid;
   DataModel_Read(
      dataModel,
      Erd_IceCabinetHighSpeedOverrideEnabledByGrid,
      &highSpeedOverrideEnabledByFreshFoodGrid);

   if(iceCabinetFanDisabledByFreshFoodAndFreezerGrid)
   {
      iceCabinetFanSpeed = FanSpeed_Off;
   }
   else if(highSpeedOverrideEnabledByFreshFoodGrid)
   {
      if(iceCabinetFanSpeed != FanSpeed_Off)
      {
         iceCabinetFanSpeed = FanSpeed_High;
      }
   }

   DataModel_Write(dataModel, Erd_IceCabinetCoolingMode, &iceCabinetCoolingMode);
   Vote(dataModel, iceCabinetFanSpeed);
}

void Grid_IceCabinet(void *context)
{
   I_DataModel_t *dataModel = context;

   GridBlockNumber_t blockNumber;
   DataModel_Read(dataModel, Erd_IceCabinetGridBlockNumberOverrideResolved, &blockNumber);

   IceCabinetCoolingMode_t newIceCabinetCoolingMode = IceCabinetCoolingMode_FallbackCooling;
   IceCabinetCoolingMode_t currentIceCabinetCoolingMode;

   DataModel_Read(dataModel, Erd_IceCabinetCoolingMode, &currentIceCabinetCoolingMode);

   switch(blockNumber)
   {
      case 0:
      case 1:
         newIceCabinetCoolingMode = IceCabinetCoolingMode_ForcedCooling;
         break;

      case 2:
         if(currentIceCabinetCoolingMode == IceCabinetCoolingMode_ForcedCooling)
         {
            newIceCabinetCoolingMode = IceCabinetCoolingMode_ForcedCooling;
         }
         else
         {
            newIceCabinetCoolingMode = IceCabinetCoolingMode_ActiveCooling;
         }
         break;

      case 3:
      case 4:
         if(currentIceCabinetCoolingMode == IceCabinetCoolingMode_Neutral)
         {
            newIceCabinetCoolingMode = IceCabinetCoolingMode_Neutral;
         }
         else
         {
            newIceCabinetCoolingMode = IceCabinetCoolingMode_ActiveCooling;
         }
         break;

      case 5:
      case 6:
      case 7:
         newIceCabinetCoolingMode = IceCabinetCoolingMode_Neutral;
         break;
   }

   bool iceCabinetThermistorIsValid;
   DataModel_Read(
      dataModel,
      Erd_IceCabinetThermistor_IsValidResolved,
      &iceCabinetThermistorIsValid);

   if(iceCabinetThermistorIsValid == false)
   {
      newIceCabinetCoolingMode = IceCabinetCoolingMode_FallbackCooling;
   }

   PublishIceCabinetModeAndVote(dataModel, newIceCabinetCoolingMode);
}
