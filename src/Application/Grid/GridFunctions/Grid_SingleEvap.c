/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid_SingleEvap.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"
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

static GridArea_t GetGridArea(I_DataModel_t *dataModel)
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

void Grid_SingleEvap(void *context)
{
   I_DataModel_t *dataModel = context;

   GridBlockNumber_t blockNumber;
   DataModel_Read(dataModel, Erd_Grid_BlockNumber, &blockNumber);

   CoolingMode_t currentCoolingMode = GetCoolingMode(dataModel);
   CoolingSpeed_t currentCoolingSpeed = GetCoolingSpeed(dataModel);
   GridArea_t currentGridArea = GetGridArea(dataModel);
   bool pullDownActive = GetPulldownActive(dataModel);
   bool compressorTripMitigationActive = GetCompressorTripMitigationActive(dataModel);

   switch(blockNumber)
   {
      case 0:
      case 1:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetPulldownActive(dataModel, SET);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 2:
      case 3:
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetPulldownActive(dataModel, SET);
         break;

      case 4:
      case 5:
      case 6:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_PullDown);
         SetGridArea(dataModel, GridArea_2);
         SetPulldownActive(dataModel, SET);
         break;

      case 7:
      case 8:
      case 14:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 9:
      case 10:
         SetCoolingSpeed(dataModel,
            (pullDownActive && compressorTripMitigationActive) ? CoolingSpeed_High
               : (currentCoolingSpeed != CoolingSpeed_High)    ? CoolingSpeed_Mid
                                                               : CoolingSpeed_High);
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
         break;

      case 16:
      case 17:
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
         break;

      case 21:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_Low);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 22:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_Low) ? CoolingSpeed_Low : currentCoolingSpeed);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 23:
      case 24:
         SetCoolingMode(dataModel, (currentCoolingSpeed == CoolingSpeed_Off) ? CoolingMode_FreshFood : currentCoolingMode);
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
         SetCoolingSpeed(dataModel, CoolingSpeed_High);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_1);
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
         break;

      case 37:
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_Off) ? CoolingSpeed_Low : CoolingSpeed_Off);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 38:
         SetCoolingMode(dataModel, (currentCoolingSpeed != CoolingSpeed_Off) ? CoolingMode_FreshFood : CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, (currentCoolingSpeed != CoolingSpeed_Off) ? CoolingSpeed_Low : CoolingSpeed_Off);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 42:
      case 43:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetCoolingSpeed(dataModel, CoolingSpeed_Off);
         SetPulldownActive(dataModel, CLEAR);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 44:
      case 45:
         SetCoolingMode(dataModel, (currentGridArea == GridArea_1) ? CoolingMode_Freezer : CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_Off);
         SetPulldownActive(dataModel, CLEAR);
         break;

      case 46:
      case 47:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_Low);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 48:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetCoolingSpeed(dataModel, CoolingSpeed_Mid);
         SetGridArea(dataModel, GridArea_2);
         break;

      default:
         break;
   }
}
