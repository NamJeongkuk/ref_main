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
#include "ResetNvErdImmediatelyWhenRamErdChangesToZero.h"
#include "Constants_Binary.h"

static const PeriodicNvUpdateServiceConfigurationEntry_t periodicNvUpdateTableEntries[] = {
   { Erd_DefrostCompressorOnTimeInSeconds, Erd_Eeprom_DefrostCompressorOnTimeInSeconds },
   { Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_LeftSideFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_RightSideFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_RightSideFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_DoorInDoorScaledDoorAccelerationInSeconds, Erd_Eeprom_DoorInDoorScaledDoorAccelerationInSeconds },
   { Erd_FreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds },
   { Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds },
   { Erd_TurboCoolOnTimeInMinutes, Erd_Eeprom_TurboCoolOnTimeInMinutes },
   { Erd_TurboFreezeOnTimeInMinutes, Erd_Eeprom_TurboFreezeOnTimeInMinutes },
   { Erd_TotalWaterVolumeUsageInOuncesX100, Erd_Eeprom_TotalWaterVolumeUsageInOuncesX100 },
   { Erd_WaterFilterCalendarUsageInSeconds, Erd_Eeprom_WaterFilterCalendarUsageInSeconds }
};

static const PeriodicNvUpdateServiceConfigurationEntry_t periodicNvUpdateDefrostHeaterOnTimeTableEntries[] = {
   { Erd_FreezerDefrostHeaterOnTimeInMinutes, Erd_Eeprom_FreezerDefrostHeaterOnTimeInMinutes }
};

static const PeriodicNvUpdateServiceConfiguration_t periodicNvUpdateConfiguration = {
   periodicNvUpdateTableEntries,
   NUM_ELEMENTS(periodicNvUpdateTableEntries)
};

static const PeriodicNvUpdateServiceConfiguration_t periodicNvUpdateDefrostHeaterOnTimeConfiguration = {
   periodicNvUpdateDefrostHeaterOnTimeTableEntries,
   NUM_ELEMENTS(periodicNvUpdateDefrostHeaterOnTimeTableEntries)
};

static const ResetNvErdImmediatelyWhenRamErdChangesToZeroEntry_t resetNvErdImmediatelyWhenRamErdChangesToZeroEntries[] = {
   { Erd_DefrostCompressorOnTimeInSeconds, Erd_Eeprom_DefrostCompressorOnTimeInSeconds },
   { Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_LeftSideFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_RightSideFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_RightSideFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_DoorInDoorScaledDoorAccelerationInSeconds, Erd_Eeprom_DoorInDoorScaledDoorAccelerationInSeconds },
   { Erd_FreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds },
   { Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds },
   { Erd_FreezerDefrostHeaterOnTimeInMinutes, Erd_Eeprom_FreezerDefrostHeaterOnTimeInMinutes },
   { Erd_TurboCoolOnTimeInMinutes, Erd_Eeprom_TurboCoolOnTimeInMinutes }
};

static const ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration_t resetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration = {
   resetNvErdImmediatelyWhenRamErdChangesToZeroEntries,
   NUM_ELEMENTS(resetNvErdImmediatelyWhenRamErdChangesToZeroEntries)
};

void PeriodicNvUpdaterPlugin_Init(PeriodicNvUpdaterPlugin_t *instance, I_DataModel_t *dataModel)
{
   const SystemMonitorData_t *systemMonitorData = PersonalityParametricData_Get(dataModel)->systemMonitorData;
   uint8_t periodicNvUpdateInMinutes = systemMonitorData->periodicNvUpdateInMinutes;
   uint8_t periodicNvUpdateDefrostHeaterOnTimeInMinutes = systemMonitorData->periodicNvUpdateDefrostHeaterOnTimeInMinutes;

   PeriodicNvUpdateService_Init(
      &instance->_private.periodicNvUpdate,
      DataModel_AsDataSource(dataModel),
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      periodicNvUpdateInMinutes * MSEC_PER_MIN,
      &periodicNvUpdateConfiguration);

   PeriodicNvUpdateService_Init(
      &instance->_private.periodicNvHeaterOnTimeUpdate,
      DataModel_AsDataSource(dataModel),
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      periodicNvUpdateDefrostHeaterOnTimeInMinutes * MSEC_PER_MIN,
      &periodicNvUpdateDefrostHeaterOnTimeConfiguration);

   ResetNvErdImmediatelyWhenRamErdChangesToZero_Init(
      dataModel,
      &resetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration);

   DataModel_Write(
      dataModel,
      Erd_PeriodicNvUpdaterReady,
      set);
}
