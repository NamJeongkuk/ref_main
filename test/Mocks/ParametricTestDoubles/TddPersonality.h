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
   TddPersonality_DevelopmentSingleEvaporator,
   TddPersonality_DevelopmentDualEvaporator,
   TddPersonality_DevelopmentTripleEvaporator,
   TddPersonality_DevelopmentSingleSpeedCompressor,
   TddPersonality_DevelopmentVariableSpeedCoolingModeIndependentCompressor,
   TddPersonality_DevelopmentVariableSpeedCoolingModeDependentCompressor,
   TddPersonality_DevelopmentMaxOpenDamperTimerZero,
   TddPersonality_DevelopmentDamperHeaterIsOnWhenDefrostHeaterIsOnIsFalse,
   TddPersonality_DevelopmentDamperHeaterZeroOnTimeMinutes,
   TddPersonality_DevelopmentSingleEvapFillTubeHeaterZeroOnTime,
   TddPersonality_DevelopmentSingleEvapFillTubeHeaterZeroDutyCycle,
   TddPersonality_DevelopmentSingleEvapFillTubeHeaterOnTimeLessThanMaxHarvestTime,
   TddPersonality_MaxDevelopment
};

#endif
