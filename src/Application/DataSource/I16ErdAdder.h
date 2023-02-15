/*!
 * @file
 * @brief I16 ERD Adder
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef I16ERDADDER_H
#define I16ERDADDER_H

#include "I_DataModel.h"
#include "ErdList.h"

typedef struct
{
   Erd_t resultErd;
   const ErdList_t i16ErdsToBeAdded;
} I16ErdAdderConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      I_DataModel_t *dataModel;
      const I16ErdAdderConfiguration_t *config;
   } _private;
} I16ErdAdder_t;

/*!
 * Initialize an I16 Erd Adder
 * @param instance The object instance
 * @param dataModel instance of dataModel
 * @param config contains result erd and erd list to be added
 */
void I16ErdAdder_Init(
   I16ErdAdder_t *instance,
   I_DataModel_t *dataModel,
   const I16ErdAdderConfiguration_t *config);

#endif
