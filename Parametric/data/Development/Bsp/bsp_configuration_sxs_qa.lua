local erd_list = {
  { input = bsp_mapped_erd.Erd_FreezerEvapFan_Pwm,                        output = bsp_erd.Erd_Pwm_PWM_25K_00 },
  { input = bsp_mapped_erd.Erd_CondenserFan_Pwm,                          output = bsp_erd.Erd_Pwm_PWM_25K_01 },
  { input = bsp_mapped_erd.Erd_FreshFoodTopLight_RampingPwm,              output = bsp_erd.Erd_RampingLedPwm_PWM_200_00 },
  { input = bsp_mapped_erd.Erd_FreshFoodBackWallLight_RampingPwm,         output = bsp_erd.Erd_RampingLedPwm_PWM_200_01 },
  { input = bsp_mapped_erd.Erd_FreezerBackWallLight_RampingPwm,           output = bsp_erd.Erd_RampingLedPwm_PWM_200_02 },
  { input = bsp_mapped_erd.Erd_FreezerTopLight_RampingPwm,                output = bsp_erd.Erd_RampingLedPwm_PWM_200_03 },

  { input = bsp_mapped_erd.Erd_FreezerEvapFan_InputCaptureTime,           output = bsp_erd.Erd_InputCapture_CAPTURE_00 },
  { input = bsp_mapped_erd.Erd_CondenserFan_InputCaptureTime,             output = bsp_erd.Erd_InputCapture_CAPTURE_01 },
  { input = bsp_mapped_erd.Erd_FlowMeter_InputCaptureCount,               output = bsp_erd.Erd_InputCapture_CAPTURE_03 },

  { input = bsp_mapped_erd.Erd_FreshFoodThermistor_AdcCount,              output = bsp_erd.Erd_Adc_ANALOG_475K_TH_00 },
  { input = bsp_mapped_erd.Erd_FreezerThermistor_AdcCount,                output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_00 },
  { input = bsp_mapped_erd.Erd_FreezerEvapThermistor_AdcCount,            output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_01 },
  { input = bsp_mapped_erd.Erd_IceMaker0_MoldThermistor_AdcCount,         output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_04 },

  { input = bsp_mapped_erd.Erd_IceMaker0_FillTubeHeater,                  output = bsp_erd.Erd_SoftPwmGpio_HTR_00 },
  { input = bsp_mapped_erd.Erd_DamperHeater,                              output = bsp_erd.Erd_SoftPwmGpio_HTR_01 },
  { input = bsp_mapped_erd.Erd_DamperStepperMotorDriveEnable,             output = bsp_erd.Erd_Gpio_MTR_DRV_EN_00},
  { input = bsp_mapped_erd.Erd_TwistIceMakerMotorDriveEnable,             output = bsp_erd.Erd_Gpio_MTR_DRV_EN_01},
  { input = bsp_mapped_erd.Erd_FreezerDefrostHeaterRelay,                 output = bsp_erd.Erd_Gpio_RLY_00 },
  { input = bsp_mapped_erd.Erd_IsolationValveRelay,                       output = bsp_erd.Erd_Gpio_RLY_01 },
  { input = bsp_mapped_erd.Erd_DispenserValveRelay,                       output = bsp_erd.Erd_Gpio_RLY_02 },
  { input = bsp_mapped_erd.Erd_IceMaker0_WaterValveRelay,                 output = bsp_erd.Erd_Gpio_RLY_06 },
  { input = bsp_mapped_erd.Erd_LeftSideFreezerDoorStatus,                 output = bsp_erd.Erd_Gpio_DOOR_INT },
  { input = bsp_mapped_erd.Erd_RightSideFreshFoodDoorStatus,              output = bsp_erd.Erd_Gpio_DOOR_01 },
  { input = bsp_mapped_erd.Erd_CompressorRelay,                           output = bsp_erd.Erd_Gpio_PWM_VAR_01 },

  { input = bsp_mapped_erd.Erd_IceMaker0_TwistTrayMotorOutput,            output = bsp_erd.Erd_TwistTrayMotorOutput_MTR_DRV_00_01 }
}

return bsp_configuration({
  binary_search_configuration = binary_search_configuration({
    erd_list = erd_list
  }),
  linear_search_configuration = linear_search_configuration({
    erd_list = erd_list
  }),
})
