/*!
 * @file
 * @brief The header file to initialize hardware dependent modules.
 * This module will fill the data source with pointers to the hardware interfaces.
 *
 * Copyright General Electric Corporation - GE Confidential - All rights reserved.
 */

#ifndef HARDWARE_H
#define HARDWARE_H

#include "I_DataModel.h"

/*!
 * Initialize the first stage of hardware
 */
void Hardware_InitializeStage1(void);

/*!
 * Initialize the second stage of hardware
 * @param dataModel
 */
void Hardware_InitializeStage2(I_DataModel_t *dataModel);

#endif
