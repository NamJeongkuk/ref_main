/*!
 * @file
 * @brief When RAM ERD is written to zero, this immediately writes to the corresponding EEPROM ERD
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RESETNVERDIMMEDIATELYWHENRAMERDCHANGESTOZERO_H
#define RESETNVERDIMMEDIATELYWHENRAMERDCHANGESTOZERO_H

#include "I_DataModel.h"

typedef struct
{
   const Erd_t sourceErd;
   const Erd_t destinationErd;
} ResetNvErdImmediatelyWhenRamErdChangesToZeroEntry_t;

typedef struct
{
   const ResetNvErdImmediatelyWhenRamErdChangesToZeroEntry_t *entries;
   size_t entryCount;
} ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration_t;

void ResetNvErdImmediatelyWhenRamErdChangesToZero_Init(
   I_DataModel_t *dataModel,
   const ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration_t *config);

#endif
