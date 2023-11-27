/*!
 * @file
 * @brief Overwrites the data on the mainboard with new filter data when a new filter is installed
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NEWFILTERINSTALLEDHANDLER_H
#define NEWFILTERINSTALLEDHANDLER_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t rfidFilterUidRfidBoardErd; // RfidUid_t
   Erd_t rfidFilterWaterVolumeUsageInOuncesX100RfidBoardErd; // VolumeInOuncesX100_t
   Erd_t rfidFilterCalendarUsageInSecondsRfidBoardErd; // CalendarUsageInSeconds_t
   Erd_t rfidFilterLastTwelveMonthsOfWaterUsageInGallonsRfidBoardErd; // RfidTwelveMonthsGallonUsage_t
   Erd_t rfidFilterNumberOfUnitsRfidFilterHasBeenOnRfidBoardErd; // RfidFilterNumberOfUnitsFilterHasBeenOn_t
   Erd_t rfidFilterUnitSerialNumberRfidBoardErd; // UnitSerialNumber_t
} NewFilterInstalledHandlerReadErds_t;

typedef struct
{
   Erd_t rfidFilterUidErd; // RfidUid_t
   Erd_t totalWaterVolumeUsageInOuncesX100Erd; // VolumeInOuncesX100_t
   Erd_t eepromTotalWaterVolumeUsageInOuncesX100Erd; // VolumeInOuncesX100_t
   Erd_t rfidFilterCalendarUsageInSecondsErd; // CalendarUsageInSeconds_t
   Erd_t eepromWaterFilterCalendarUsageInSecondsErd; // CalendarUsageInSeconds_t
   Erd_t rfidFilterLastTwelveMonthsOfWaterUsageInGallonsErd; // RfidTwelveMonthsGallonUsage_t
   Erd_t rfidFilterNumberOfUnitsFilterHasBeenOnErd; // RfidFilterNumberOfUnitsFilterHasBeenOn_t
   Erd_t rfidFilterPreviousUnitSerialNumberErd; // UnitSerialNumber_t
   Erd_t totalValveOnTimeInSecondsErd; // uint32_t
} NewFilterInstalledHandlerWriteErds_t;

typedef struct
{
   const NewFilterInstalledHandlerReadErds_t *readErds;
   const NewFilterInstalledHandlerWriteErds_t *writeErds;
   Erd_t newFilterInstalledSignalErd; // Signal_t
   Erd_t rfidBoardInSystemErd; // bool
   Erd_t rfidFilterIdentifierRfidBoardErd; // RfidFilterIdentifier_t
   Erd_t rfidFilterLeakDetectedFaultErd; // bool
   Erd_t rfidFilterBlockedTagFaultErd; // bool
   Erd_t bypassPlugInstalledErd; // bool
   Erd_t rfidFilterUnitSerialNumberErd; // UnitSerialNumber_t
} NewFilterInstalledHandlerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const NewFilterInstalledHandlerConfig_t *config;
      EventSubscription_t newFilterInstalledSignalChangedSubscription;
   } _private;
} NewFilterInstalledHandler_t;

void NewFilterInstalledHandler_Init(
   NewFilterInstalledHandler_t *instance,
   I_DataModel_t *dataModel,
   const NewFilterInstalledHandlerConfig_t *config);

#endif
