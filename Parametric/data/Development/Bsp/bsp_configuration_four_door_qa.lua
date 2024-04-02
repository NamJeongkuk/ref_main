local erd_list = {
   { input = bsp_mapped_erd.Erd_FreezerEvapFan_Pwm,                        output = bsp_erd.Erd_Pwm_PWM_25K_00 },
   { input = bsp_mapped_erd.Erd_CondenserFan_Pwm,                          output = bsp_erd.Erd_Pwm_PWM_25K_01 },
   { input = bsp_mapped_erd.Erd_IceCabinetFan_Pwm,                         output = bsp_erd.Erd_Pwm_PWM_25K_02 },
   { input = bsp_mapped_erd.Erd_FreshFoodEvapFan_Pwm,                      output = bsp_erd.Erd_Pwm_PWM_25K_03 }, 
   { input = bsp_mapped_erd.Erd_FreshFoodTopLight_RampingPwm,              output = bsp_erd.Erd_RampingLedPwm_PWM_200_00 },
   { input = bsp_mapped_erd.Erd_FreshFoodBackWallLight_RampingPwm,         output = bsp_erd.Erd_RampingLedPwm_PWM_200_01 },
   { input = bsp_mapped_erd.Erd_FreezerTopLight_RampingPwm,                output = bsp_erd.Erd_RampingLedPwm_PWM_200_03 },
 
   { input = bsp_mapped_erd.Erd_FreezerEvapFan_InputCaptureTime,           output = bsp_erd.Erd_InputCapture_CAPTURE_00 },
   { input = bsp_mapped_erd.Erd_CondenserFan_InputCaptureTime,             output = bsp_erd.Erd_InputCapture_CAPTURE_01 },
   { input = bsp_mapped_erd.Erd_IceCabinetFan_InputCaptureTime,            output = bsp_erd.Erd_InputCapture_CAPTURE_02 },
   { input = bsp_mapped_erd.Erd_FreshFoodEvapFan_InputCaptureTime,         output = bsp_erd.Erd_InputCapture_CAPTURE_04 },
   { input = bsp_mapped_erd.Erd_FreshFoodThermistor_AdcCount,              output = bsp_erd.Erd_Adc_ANALOG_475K_TH_00 },
   { input = bsp_mapped_erd.Erd_FreezerThermistor_AdcCount,                output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_00 },
   { input = bsp_mapped_erd.Erd_FreezerEvapThermistor_AdcCount,            output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_01 },
   { input = bsp_mapped_erd.Erd_IceMaker0_MoldThermistor_AdcCount,         output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_04 },
   { input = bsp_mapped_erd.Erd_FreshFoodEvapThermistor_AdcCount,          output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_07 },
   { input = bsp_mapped_erd.Erd_ConvertibleCompartmentCabinetThermistor_AdcCount,  output = bsp_erd.Erd_Adc_ANALOG_TH_LOW_08 },
 
   { input = bsp_mapped_erd.Erd_IceMaker0_FillTubeHeater,                  output = bsp_erd.Erd_SoftPwmGpio_HTR_00 },
   -- { input = bsp_mapped_erd.Erd_IcePortHeater,                          output = bsp_erd.Erd_SoftPwmGpio_HTR_01 }, uncomment when available
   -- { input = bsp_mapped_erd.Erd_DoorInDoorHeater,                       output = bsp_erd.Erd_SoftPwmGpio_HTR_02 }, uncomment when available
   { input = bsp_mapped_erd.Erd_DamperHeater,                              output = bsp_erd.Erd_SoftPwmGpio_HTR_03 },
   -- { input = bsp_mapped_erd.Erd_CameraHeater,                           output = bsp_erd.Erd_SoftPwmGpio_HTR_05 }, uncomment when available
   { input = bsp_mapped_erd.Erd_DamperStepperMotorDriveEnable,             output = bsp_erd.Erd_Gpio_MTR_DRV_EN_00},
   -- { input = bsp_mapped_erd.Erd_ConvertibleCompartmentHeater,           output = bsp_erd.Erd_SoftPwmGpio_HTR_04}, Uncomment when available
   { input = bsp_mapped_erd.Erd_FreezerDefrostHeaterRelay,                 output = bsp_erd.Erd_Gpio_RLY_00 },
   { input = bsp_mapped_erd.Erd_IceMaker0_HeaterRelay,                     output = bsp_erd.Erd_Gpio_RLY_01 },
 
   { input = bsp_mapped_erd.Erd_IceMaker0_RakeMotorRelay,                  output = bsp_erd.Erd_Gpio_RLY_03 },
   { input = bsp_mapped_erd.Erd_IsolationValveRelay,                       output = bsp_erd.Erd_Gpio_RLY_04 },
   { input = bsp_mapped_erd.Erd_IceMaker0_WaterValveRelay,                 output = bsp_erd.Erd_Gpio_RLY_05 },
   { input = bsp_mapped_erd.Erd_FreshFoodDefrostHeaterRelay,               output = bsp_erd.Erd_Gpio_RLY_07 },
   { input = bsp_mapped_erd.Erd_CompressorRelay,                           output = bsp_erd.Erd_Gpio_PWM_VAR_01 },
 
   { input = bsp_mapped_erd.Erd_LeftSideFreshFoodDoorStatus,               output = bsp_erd.Erd_Gpio_DOOR_INT },
   { input = bsp_mapped_erd.Erd_RightSideFreshFoodDoorStatus,              output = bsp_erd.Erd_Gpio_DOOR_01 },
   { input = bsp_mapped_erd.Erd_BottomFreezerDrawerStatus,                 output = bsp_erd.Erd_Gpio_DOOR_02 },
   { input = bsp_mapped_erd.Erd_ConvertibleCompartmentDoorStatus,          output = bsp_erd.Erd_Gpio_DOOR_03 },
   -- { input = bsp_mapped_erd.Erd_DoorInDoorDoorStatus,                   output = bsp_erd.Erd_Gpio_DOOR_04 }, uncomment when available
   { input = bsp_mapped_erd.Erd_IceMaker0_FeelerArmPosition,               output = bsp_erd.Erd_Gpio_GPIO_IN_03 },
   { input = bsp_mapped_erd.Erd_IceMaker0_RakePosition,                    output = bsp_erd.Erd_Gpio_CAPTURE_03 }
 }
 
 return bsp_configuration({
   binary_search_configuration = binary_search_configuration({
     erd_list = erd_list
   }),
   linear_search_configuration = linear_search_configuration({
     erd_list = erd_list
   }),
 })