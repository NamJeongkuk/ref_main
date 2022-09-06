/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef INPUTCAPTURE_FEEDBACKCOUNTCALCULATION_H
#define INPUTCAPTURE_FEEDBACKCOUNTCALCULATION_H

#include "InputCaptureType.h"

InputCaptureCounts_t CalculateFeedbackCounts(uint16_t previousCounts, uint16_t currentCounts, uint8_t overflows);

#endif
