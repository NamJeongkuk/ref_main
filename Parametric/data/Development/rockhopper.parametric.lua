return parametric_data({
  major_data_structure_version = 0,
  minor_data_structure_version = 14,
  major_version = 0,
  minor_version = 0,
  personalities = {
    import('./Personality/personalitySwqa.lua'),
    import('./Personality/personalityTestDrivenDevelopmentSingleEvap.lua'),
    import('./Personality/personalityTestDrivenDevelopmentDualEvap.lua'),
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
    import('./Personality/personalityTestDrivenDevelopmentSingleEvapCondenserFanNoCoolingMode.lua')
  }
})
