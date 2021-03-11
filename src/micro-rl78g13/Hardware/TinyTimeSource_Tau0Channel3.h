/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TINYTIMESOURCE_TAU0CHANNEL3_H
#define TINYTIMESOURCE_TAU0CHANNEL3_H

#include "I_TinyTimeSource.h"
#include "I_TinyEvent.h"

/*!
 * @return
 */
I_TinyTimeSource_t *TinyTimeSource_Tau0Channel3_Init(void);

/*!
 * @return
 */
I_TinyEvent_t *TinyTimeSource_Tau0Channel3_Interrupt(void);

#endif
