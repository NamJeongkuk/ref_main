/*!
 * @file
 * @brief Handles receiving and sending messages to the RFID board
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RFIDCOMMUNICATOR_H
#define RFIDCOMMUNICATOR_H

#include "I_DataSource.h"
#include "I_Event.h"
#include "RfidTypes.h"

typedef struct
{
   ReadWriteResult_t readWriteResult;
   RfidUid_t uid;
   RfidFilterIdentifier_t filterIdentifier;
   VolumeInOunces_t volumeUsageInOunces;
   CalendarUsageInSeconds_t calendarUsageInSeconds;
   LeakDetectedInSeconds_t leakDetectedTimeInSeconds;
   NumberOfWriteOperations_t numberOfWriteOperations;
   FilterStatusRfidBoard_t filterStatus;
   RfidFilterInitializationErrorBitfield_t initErrorBitfield;
   uint8_t unused[3];
   uint8_t lock;
   RfidFilterNumberOfUnitsFilterHasBeenOn_t numberOfUnitsFilterHasBeenOn;
   UnitSerialNumber_t unitSerialNumber;
   UnitSerialNumber_t previousUnitSerialNumber;
   RfidTwelveMonthsGallonUsage_t lastTwelveMonthsOfWaterUsageInGallons;
} RfidReceivedPayload_t;

typedef struct
{
   ReadWriteRequest_t readWriteRequest;
   RfidUid_t uid;
   uint32_t volumeUsageInOunces;
   uint32_t calendarUsageInSeconds;
   RfidFilterStatusMainboard_t filterStatus;
   uint8_t unused;
   uint8_t lock;
   RfidFilterNumberOfUnitsFilterHasBeenOn_t numberOfUnitsFilterHasBeenOn;
   UnitSerialNumber_t unitSerialNumber;
   UnitSerialNumber_t previousUnitSerialNumber;
   RfidTwelveMonthsGallonUsage_t lastTwelveMonthsOfWaterUsageInGallons;
} RfidMessagePayload_t;

typedef struct
{
   Erd_t readWriteResult; //! ReadWriteResult_t
   Erd_t filterUid; //! RfidUid_t
   Erd_t filterIdentifier; //! RfidFilterIdentifier_t
   Erd_t volumeWaterUsageInOunces; //! VolumeInOunces_t
   Erd_t calendarUsageInSeconds; //! CalendarUsageInSeconds_t
   Erd_t leakDetectedTimeInSeconds; //! LeakDetectedInSeconds_t
   Erd_t numberOfWriteOperations; //! NumberOfWriteOperations_t
   Erd_t filterStatus; //! FilterStatusRfidBoard_t
   Erd_t initializationErrorBitfield; //! RfidFilterInitializationErrorBitfield_t
   Erd_t lockByte; //! uint8_t
   Erd_t numberOfUnitsFilterHasBeenOn; //! RfidFilterNumberOfUnitsFilterHasBeenOn_t
   Erd_t unitSerialNumber; //! UnitSerialNumber_t
   Erd_t previousUnitSerialNumber; //! UnitSerialNumber_t
   Erd_t filterWaterUsageInGallons; //! RfidTwelveMonthsGallonUsage_t
} RfidCommunicatorRfidBoardErds_t;

typedef struct
{
   Erd_t uid; //! RfidUid_t
   Erd_t volumeWaterUsageInOunces; //! VolumeInOunces_t
   Erd_t calendarUsageInSeconds; //! CalendarUsageInSeconds_t
   Erd_t filterStatus; //! FilterStatusRfidBoard_t
   Erd_t lockByte; //! uint8_t
   Erd_t numberOfUnitsFilterHasBeenOn; //! NumberOfWriteOperations_t
   Erd_t unitSerialNumber; //! UnitSerialNumber_t
   Erd_t previousUnitSerialNumber; //! UnitSerialNumber_t
   Erd_t filterWaterUsageInGallons; //! RfidTwelveMonthsGallonUsage_t
} RfidCommunicatorMainboardErds_t;

typedef struct
{
   const RfidCommunicatorRfidBoardErds_t *rfidBoardErds;
   const RfidCommunicatorMainboardErds_t *mainboardErds;
   Erd_t dataRequestErd; //! ReadWriteRequest_t
   Erd_t geaMessageEndpointErd; //! I_Gea2MessageEndpoint_t
} RfidCommunicatorConfiguration_t;

typedef struct
{
   struct
   {
      I_DataSource_t *externalDataSource;
      EventSubscription_t dataSourceOnChangeSubscription;
      EventSubscription_t messageReceived;
      const RfidCommunicatorConfiguration_t *config;
   } _private;
} RfidCommunicator_t;

/*!
 * @brief
 * @param instance
 * @param dataSource
 * @param config
 */
void RfidCommunicator_Init(
   RfidCommunicator_t *instance,
   I_DataSource_t *externalDataSource,
   const RfidCommunicatorConfiguration_t *config);

#endif
