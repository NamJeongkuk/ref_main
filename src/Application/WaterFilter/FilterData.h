/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FILTERDATA_H
#define FILTERDATA_H

#include "CommonFilterData.h"
#include "RfidFilterData.h"

typedef struct
{
   CommonFilterData_t *commonFilterData;
   RfidFilterData_t *rfidFilterData;
} FilterData_t;

#endif
