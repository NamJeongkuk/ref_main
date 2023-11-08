/*!
 * @file
 * @brief Types for RFID communication
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RFIDTYPES_H
#define RFIDTYPES_H

#include "Signal.h"
#include <stdint.h>

enum
{
   RfidUidSizeInBytes = 7,
   FilterIdentifierSizeInBytes = 16,
   UnitSerialNumberSizeInBytes = 8,
   MonthlyGallonUsageSizeInBytes = 12
};

enum
{
   ReadWriteResult_Success,
   ReadWriteResult_ReadFailure,
   ReadWriteResult_EepromWriteFailure,
   ReadWriteResult_UidMismatch,
   ReadWriteResult_TagUidIsInvalid,
   ReadWriteResult_ReadBeforeWriteFailure,
   ReadWriteResult_ReadAfterWriteFailure,
   ReadWriteResult_HardwareFailure
};
typedef uint8_t ReadWriteResult_t;

enum
{
   ReadWriteRequest_Read = 0,
   ReadWriteRequest_Write = 1,
   ReadWriteRequest_Idle = 0xFF
};
typedef uint8_t ReadWriteRequest_t;

typedef struct
{
   uint8_t tagUid[RfidUidSizeInBytes];
} RfidUid_t;

typedef struct
{
   uint8_t filterIdentifier[FilterIdentifierSizeInBytes];
} RfidFilterIdentifier_t;

typedef uint32_t LeakDetectedInSeconds_t;

#define SWAP_DEFINITION_LeakDetectedInSeconds_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

typedef uint32_t NumberOfWriteOperations_t;

#define SWAP_DEFINITION_NumberOfWriteOperations_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

enum
{
   RfidFilterStatusBits_Blocked = 0,
};
typedef uint8_t RfidFilterStatusBits_t;

typedef uint8_t RfidFilterStatus_t;

enum
{
   RfidFilterStatusBits_Blocked_RfidBoard = 0,
   RfidFilterStatusBits_LeakDetected = 1
};
typedef uint8_t RfidFilterStatusBits_RfidBoard_t;

typedef uint16_t FilterStatusRfidBoard_t;
#define SWAP_DEFINITION_FilterStatusRfidBoard_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

enum
{
   RfidFilterInitializationErrorBits_SamInitializationFailure = 0,
   RfidFilterInitializationErrorBits_TransceiverInitializationFailure = 1
};
typedef uint8_t RfidFilterInitializationErrorBits_t;

typedef uint8_t RfidFilterInitializationErrorBitfield_t;

typedef uint8_t RfidFilterNumberOfUnitsFilterHasBeenOn_t;

typedef struct
{
   uint8_t unitSerialNumber[UnitSerialNumberSizeInBytes];
} UnitSerialNumber_t;

typedef struct
{
   uint8_t gallonUsage[MonthlyGallonUsageSizeInBytes];
} RfidTwelveMonthsGallonUsage_t;

typedef struct
{
   ReadWriteResult_t result;
   Signal_t signal;
} RfidFilterReadWriteResult_t;

#endif
