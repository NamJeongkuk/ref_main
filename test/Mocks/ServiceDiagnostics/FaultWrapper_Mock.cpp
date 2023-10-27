/*!
 * @file Fault Wrapper Mock
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FaultWrapper_Mock.h"
#include "CppUTestExt/MockSupport.h"

static void SetRequest(I_FaultWrapper_t *instance)
{
   mock().actualCall("SetRequest").onObject(instance);
}

static void ClearRequest(I_FaultWrapper_t *instance)
{
   mock().actualCall("ClearRequest").onObject(instance);
}

static void Reset(I_FaultWrapper_t *instance)
{
   mock().actualCall("Reset").onObject(instance);
}

static const I_FaultWrapper_Api_t api = { SetRequest, ClearRequest, Reset };

void FaultWrapper_Mock_Init(FaultWrapper_Mock_t *instance)
{
   instance->interface.api = &api;
}
