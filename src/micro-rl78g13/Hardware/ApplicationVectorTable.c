/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

static void Ignore(void) __attribute__((interrupt));
static void Ignore(void)
{
}

void INT_WDTI(void) __attribute__((weak, alias("Ignore")));
void INT_LVI(void) __attribute__((weak, alias("Ignore")));
void INT_P0(void) __attribute__((weak, alias("Ignore")));
void INT_P1(void) __attribute__((weak, alias("Ignore")));
void INT_P2(void) __attribute__((weak, alias("Ignore")));
void INT_P3(void) __attribute__((weak, alias("Ignore")));
void INT_P4(void) __attribute__((weak, alias("Ignore")));
void INT_P5(void) __attribute__((weak, alias("Ignore")));
void INT_ST2(void) __attribute__((weak, alias("Ignore")));
void INT_SR2(void) __attribute__((weak, alias("Ignore")));
void INT_SRE2(void) __attribute__((weak, alias("Ignore")));
void INT_DMA0(void) __attribute__((weak, alias("Ignore")));
void INT_DMA1(void) __attribute__((weak, alias("Ignore")));
void INT_ST0(void) __attribute__((weak, alias("Ignore")));
void INT_SR0(void) __attribute__((weak, alias("Ignore")));
void INT_TM01H(void) __attribute__((weak, alias("Ignore")));
void INT_ST1(void) __attribute__((weak, alias("Ignore")));
void INT_SR1(void) __attribute__((weak, alias("Ignore")));
void INT_TM03H(void) __attribute__((weak, alias("Ignore")));
void INT_IICA0(void) __attribute__((weak, alias("Ignore")));
void INT_TM00(void) __attribute__((weak, alias("Ignore")));
void INT_TM01(void) __attribute__((weak, alias("Ignore")));
void INT_TM02(void) __attribute__((weak, alias("Ignore")));
void INT_TM03(void) __attribute__((weak, alias("Ignore")));
void INT_AD(void) __attribute__((weak, alias("Ignore")));
void INT_RTC(void) __attribute__((weak, alias("Ignore")));
void INT_IT(void) __attribute__((weak, alias("Ignore")));
void INT_TM04(void) __attribute__((weak, alias("Ignore")));
void INT_TM05(void) __attribute__((weak, alias("Ignore")));
void INT_TM06(void) __attribute__((weak, alias("Ignore")));
void INT_TM07(void) __attribute__((weak, alias("Ignore")));
void INT_MD(void) __attribute__((weak, alias("Ignore")));
void INT_FL(void) __attribute__((weak, alias("Ignore")));
void INT_BRK_I(void) __attribute__((weak, alias("Ignore")));

const void *const appVectors[] = {
   //INT_WDTI (0x4)
   INT_WDTI,
   //INT_LVI (0x6)
   INT_LVI,
   //INT_P0 (0x8)
   INT_P0,
   //INT_P1 (0xA)
   INT_P1,
   //INT_P2 (0xC)
   INT_P2,
   //INT_P3 (0xE)
   INT_P3,
   //INT_P4 (0x10)
   INT_P4,
   //INT_P5 (0x12)
   INT_P5,
   //INT_CSI20/INT_IIC20/INT_ST2 (0x14)
   INT_ST2,
   //INT_SR2 (0x16)
   INT_SR2,
   //INT_SRE2 (0x18)
   INT_SRE2,
   //INT_DMA0 (0x1A)
   INT_DMA0,
   //INT_DMA1 (0x1C)
   INT_DMA1,
   //INT_CSI00/INT_IIC00/INT_ST0 (0x1E)
   INT_ST0,
   //INT_SR0 (0x20)
   INT_SR0,
   //INT_SRE0/INT_TM01H (0x22)
   INT_TM01H,
   //INT_ST1 (0x24)
   INT_ST1,
   //INT_CSI11/INT_IIC11/INT_SR1 (0x26)
   INT_SR1,
   //INT_SRE1/INT_TM03H (0x28)
   INT_TM03H,
   //INT_IICA0 (0x2A)
   INT_IICA0,
   //INT_TM00 (0x2C)
   INT_TM00,
   //INT_TM01 (0x2E)
   INT_TM01,
   //INT_TM02 (0x30)
   INT_TM02,
   //INT_TM03 (0x32)
   INT_TM03,
   //INT_AD (0x34)
   INT_AD,
   //INT_RTC (0x36)
   INT_RTC,
   //INT_IT (0x38)
   INT_IT,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   //INT_TM04 (0x42)
   INT_TM04,
   //INT_TM05 (0x44)
   INT_TM05,
   //INT_TM06 (0x46)
   INT_TM06,
   //INT_TM07 (0x48)
   INT_TM07,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   //INT_MD (0x5E)
   INT_MD,
   // Padding
   (void *)0xFFFF,
   //INT_FL (0x62)
   INT_FL,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   // Padding
   (void *)0xFFFF,
   //INT_BRK_I (0x7E)
   INT_BRK_I,
};
