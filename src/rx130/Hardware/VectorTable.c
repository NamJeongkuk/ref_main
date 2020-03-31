/*!
 * @file
 * @brief Vector table for RX130/RX210/RX231 Micro
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Action_Rx2xxSystemReset.h"
#include "IdCode.h"

typedef void (*fp)(void);

extern void PowerON_Reset(void);

static void Default_Handler(void);

void PrivilegedInstructionException(void) __attribute__((weak, alias("Default_Handler")));
void AccessException(void) __attribute__((weak, alias("Default_Handler")));
void UndefinedInstructionException(void) __attribute__((weak, alias("Default_Handler")));
void FloatingPointException(void) __attribute__((weak, alias("Default_Handler")));
void NonMaskableInterrupt(void) __attribute__((weak, alias("Default_Handler")));

const fp fixedVectorTable[] __attribute__((section(".fixedVectorTable"))) = {
//;0xffffff80  MDES  Endian Select Register
#ifdef __RX_LITTLE_ENDIAN__
   (fp)0xffffffff,
#endif
#ifdef __RX_BIG_ENDIAN__
   (fp)0xfffffff8,
#endif
   Default_Handler, // Reserved 0xffffff84
   (fp)0xffffffff, // OFS1 0xffffff88
   (fp)0xffffffff, // OFS0 0xffffff8C
   Default_Handler, // Reserved 0xffffff90
   Default_Handler, // Reserved 0xffffff94
   Default_Handler, // Reserved 0xffffff98
   Default_Handler, // Reserved 0xffffff9C
   (fp)ID_CODE_LONG_1, // Reserved ID Code 0xffffffA0
   (fp)ID_CODE_LONG_2, // Reserved ID Code 0xffffffA4
   (fp)ID_CODE_LONG_3, // Reserved ID Code 0xffffffA8
   (fp)ID_CODE_LONG_4, // Reserved ID Code 0xffffffAC
   Default_Handler, // Reserved 0xffffffB0
   Default_Handler, // Reserved 0xffffffB4
   Default_Handler, // Reserved 0xffffffB8
   Default_Handler, // Reserved 0xffffffBC
   Default_Handler, // Reserved 0xffffffC0
   Default_Handler, // Reserved 0xffffffC4
   Default_Handler, // Reserved 0xffffffC8
   Default_Handler, // Reserved 0xffffffCC
   PrivilegedInstructionException, // 0xffffffd0
   AccessException, // 0xffffffd4
   Default_Handler, // Reserved 0xffffffd8
   UndefinedInstructionException, // 0xffffffdc
   Default_Handler, // Reserved 0xffffffe0
   FloatingPointException, // 0xffffffe4
   Default_Handler, // Reserved 0xffffffe8
   Default_Handler, // Reserved 0xffffffec
   Default_Handler, // Reserved 0xfffffff0
   Default_Handler, // Reserved 0xfffffff4
   NonMaskableInterrupt, // 0xfffffff8
   PowerON_Reset // Reset 0xfffffffc
};

static void Excep_BRK(void) __attribute__((interrupt));
static void Excep_BRK(void)
{
   __builtin_rx_wait();
}

void BSC_BUSERR(void) __attribute__((weak, alias("Ignore")));
void FCU_FIFERR(void) __attribute__((weak, alias("Ignore")));
void FCU_FRDYI(void) __attribute__((weak, alias("Ignore")));
void ICU_SWINT(void) __attribute__((weak, alias("Ignore")));
void CMT0_CMI0(void) __attribute__((weak, alias("Ignore")));
void CMT1_CMI1(void) __attribute__((weak, alias("Ignore")));
void CMT2_CMI2(void) __attribute__((weak, alias("Ignore")));
void CMT3_CMI3(void) __attribute__((weak, alias("Ignore")));
void CAC_FERRF(void) __attribute__((weak, alias("Ignore")));
void CAC_MENDF(void) __attribute__((weak, alias("Ignore")));
void CAC_OVFF(void) __attribute__((weak, alias("Ignore")));
void USB0_D0FIFO0(void) __attribute__((weak, alias("Ignore")));
void USB1_D0FIFO0(void) __attribute__((weak, alias("Ignore")));
void USB0_USBI0(void) __attribute__((weak, alias("Ignore")));
void SDHI_SBFAI(void) __attribute__((weak, alias("Ignore")));
void SDHI_CDETI(void) __attribute__((weak, alias("Ignore")));
void SDHI_CACI(void) __attribute__((weak, alias("Ignore")));
void SDHI_SDACI(void) __attribute__((weak, alias("Ignore")));
void RSPI0_SPEI0(void) __attribute__((weak, alias("Ignore")));
void RSPI0_SPRI0(void) __attribute__((weak, alias("Ignore")));
void RSPI0_SPTI0(void) __attribute__((weak, alias("Ignore")));
void RSPI0_SPII0(void) __attribute__((weak, alias("Ignore")));
void CAN_COMFRXINT(void) __attribute__((weak, alias("Ignore")));
void CAN_RXFINT(void) __attribute__((weak, alias("Ignore")));
void CAN_TXINT(void) __attribute__((weak, alias("Ignore")));
void CAN_CHERRINT(void) __attribute__((weak, alias("Ignore")));
void CAN_GLERRINT(void) __attribute__((weak, alias("Ignore")));
void DOC_DOPCF(void) __attribute__((weak, alias("Ignore")));
void CMPB_CMPB0(void) __attribute__((weak, alias("Ignore")));
void CMPB_CMPB1(void) __attribute__((weak, alias("Ignore")));
void CTSU_CTSUWR(void) __attribute__((weak, alias("Ignore")));
void CTSU_CTSURD(void) __attribute__((weak, alias("Ignore")));
void CTSU_CTSUFN(void) __attribute__((weak, alias("Ignore")));
void RTC_CUP(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ0(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ1(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ2(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ3(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ4(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ5(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ6(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ7(void) __attribute__((weak, alias("Ignore")));
void ELC_ELSR8I(void) __attribute__((weak, alias("Ignore")));
void LVD_LVD1(void) __attribute__((weak, alias("Ignore")));
void LVD_LVD2(void) __attribute__((weak, alias("Ignore")));
void USB0_USBR0(void) __attribute__((weak, alias("Ignore")));
void VBATT_VBTLVDI(void) __attribute__((weak, alias("Ignore")));
void RTC_ALM(void) __attribute__((weak, alias("Ignore")));
void RTC_PRD(void) __attribute__((weak, alias("Ignore")));
void S12AD_S12ADI0(void) __attribute__((weak, alias("Ignore")));
void S12AD_GBADI(void) __attribute__((weak, alias("Ignore")));
void CMPB1_CMPB2(void) __attribute__((weak, alias("Ignore")));
void CMPB1_CMPB3(void) __attribute__((weak, alias("Ignore")));
void ELC_ELSR18I(void) __attribute__((weak, alias("Ignore")));
void ELC_ELSR19I(void) __attribute__((weak, alias("Ignore")));
void SSI0_SSIF0(void) __attribute__((weak, alias("Ignore")));
void SSI0_SSIRXI0(void) __attribute__((weak, alias("Ignore")));
void SSI0_SSITXI0(void) __attribute__((weak, alias("Ignore")));
void Security_RD(void) __attribute__((weak, alias("Ignore")));
void Security_WR(void) __attribute__((weak, alias("Ignore")));
void Security_Error(void) __attribute__((weak, alias("Ignore")));
void MTU0_TGIA0(void) __attribute__((weak, alias("Ignore")));
void MTU0_TGIB0(void) __attribute__((weak, alias("Ignore")));
void MTU0_TGIC0(void) __attribute__((weak, alias("Ignore")));
void MTU0_TGID0(void) __attribute__((weak, alias("Ignore")));
void MTU0_TCIV0(void) __attribute__((weak, alias("Ignore")));
void MTU0_TGIE0(void) __attribute__((weak, alias("Ignore")));
void MTU0_TGIF0(void) __attribute__((weak, alias("Ignore")));
void MTU1_TGIA1(void) __attribute__((weak, alias("Ignore")));
void MTU1_TGIB1(void) __attribute__((weak, alias("Ignore")));
void MTU1_TCIV1(void) __attribute__((weak, alias("Ignore")));
void MTU1_TCIU1(void) __attribute__((weak, alias("Ignore")));
void MTU2_TGIA2(void) __attribute__((weak, alias("Ignore")));
void MTU2_TGIB2(void) __attribute__((weak, alias("Ignore")));
void MTU2_TCIV2(void) __attribute__((weak, alias("Ignore")));
void MTU2_TCIU2(void) __attribute__((weak, alias("Ignore")));
void MTU3_TGIA3(void) __attribute__((weak, alias("Ignore")));
void MTU3_TGIB3(void) __attribute__((weak, alias("Ignore")));
void MTU3_TGIC3(void) __attribute__((weak, alias("Ignore")));
void MTU3_TGID3(void) __attribute__((weak, alias("Ignore")));
void MTU3_TCIV3(void) __attribute__((weak, alias("Ignore")));
void MTU4_TGIA4(void) __attribute__((weak, alias("Ignore")));
void MTU4_TGIB4(void) __attribute__((weak, alias("Ignore")));
void MTU4_TGIC4(void) __attribute__((weak, alias("Ignore")));
void MTU4_TGID4(void) __attribute__((weak, alias("Ignore")));
void MTU4_TCIV4(void) __attribute__((weak, alias("Ignore")));
void MTU5_TGIU5(void) __attribute__((weak, alias("Ignore")));
void MTU5_TGIV5(void) __attribute__((weak, alias("Ignore")));
void MTU5_TGIW5(void) __attribute__((weak, alias("Ignore")));
void TPU0_TGI0A(void) __attribute__((weak, alias("Ignore")));
void TPU0_TGI0B(void) __attribute__((weak, alias("Ignore")));
void TPU0_TGI0C(void) __attribute__((weak, alias("Ignore")));
void TPU0_TGI0D(void) __attribute__((weak, alias("Ignore")));
void TPU0_TCI0V(void) __attribute__((weak, alias("Ignore")));
void TPU1_TGI1A(void) __attribute__((weak, alias("Ignore")));
void TPU1_TGI1B(void) __attribute__((weak, alias("Ignore")));
void TPU1_TCI1V(void) __attribute__((weak, alias("Ignore")));
void TPU1_TCI1U(void) __attribute__((weak, alias("Ignore")));
void TPU2_TGI2A(void) __attribute__((weak, alias("Ignore")));
void TPU2_TGI2B(void) __attribute__((weak, alias("Ignore")));
void TPU2_TCI2V(void) __attribute__((weak, alias("Ignore")));
void TPU2_TCI2U(void) __attribute__((weak, alias("Ignore")));
void TPU3_TGI3A(void) __attribute__((weak, alias("Ignore")));
void TPU3_TGI3B(void) __attribute__((weak, alias("Ignore")));
void TPU3_TGI3C(void) __attribute__((weak, alias("Ignore")));
void TPU3_TGI3D(void) __attribute__((weak, alias("Ignore")));
void TPU3_TCI3V(void) __attribute__((weak, alias("Ignore")));
void TPU4_TGI4A(void) __attribute__((weak, alias("Ignore")));
void TPU4_TGI4B(void) __attribute__((weak, alias("Ignore")));
void TPU4_TCI4V(void) __attribute__((weak, alias("Ignore")));
void TPU4_TCI4U(void) __attribute__((weak, alias("Ignore")));
void TPU5_TGI5A(void) __attribute__((weak, alias("Ignore")));
void TPU5_TGI5B(void) __attribute__((weak, alias("Ignore")));
void TPU5_TCI5V(void) __attribute__((weak, alias("Ignore")));
void TPU5_TCI5U(void) __attribute__((weak, alias("Ignore")));
void POE_OEI1(void) __attribute__((weak, alias("Ignore")));
void POE_OEI2(void) __attribute__((weak, alias("Ignore")));
void TMR0_CMIA0(void) __attribute__((weak, alias("Ignore")));
void TMR0_CMIB0(void) __attribute__((weak, alias("Ignore")));
void TMR0_OVI0(void) __attribute__((weak, alias("Ignore")));
void TMR1_CMIA1(void) __attribute__((weak, alias("Ignore")));
void TMR1_CMIB1(void) __attribute__((weak, alias("Ignore")));
void TMR1_OVI1(void) __attribute__((weak, alias("Ignore")));
void TMR2_CMIA2(void) __attribute__((weak, alias("Ignore")));
void TMR2_CMIB2(void) __attribute__((weak, alias("Ignore")));
void TMR2_OVI2(void) __attribute__((weak, alias("Ignore")));
void TMR3_CMIA3(void) __attribute__((weak, alias("Ignore")));
void TMR3_CMIB3(void) __attribute__((weak, alias("Ignore")));
void TMR3_OVI3(void) __attribute__((weak, alias("Ignore")));
void SCI2_ERI2(void) __attribute__((weak, alias("Ignore")));
void SCI2_RXI2(void) __attribute__((weak, alias("Ignore")));
void SCI2_TXI2(void) __attribute__((weak, alias("Ignore")));
void SCI2_TEI2(void) __attribute__((weak, alias("Ignore")));
void SCI3_ERI3(void) __attribute__((weak, alias("Ignore")));
void SCI3_RXI3(void) __attribute__((weak, alias("Ignore")));
void SCI3_TXI3(void) __attribute__((weak, alias("Ignore")));
void SCI3_TEI3(void) __attribute__((weak, alias("Ignore")));
void SCI4_ERI4(void) __attribute__((weak, alias("Ignore")));
void SCI4_RXI4(void) __attribute__((weak, alias("Ignore")));
void SCI4_TXI4(void) __attribute__((weak, alias("Ignore")));
void SCI4_TEI4(void) __attribute__((weak, alias("Ignore")));
void DMAC_DMAC0I(void) __attribute__((weak, alias("Ignore")));
void DMAC_DMAC1I(void) __attribute__((weak, alias("Ignore")));
void DMAC_DMAC2I(void) __attribute__((weak, alias("Ignore")));
void DMAC_DMAC3I(void) __attribute__((weak, alias("Ignore")));
void SCI7_ERI7(void) __attribute__((weak, alias("Ignore")));
void SCI7_RXI7(void) __attribute__((weak, alias("Ignore")));
void SCI7_TXI7(void) __attribute__((weak, alias("Ignore")));
void SCI7_TEI7(void) __attribute__((weak, alias("Ignore")));
void SCI10_ERI10(void) __attribute__((weak, alias("Ignore")));
void SCI10_RXI10(void) __attribute__((weak, alias("Ignore")));
void SCI10_TXI10(void) __attribute__((weak, alias("Ignore")));
void SCI10_TEI10(void) __attribute__((weak, alias("Ignore")));
void SCI0_ERI0(void) __attribute__((weak, alias("Ignore")));
void SCI0_RXI0(void) __attribute__((weak, alias("Ignore")));
void SCI0_TXI0(void) __attribute__((weak, alias("Ignore")));
void SCI0_TEI0(void) __attribute__((weak, alias("Ignore")));
void SCI1_ERI1(void) __attribute__((weak, alias("Ignore")));
void SCI1_RXI1(void) __attribute__((weak, alias("Ignore")));
void SCI1_TXI1(void) __attribute__((weak, alias("Ignore")));
void SCI1_TEI1(void) __attribute__((weak, alias("Ignore")));
void SCI5_ERI5(void) __attribute__((weak, alias("Ignore")));
void SCI5_RXI5(void) __attribute__((weak, alias("Ignore")));
void SCI5_TXI5(void) __attribute__((weak, alias("Ignore")));
void SCI5_TEI5(void) __attribute__((weak, alias("Ignore")));
void SCI6_ERI6(void) __attribute__((weak, alias("Ignore")));
void SCI6_RXI6(void) __attribute__((weak, alias("Ignore")));
void SCI6_TXI6(void) __attribute__((weak, alias("Ignore")));
void SCI6_TEI6(void) __attribute__((weak, alias("Ignore")));
void SCI8_ERI8(void) __attribute__((weak, alias("Ignore")));
void SCI8_RXI8(void) __attribute__((weak, alias("Ignore")));
void SCI8_TXI8(void) __attribute__((weak, alias("Ignore")));
void SCI8_TEI8(void) __attribute__((weak, alias("Ignore")));
void SCI9_ERI9(void) __attribute__((weak, alias("Ignore")));
void SCI9_RXI9(void) __attribute__((weak, alias("Ignore")));
void SCI9_TXI9(void) __attribute__((weak, alias("Ignore")));
void SCI9_TEI9(void) __attribute__((weak, alias("Ignore")));
void SCI12_ERI12(void) __attribute__((weak, alias("Ignore")));
void SCI12_RXI12(void) __attribute__((weak, alias("Ignore")));
void SCI12_TXI12(void) __attribute__((weak, alias("Ignore")));
void SCI12_TEI12(void) __attribute__((weak, alias("Ignore")));
void SCI12_SCIX0(void) __attribute__((weak, alias("Ignore")));
void SCI12_SCIX1(void) __attribute__((weak, alias("Ignore")));
void SCI12_SCIX2(void) __attribute__((weak, alias("Ignore")));
void SCI12_SCIX3(void) __attribute__((weak, alias("Ignore")));
void RIIC0_EEI0(void) __attribute__((weak, alias("Ignore")));
void RIIC0_RXI0(void) __attribute__((weak, alias("Ignore")));
void RIIC0_TXI0(void) __attribute__((weak, alias("Ignore")));
void RIIC0_TEI0(void) __attribute__((weak, alias("Ignore")));
void SCI11_ERI11(void) __attribute__((weak, alias("Ignore")));
void SCI11_RXI11(void) __attribute__((weak, alias("Ignore")));
void SCI11_TXI11(void) __attribute__((weak, alias("Ignore")));
void SCI11_TEI11(void) __attribute__((weak, alias("Ignore")));

const fp relocatableVectorTable[] __attribute__((section(".relocatableVectorTable"))) = {
   Excep_BRK, // Reserved 0 0x0000
   0, // Reserved 1 0x0004
   0, // Reserved 2 0x0008
   0, // Reserved 3 0x000C
   0, // Reserved 4 0x0010
   0, // Reserved 5 0x0014
   0, // Reserved 6 0x0018
   0, // Reserved 7 0x001C
   0, // Reserved 8 0x0020
   0, // Reserved 9 0x0024
   0, // Reserved 10 0x0028
   0, // Reserved 11 0x002C
   0, // Reserved 12 0x0030
   0, // Reserved 13 0x0034
   0, // Reserved 14 0x0038
   0, // Reserved 15 0x003C
   BSC_BUSERR, // BSC BUSERR 16 0x0040
   0, // Reserved 17 0x0044
   0, // Reserved 18 0x0048
   0, // Reserved 19 0x004C
   0, // Reserved 20 0x0050
   FCU_FIFERR, // FCU FIFERR 21 0x0054
   0, // Reserved 22 0x0058
   FCU_FRDYI, // FCU FRDYI 23 0x005C
   0, // Reserved 24 0x0060
   0, // Reserved 25 0x0064
   0, // Reserved 26 0x0068
   ICU_SWINT, // ICU SWINT 27 0x006C
   CMT0_CMI0, // CMT0_CMI0 28 0x0070
   CMT1_CMI1, // CMT1 CMI1 29 0x0074
   CMT2_CMI2, // CMT2 CMI2 30 0x0078
   CMT3_CMI3, // CMT3 CMI3 31 0x007C
   CAC_FERRF, // CAC FERRF 32 0x0080
   CAC_MENDF, // CAC MENDF 33 0x0084
   CAC_OVFF, // CAC OVFF 34 0x0088
   0, // Reserved 35 0x008C
   USB0_D0FIFO0, // USB0 D0FIFO0 36 0x0090 (RX231)
   USB1_D0FIFO0, // USB1 D0FIFO0 37 0x0094 (RX231)
   USB0_USBI0, // USB0 USBI0 38 0x0098 (RX231)
   0, // Reserved 39 0x009C
   SDHI_SBFAI, // SDHI SBFAI 40 0x00A0 (RX231)
   SDHI_CDETI, // SDHI CDETI 41 0x00A4 (RX231)
   SDHI_CACI, // SDHI CACI 42 0x00A8 (RX231)
   SDHI_SDACI, // SDHI SDACI 43 0x00AC (RX231)
   RSPI0_SPEI0, // RSPI0 SPEI0 44 0x00B0
   RSPI0_SPRI0, // RSPI0 SPRI0 45 0x00B4
   RSPI0_SPTI0, // RSPI0 SPTI0 46 0x00B8
   RSPI0_SPII0, // RSPI0 SPII0 47 0x00BC
   0, // Reserved 48 0x00A0
   0, // Reserved 49 0x00A4
   0, // Reserved 50 0x00A8
   0, // Reserved 51 0x00AC
   CAN_COMFRXINT, // CAN COMFRXINT 52 0x00A0 (RX231)
   CAN_RXFINT, // CAN RXFINT 53 0x00A4 (RX231)
   CAN_TXINT, // CAN TXINT 54 0x00A8 (RX231)
   CAN_CHERRINT, // CAN CHERRINT 55 0x00AC (RX231)
   CAN_GLERRINT, // CAN GLERRINT 56 0x00AC (RX231)
   DOC_DOPCF, // DOC DOPCF 57 0x00E4
   CMPB_CMPB0, // CMPB CMPB0 58 0x00E8
   CMPB_CMPB1, // CMPB CMPB0 59 0x00EC
   CTSU_CTSUWR, // CTSU CTSUWR 60 0x00F0 (RX231)
   CTSU_CTSURD, // CTSU CTSURD 61 0x00F4 (RX231)
   CTSU_CTSUFN, // CTSU CTSUFN 62 0x00F8 (RX231)
   RTC_CUP, // RTC CUP 63 0x00FC
   ICU_IRQ0, // ICU IRQ0 64 0x0100
   ICU_IRQ1, // ICU IRQ1 65 0x0104
   ICU_IRQ2, // ICU IRQ2 66 0x0108
   ICU_IRQ3, // ICU IRQ3 67 0x010C
   ICU_IRQ4, // ICU IRQ4 68 0x0110
   ICU_IRQ5, // ICU IRQ5 69 0x0114
   ICU_IRQ6, // ICU IRQ6 70 0x0118
   ICU_IRQ7, // ICU IRQ7 71 0x011C
   0, // Reserved 72 0x0120
   0, // Reserved 73 0x0124
   0, // Reserved 74 0x0128
   0, // Reserved 75 0x012C
   0, // Reserved 76 0x0130
   0, // Reserved 77 0x0134
   0, // Reserved 78 0x0138
   0, // Reserved 79 0x013C
   ELC_ELSR8I, // ELC ELSR8I 80 0x0140 (RX231)
   0, // Reserved 81 0x0144
   0, // Reserved 82 0x0148
   0, // Reserved 83 0x014C
   0, // Reserved 84 0x0150
   0, // Reserved 85 0x0154
   0, // Reserved 86 0x0158
   0, // Reserved 87 0x015C
   LVD_LVD1, // LVD LVD1 / CMPA CMPA1 88 0x0160
   LVD_LVD2, // LVD LVD2 / CMPA CMPA2 89 0x0164
   USB0_USBR0, // USB0 USBR0 90 0x168 (RX231)
   VBATT_VBTLVDI, // VBATT VBTLVDI 91 0x16C (RX231)
   RTC_ALM, // RTC ALM 92 0x0170
   RTC_PRD, // RTC PRD 93 0x0174
   0, // Reserved 94 0x0178
   0, // Reserved 95 0x017C
   0, // Reserved 96 0x0180
   0, // Reserved 97 0x0184
   0, // Reserved 98 0x0188
   0, // Reserved 99 0x018C
   0, // Reserved 100 0x0190
   0, // Reserved 101 0x0194
   S12AD_S12ADI0, // S12AD S12ADI0 102 0x198
   S12AD_GBADI, // S12AD GBADI 103 0x19C
   CMPB1_CMPB2, // CMPB1 CMPB2 104 0x1A0 (RX231)
   CMPB1_CMPB3, // CMPB1 CMPB3 105 0x1A4 (RX231)
   ELC_ELSR18I, // ELC ELSR18I 106 0x148
   ELC_ELSR19I, // ELC ELSR19I 107 0x14C
   SSI0_SSIF0, // SSI0 SSIF0 108 0x01B0 (RX231)
   SSI0_SSIRXI0, // SSI0 SSIRXI0 109 0x01B4 (RX231)
   SSI0_SSITXI0, // SSI0 SSITXI0 110 0x01B8 (RX231)
   Security_RD, // Security RD 111 0x01BC (RX231)
   Security_WR, // Security WR 112 0x01C0 (RX231)
   Security_Error, // Security Error 113 0x01C4 (RX231)
   MTU0_TGIA0, // MTU0 TGIA0 114 0x01C8
   MTU0_TGIB0, // MTU0 TGIB0 115 0x01CC
   MTU0_TGIC0, // MTU0 TGIC0 116 0x01D0
   MTU0_TGID0, // MTU0 TGID0 117 0x01D4
   MTU0_TCIV0, // MTU0 TGIV0 118 0x01D8
   MTU0_TGIE0, // MTU0 TGIE0 119 0x01DC
   MTU0_TGIF0, // MTU0 TGIF0 120 0x01E0
   MTU1_TGIA1, // MTU1 TGIA1 121 0x01E4
   MTU1_TGIB1, // MTU1 TGI1B 122 0x01E8
   MTU1_TCIV1, // MTU1 TCI1V 123 0x01EC
   MTU1_TCIU1, // MTU1 TCI1U 124 0x01F0
   MTU2_TGIA2, // MTU2 TGI2A 125 0x01F4
   MTU2_TGIB2, // MTU2 TGI2B 126 0x01F8
   MTU2_TCIV2, // MTU2 TCI2V 127 0x01FC
   MTU2_TCIU2, // MTU2 TCI2U 128 0x0200
   MTU3_TGIA3, // MTU3 TGI3A 129 0x0204
   MTU3_TGIB3, // MTU3 TGI3B 130 0x0208
   MTU3_TGIC3, // MTU3 TGI3C 131 0x020C
   MTU3_TGID3, // MTU3 TGI3D 132 0x0210
   MTU3_TCIV3, // MTU3 TCI3V 133 0x0214
   MTU4_TGIA4, // MTU4 TGI4A 134 0x0218
   MTU4_TGIB4, // MTU4 TGI4B 135 0x021C
   MTU4_TGIC4, // MTU4 TGI4C 136 0x0220
   MTU4_TGID4, // MTU4 TGI4D 137 0x0224
   MTU4_TCIV4, // MTU4 TCI4V 138 0x0228
   MTU5_TGIU5, // MTU5 TGI5U 139 0x022C
   MTU5_TGIV5, // MTU5 TGI5V 140 0x0230
   MTU5_TGIW5, // MTU5 TGI5W 141 0x0234
   TPU0_TGI0A, // TPU0 TGI0A 142 0x0238
   TPU0_TGI0B, // TPU0 TGI0B 143 0x023C
   TPU0_TGI0C, // TPU0 TGI0C 144 0x0240
   TPU0_TGI0D, // TPU0 TGI0D 145 0x0244
   TPU0_TCI0V, // TPU0 TCI0V 146 0x0248
   TPU1_TGI1A, // TPU1 TGI1A 147 0x024C
   TPU1_TGI1B, // TPU1 TGI1B 148 0x0250
   TPU1_TCI1V, // TPU1 TCI1V 149 0x0254
   TPU1_TCI1U, // TPU1 TCI1U 150 0x0258
   TPU2_TGI2A, // TPU2 TGI2A 151 0x025C
   TPU2_TGI2B, // TPU2 TGI2B 152 0x0260
   TPU2_TCI2V, // TPU2 TCI2V 153 0x0264
   TPU2_TCI2U, // TPU2 TCI2U 154 0x0268
   TPU3_TGI3A, // TPU3 TGI3A 155 0x026C
   TPU3_TGI3B, // TPU3 TGI3B 156 0x0270
   TPU3_TGI3C, // TPU3 TGI3C 157 0x0274
   TPU3_TGI3D, // TPU3 TGI3D 158 0x0278
   TPU3_TCI3V, // TPU3 TCI3V 159 0x027C
   TPU4_TGI4A, // TPU4 TGI4A 160 0x0280
   TPU4_TGI4B, // TPU4 TGI4B 161 0x0284
   TPU4_TCI4V, // TPU4 TCI4V 162 0x0288
   TPU4_TCI4U, // TPU4 TCI4U 163 0x028C
   TPU5_TGI5A, // TPU5 TGI5A 164 0x0290
   TPU5_TGI5B, // TPU5 TGI5B 165 0x0294
   TPU5_TCI5V, // TPU5 TCI5V 166 0x0298
   TPU5_TCI5U, // TPU5 TCI5U 167 0x029C
   0, // Reserved 168 0x02A0
   0, // Reserved 169 0x02A4
   POE_OEI1, // POE OEI1 170 0x02A8
   POE_OEI2, // POE OEI2 171 0x02AC
   0, // Reserved 172 0x02B0
   0, // Reserved 173 0x02B4
   TMR0_CMIA0, // TMR0 CMIA0 174 0x02B8
   TMR0_CMIB0, // TMR0 CMIB0 175 0x02BC
   TMR0_OVI0, // TMR0 OVI0 176 0x02C0
   TMR1_CMIA1, // TMR1 CMIA1 177 0x02C4
   TMR1_CMIB1, // TMR1 CMIB1 178 0x02C8
   TMR1_OVI1, // TMR1 OVI1 179 0x02CC
   TMR2_CMIA2, // TMR2 CMIA2 180 0x02D0
   TMR2_CMIB2, // TMR2 CMIB2 181 0x02D4
   TMR2_OVI2, // TMR2 OVI2 182 0x02D8
   TMR3_CMIA3, // TMR3 CMIA3 183 0x02DC
   TMR3_CMIB3, // TMR3 CMIB3 184 0x02E0
   TMR3_OVI3, // TMR3 OVI3 185 0x02E4
   SCI2_ERI2, // SCI2 ERI2 186 0x02E8 (RX210)
   SCI2_RXI2, // SCI2 RXI2 187 0x02EC (RX210)
   SCI2_TXI2, // SCI2 TXI2 188 0x02F0 (RX210)
   SCI2_TEI2, // SCI2 TEI2 189 0x02F4 (RX210)
   SCI3_ERI3, // SCI3 ERI3 190 0x02F8 (RX210)
   SCI3_RXI3, // SCI3 RXI3 191 0x02FC (RX210)
   SCI3_TXI3, // SCI3 TXI3 192 0x0300 (RX210)
   SCI3_TEI3, // SCI3 TEI3 193 0x0304 (RX210)
   SCI4_ERI4, // SCI4 ERI4 194 0x0308 (RX210)
   SCI4_RXI4, // SCI4 RXI4 195 0x030C (RX210)
   SCI4_TXI4, // SCI4 TXI4 196 0x0310 (RX210)
   SCI4_TEI4, // SCI4 TEI4 197 0x0314 (RX210)
   DMAC_DMAC0I, // DMAC DMAC0I 198 0x0318
   DMAC_DMAC1I, // DMAC DMAC1I 199 0x031C
   DMAC_DMAC2I, // DMAC DMAC2I 200 0x0320
   DMAC_DMAC3I, // DMAC DMAC3I 201 0x0324
   0, // Reserved 202 0x0328
   0, // Reserved 203 0x032C
   0, // Reserved 204 0x0330
   0, // Reserved 205 0x0334
   SCI7_ERI7, // SCI7 ERI7 206 0x0338 (RX210)
   SCI7_RXI7, // SCI7 RXI7 207 0x033C (RX210)
   SCI7_TXI7, // SCI7 TXI7 208 0x0340 (RX210)
   SCI7_TEI7, // SCI7 TEI7 209 0x0344 (RX210)
   SCI10_ERI10, // SCI10 ERI10 210 0x0348 (RX210)
   SCI10_RXI10, // SCI10 RXI10 211 0x034C (RX210)
   SCI10_TXI10, // SCI10 TXI10 212 0x0350 (RX210)
   SCI10_TEI10, // SCI10 TEI10 213 0x0354 (RX210)
   SCI0_ERI0, // SCI0 ERI0 214 0x0358
   SCI0_RXI0, // SCI0 RXI0 215 0x035C
   SCI0_TXI0, // SCI0 TXI0 216 0x0360
   SCI0_TEI0, // SCI0 TEI0 217 0x0364
   SCI1_ERI1, // SCI1 ERI1 218 0x0368
   SCI1_RXI1, // SCI1 RXI1 219 0x036C
   SCI1_TXI1, // SCI1 TXI1 220 0x0370
   SCI1_TEI1, // SCI1 TEI1 221 0x0374
   SCI5_ERI5, // SCI5 ERI5 222 0x0378
   SCI5_RXI5, // SCI5 RXI5 223 0x037C
   SCI5_TXI5, // SCI5 TXI5 224 0x0380
   SCI5_TEI5, // SCI5 TEI5 225 0x0384
   SCI6_ERI6, // SCI6 ERI6 226 0x0388
   SCI6_RXI6, // SCI6 RXI6 227 0x038C
   SCI6_TXI6, // SCI6 TXI6 228 0x0390
   SCI6_TEI6, // SCI6 TEI6 229 0x0394
   SCI8_ERI8, // SCI8 ERI8 230 0x0398
   SCI8_RXI8, // SCI8 RXI8 231 0x039C
   SCI8_TXI8, // SCI8 TXI8 232 0x03A0
   SCI8_TEI8, // SCI8 TEI8 233 0x03A4
   SCI9_ERI9, // SCI9 ERI9 234 0x03A8
   SCI9_RXI9, // SCI9 RXI9 235 0x03AC
   SCI9_TXI9, // SCI9 TXI9 236 0x03B0
   SCI9_TEI9, // SCI9 TEI9 237 0x03B4
   SCI12_ERI12, // SCI12 ERI12 238 0x03B8
   SCI12_RXI12, // SCI12 RXI12 239 0x03BC
   SCI12_TXI12, // SCI12 TXI12 240 0x03C0
   SCI12_TEI12, // SCI12 TEI12 241 0x03C4
   SCI12_SCIX0, // SCI12 SCIX0 242 0x03C8
   SCI12_SCIX1, // SCI12 SCIX1 243 0x03CC
   SCI12_SCIX2, // SCI12 SCIX2 244 0x03D0
   SCI12_SCIX3, // SCI12 SCIX3 245 0x03D4
   RIIC0_EEI0, // RIIC0 EEI0 246 0x03D8
   RIIC0_RXI0, // RIIC0 RXI0 247 0x03DC
   RIIC0_TXI0, // RIIC0 TXI0 248 0x03E0
   RIIC0_TEI0, // RIIC0 TEI0 249 0x03E4
   SCI11_ERI11, // SCI11 ERI11 250 0x03E8 (RX210)
   SCI11_RXI11, // SCI11 RXI11 251 0x03EC (RX210)
   SCI11_TXI11, // SCI11 TXI11 252 0x03F0 (RX210)
   SCI11_TEI11, // SCI11 TEI11 253 0x03F4 (RX210)
   0, // Reserved 244 0x03F8
   0, // Reserved 255 0x03FC
};

static void Default_Handler(void) __attribute__((interrupt));
static void Default_Handler(void)
{
   Action_Invoke(Action_Rx2xxSystemReset_Init());
}

static void Ignore(void) __attribute__((interrupt));
static void Ignore(void)
{
}
