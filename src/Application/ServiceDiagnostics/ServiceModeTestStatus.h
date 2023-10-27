/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODETESTSTATUS_H
#define SERVICEMODETESTSTATUS_H

#include <stdint.h>
#include "ServiceModeTestNumber.h"

enum
{
   ServiceModeTestStatusResponse_Inactive,
   ServiceModeTestStatusResponse_Running,
   ServiceModeTestStatusResponse_Stopped,
   ServiceModeTestStatusResponse_NotSupported = 0xFF
};
typedef uint8_t ServiceModeTestStatusResponse_t;

enum
{
   ServiceModeTestStatusDataFormat_Unused,
   ServiceModeTestStatusDataFormat_DegreesFx100,
   ServiceModeTestStatusDataFormat_HumidityPercentagex100,
   ServiceModeTestStatusDataFormat_Rpm,
   ServiceModeTestStatusDataFormat_Bool,
   ServiceModeTestStatusDataFormat_TwoU8s,
   ServiceModeTestStatusDataFormat_TwoI8s,
   ServiceModeTestStatusDataFormat_U16,
   ServiceModeTestStatusDataFormat_I16,
   ServiceModeTestStatusDataFormat_OutOfRange,
   ServiceModeTestStatusDataFormat_Voltsx100,
   ServiceModeTestStatusDataFormat_VersionInfo,
   ServiceModeTestStatusDataFormat_FullNotFull,
   ServiceModeTestStatusDataFormat_PresentNotPresent,
   ServiceModeTestStatusDataFormat_Pwm
};
typedef uint8_t ServiceModeTestStatusDataFormat_t;

typedef struct
{
   ServiceModeTestStatusResponse_t testResponse;
   ServiceModeTestStatusDataFormat_t dataFormat;
   ServiceModeTestNumber_t testNumber;
   uint8_t diagnosticData[4];
} ServiceModeTestStatus_t;

#define SWAP_DEFINITION_ServiceModeTestStatus_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, testNumber),

#endif
