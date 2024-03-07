import("../../versions.lua")

return parametric_data({
  major_data_structure_version = major_data_structure_version,
  minor_data_structure_version = minor_data_structure_version,
  major_version = major_version,
  minor_version = minor_version,
  personalities = {
    import('./Personality/personalitySwqa.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvap.lua'),
    import('./Personality/personalityTestDrivenDevelopmentDualEvapThreeDoor.lua'),
    import('./Personality/personalityTestDrivenDevelopmentTripleEvap.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleSpeedCompressor.lua'),
    import('./Personality/personalityTestDrivenDevelopmentVariableSpeedCoolingModeIndependentCompressor.lua'),
    import('./Personality/personalityTestDrivenDevelopmentVariableSpeedCoolingModeDependentCompressor.lua'),
    import('./Personality/personalityTestDrivenDevelopmentMaxOpenDamperTimerZero.lua'),
    import('./Personality/personalityTestDrivenDevelopmentDamperHeaterOnWhenDefrostHeaterIsOnIsFalse.lua'),
    import('./Personality/personalityTestDrivenDevelopmentDamperHeaterZeroOnTimeMinutes.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvapFillTubeHeaterNonHarvestControlZeroPercentDutyCycle.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleSpeedCompressorGridIntegration.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvapEnableExternalSensorIsFalse.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvapCondenserFanCoolingModeWithoutSetpoint.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvapCondenserFanNoCoolingMode.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvapShorterTimeBetweenDefrostsThanNormalSingleEvap.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvapSingleSpeedCompressorGridIntegrationAndSabbathReadyToDefrostDuringEnhancedSabbathOffStage.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvapSingleSpeedCompressorGridIntegrationAndSabbathReadyToDefrostDuringEnhancedSabbathFreshFoodStage.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvapSingleSpeedCompressorGridIntegrationAndSabbathReadyToDefrostDuringEnhancedSabbathFreezerStage.lua'),
    import('./Personality/personalityTestDrivenDevelopmentDualEvapFourDoor.lua'),
    import('./Personality/personalityTestDrivenDevelopmentNoAdcsMapped.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleDoorSingleEvapAllFreezer.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleDoorSingleEvapAllFreshFood.lua')
  }
})
