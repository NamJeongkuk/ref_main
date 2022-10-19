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

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForHeartbeatLed)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_HeartbeatLed)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_HeartbeatLed);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForRelayWatchdog)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_RelayWatchdog)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_RelayWatchdog);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForOtherLed)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_OtherLed)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_OtherLed);
}

TEST(ParametricDataBspErds_Input, ShouldHaveMatchingErdNumbersToSystemErdTableForPushButtonSwitch)
{
   GivenDataHasBeenGeneratedFor("core.u16(bsp_mapped_erd.Erd_PushButtonSwitch)");

   auto erd = reinterpret_cast<const Erd_t *>(ParametricData());
   CHECK_EQUAL(*erd, Erd_PushButtonSwitch);
}

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
