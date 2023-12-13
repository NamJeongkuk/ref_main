/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "NewFilterInstalledHandler.h"
#include "utils.h"
#include "RfidTypes.h"
#include "CalendarUsageInSeconds.h"
#include "VolumeInOuncesX100.h"
#include "Constants_Binary.h"
#include "RfidFaultHandler.h"

static const uint8_t BypassFilterIdentifier[FilterIdentifierSizeInBytes] = "BPXWFE";

static void CopyUidToMainboard(NewFilterInstalledHandler_t *instance)
{
   RfidUid_t rfidFilterUidRfidBoard;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->readErds->rfidFilterUidRfidBoardErd,
      &rfidFilterUidRfidBoard);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->writeErds->rfidFilterUidErd,
      &rfidFilterUidRfidBoard);
}

static void WriteToVolumeUsageErds(NewFilterInstalledHandler_t *instance, VolumeInOuncesX100_t volumeUsageInOuncesX100)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->writeErds->waterFilterVolumeUsageInOuncesX100Erd,
      &volumeUsageInOuncesX100);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->writeErds->eepromwaterFilterVolumeUsageInOuncesX100Erd,
      &volumeUsageInOuncesX100);
}

static void WriteToCalendarUsageErds(NewFilterInstalledHandler_t *instance, CalendarUsageInSeconds_t calendarUsageInSeconds)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->writeErds->rfidFilterCalendarUsageInSecondsErd,
      &calendarUsageInSeconds);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->writeErds->eepromWaterFilterCalendarUsageInSecondsErd,
      &calendarUsageInSeconds);
}

static void CopyVolumeUsageToMainboard(NewFilterInstalledHandler_t *instance)
{
   VolumeInOuncesX100_t volumeUsageInOuncesX100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->readErds->rfidFilterWaterVolumeUsageInOuncesX100RfidBoardErd,
      &volumeUsageInOuncesX100);

   WriteToVolumeUsageErds(instance, volumeUsageInOuncesX100);
}

static void CopyCalendarUsageToMainboard(NewFilterInstalledHandler_t *instance)
{
   CalendarUsageInSeconds_t calendarUsageInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->readErds->rfidFilterCalendarUsageInSecondsRfidBoardErd,
      &calendarUsageInSeconds);

   WriteToCalendarUsageErds(instance, calendarUsageInSeconds);
}

static void CopyLastTwelveMonthsOfWaterUsageToMainboard(NewFilterInstalledHandler_t *instance)
{
   RfidTwelveMonthsGallonUsage_t lastTwelveMonthsGallons;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->readErds->rfidFilterLastTwelveMonthsOfWaterUsageInGallonsRfidBoardErd,
      &lastTwelveMonthsGallons);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->writeErds->rfidFilterLastTwelveMonthsOfWaterUsageInGallonsErd,
      &lastTwelveMonthsGallons);
}

static void CopyNumberOfUnitsRfidFilterHasBeenOnToMainboard(NewFilterInstalledHandler_t *instance)
{
   RfidFilterNumberOfUnitsFilterHasBeenOn_t numberOfUnits;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->readErds->rfidFilterNumberOfUnitsRfidFilterHasBeenOnRfidBoardErd,
      &numberOfUnits);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->writeErds->rfidFilterNumberOfUnitsFilterHasBeenOnErd,
      &numberOfUnits);
}

static void CopyCurrentSerialNumberToPreviousSerialNumber(NewFilterInstalledHandler_t *instance)
{
   UnitSerialNumber_t rfidFilterUnitSerialNumber;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->readErds->rfidFilterUnitSerialNumberRfidBoardErd,
      &rfidFilterUnitSerialNumber);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->writeErds->rfidFilterPreviousUnitSerialNumberErd,
      &rfidFilterUnitSerialNumber);
}

static void UpdateUnitWithNewFilterData(NewFilterInstalledHandler_t *instance)
{
   CopyUidToMainboard(instance);
   CopyVolumeUsageToMainboard(instance);
   CopyCalendarUsageToMainboard(instance);
   CopyLastTwelveMonthsOfWaterUsageToMainboard(instance);
   CopyNumberOfUnitsRfidFilterHasBeenOnToMainboard(instance);
}

static void ClearVolumeAndCalendarUsage(NewFilterInstalledHandler_t *instance)
{
   WriteToVolumeUsageErds(instance, CLEAR);
   WriteToCalendarUsageErds(instance, CLEAR);
}

static bool RfidBoardInSystem(NewFilterInstalledHandler_t *instance)
{
   bool rfidBoardInSystem;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidBoardInSystemErd,
      &rfidBoardInSystem);

   return rfidBoardInSystem;
}

static bool FilterIsBypassFilter(NewFilterInstalledHandler_t *instance)
{
   RfidFilterIdentifier_t filterId;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterIdentifierRfidBoardErd,
      &filterId);
   return memcmp(&filterId, &BypassFilterIdentifier, sizeof(RfidFilterIdentifier_t)) == 0;
}

static void SetBypassPlugInstalledTo(NewFilterInstalledHandler_t *instance, const bool *state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->bypassPlugInstalledErd,
      state);
}

static void RequestRfidFault(NewFilterInstalledHandler_t *instance, Erd_t erd, bool status)
{
   RfidFaultRequest_t rfidFaultRequest;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFaultRequestErd,
      &rfidFaultRequest);

   rfidFaultRequest.faultErd = erd;
   rfidFaultRequest.requestStatus = status;
   rfidFaultRequest.signal++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rfidFaultRequestErd,
      &rfidFaultRequest);
}

static void ClearRfidLeakFault(NewFilterInstalledHandler_t *instance)
{
   RequestRfidFault(instance, instance->_private.config->rfidFilterLeakDetectedFaultErd, CLEAR);
}

static void ClearRfidBlockedTagFault(NewFilterInstalledHandler_t *instance)
{
   RequestRfidFault(instance, instance->_private.config->rfidFilterBlockedTagFaultErd, CLEAR);
}

static bool SerialNumbersMatch(UnitSerialNumber_t serialNumber, UnitSerialNumber_t unitSerialNumber)
{
   return memcmp(&serialNumber, &unitSerialNumber, sizeof(UnitSerialNumber_t)) == 0;
}

static bool NewSerialNumber(NewFilterInstalledHandler_t *instance)
{
   UnitSerialNumber_t unitSerialNumber;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->readErds->rfidFilterUnitSerialNumberRfidBoardErd,
      &unitSerialNumber);

   UnitSerialNumber_t rfidFilterUnitSerialNumber;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterUnitSerialNumberErd,
      &rfidFilterUnitSerialNumber);

   return !SerialNumbersMatch(unitSerialNumber, rfidFilterUnitSerialNumber);
}

static void IncrementNumberOfUnitsFilterHasBeenOn(NewFilterInstalledHandler_t *instance)
{
   uint8_t numberOfUnitsFilterHasBeenOn;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->writeErds->rfidFilterNumberOfUnitsFilterHasBeenOnErd,
      &numberOfUnitsFilterHasBeenOn);
   numberOfUnitsFilterHasBeenOn = CLAMP(numberOfUnitsFilterHasBeenOn + 1, 0, UINT8_MAX);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->writeErds->rfidFilterNumberOfUnitsFilterHasBeenOnErd,
      &numberOfUnitsFilterHasBeenOn);
}

static void NewFilterInstalledSignalChanged(void *context, const void *args)
{
   NewFilterInstalledHandler_t *instance = context;
   IGNORE(args);

   if(RfidBoardInSystem(instance))
   {
      UpdateUnitWithNewFilterData(instance);

      if(FilterIsBypassFilter(instance))
      {
         SetBypassPlugInstalledTo(instance, set);
         ClearRfidLeakFault(instance);
         ClearRfidBlockedTagFault(instance);
      }
      else
      {
         SetBypassPlugInstalledTo(instance, clear);
      }

      if(NewSerialNumber(instance))
      {
         IncrementNumberOfUnitsFilterHasBeenOn(instance);
         CopyCurrentSerialNumberToPreviousSerialNumber(instance);
      }
   }
   else
   {
      ClearVolumeAndCalendarUsage(instance);
   }
}

void NewFilterInstalledHandler_Init(
   NewFilterInstalledHandler_t *instance,
   I_DataModel_t *dataModel,
   const NewFilterInstalledHandlerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.newFilterInstalledSignalChangedSubscription,
      instance,
      NewFilterInstalledSignalChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->newFilterInstalledSignalErd,
      &instance->_private.newFilterInstalledSignalChangedSubscription);
}
