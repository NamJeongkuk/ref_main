/*!
 * @file
 * @brief Dispensing Request owned by the UI to send to the Mainboard.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSINGREQUEST_H
#define DISPENSINGREQUEST_H

#include <stdint.h>
#include "DispensingAction.h"
#include "DispensingRequestSelection.h"
#include "DispensingSpecialOptions.h"
#include "DispenseStatus.h"

typedef struct
{
   DispensingAction_t action;
   DispensingRequestSelection_t selection;
   DispensingSpecialOptions_t specialOptions;
   uint8_t padding;
   uint16_t preciseFillOuncesx100;
} DispensingRequest_t;

typedef struct
{
   DispensingAction_t action;
   DispensingRequestSelection_t selection;
   DispensingSpecialOptions_t specialOptions;
   DispenseStatus_t status;
   uint16_t preciseFillOuncesx100;
} DispensingRequestStatus_t;

#endif
