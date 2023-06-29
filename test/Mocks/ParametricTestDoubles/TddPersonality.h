/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TDDPERSONALITY_H
#define TDDPERSONALITY_H

enum
{
   TddPersonality_Swqa,
   TddPersonality_DevelopmentSingleEvaporator,
   TddPersonality_DevelopmentDualEvaporator,
   TddPersonality_DevelopmentTripleEvaporator,
   TddPersonality_DevelopmentSingleSpeedCompressor,
   TddPersonality_DevelopmentVariableSpeedCoolingModeIndependentCompressor,
   TddPersonality_DevelopmentVariableSpeedCoolingModeDependentCompressor,
   TddPersonality_DevelopmentMaxOpenDamperTimerZero,
   TddPersonality_DevelopmentDamperHeaterIsOnWhenDefrostHeaterIsOnIsFalse,
   TddPersonality_DevelopmentDamperHeaterZeroOnTimeMinutes,
   TddPersonality_DevelopmentSingleEvapFillTubeHeaterNonHarvestControlZeroPercentDutyCycle,
   TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration,
   TddPersonality_DevelopmentExternalAmbientSensorIsDisabled,
   TddPersonality_DevelopmentCondenserFanCoolingModeWithoutSetpoint,
   TddPersonality_DevelopmentCondenserFanNoCoolingMode,
   TddPersonality_MaxDevelopment
};

#endif
