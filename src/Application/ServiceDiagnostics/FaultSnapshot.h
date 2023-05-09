/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FAULTSNAPSHOT_H
#define FAULTSNAPSHOT_H

#include "ServiceDiagnosticsRevision3.h"
#include "FaultSnapshotData.h"

typedef struct
{
   ServiceDiagnosticsRevision3SnapshotHeader_t header;
   FaultSnapshotData_t data;
} FaultSnapshot_t;

#define SWAP_DEFINITION_FaultSnapshot_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, header.cycleNumber),   \
      SWAPPED_FIELD(_name, _type, header.timeStamp),  \
      SWAPPED_FIELD(_name, _type, header.faultCode),

#endif
