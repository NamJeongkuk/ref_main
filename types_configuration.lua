return {
  -- Types that will show up in the preprocessed header but don't need to be
  -- defined correctly (ie: they won't actually be in any ERDs)
  ignored_types = {
    'Timer_t',
    'max_align_t',
    'KeyErdStreamData_t',
    'StreamedErd_t',
    '_mbstate_t',
    'FanCareAboutCoolingModeSpeedData_t',
    'FanData_t',
    'CompressorSpeeds_t',
    'VariableSweatHeaterData_t'
  },

  -- Types that can't be automatically generated but need to be defined
  type_overrides = {
    GridBlockNumber_t = 'uint8_t',
    FreshFoodCalculatedAxisGridLines_t = [[
      struct {
        TemperatureDegFx100_t 'nflGridLinesDegFx100',
        TemperatureDegFx100_t 'lowHystGridLinesDegFx100',
        TemperatureDegFx100_t 'lowHystDeltaGridLinesDegFx100',
        TemperatureDegFx100_t 'highHystGridLinesDegFx100',
        TemperatureDegFx100_t 'extraHighGridLinesDegFx100',
        TemperatureDegFx100_t 'superHighGridLinesDegFx100',
      }
    ]],
    FreezerCalculatedAxisGridLines_t = [[
      struct {
        TemperatureDegFx100_t 'lowHystGridLinesDegFx100',
        TemperatureDegFx100_t 'deltaGridLinesDegFx100',
        TemperatureDegFx100_t 'highHystGridLinesDegFx100',
        TemperatureDegFx100_t 'extraHighGridLinesDegFx100',
        TemperatureDegFx100_t 'superHighGridLinesDegFx100',
        TemperatureDegFx100_t 'extremeHighGridLinesDegFx100',
      }
    ]],
    FanControl_t = [[
      struct {
        FanControlType_t 'type',
        FanRpm_t 'rpmOrDutyCycle',
      }
    ]],
    PwmDutyCycle_t = 'uint16_t',
    PercentageDutyCycle_t = 'uint8_t',
    PwmFrequency_t = 'uint16_t',
    RakePosition_t = 'uint8_t',
    GitHash_t = 'raw[20]',
    ShortGitHash_t = 'raw[4]',
    IceMakerEnableRequest_t = 'bool',
    TurboModeRequest_t = 'bool',
    CoolingSystemRequest_t = 'bool',
    DoorAlarmEnableRequest_t = 'bool',
    RfidFilterIdentifier_t = 'raw[16]',
    PresenceSensingEnableRequest_t = 'bool',
    PresenceActivatesRecessLightEnableRequest_t = 'bool',
    IceMakerEnableRequest_t = 'bool',
    RfidUid_t = 'raw[7]',
    UnitSerialNumber_t = 'raw[8]'
  }
}
