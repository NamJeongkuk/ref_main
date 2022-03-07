/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_PERSONALITY_H
#define DATASOURCE_PERSONALITY_H

#include "I_DataSource.h"
#include "BspErdRanges.h"
#include "utils.h"

enum
{
   Erd_Bsp_Personality = BspErdPersonalityStart
};

/*!
 */
I_DataSource_t *DataSource_Personality_Init(void);

#endif
