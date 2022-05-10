/*!
 * @file
 * @brief Interface to set and clear faults.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef I_FAULTWRAPPER_H
#define I_FAULTWRAPPER_H

#include "I_DataModel.h"

typedef struct
{
   const struct I_FaultWrapper_Api_t *api;
} I_FaultWrapper_t;

typedef struct I_FaultWrapper_Api_t
{
   /*!
    * Request to the wrapper to set the fault
    * @param instance The instance of the I_FaultWrapper_t
    */
   void (*SetRequest)(I_FaultWrapper_t *instance);

   /*!
    * Request to the wrapper to clear the fault.
    * @param instance The instance of the I_FaultWrapper_t
    */
   void (*ClearRequest)(I_FaultWrapper_t *instance);

   /*!
    * Request to the wrapper to reset.
    * Clears fault and
    * resets internal counts to zero so the fault wrapper
    * can "Start over" regardless of previous requests.
    * @param instance The instance of the I_FaultWrapper_t
    */
   void (*Reset)(I_FaultWrapper_t *instance);
} I_FaultWrapper_Api_t;

#define FaultWrapper_SetRequest(instance) \
   (instance)->api->SetRequest((instance))

#define FaultWrapper_ClearRequest(instance) \
   (instance)->api->ClearRequest((instance))

#define FaultWrapper_Reset(instance) \
   (instance)->api->Reset((instance))

#endif
