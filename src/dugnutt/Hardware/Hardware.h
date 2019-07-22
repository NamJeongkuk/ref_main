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
 * The hardware initialization function before start main function.
 * @param applicationDataModel Data Model to store hardware pointers.
 */
void Hardware_Init(I_DataModel_t *internalDataModel);

#endif
