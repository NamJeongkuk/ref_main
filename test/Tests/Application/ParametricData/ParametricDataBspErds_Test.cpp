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
#include "DataSource_OldHw_Pwm.h"
#include "DataSource_OldHw_InputCapture.h"
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

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForConvertibleCompartmentEvapThermistorAdcCount)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_ConvertibleCompartmentEvapThermistor_AdcCount)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ConvertibleCompartmentEvapThermistor_AdcCount);
}

TEST_GROUP(ParametricDataBspErds_Output){};

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm0)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspPwm_PWM_0)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspPwm_PWM_0);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm1)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspPwm_PWM_1)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspPwm_PWM_1);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm2)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspPwm_PWM_2)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspPwm_PWM_2);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm4)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspPwm_PWM_4)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspPwm_PWM_4);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspPwm5)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspPwm_PWM_5)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspPwm_PWM_5);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspFanInputCapture0)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspInputCapture_CAPT_0)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspInputCapture_CAPT_0);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspFanInputCapture1)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspInputCapture_CAPT_1)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspInputCapture_CAPT_1);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspFanInputCapture2)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspInputCapture_CAPT_2)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspInputCapture_CAPT_2);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspFanInputCapture4)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspInputCapture_CAPT_4)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspInputCapture_CAPT_4);
}

TEST(ParametricDataBspErds_Output, ShouldHaveMatchingErdNumbersToSystemErdTableForBspFanInputCapture5)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_erd.Erd_ApplicationToBspInputCapture_CAPT_5)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_ApplicationToBspInputCapture_CAPT_5);
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
