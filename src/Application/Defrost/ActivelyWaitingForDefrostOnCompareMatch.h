/*!
 * @file
 * @brief Matches Defrost HSM state to whether or not actively waiting for next defrost
 * Actively waiting for next defrost is true when Defrost HSM state is Idle or Post Dwell
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ACTIVELYWAITINGFORDEFROSTONCOMPAREMATCH_H
#define ACTIVELYWAITINGFORDEFROSTONCOMPAREMATCH_H

#include "I_DataModel.h"

void ActivelyWaitingForDefrostOnCompareMatch(I_DataModel_t *dataModel);

#endif
