/*!
 * @file
 * @brief fan parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FANDATA_H
#define FANDATA_H

#include "I_PidController.h"
#include "FanControl.h"

typedef struct
{
   FanControl_t minSpeed;
   FanControl_t lowSpeed;
   FanControl_t mediumSpeed;
   FanControl_t highSpeed;
   FanControl_t maxSpeed;
} ValveIndependentFanData_t;

typedef struct
{
   bool careAboutSetpoint;
   FanControl_t minSpeed;
   FanControl_t minSpeedHighAmbientTemperature;

   union
   {
      struct
      {
         FanControl_t lowSpeedWithColdSetpointValveA;
         FanControl_t lowSpeedWithColdSetpointValveB;
         FanControl_t lowSpeedWithColdSetpointValveC;
         FanControl_t lowSpeedWithMediumSetpointValveA;
         FanControl_t lowSpeedWithMediumSetpointValveB;
         FanControl_t lowSpeedWithMediumSetpointValveC;
         FanControl_t lowSpeedWithWarmSetpointValveA;
         FanControl_t lowSpeedWithWarmSetpointValveB;
         FanControl_t lowSpeedWithWarmSetpointValveC;
      };

      struct
      {
         FanControl_t lowSpeedValveA;
         FanControl_t lowSpeedValveB;
         FanControl_t lowSpeedValveC;
         FanControl_t unused1;
         FanControl_t unused2;
         FanControl_t unused3;
         FanControl_t unused4;
         FanControl_t unused5;
         FanControl_t unused6;
      };
   };
   FanControl_t mediumSpeedValveA;
   FanControl_t mediumSpeedValveB;
   FanControl_t mediumSpeedValveC;
   FanControl_t highSpeedValveA;
   FanControl_t highSpeedValveB;
   FanControl_t highSpeedValveC;
   FanControl_t maxSpeed;
} ValveDependentFanData_t;

typedef struct
{
   uint8_t fanId;
   uint8_t numberOfPoles;
   bool valveDependent;
   uint16_t powerUsageInWatts;
   PidControllerGains_t gains;
   int32_t lowerLimit;
   int32_t upperLimit;
   uint16_t fanMissedTargetFaultTimeoutInSeconds;
   uint16_t missingFanFeedbackFaultTimeoutInSeconds;
   uint16_t feedbackPresentWhenFanIsOffTimeoutInSeconds;

   union
   {
      ValveIndependentFanData_t independentData;
      ValveDependentFanData_t dependentData;
   };
} ValveFanData_t;

typedef struct
{
   ValveFanData_t condenserFan;
   ValveFanData_t freshFoodFan;
   ValveFanData_t freezerFan;
   ValveFanData_t convertibleCompartmentFan;
   ValveFanData_t iceCabinetFan;
   ValveFanData_t deliPanFan;
} FanData_t;

#endif
