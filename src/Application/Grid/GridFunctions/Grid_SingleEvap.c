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

static void SetCoolingMode(I_DataModel_t *dataModel, CoolingMode_t coolingMode)
{
   DataModel_Write(dataModel, Erd_CoolingMode, &coolingMode);
}

static CoolingSpeed_t GetCoolingSpeed(I_DataModel_t *dataModel)
{
   CoolingSpeed_t coolingSpeed;
   DataModel_Read(dataModel, Erd_CoolingSpeed, &coolingSpeed);
   return coolingSpeed;
}

static void SetGridArea(I_DataModel_t *dataModel, GridArea_t gridArea)
{
   DataModel_Write(dataModel, Erd_GridArea, &gridArea);
}

static GridArea_t GetGridArea(I_DataModel_t *dataModel)
{
   GridArea_t gridArea;
   DataModel_Read(dataModel, Erd_GridArea, &gridArea);
   return gridArea;
}

void Grid_SingleEvap(void *context)
{
   I_DataModel_t *dataModel = context;

   GridBlockNumber_t blockNumber;
   DataModel_Read(dataModel, Erd_Grid_BlockNumber, &blockNumber);

   switch(blockNumber)
   {
      case 0:
      case 1:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 2:
      case 3:
         break;

      case 4:
      case 5:
      case 6:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 7:
      case 8:
      case 14:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 9:
      case 10:
         break;

      case 11:
      case 12:
      case 13:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 15:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 16:
      case 17:
         break;

      case 18:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 19:
      case 20:
      case 27:
      case 34:
      case 41:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 21:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 22:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 23:
      case 24:
         if(GetCoolingSpeed(dataModel) == CoolingSpeed_Off)
         {
            SetCoolingMode(dataModel, CoolingMode_FreshFood);
         }
         break;

      case 25:
      case 32:
      case 39:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 26:
      case 33:
      case 40:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 28:
      case 29:
      case 35:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 30:
         break;

      case 31:
         if(GetCoolingSpeed(dataModel) == CoolingSpeed_Off)
         {
            SetCoolingMode(dataModel, CoolingMode_Freezer);
         }
         else
         {
            SetCoolingMode(dataModel, CoolingMode_FreshFood);
         }
         break;

      case 36:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 37:
         break;

      case 38:
         if(GetCoolingSpeed(dataModel) == CoolingSpeed_Off)
         {
            SetCoolingMode(dataModel, CoolingMode_Freezer);
         }
         else
         {
            SetCoolingMode(dataModel, CoolingMode_FreshFood);
         }
         break;

      case 42:
      case 43:
         SetCoolingMode(dataModel, CoolingMode_Freezer);
         SetGridArea(dataModel, GridArea_1);
         break;

      case 44:
      case 45:
         if(GetGridArea(dataModel) == GridArea_1)
         {
            SetCoolingMode(dataModel, CoolingMode_Freezer);
         }
         else
         {
            SetCoolingMode(dataModel, CoolingMode_FreshFood);
         }
         break;

      case 46:
      case 47:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetGridArea(dataModel, GridArea_2);
         break;

      case 48:
         SetCoolingMode(dataModel, CoolingMode_FreshFood);
         SetGridArea(dataModel, GridArea_2);
         break;

      default:
         break;
   }
}
