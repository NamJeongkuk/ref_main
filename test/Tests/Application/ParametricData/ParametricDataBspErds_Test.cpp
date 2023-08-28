/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SystemErds.h"
#include "DataSource_Gpio.h"
#include "DataSource_Pwm.h"
#include "DataSource_InputCapture.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "LuaParametricTestUtils.h"

TEST_GROUP(ParametricDataBspErds_Input){};

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreezerEvapFanPwm)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreezerEvapFan_Pwm)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreezerEvapFan_Pwm);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForCondenserFanPwm)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_CondenserFan_Pwm)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_CondenserFan_Pwm);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreshFoodEvapFanPwm)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreshFoodEvapFan_Pwm)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreshFoodEvapFan_Pwm);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForIceCabinetFanPwm)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_IceCabinetFan_Pwm)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_IceCabinetFan_Pwm);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForDeliFanPwm)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_DeliFan_Pwm)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_DeliFan_Pwm);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForBackwallLightPwm)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreshFoodBackWallLight_RampingPwm)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreshFoodBackWallLight_RampingPwm);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreshFoodLightPwm)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreshFoodTopLight_RampingPwm)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreshFoodTopLight_RampingPwm);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFeezerLightPwm)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreezerTopLight_RampingPwm)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreezerTopLight_RampingPwm);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForIceMakerMoldThermistorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_AluminumMoldIceMakerMoldThermistor_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_AluminumMoldIceMakerMoldThermistor_AdcCount);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForIceBoxThermistorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_IceMakerBoxThermistor_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_IceMakerBoxThermistor_AdcCount);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForCompressorRelay)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_CompressorRelay)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_CompressorRelay);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFillTubeHeater)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FillTubeHeater)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FillTubeHeater);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreezerDefrostHeaterRelay)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreezerDefrostHeaterRelay)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreezerDefrostHeaterRelay);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreshFoodDamperHeater)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreshFoodDamperHeater)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreshFoodDamperHeater);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForAluminumMoldIceMakerWaterValveRelay)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_AluminumMoldIceMakerWaterValveRelay)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_AluminumMoldIceMakerWaterValveRelay);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForTwistTrayIceMakerWaterValveRelay)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_TwistTrayIceMakerWaterValveRelay)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_TwistTrayIceMakerWaterValveRelay);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForAluminumMoldIceMakerRelay)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_AluminumMoldIceMakerHeaterRelay)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_AluminumMoldIceMakerHeaterRelay);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForIceMakerMotorRelay)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_IceMakerRakeMotorRelay)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_IceMakerRakeMotorRelay);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForDispenserValveRelay)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_DispenserValveRelay)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_DispenserValveRelay);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForIsolationValveRelay)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_IsolationValveRelay)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_IsolationValveRelay);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForRightSideFreshFoodDoorIsOpen)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_RightSideFreshFoodDoorStatus)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_RightSideFreshFoodDoorStatus);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreezerDoorIsOpen)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_LeftSideFreezerDoorStatus)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_LeftSideFreezerDoorStatus);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForIceMakerFeelerArmPosition)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_AluminumMoldIceMakerFeelerArmPosition)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_AluminumMoldIceMakerFeelerArmPosition);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForIceMakerRakePosition)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_AluminumMoldIceMakerRakePosition)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_AluminumMoldIceMakerRakePosition);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreezerEvapFanInputCaptureTime)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreezerEvapFan_InputCaptureTime)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreezerEvapFan_InputCaptureTime);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForIceCabinetFanInputCaptureTime)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_IceCabinetFan_InputCaptureTime)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_IceCabinetFan_InputCaptureTime);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForAluminumMoldIceMakerRakeInputCaptureTime)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_AluminumIceMakerRake_InputCaptureTime)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_AluminumIceMakerRake_InputCaptureTime);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreshFoodThermistorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreshFoodThermistor_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreshFoodThermistor_AdcCount);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreezerThermistorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreezerThermistor_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreezerThermistor_AdcCount);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreshFoodEvapThermistorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreshFoodEvapThermistor_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreshFoodEvapThermistor_AdcCount);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForFreezerEvapThermistorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_FreezerEvapThermistor_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_FreezerEvapThermistor_AdcCount);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForConvertibleCompartmentCabinetThermistorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_ConvertibleCompartmentCabinetThermistor_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ConvertibleCompartmentCabinetThermistor_AdcCount);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForAmbientHumiditySensorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_AmbientHumidity_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_AmbientHumidity_AdcCount);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForConvertibleCompartmentEvapThermistorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_ConvertibleCompartmentEvapThermistor_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ConvertibleCompartmentEvapThermistor_AdcCount);
}

TEST_GROUP(ParametricDataBspErds_Output){};

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm25k00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Pwm_PWM_25K_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Pwm_PWM_25K_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm25k_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Pwm_PWM_25K_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Pwm_PWM_25K_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm25k_02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Pwm_PWM_25K_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Pwm_PWM_25K_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm25k_03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Pwm_PWM_25K_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Pwm_PWM_25K_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm25k_04)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Pwm_PWM_25K_04)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Pwm_PWM_25K_04);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspRampingLedPwm200_00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_RampingLedPwm_PWM_200_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_RampingLedPwm_PWM_200_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspRampingLedPwm200_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_RampingLedPwm_PWM_200_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_RampingLedPwm_PWM_200_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspRampingLedPwm200_02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_RampingLedPwm_PWM_200_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_RampingLedPwm_PWM_200_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspRampingLedPwm200_03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_RampingLedPwm_PWM_200_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_RampingLedPwm_PWM_200_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspInputCapture_00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_InputCapture_CAPTURE_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_InputCapture_CAPTURE_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspInputCapture_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_InputCapture_CAPTURE_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_InputCapture_CAPTURE_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspInputCapture_02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_InputCapture_CAPTURE_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_InputCapture_CAPTURE_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspInputCapture_03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_InputCapture_CAPTURE_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_InputCapture_CAPTURE_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspInputCapture_04)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_InputCapture_CAPTURE_04)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_InputCapture_CAPTURE_04);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspInputCapture_05)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_InputCapture_CAPTURE_05)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_InputCapture_CAPTURE_05);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForGpioOut_00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_GPIO_OUT_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_GPIO_OUT_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForGpioOut_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_GPIO_OUT_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_GPIO_OUT_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForGpioOut_02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_GPIO_OUT_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_GPIO_OUT_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForHeater_00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_HTR_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_HTR_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForHeater_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_HTR_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_HTR_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForHeater_02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_HTR_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_HTR_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForHeater_03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_HTR_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_HTR_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForHeater_04)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_HTR_04)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_HTR_04);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForMotorDriveEnable_00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_MTR_DRV_EN_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_MTR_DRV_EN_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForMotorDriveEnable_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_MTR_DRV_EN_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_MTR_DRV_EN_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForRefVal_00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_REF_VAL_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_REF_VAL_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForRefVal_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_REF_VAL_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_REF_VAL_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForRefVal_02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_REF_VAL_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_REF_VAL_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForRefVal_03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_REF_VAL_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_REF_VAL_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableRelay_00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_RLY_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_RLY_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableRelay_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_RLY_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_RLY_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableRelay_02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_RLY_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_RLY_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableRelay_03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_RLY_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_RLY_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableRelay_04)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_RLY_04)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_RLY_04);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableRelay_05)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_RLY_05)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_RLY_05);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableRelay_06)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_RLY_06)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_RLY_06);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableRelay_07)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_RLY_07)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_RLY_07);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableRelay_08)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_RLY_08)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_RLY_08);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableDoorInt)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_DOOR_INT)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_DOOR_INT);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableDoor_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_DOOR_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_DOOR_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableDoor_02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_DOOR_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_DOOR_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableDoor_03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_DOOR_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_DOOR_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableDoor_04)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_DOOR_04)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_DOOR_04);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableGpioIn_00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_GPIO_IN_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_GPIO_IN_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableGpioIn_01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_GPIO_IN_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_GPIO_IN_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableGpioIn_02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_GPIO_IN_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_GPIO_IN_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableGpioIn_03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_GPIO_IN_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_GPIO_IN_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableGpioCapture_03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_CAPTURE_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_CAPTURE_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTablePwVar_00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Gpio_PWM_VAR_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Gpio_PWM_VAR_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcAnalog475kTh00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_ANALOG_475K_TH_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_ANALOG_475K_TH_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcAnalogThLow01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_ANALOG_TH_LOW_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_ANALOG_TH_LOW_01);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcAnalogThLow02)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_ANALOG_TH_LOW_02)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_ANALOG_TH_LOW_02);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcAnalogThLow03)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_ANALOG_TH_LOW_03)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_ANALOG_TH_LOW_03);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcAnalogThLow04)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_ANALOG_TH_LOW_04)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_ANALOG_TH_LOW_04);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcAnalogThLow05)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_ANALOG_TH_LOW_05)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_ANALOG_TH_LOW_05);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcAnalogThLow06)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_ANALOG_TH_LOW_06)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_ANALOG_TH_LOW_06);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcAnalogThLow07)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_ANALOG_TH_LOW_07)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_ANALOG_TH_LOW_07);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcAnalogThLow08)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_ANALOG_TH_LOW_08)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_ANALOG_TH_LOW_08);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcHwPersonality00)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_HW_PERSONALITY_00)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_HW_PERSONALITY_00);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForAdcHwPersonality01)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_Adc_HW_PERSONALITY_01)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_Adc_HW_PERSONALITY_01);
}
