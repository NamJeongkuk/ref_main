/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SabbathPlugin.h"
#include "SabbathParameters.h"
#include "SystemErds.h"

static struct
{
   SabbathParameters_t sabbathParameters;
} instance;

static const SabbathParametersConfig_t sabbathParametersConfig = {
   .sabbathModeErd = Erd_SabbathMode,
   .maxTimeBetweenDefrostsInMinutesErd = Erd_MaxTimeBetweenDefrostsInMinutes
};

void SabbathPlugin_Init(I_DataModel_t *dataModel)
{
   SabbathParameters_Init(&instance.sabbathParameters, dataModel, &sabbathParametersConfig);
}
