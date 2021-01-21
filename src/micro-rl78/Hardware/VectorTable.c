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
   // B3-B0   1000  FRQSEL3 FRQSEL2 FRQSEL1 FRQSEL0 set for 32mhz internal clock
   0xE8,

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

// Places the vector to jump to into ax
#define JumpToVectorAtOffset(_which) \
   asm("push   ax");                 \
   asm("push   de");                 \
   asm("movw   ax, !__vtor");        \
   asm("addw   ax, #" #_which);      \
   asm("br     !jump_to_vector");

asm("jump_to_vector:");

// ax <= vector address
asm("movw   de, ax");
asm("movw   ax, [de]");

// __isr = vector address
asm("mov    !__isr_msb, a");
asm("shlw   ax, 8");
asm("mov    !__isr_lsb, a");

// Cleanup from setup
asm("pop    de");
asm("pop    ax");

asm("br     !!__br");

void JUMP_INT_WDTI(void) __attribute__((naked));
void JUMP_INT_WDTI(void)
{
   JumpToVectorAtOffset(0);
}

void JUMP_INT_LVI(void) __attribute__((naked));
void JUMP_INT_LVI(void)
{
   JumpToVectorAtOffset(2);
}

void JUMP_INT_P0(void) __attribute__((naked));
void JUMP_INT_P0(void)
{
   JumpToVectorAtOffset(4);
}

void JUMP_INT_P1(void) __attribute__((naked));
void JUMP_INT_P1(void)
{
   JumpToVectorAtOffset(6);
}

void JUMP_INT_P2(void) __attribute__((naked));
void JUMP_INT_P2(void)
{
   JumpToVectorAtOffset(8);
}

void JUMP_INT_P3(void) __attribute__((naked));
void JUMP_INT_P3(void)
{
   JumpToVectorAtOffset(10);
}

void JUMP_INT_P4(void) __attribute__((naked));
void JUMP_INT_P4(void)
{
   JumpToVectorAtOffset(12);
}

void JUMP_INT_P5(void) __attribute__((naked));
void JUMP_INT_P5(void)
{
   JumpToVectorAtOffset(14);
}

void JUMP_INT_ST2(void) __attribute__((naked));
void JUMP_INT_ST2(void)
{
   JumpToVectorAtOffset(16);
}

void JUMP_INT_SR2(void) __attribute__((naked));
void JUMP_INT_SR2(void)
{
   JumpToVectorAtOffset(18);
}

void JUMP_INT_SRE2(void) __attribute__((naked));
void JUMP_INT_SRE2(void)
{
   JumpToVectorAtOffset(20);
}

void JUMP_INT_DMA0(void) __attribute__((naked));
void JUMP_INT_DMA0(void)
{
   JumpToVectorAtOffset(22);
}

void JUMP_INT_DMA1(void) __attribute__((naked));
void JUMP_INT_DMA1(void)
{
   JumpToVectorAtOffset(24);
}

void JUMP_INT_ST0(void) __attribute__((naked));
void JUMP_INT_ST0(void)
{
   JumpToVectorAtOffset(26);
}

void JUMP_INT_SR0(void) __attribute__((naked));
void JUMP_INT_SR0(void)
{
   JumpToVectorAtOffset(28);
}

void JUMP_INT_TM01H(void) __attribute__((naked));
void JUMP_INT_TM01H(void)
{
   JumpToVectorAtOffset(30);
}

void JUMP_INT_ST1(void) __attribute__((naked));
void JUMP_INT_ST1(void)
{
   JumpToVectorAtOffset(32);
}

void JUMP_INT_SR1(void) __attribute__((naked));
void JUMP_INT_SR1(void)
{
   JumpToVectorAtOffset(34);
}

void JUMP_INT_TM03H(void) __attribute__((naked));
void JUMP_INT_TM03H(void)
{
   JumpToVectorAtOffset(36);
}

void JUMP_INT_IICA0(void) __attribute__((naked));
void JUMP_INT_IICA0(void)
{
   JumpToVectorAtOffset(38);
}

void JUMP_INT_TM00(void) __attribute__((naked));
void JUMP_INT_TM00(void)
{
   JumpToVectorAtOffset(40);
}

void JUMP_INT_TM01(void) __attribute__((naked));
void JUMP_INT_TM01(void)
{
   JumpToVectorAtOffset(42);
}

void JUMP_INT_TM02(void) __attribute__((naked));
void JUMP_INT_TM02(void)
{
   JumpToVectorAtOffset(44);
}

void JUMP_INT_TM03(void) __attribute__((naked));
void JUMP_INT_TM03(void)
{
   JumpToVectorAtOffset(46);
}

void JUMP_INT_AD(void) __attribute__((naked));
void JUMP_INT_AD(void)
{
   JumpToVectorAtOffset(48);
}

void JUMP_INT_RTC(void) __attribute__((naked));
void JUMP_INT_RTC(void)
{
   JumpToVectorAtOffset(50);
}

void JUMP_INT_IT(void) __attribute__((naked));
void JUMP_INT_IT(void)
{
   JumpToVectorAtOffset(52);
}

void JUMP_INT_TM04(void) __attribute__((naked));
void JUMP_INT_TM04(void)
{
   JumpToVectorAtOffset(62);
}

void JUMP_INT_TM05(void) __attribute__((naked));
void JUMP_INT_TM05(void)
{
   JumpToVectorAtOffset(64);
}

void JUMP_INT_TM06(void) __attribute__((naked));
void JUMP_INT_TM06(void)
{
   JumpToVectorAtOffset(66);
}

void JUMP_INT_TM07(void) __attribute__((naked));
void JUMP_INT_TM07(void)
{
   JumpToVectorAtOffset(68);
}

void JUMP_INT_MD(void) __attribute__((naked));
void JUMP_INT_MD(void)
{
   JumpToVectorAtOffset(90);
}

void JUMP_INT_FL(void) __attribute__((naked));
void JUMP_INT_FL(void)
{
   JumpToVectorAtOffset(94);
}

void JUMP_INT_BRK_I(void) __attribute__((naked));
void JUMP_INT_BRK_I(void)
{
   JumpToVectorAtOffset(122);
}

#define VECT_SECT __attribute__((section(".vects")))
const void *Vectors[] VECT_SECT = {
   // INT_WDTI (0)
   JUMP_INT_WDTI,
   // INT_LVI (2)
   JUMP_INT_LVI,
   // INT_P0 (4)
   JUMP_INT_P0,
   // INT_P1 (6)
   JUMP_INT_P1,
   // INT_P2 (8)
   JUMP_INT_P2,
   // INT_P3 (10)
   JUMP_INT_P3,
   // INT_P4 (12)
   JUMP_INT_P4,
   // INT_P5 (14)
   JUMP_INT_P5,
   // INT_CSI20/INT_IIC20/INT_ST2 (16)
   JUMP_INT_ST2,
   // INT_SR2 (18)
   JUMP_INT_SR2,
   // INT_SRE2 (20)
   JUMP_INT_SRE2,
   // INT_DMA0 (22)
   JUMP_INT_DMA0,
   // INT_DMA1 (24)
   JUMP_INT_DMA1,
   // INT_CSI00/INT_IIC00/INT_ST0 (26)
   JUMP_INT_ST0,
   // INT_SR0 (28)
   JUMP_INT_SR0,
   // INT_SRE0/INT_TM01H (30)
   JUMP_INT_TM01H,
   // INT_ST1 (32)
   JUMP_INT_ST1,
   // INT_CSI11/INT_IIC11/INT_SR1 (34)
   JUMP_INT_SR1,
   // INT_SRE1/INT_TM03H (36)
   JUMP_INT_TM03H,
   // INT_IICA0 (38)
   JUMP_INT_IICA0,
   // INT_TM00 (40)
   JUMP_INT_TM00,
   // INT_TM01 (42)
   JUMP_INT_TM01,
   // INT_TM02 (44)
   JUMP_INT_TM02,
   // INT_TM03 (46)
   JUMP_INT_TM03,
   // INT_AD (48)
   JUMP_INT_AD,
   // INT_RTC (50)
   JUMP_INT_RTC,
   // INT_IT (52)
   JUMP_INT_IT,
   // Padding (54)
   (void *)0xFFFF,
   // Padding (56)
   (void *)0xFFFF,
   // Padding (58)
   (void *)0xFFFF,
   // Padding (60)
   (void *)0xFFFF,
   // INT_TM04 (62)
   JUMP_INT_TM04,
   // INT_TM05 (64)
   JUMP_INT_TM05,
   // INT_TM06 (66)
   JUMP_INT_TM06,
   // INT_TM07 (68)
   JUMP_INT_TM07,
   // Padding (70)
   (void *)0xFFFF,
   // Padding (72)
   (void *)0xFFFF,
   // Padding (74)
   (void *)0xFFFF,
   // Padding (76)
   (void *)0xFFFF,
   // Padding (78)
   (void *)0xFFFF,
   // Padding (80)
   (void *)0xFFFF,
   // Padding (82)
   (void *)0xFFFF,
   // Padding (84)
   (void *)0xFFFF,
   // Padding (86)
   (void *)0xFFFF,
   // Padding (88)
   (void *)0xFFFF,
   // INT_MD (90)
   JUMP_INT_MD,
   // Padding (92)
   (void *)0xFFFF,
   // INT_FL (94)
   JUMP_INT_FL,
   // Padding (96)
   (void *)0xFFFF,
   // Padding (98)
   (void *)0xFFFF,
   // Padding (100)
   (void *)0xFFFF,
   // Padding (102)
   (void *)0xFFFF,
   // Padding (104)
   (void *)0xFFFF,
   // Padding (106)
   (void *)0xFFFF,
   // Padding (108)
   (void *)0xFFFF,
   // Padding (110)
   (void *)0xFFFF,
   // Padding (112)
   (void *)0xFFFF,
   // Padding (114)
   (void *)0xFFFF,
   // Padding (116)
   (void *)0xFFFF,
   // Padding (118)
   (void *)0xFFFF,
   // Padding (120)
   (void *)0xFFFF,
   // INT_BRK_I (122)
   JUMP_INT_BRK_I,
};
