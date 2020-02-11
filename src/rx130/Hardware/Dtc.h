/*!
 * @file
 * @brief The DTC module
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DTC_H
#define DTC_H

#include <stdint.h>
#include "BufferedUart_Channel5.h"
#include "BufferedUart_Channel6.h"
#include "Event_Synchronous.h"

// This is valid for little endian only.
struct st_dtc_full {
    uint16_t reserved;
    uint8_t MRB;
    uint8_t MRA;
    void * SAR;
    void * DAR;
    struct{
        uint16_t CRB:16;
        uint16_t CRA:16;
    }CR;
};

/*!
 * Initialize and start DTC
 */
void Dtc_Init(void);

#endif
