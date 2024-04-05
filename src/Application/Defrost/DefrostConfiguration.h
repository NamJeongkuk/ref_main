/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTCONFIGURATION_H
#define DEFROSTCONFIGURATION_H

#include "Defrost.h"
#include "DefrostHeaterMaxOnTime.h"
#include "TimeThatPrechillConditionsAreMet.h"
#include "DefrostTestRequestHandler.h"
#include "SabbathReadyToDefrost.h"

extern const DefrostConfiguration_t defrostConfiguration;
extern const DefrostHeaterMaxOnTimeConfiguration_t defrostHeaterMaxOnTimeConfig;
extern const TimeThatPrechillConditionsAreMetConfiguration_t timeThatPrechillConditionsAreMetConfig;
extern const DefrostTestRequestHandlerConfiguration_t defrostTestRequestHandlerConfig;
extern const SabbathReadyToDefrostConfig_t sabbathReadyToDefrostConfig;

#endif
