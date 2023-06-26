local erd_list = {
  { input = bsp_mapped_erd.Erd_FreezerEvapFan_Pwm,                        output = bsp_erd.Erd_Pwm_PWM_25K_00 },
  { input = bsp_mapped_erd.Erd_CondenserFan_Pwm,                          output = bsp_erd.Erd_Pwm_PWM_25K_01 },
  { input = bsp_mapped_erd.Erd_FreshFoodBackWallLight_Pwm,                output = bsp_erd.Erd_Pwm_PWM_200_00 },
  { input = bsp_mapped_erd.Erd_FreshFoodTopLight_Pwm,                     output = bsp_erd.Erd_Pwm_PWM_200_01 },
  { input = bsp_mapped_erd.Erd_FreezerBackWallLight_Pwm,                  output = bsp_erd.Erd_Pwm_PWM_200_02 },
  { input = bsp_mapped_erd.Erd_FreezerTopLight_Pwm,                       output = bsp_erd.Erd_Pwm_PWM_200_03 },

  { input = bsp_mapped_erd.Erd_FreezerEvapFan_InputCaptureTime,           output = bsp_erd.Erd_InputCapture_CAPTURE_00 },
  { input = bsp_mapped_erd.Erd_CondenserFan_InputCaptureTime,             output = bsp_erd.Erd_InputCapture_CAPTURE_01 },
  { input = bsp_mapped_erd.Erd_FlowMeter_InputCaptureCount,               output = bsp_erd.Erd_InputCapture_CAPTURE_03 },

  { input = bsp_mapped_erd.Erd_FreshFoodThermistor_AdcCount,              output = bsp_erd.Erd_Adc_ANALOG_475K_TH_00 },
  { input = bsp_mapped_erd.Erd_FreezerThermistor_AdcCount,                output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_00 },
  { input = bsp_mapped_erd.Erd_FreezerEvapThermistor_AdcCount,            output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_01 },
  { input = bsp_mapped_erd.Erd_IceMakerBoxThermistor_AdcCount,            output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_03 },
  { input = bsp_mapped_erd.Erd_TwistTrayIceMakerThermistor_AdcCount,      output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_04 },
  { input = bsp_mapped_erd.Erd_AluminumMoldIceMakerMoldThermistor_AdcCount, output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_05 },

  { input = bsp_mapped_erd.Erd_FillTubeHeater,                            output = bsp_erd.Erd_Gpio_HTR_00 },
  { input = bsp_mapped_erd.Erd_FreshFoodDamperHeater,                     output = bsp_erd.Erd_Gpio_HTR_01 },
  { input = bsp_mapped_erd.Erd_FreshFoodDamperStepperMotorDriveEnable,    output = bsp_erd.Erd_Gpio_MTR_DRV_EN_00},
  { input = bsp_mapped_erd.Erd_TwistIceMakerMotorDriveEnable,             output = bsp_erd.Erd_Gpio_MTR_DRV_EN_01},
  { input = bsp_mapped_erd.Erd_FreezerDefrostHeaterRelay,                 output = bsp_erd.Erd_Gpio_RLY_00 },
  { input = bsp_mapped_erd.Erd_IsolationValveRelay,                       output = bsp_erd.Erd_Gpio_RLY_01 },
  { input = bsp_mapped_erd.Erd_DispenserValveRelay,                       output = bsp_erd.Erd_Gpio_RLY_02 },
  { input = bsp_mapped_erd.Erd_AluminumMoldIceMakerWaterValveRelay,       output = bsp_erd.Erd_Gpio_RLY_03 },
  { input = bsp_mapped_erd.Erd_AluminumMoldIceMakerHeaterRelay,           output = bsp_erd.Erd_Gpio_RLY_04 },
  { input = bsp_mapped_erd.Erd_IceMakerRakeMotorRelay,                    output = bsp_erd.Erd_Gpio_RLY_05 },
  { input = bsp_mapped_erd.Erd_TwistTrayIceMakerWaterValveRelay,          output = bsp_erd.Erd_Gpio_RLY_06 },
  { input = bsp_mapped_erd.Erd_LeftSideFreezerDoorStatus,                 output = bsp_erd.Erd_Gpio_DOOR_INT },
  { input = bsp_mapped_erd.Erd_RightSideFreshFoodDoorStatus,              output = bsp_erd.Erd_Gpio_DOOR_01 },
  { input = bsp_mapped_erd.Erd_AluminumMoldIceMakerFeelerArmPosition,     output = bsp_erd.Erd_Gpio_GPIO_IN_03 },
  { input = bsp_mapped_erd.Erd_CompressorRelay,                           output = bsp_erd.Erd_Gpio_PWM_VAR_01 },
  { input = bsp_mapped_erd.Erd_AluminumMoldIceMakerRakePosition,          output = bsp_erd.Erd_Gpio_CAPTURE_03 }

}

return bsp_configuration({
  binary_search_configuration = binary_search_configuration({
    erd_list = erd_list
  }),
  linear_search_configuration = linear_search_configuration({
    erd_list = erd_list
  }),
})