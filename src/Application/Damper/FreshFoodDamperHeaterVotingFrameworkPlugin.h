/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODDAMPERHEATERVOTINGFRAMEWORKPLUGIN_H
#define FRESHFOODDAMPERHEATERVOTINGFRAMEWORKPLUGIN_H

#include "ErdResolver.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      ErdResolver_t freshFoodDamperHeaterErdResolver;
   } _private;
} FreshFoodDamperHeaterVotingFrameworkPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FreshFoodDamperHeaterVotingFrameworkPlugin_Init(FreshFoodDamperHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel);

#endif
