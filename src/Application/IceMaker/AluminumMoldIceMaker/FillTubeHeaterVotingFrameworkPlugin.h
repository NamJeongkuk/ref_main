/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FILLTUBEHEATERVOTINGFRAMEWORKPLUGIN_H
#define FILLTUBEHEATERVOTINGFRAMEWORKPLUGIN_H

#include "ErdResolver.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      ErdResolver_t fillTubeHeaterErdResolver;
   } _private;
} FillTubeHeaterVotingFrameworkPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FillTubeHeaterVotingFrameworkPlugin_Init(FillTubeHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel);

#endif
