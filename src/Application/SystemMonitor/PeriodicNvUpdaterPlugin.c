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
   { Erd_FreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_FreshFoodScaledDoorAccelerationInSeconds },
   { Erd_DoorInDoorScaledDoorAccelerationInSeconds, Erd_Eeprom_DoorInDoorScaledDoorAccelerationInSeconds },
   { Erd_FreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds },
   { Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds },
   { Erd_TurboCoolOnTimeInMinutes, Erd_Eeprom_TurboCoolOnTimeInMinutes },
   { Erd_TurboFreezeOnTimeInMinutes, Erd_Eeprom_TurboFreezeOnTimeInMinutes },
   { Erd_WaterFilterVolumeUsageInOuncesX100, Erd_Eeprom_WaterFilterVolumeUsageInOuncesX100 },
   { Erd_WaterFilterCalendarUsageInSeconds, Erd_Eeprom_WaterFilterCalendarUsageInSeconds },
   { Erd_UnitLifetimeDispensedWaterInOuncesX100, Erd_Eeprom_UnitLifetimeDispensedWaterInOuncesX100 },
   { Erd_SabbathWaitingForDefrostTimeInMinutes, Erd_Eeprom_SabbathWaitingForDefrostTimeInMinutes },
   { Erd_WaterFilterUsageSinceExpiration, Erd_Eeprom_WaterFilterUsageSinceExpiration },
   { Erd_EnhancedSabbathRunTimeInMinutes, Erd_Eeprom_EnhancedSabbathRunTimeInMinutes },
   { Erd_CurrentWaterFilterMonthTimeInMinutes, Erd_Eeprom_CurrentWaterFilterMonthTimeInMinutes },
   { Erd_PreviousWaterFilterVolumeUsageInOuncesX100, Erd_Eeprom_PreviousWaterFilterVolumeUsageInOuncesX100 }
};

static const PeriodicNvUpdateServiceConfiguration_t periodicNvUpdateConfiguration = {
   periodicNvUpdateTableEntries,
   NUM_ELEMENTS(periodicNvUpdateTableEntries)
};

static const ResetNvErdImmediatelyWhenRamErdChangesToZeroEntry_t resetNvErdImmediatelyWhenRamErdChangesToZeroEntries[] = {
   { Erd_DefrostCompressorOnTimeInSeconds, Erd_Eeprom_DefrostCompressorOnTimeInSeconds },
   { Erd_FreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_FreshFoodScaledDoorAccelerationInSeconds },
   { Erd_DoorInDoorScaledDoorAccelerationInSeconds, Erd_Eeprom_DoorInDoorScaledDoorAccelerationInSeconds },
   { Erd_FreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds },
   { Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds },
   { Erd_TurboCoolOnTimeInMinutes, Erd_Eeprom_TurboCoolOnTimeInMinutes },
   { Erd_TurboFreezeOnTimeInMinutes, Erd_Eeprom_TurboFreezeOnTimeInMinutes },
   { Erd_SabbathWaitingForDefrostTimeInMinutes, Erd_Eeprom_SabbathWaitingForDefrostTimeInMinutes },
   { Erd_WaterFilterUsageSinceExpiration, Erd_Eeprom_WaterFilterUsageSinceExpiration },
   { Erd_EnhancedSabbathRunTimeInMinutes, Erd_Eeprom_EnhancedSabbathRunTimeInMinutes },
   { Erd_CurrentWaterFilterMonthTimeInMinutes, Erd_Eeprom_CurrentWaterFilterMonthTimeInMinutes }
};

static const ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration_t resetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration = {
   resetNvErdImmediatelyWhenRamErdChangesToZeroEntries,
   NUM_ELEMENTS(resetNvErdImmediatelyWhenRamErdChangesToZeroEntries)
};

void PeriodicNvUpdaterPlugin_Init(PeriodicNvUpdaterPlugin_t *instance, I_DataModel_t *dataModel)
{
   const SystemMonitorData_t *systemMonitorData = PersonalityParametricData_Get(dataModel)->systemMonitorData;
   uint8_t periodicNvUpdateInMinutes = systemMonitorData->periodicNvUpdateInMinutes;

   PeriodicNvUpdateService_Init(
      &instance->_private.periodicNvUpdate,
      DataModel_AsDataSource(dataModel),
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      periodicNvUpdateInMinutes * MSEC_PER_MIN,
      &periodicNvUpdateConfiguration);

   ResetNvErdImmediatelyWhenRamErdChangesToZero_Init(
      dataModel,
      &resetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration);

   DataModel_Write(
      dataModel,
      Erd_PeriodicNvUpdaterReady,
      set);
}
