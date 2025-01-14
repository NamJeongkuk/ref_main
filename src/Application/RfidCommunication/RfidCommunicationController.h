/*!
 * @file
 * @brief Rfid Communication Controller HSM. This module controls when the RFID Board will be
 *        written to and read from. This module is responsible for populating some of the
 *        RFID mainboard ERDs
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RFIDCOMMUNICATIONCONTROLLER_H
#define RFIDCOMMUNICATIONCONTROLLER_H

#include "I_DataModel.h"
#include "Hsm.h"
#include "RfidFilterUpdateRateData.h"
#include "Timer.h"
#include "Input_OredErds.h"

typedef struct
{
   Erd_t rfidCommunicationControllerStateErd; // RfidCommunicationControllerState_t
   Erd_t rfidFilterReadWriteResultErd; // RfidFilterReadWriteResult_t
   Erd_t allFreshFoodDoorsAreClosedErd; // bool
   Erd_t rfidFilterUidRfidBoardErd; // RfidUid_t
   Erd_t rfidFilterUidErd; // RfidUid_t
   Erd_t rfidFilterStatusRfidBoardErd; // FilterStatusRfidBoard_t
   Erd_t rfidFilterIdentifierErd; // RfidFilterIdentifier_t
   Erd_t demoModeEnableErd; // bool
   Erd_t waterFilterTypeErd; // WaterFilterType_t
   Erd_t rfidFaultRequestErd; // RfidFaultRequest_t
   Erd_t rfidBoardTagAuthenticationFailedFault; // bool
   Erd_t rfidBoardHardwareFailureFaultErd; // bool
   Erd_t rfidBoardLeakDetectedFaultErd; // bool
   Erd_t rfidBoardBlockedTagFaultErd; // bool
   Erd_t rfidFilterDataRequestErd; // ReadWriteRequest_t
   Erd_t newFilterInstalledSignalErd; // Signal_t
} RfidCommunicationControllerConfig_t;

typedef struct
{
   struct
   {
      Hsm_t hsm;
      I_DataModel_t *dataModel;
      const RfidCommunicationControllerConfig_t *config;
      const RfidFilterUpdateRateData_t *rfidFilterUpdateRateData;
      EventSubscription_t onDataModelChangeSubscription;
      Timer_t readRequestTimer;
      Timer_t readWriteRequestTimer;
      Timer_t timeoutTimer;
   } _private;
} RfidCommunicationController_t;

/*!
 * @brief
 * @param instance
 * @param dataModel
 * @param config
 * @param rfidFilterUpdateRateData
 */
void RfidCommunicationController_Init(
   RfidCommunicationController_t *instance,
   I_DataModel_t *dataModel,
   const RfidCommunicationControllerConfig_t *config,
   const RfidFilterUpdateRateData_t *rfidFilterUpdateRateData);

#endif
