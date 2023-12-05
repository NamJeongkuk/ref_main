/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODETESTREQUEST_H
#define SERVICEMODETESTREQUEST_H

#include <stdint.h>
#include "ServiceModeTestNumber.h"

enum
{
   ServiceModeTestRequestCommand_Stop,
   ServiceModeTestRequestCommand_Start,
   ServiceModeTestRequestCommand_Clear = 0xFF
};
typedef uint8_t ServiceModeTestRequestCommand_t;

typedef struct
{
   ServiceModeTestNumber_t testNumber;
   ServiceModeTestRequestCommand_t testRequestCommand;
   uint8_t padding;
} ServiceModeTestRequest_t;

#define SWAP_DEFINITION_ServiceModeTestRequest_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, testNumber),

#endif
