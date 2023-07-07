
local enum = require 'lua-common'.utilities.enum

return enum({
  Erd_RightSideFreshFoodDoorStatus = 0xF0A8,
  Erd_LeftSideFreezerDoorStatus = 0xF0A9,
  Erd_TwistTrayIceMakerThermistor_AdcCount = 0xF0B0,
  Erd_AluminumMoldIceMakerMoldThermistor_AdcCount = 0xF0B1,
  Erd_IceMakerBoxThermistor_AdcCount = 0xF0B2,
  Erd_FreshFoodThermistor_AdcCount = 0xF0B3,
  Erd_FreezerThermistor_AdcCount = 0xF0B4,
  Erd_FreshFoodEvapThermistor_AdcCount = 0xF0B5,
  Erd_FreezerEvapThermistor_AdcCount = 0xF0B6,
  Erd_ConvertibleCompartmentCabinetThermistor_AdcCount = 0xF0B7,
  Erd_AmbientThermistor_AdcCount = 0xF0B8,
  Erd_ConvertibleCompartmentEvapThermistor_AdcCount = 0xF0B9,

  Erd_CompressorRelay = 0xF503,
  Erd_FillTubeHeater = 0xF504,
  Erd_FreezerDefrostHeaterRelay = 0xF505,
  Erd_FreshFoodDamperHeater = 0xF506,
  Erd_AluminumMoldIceMakerWaterValveRelay = 0xF507,
  Erd_AluminumMoldIceMakerHeaterRelay = 0xF508,
  Erd_IceMakerRakeMotorRelay = 0xF509,
  Erd_DispenserValveRelay = 0xF50C,
  Erd_IsolationValveRelay = 0xF50D,
  Erd_TwistTrayIceMakerWaterValveRelay = 0xF50E,
  Erd_AluminumMoldIceMakerFeelerArmPosition = 0xF151,
  Erd_AluminumMoldIceMakerRakePosition = 0xF156,

  Erd_FreshFoodBackWallLight_Pwm = 0xF55B,
  Erd_FreshFoodTopLight_Pwm = 0xF55C,
  Erd_FreezerTopLight_Pwm = 0xF55D,
  Erd_FreezerBackWallLight_Pwm = 0xF55F,
  Erd_CondenserFan_Pwm = 0xF51A,
  Erd_IceCabinetFan_Pwm = 0xF51B,
  Erd_ConvertibleCompartmentFan_Pwm = 0xF51C,
  Erd_DeliFan_Pwm = 0xF51D,
  Erd_FreezerEvapFan_Pwm = 0xF51E,
  Erd_FreshFoodEvapFan_Pwm = 0xF51F,

  Erd_CondenserFan_InputCaptureTime = 0xF53A,
  Erd_IceCabinetFan_InputCaptureTime = 0xF53B,
  Erd_ConvertibleCompartmentFan_InputCaptureTime = 0xF53C,
  Erd_DeliFan_InputCaptureTime = 0xF53D,
  Erd_FreezerEvapFan_InputCaptureTime = 0xF53E,
  Erd_FreshFoodEvapFan_InputCaptureTime = 0xF53F,
  Erd_AluminumIceMakerRake_InputCaptureTime = 0xF540,
  Erd_FlowMeter_InputCaptureCount = 0xF541,

  Erd_FreshFoodDamperStepperMotorDriveEnable = 0xF54F,
  Erd_TwistIceMakerMotorDriveEnable = 0xF550,
})
