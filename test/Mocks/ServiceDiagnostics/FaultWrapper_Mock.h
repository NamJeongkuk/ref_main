/*!
 * @file Fault Wrapper Mock
 * @brief Fault Wrapper Mock to ensure that fault requests get called sequentially
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FAULTWRAPPER_MOCK_CPP
#define FAULTWRAPPER_MOCK_CPP

#include "I_FaultWrapper.h"

typedef struct
{
   I_FaultWrapper_t interface;
} FaultWrapper_Mock_t;

void FaultWrapper_Mock_Init(FaultWrapper_Mock_t *instance);

#endif
