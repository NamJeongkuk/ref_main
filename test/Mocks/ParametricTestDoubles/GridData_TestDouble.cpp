/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GridData_TestDouble.h"
#include "Constants_Time.h"
#include <string.h>

enum
{
   SixGridLines = 6,
   TwoDimensional = 2,
};

static const DeltaGridLineData_t freshFoodGridLineData[] = {
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b0010,
   },
   {
      .gridLinesDegFx100 = -450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 150,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 950,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 1150,
      .bitMapping = 0b1000,
   },
};

static const DeltaGridLineData_t freezerGridLineData[] = {
   {
      .gridLinesDegFx100 = -250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 600,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 750,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 5500,
      .bitMapping = 0b0010,
   },
};

static const DeltaAxisGridLines_t freshFoodAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freshFoodGridLineData
};

static const DeltaAxisGridLines_t freezerAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freezerGridLineData
};

static DeltaAxisGridLines_t parametricGrid[] = { freshFoodAxis, freezerAxis };

static DeltaGridLines_t deltaGrid = {
   .dimensions = TwoDimensional,
   .gridLines = parametricGrid
};

void GridData_TestDouble_Init(GridData_t *gridData)
{
   memset(gridData, 0, sizeof(GridData_t));

   gridData->gridId = 0;
   gridData->gridConfigWord = 0;
   gridData->deltaGridLines = &deltaGrid;
   gridData->gridPeriodicRunRateInMSec = 1 * MSEC_PER_SEC;
}
