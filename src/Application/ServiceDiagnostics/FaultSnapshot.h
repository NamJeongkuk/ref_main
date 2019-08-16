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

#endif
