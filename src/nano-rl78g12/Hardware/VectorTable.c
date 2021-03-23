/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern void PowerON_Reset(void);

const unsigned char Option_Bytes[] __attribute__((section(".option_bytes"))) = {
// B7         0  WDTINIT, WDT interupt disabled
// B6,B5     11  WINDOW1 WINDOW0, write window is 100% window
// B4         1  WDTON, Counter enabled after reset
// B3,B2,B1 101  WDCS2 WDCS1 WDCS0, WDT time set to 213/fIL (474.90 ms)
// B0         1  WDSTBYON, Counter operation enabled in HALT/STOP mode
#ifdef DEBUG
   // Disable WDT
   0x6B,
#else
   // Enable WDT
   0x7B,
#endif

   // B7,B6,B5 011  VPOC2 VPOC1 VPOC0 voltage trip 4.06 V 3.98 V
   // B4         1  Must be set to 1
   // B3,B2     00  LVIS1 LVIS0
   // B1,B0     11  LVIMDS1 LVIMDS0
   0xFF,

   // B7, B6    11  CMODE1 CMODE0 - HS (high speed main) mode
   // B6         1  Must be set to 1
   // B5         0  Must be set to 0
   // B3-B0   0000  FRQSEL3 FRQSEL2 FRQSEL1 FRQSEL0 set for 24mhz internal clock
   0xE0,

   // B7         1  OCDENSET  - Enables on-chip debugging.
   // B6-B3   0000  Must be set to 0
   // B2         1  Must be set to 1
   // B1         0  Must be set to 0
   // B0         1  OCDERSD - Erases data of flash memory in case of failures in authenticating
   0x85
};

const unsigned char Security_Id[] __attribute__((section(".security_id"))) = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define VEC __attribute__((section(".vec")))
const void *HardwareVectors[] VEC = {
   // Address 0x0
   PowerON_Reset,
   // Secure for Debugging
   (void *)0xFFFF
};

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
void INT_DMA0(void) __attribute__((weak, alias("Ignore")));
void INT_DMA1(void) __attribute__((weak, alias("Ignore")));
void INT_ST0(void) __attribute__((weak, alias("Ignore")));
void INT_SR0(void) __attribute__((weak, alias("Ignore")));
void INT_SRE0(void) __attribute__((weak, alias("Ignore")));
void INT_TM01H(void) __attribute__((weak, alias("Ignore")));
void INT_TM03H(void) __attribute__((weak, alias("Ignore")));
void INT_IICA0(void) __attribute__((weak, alias("Ignore")));
void INT_TM00(void) __attribute__((weak, alias("Ignore")));
void INT_TM01(void) __attribute__((weak, alias("Ignore")));
void INT_TM02(void) __attribute__((weak, alias("Ignore")));
void INT_TM03(void) __attribute__((weak, alias("Ignore")));
void INT_AD(void) __attribute__((weak, alias("Ignore")));
void INT_IT(void) __attribute__((weak, alias("Ignore")));
void INT_KR(void) __attribute__((weak, alias("Ignore")));
void INT_MD(void) __attribute__((weak, alias("Ignore")));
void INT_FL(void) __attribute__((weak, alias("Ignore")));
void INT_BRK_I(void) __attribute__((weak, alias("Ignore")));

#define VECT_SECT __attribute__((section(".vects")))
const void *Vectors[] VECT_SECT = {
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
   //INT_DMA0 (0x10)
   INT_DMA0,
   //INT_DMA1 (0x12)
   INT_DMA1,
   //INT_CSI00/INT_IIC00/INT_ST0 (0x14)
   INT_ST0,
   //INT_CSI01/INT_IIC01/INT_SR0 (0x16)
   INT_SR0,
   //INT_SRE0 (0x18)
   INT_SRE0,
   //INT_TM01H (0x1A)
   INT_TM01H,
   //INT_TM03H (0x1C)
   INT_TM03H,
   //INT_IICA0 (0x1E)
   INT_IICA0,
   //INT_TM00 (0x20)
   INT_TM00,
   //INT_TM01 (0x22)
   INT_TM01,
   //INT_TM02 (0x24)
   INT_TM02,
   //INT_TM03 (0x26)
   INT_TM03,
   //INT_AD (0x28)
   INT_AD,
   //INT_IT (0x2A)
   INT_IT,
   //INT_KR (0x2C)
   INT_KR,
   //INT_MD (0x2E)
   INT_MD,
   //INT_FL (0x30)
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
