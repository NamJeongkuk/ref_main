/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PeriodicNvUpdaterPlugin.h"
#include "Constants_Time.h"
#include "PersonalityParametricData.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

static const PeriodicNvUpdateServiceConfigurationEntry_t periodicNvUpdateTableEntries[] = {
   { Erd_DefrostTimerCountInSeconds, Erd_Eeprom_DefrostTimerCountInSeconds },
   { Erd_DefrostFreezerDoorAccelerationCount, Erd_Eeprom_DefrostFreezerDoorAccelerationCount },
   { Erd_DefrostFreshFoodDoorAccelerationCount, Erd_Eeprom_DefrostFreshFoodDoorAccelerationCount },
   { Erd_DefrostConvertibleCompartmentDoorAccelerationCount, Erd_Eeprom_DefrostConvertibleCompartmentDoorAccelerationCount }
};

static const PeriodicNvUpdateServiceConfiguration_t periodicNvUpdateConfiguration = {
   periodicNvUpdateTableEntries,
   NUM_ELEMENTS(periodicNvUpdateTableEntries)
};

void PeriodicNvUpdaterPlugin_Init(PeriodicNvUpdaterPlugin_t *instance, I_DataModel_t *dataModel)
{
   uint8_t periodicNvUpdateInMinutes = PersonalityParametricData_Get(dataModel)->systemMonitorData->periodicNvUpdateInMinutes;

   PeriodicNvUpdateService_Init(
      &instance->_private.periodicNvUpdate,
      DataModel_AsDataSource(dataModel),
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      periodicNvUpdateInMinutes * MSEC_PER_MIN,
      &periodicNvUpdateConfiguration);
}
