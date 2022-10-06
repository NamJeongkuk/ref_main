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
   bool careAboutHighAmbientTemperature;
   FanControl_t superLowSpeed;
   FanControl_t lowSpeed;
   FanControl_t mediumSpeed;
   FanControl_t highSpeed;
   FanControl_t superHighSpeed;
   FanControl_t superLowSpeedHighAmbientTemperature;
} FanSpeedData_t;

typedef struct
{
   bool careAboutSetpoint;
   union
   {
      struct
      {
         FanControl_t superLowSpeed;
         FanControl_t lowSpeedFreshFood;
         FanControl_t lowSpeedFreezerWithColdSetpoint;
         FanControl_t lowSpeedFreezerWithMediumSetpoint;
         FanControl_t lowSpeedFreezerWithWarmSetpoint;
         FanControl_t lowSpeedConvertibleCompartmentWithColdSetpoint;
         FanControl_t lowSpeedConvertibleCompartmentWithMediumSetpoint;
         FanControl_t lowSpeedConvertibleCompartmentWithWarmSetpoint;
         FanControl_t mediumSpeedFreshFood;
         FanControl_t mediumSpeedFreezer;
         FanControl_t mediumSpeedConvertibleCompartment;
         FanControl_t highSpeedSpeedFreshFood;
         FanControl_t highSpeedSpeedFreezer;
         FanControl_t highSpeedSpeedConvertibleCompartment;
         FanControl_t superHighSpeed;
      } setpointSpeeds;

      struct
      {
         FanControl_t superLowSpeed;
         FanControl_t lowSpeedFreshFood;
         FanControl_t lowSpeedFreezer;
         FanControl_t mediumSpeedFreshFood;
         FanControl_t mediumSpeedFreezer;
         FanControl_t highSpeedSpeedFreshFood;
         FanControl_t highSpeedSpeedFreezer;
         FanControl_t superHighSpeed;
      } nonSetpointSpeeds;
   } careAboutSetpointData;
} FanCareAboutCoolingModeSpeedData_t;

typedef struct
{
   uint8_t fanId;
   uint8_t pulsesPerRevolution;
   bool careAboutCoolingMode;
   uint16_t powerUsageInWatts;
   PidControllerGains_t gains;
   int32_t lowerLimit;
   int32_t upperLimit;
   uint16_t fanMissedTargetFaultTimeoutInSeconds;
   uint16_t missingFanFeedbackFaultTimeoutInSeconds;
   uint16_t feedbackPresentWhenFanIsOffTimeoutInSeconds;

   union
   {
      FanSpeedData_t speedData;
      FanCareAboutCoolingModeSpeedData_t careAboutCoolingModeSpeedData;
   };
} FanData_t;

typedef struct
{
   FanData_t condenserFan;
   FanData_t convertibleCompartmentFan;
   FanData_t iceCabinetFan;
   FanData_t deliPanFan;
   FanData_t freezerEvapFan;
   FanData_t freshFoodEvapFan;
} CombinedFanData_t;

#endif
