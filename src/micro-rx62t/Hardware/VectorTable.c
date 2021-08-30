/*!
 * @file
 * @brief Vector table for RX210/RX231 Micro
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Reset.h"
#include "IdCode.h"

typedef void (*fp)(void);

extern void PowerON_Reset(void);

static void Default_Handler(void);

void PrivilegedInstructionException(void) __attribute__((weak, alias("Default_Handler")));
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
   Default_Handler, // Reserved 0xffffff88
   Default_Handler, // Reserved 0xffffff8C
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
   Default_Handler, // Reserved 0xffffffd4
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
void FCUIF_FIFERR(void) __attribute__((weak, alias("Ignore")));
void FCUIF_FRDYI(void) __attribute__((weak, alias("Ignore")));
void ICU_SWINT(void) __attribute__((weak, alias("Ignore")));
void CMT0_CMI0(void) __attribute__((weak, alias("Ignore")));
void CMT1_CMI1(void) __attribute__((weak, alias("Ignore")));
void CMT2_CMI2(void) __attribute__((weak, alias("Ignore")));
void CMT3_CMI3(void) __attribute__((weak, alias("Ignore")));
void RSPI0_SPEI0(void) __attribute__((weak, alias("Ignore")));
void RSPI0_SPRI0(void) __attribute__((weak, alias("Ignore")));
void RSPI0_SPTI0(void) __attribute__((weak, alias("Ignore")));
void RSPI0_SPII0(void) __attribute__((weak, alias("Ignore")));
void CAN0_ERS0(void) __attribute__((weak, alias("Ignore")));
void CAN0_RXF0(void) __attribute__((weak, alias("Ignore")));
void CAN0_TXF0(void) __attribute__((weak, alias("Ignore")));
void CAN0_RXM0(void) __attribute__((weak, alias("Ignore")));
void CAN0_TXM0(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ0(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ1(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ2(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ3(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ4(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ5(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ6(void) __attribute__((weak, alias("Ignore")));
void ICU_IRQ7(void) __attribute__((weak, alias("Ignore")));
void WDT_WOVI(void) __attribute__((weak, alias("Ignore")));
void AD0_ADI0(void) __attribute__((weak, alias("Ignore")));
void S12AD0_S12ADI0(void) __attribute__((weak, alias("Ignore")));
void S12AD1_S12ADI1(void) __attribute__((weak, alias("Ignore")));
void CMPB_CMPI(void) __attribute__((weak, alias("Ignore")));
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
void MTU6_TGIA6(void) __attribute__((weak, alias("Ignore")));
void MTU6_TGIB6(void) __attribute__((weak, alias("Ignore")));
void MTU6_TGIC6(void) __attribute__((weak, alias("Ignore")));
void MTU6_TGID6(void) __attribute__((weak, alias("Ignore")));
void MTU6_TCIV6(void) __attribute__((weak, alias("Ignore")));
void MTU7_TGIA7(void) __attribute__((weak, alias("Ignore")));
void MTU7_TGIB7(void) __attribute__((weak, alias("Ignore")));
void MTU7_TGIC7(void) __attribute__((weak, alias("Ignore")));
void MTU7_TGID7(void) __attribute__((weak, alias("Ignore")));
void MTU7_TCIV7(void) __attribute__((weak, alias("Ignore")));
void POE_OEI1(void) __attribute__((weak, alias("Ignore")));
void POE_OEI2(void) __attribute__((weak, alias("Ignore")));
void POE_OEI3(void) __attribute__((weak, alias("Ignore")));
void POE_OEI4(void) __attribute__((weak, alias("Ignore")));
void GPT0_GTCIA0(void) __attribute__((weak, alias("Ignore")));
void GPT0_GTCIB0(void) __attribute__((weak, alias("Ignore")));
void GPT0_GTCIC0(void) __attribute__((weak, alias("Ignore")));
void GPT0_GTCIE0(void) __attribute__((weak, alias("Ignore")));
void GPT0_GTCIV0(void) __attribute__((weak, alias("Ignore")));
void GPT0_LOCO1(void) __attribute__((weak, alias("Ignore")));
void GPT1_GTCIA1(void) __attribute__((weak, alias("Ignore")));
void GPT1_GTCIB1(void) __attribute__((weak, alias("Ignore")));
void GPT1_GTCIC1(void) __attribute__((weak, alias("Ignore")));
void GPT1_GTCIE1(void) __attribute__((weak, alias("Ignore")));
void GPT1_GTCIV1(void) __attribute__((weak, alias("Ignore")));
void GPT2_GTCIA2(void) __attribute__((weak, alias("Ignore")));
void GPT2_GTCIB2(void) __attribute__((weak, alias("Ignore")));
void GPT2_GTCIC2(void) __attribute__((weak, alias("Ignore")));
void GPT2_GTCIE2(void) __attribute__((weak, alias("Ignore")));
void GPT2_GTCIV2(void) __attribute__((weak, alias("Ignore")));
void GPT3_GTCIA3(void) __attribute__((weak, alias("Ignore")));
void GPT3_GTCIB3(void) __attribute__((weak, alias("Ignore")));
void GPT3_GTCIC3(void) __attribute__((weak, alias("Ignore")));
void GPT3_GTCIE3(void) __attribute__((weak, alias("Ignore")));
void GPT3_GTCIV3(void) __attribute__((weak, alias("Ignore")));
void SCI0_ERI0(void) __attribute__((weak, alias("Ignore")));
void SCI0_RXI0(void) __attribute__((weak, alias("Ignore")));
void SCI0_TXI0(void) __attribute__((weak, alias("Ignore")));
void SCI0_TEI0(void) __attribute__((weak, alias("Ignore")));
void SCI1_ERI1(void) __attribute__((weak, alias("Ignore")));
void SCI1_RXI1(void) __attribute__((weak, alias("Ignore")));
void SCI1_TXI1(void) __attribute__((weak, alias("Ignore")));
void SCI1_TEI1(void) __attribute__((weak, alias("Ignore")));
void SCI2_ERI2(void) __attribute__((weak, alias("Ignore")));
void SCI2_RXI2(void) __attribute__((weak, alias("Ignore")));
void SCI2_TXI2(void) __attribute__((weak, alias("Ignore")));
void SCI2_TEI2(void) __attribute__((weak, alias("Ignore")));
void RIIC0_ICEEI0(void) __attribute__((weak, alias("Ignore")));
void RIIC0_ICRXI0(void) __attribute__((weak, alias("Ignore")));
void RIIC0_ICTXI0(void) __attribute__((weak, alias("Ignore")));
void RIIC0_ICTEI0(void) __attribute__((weak, alias("Ignore")));
void LIN0_LIN0(void) __attribute__((weak, alias("Ignore")));

const fp relocatableVectorTable[] __attribute__((section(".relocatableVectorTable"))) = {
   Excep_BRK, // Reserved 0 0x0000
   0, // Reserved 1 0004h
   0, // Reserved 2 0008h
   0, // Reserved 3 000Ch
   0, // Reserved 4 0010h
   0, // Reserved 5 0014h
   0, // Reserved 6 0018h
   0, // Reserved 7 001Ch
   0, // Reserved 8 0020h
   0, // Reserved 9 0024h
   0, // Reserved 10 0028h
   0, // Reserved 11 002Ch
   0, // Reserved 12 0030h
   0, // Reserved 13 0034h
   0, // Reserved 14 0038h
   0, // Reserved 15 003Ch
   BSC_BUSERR, // Bus error BUSERR 16 0040h
   0, // Reserved 17 0044h
   0, // Reserved 18 0048h
   0, // Reserved 19 004Ch
   0, // Reserved 20 0050h
   FCUIF_FIFERR, // FCUIF FIFERR 21 0054h
   0, // Reserved 22 0058h
   FCUIF_FRDYI, // FRDYI 23 005Ch
   0, // Reserved 24 0060h
   0, // Reserved 25 0064h
   0, // Reserved 26 0068h
   ICU_SWINT, // ICU SWINT 27 006Ch
   CMT0_CMI0, // CMT0 CMI0 28 0070h
   CMT1_CMI1, // CMT0 CMI1 29 0074h
   CMT2_CMI2, // CMT1 CMI2 30 0078h
   CMT3_CMI3, // CMT1 CMI3 31 007Ch
   0, // Reserved 32 0080h
   0, // Reserved 33 0084h
   0, // Reserved 34 0088h
   0, // Reserved 35 008Ch
   0, // Reserved 36 0090h
   0, // Reserved 37 0094h
   0, // Reserved 38 0098h
   0, // Reserved 39 009Ch
   0, // Reserved 40 00A0h
   0, // Reserved 41 00A4h
   0, // Reserved 42 00A8h
   0, // Reserved 43 00ACh
   RSPI0_SPEI0, // RSPIO_SPEI0 44 00B0h
   RSPI0_SPRI0, // RSPIO_SPRI0 45 00B4h
   RSPI0_SPTI0, // RSPIO_SPTI0 46 00B8h
   RSPI0_SPII0, // RSPIO_SPII0 47 00BCh
   0, // Reserved 48 00C0h
   0, // Reserved 49 00C4h
   0, // Reserved 50 00C8h
   0, // Reserved 51 00CCh
   0, // Reserved 52 00D0h
   0, // Reserved 53 00D4h
   0, // Reserved 54 00D8h
   0, // Reserved 55 00DCh
   CAN0_ERS0, // CAN0_ERS0 56 00E0h
   CAN0_RXF0, // CAN0_RXF0 57 00E4h
   CAN0_TXF0, // CAN0_TXF0 58 00E8h
   CAN0_RXM0, // CAN0_RXM0 59 00ECh
   CAN0_TXM0, // CAN0_TXM0 60 00F0h
   0, // Reserved 61 00F4h
   0, // Reserved 62 00F8h
   0, // Reserved 63 00FCh
   ICU_IRQ0, // IRQ0 64 0100h
   ICU_IRQ1, // IRQ1 65 0104h
   ICU_IRQ2, // IRQ2 66 0108h
   ICU_IRQ3, // IRQ3 67 010Ch
   ICU_IRQ4, // IRQ4 68 0110h
   ICU_IRQ5, // IRQ5 69 0114h
   ICU_IRQ6, // IRQ6 70 0118h
   ICU_IRQ7, // IRQ7 71 011Ch
   0, // Reserved 72 0120h
   0, // Reserved 73 0124h
   0, // Reserved 74 0128h
   0, // Reserved 75 012Ch
   0, // Reserved 76 0130h
   0, // Reserved 77 0134h
   0, // Reserved 78 0138h
   0, // Reserved 79 013Ch
   0, // Reserved 80 0140h
   0, // Reserved 81 0144h
   0, // Reserved 82 0148h
   0, // Reserved 83 014Ch
   0, // Reserved 84 0150h
   0, // Reserved 85 0154h
   0, // Reserved 86 0158h
   0, // Reserved 87 015Ch
   0, // Reserved 88 0160h
   0, // Reserved 89 0164h
   0, // Reserved 90 0168h
   0, // Reserved 91 016Ch
   0, // Reserved 92 0170h
   0, // Reserved 93 0174h
   0, // Reserved 94 0178h
   0, // Reserved 95 017Ch
   WDT_WOVI, // WDT WOVI 96 0180h
   0, // Reserved 97 0184h
   AD0_ADI0, // ADA0 ADI0 98 0188h
   0, // Reserved 99 018Ch
   0, // Reserved 100 0190h
   0, // Reserved 101 0194h
   S12AD0_S12ADI0, // S12ADA0 S12ADI0 102 0198h
   S12AD1_S12ADI1, // S12ADA1 S12ADI1 103 019Ch
   0, // Reserved 104 01A0h
   0, // Reserved 105 01A4h
   CMPB_CMPI, // CMPI 106 01A8h
   0, // Reserved 107 01ACh
   0, // Reserved 108 01B0h
   0, // Reserved 109 01B4h
   0, // Reserved 110 01B8h
   0, // Reserved 111 01BCh
   0, // Reserved 112 01C0h
   0, // Reserved 113 01C4h
   MTU0_TGIA0, // MTU_0 TGIA0 114 01C8h
   MTU0_TGIB0, // MTU_0TGIB0 115 01CCh
   MTU0_TGIC0, // MTU_0TGIC0 116 01D0h
   MTU0_TGID0, // MTU_0 TGID0 117 01D4h
   MTU0_TCIV0, // MTU_0 TCIV0 118 01D8h
   MTU0_TGIE0, // MTU_0 TGIE0 119 01DCh
   MTU0_TGIF0, // MTU_0 TGIF0 120 01E0h
   MTU1_TGIA1, // MTU_1 TGIA1 121 01E4h
   MTU1_TGIB1, // MTU_1 TGIB1 122 01E8h
   MTU1_TCIV1, // MTU_1_TCIV1 123 01ECh
   MTU1_TCIU1, // MTU_1_TCIU1 124 01F0h
   MTU2_TGIA2, // MTU_2_TGIA2 125 01F4h
   MTU2_TGIB2, // MTU_2_TGIB2 126 01F8h
   MTU2_TCIV2, // MTU_2_TCIV2 127 01FCh
   MTU2_TCIU2, // MTU_2_TCIU2 128 0200h
   MTU3_TGIA3, // MTU_3_TGIA3 129 0204h
   MTU3_TGIB3, // MTU_3_TGIB3 130 0208h
   MTU3_TGIC3, // MTU_3_TGIC3 131 020Ch
   MTU3_TGID3, // MTU_3_TGID3 132 0210h
   MTU3_TCIV3, // MTU_3_TCIV3 133 0214h
   MTU4_TGIA4, // MTU_4_TGIA4 134 0218h
   MTU4_TGIB4, // MTU_4_TGIB4 135 021Ch
   MTU4_TGIC4, // MTU_4_TGIC4 136 0220h
   MTU4_TGID4, // MTU_4_TGID4 137 0224h
   MTU4_TCIV4, // MTU_4_TCIV4 138 0228h
   MTU5_TGIU5, // MTU_5 TGIU5 139 022Ch
   MTU5_TGIV5, // MTU_5 TGIV5 140 0230h
   MTU5_TGIW5, // MTU_5 TGIW5 141 0234h
   MTU6_TGIA6, // MTU_6 TGIA6 142 0238h
   MTU6_TGIB6, // MTU_6 TGIB6 143 023Ch
   MTU6_TGIC6, // MTU_6 TGIC6 144 0240h
   MTU6_TGID6, // MTU_6 TGID6 145 0244h
   MTU6_TCIV6, // MTU_6 TCIV6 146 0248h
   0, // Reserved 147 024Ch
   0, // Reserved 148 0250h
   MTU7_TGIA7, // MTU_7 TGIA7 149 0254h
   MTU7_TGIB7, // MTU_7 TGIB7 150 0258h
   MTU7_TGIC7, // MTU_7 TGIC7 151 025Ch
   MTU7_TGID7, // MTU_7 TGID7 152 0260h
   MTU7_TCIV7, // MTU_7 TCIV7 153 0264h
   0, // Reserved 154 0268h
   0, // Reserved 155 026Ch
   0, // Reserved 156 0270h
   0, // Reserved 157 0274h
   0, // Reserved 158 0278h
   0, // Reserved 159 027Ch
   0, // Reserved 160 0280h
   0, // Reserved 161 0284h
   0, // Reserved 162 0288h
   0, // Reserved 163 028Ch
   0, // Reserved 164 0290h
   0, // Reserved 165 0294h
   0, // Reserved 166 0298h
   0, // Reserved 167 029Ch
   0, // Reserved 168 02A0h
   0, // Reserved 169 02A4h
   POE_OEI1, // POE OE11 170 02A8h
   POE_OEI2, // POE OE12 171 02ACh
   POE_OEI3, // POE OE13 172 02B0h
   POE_OEI4, // POE OE14 173 02B4h
   GPT0_GTCIA0, // GPT0_GTCIA0 174 02B8h
   GPT0_GTCIB0, // GPT0_GTCIB0 175 02BCh
   GPT0_GTCIC0, // GPT0_GTCIC0 176 02C0h
   GPT0_GTCIE0, // GPT0_GTCIE0 177 02C4h
   GPT0_GTCIV0, // GPT0_GTCIV0 178 02C8h
   GPT0_LOCO1, // GPT0_LOCO1 179 02CCh
   GPT1_GTCIA1, // GPT1_GTCIA1 180 02D0h
   GPT1_GTCIB1, // GPT1_GTCIB1 181 02D4h
   GPT1_GTCIC1, // GPT1_GTCIC1 182 02D8h
   GPT1_GTCIE1, // GPT1_GTCIE1 183 02DCh
   GPT1_GTCIV1, // GPT1_GTCIV1 184 02E0h
   0, // Reserved 185 02E4h
   GPT2_GTCIA2, // GPT2_GTCIA2 186 02E8h
   GPT2_GTCIB2, // GPT2_GTCIB2 187 02ECh
   GPT2_GTCIC2, // GPT2_GTCIC2 188 02F0h
   GPT2_GTCIE2, // GPT2_GTCIE2 189 02F4h
   GPT2_GTCIV2, // GPT2_GTCIV2 190 02F8h
   0, // Reserved 191 02FCh
   GPT3_GTCIA3, // GPT3_GTCIA3 192 0300h
   GPT3_GTCIB3, // GPT3_GTCIB3 193 0304h
   GPT3_GTCIC3, // GPT3_GTCIC3 194 0308h
   GPT3_GTCIE3, // GPT3_GTCIE3 195 030Ch
   GPT3_GTCIV3, // GPT3_GTCIV3 196 0310h
   0, // Reserved 197 0314h
   0, // Reserved 198 0318h
   0, // Reserved 199 031Ch
   0, // Reserved 200 0320h
   0, // Reserved 201 0324h
   0, // Reserved 202 0328h
   0, // Reserved 203 032Ch
   0, // Reserved 204 0330h
   0, // Reserved 205 0334h
   0, // Reserved 206 0338h
   0, // Reserved 207 033Ch
   0, // Reserved 208 0340h
   0, // Reserved 209 0344h
   0, // Reserved 210 0348h
   0, // Reserved 211 034Ch
   0, // Reserved 212 0350h
   0, // Reserved 213 0354h
   SCI0_ERI0, // SCI0_ERI0 214 0358h
   SCI0_RXI0, // SCI0_RXI0 215 035Ch
   SCI0_TXI0, // SCI0_TXI0 216 0360h
   SCI0_TEI0, // SCI0_TEI0 217 0364h
   SCI1_ERI1, // SCI1_ERI1 218 0368h
   SCI1_RXI1, // SCI1_RXI1 219 036Ch
   SCI1_TXI1, // SCI1_TXI1 220 0370h
   SCI1_TEI1, // SCI1_TEI1 221 0374h
   SCI2_ERI2, // SCI2_ERI2 222 0378h
   SCI2_RXI2, // SCI2_RXI2 223 037Ch
   SCI2_TXI2, // SCI2_TXI2 224 0380h
   SCI2_TEI2, // SCI2_TEI2 225 0384h
   0, // Reserved 226 0388h
   0, // Reserved 227 038Ch
   0, // Reserved 228 0390h
   0, // Reserved 229 0394h
   0, // Reserved 230 0398h
   0, // Reserved 231 039Ch
   0, // Reserved 232 03A0h
   0, // Reserved 233 03A4h
   0, // Reserved 234 03A8h
   0, // Reserved 235 03ACh
   0, // Reserved 236 03B0h
   0, // Reserved 237 03B4h
   0, // Reserved 238 03B8h
   0, // Reserved 239 03BCh
   0, // Reserved 240 03C0h
   0, // Reserved 241 03C4h
   0, // Reserved 242 03C8h
   0, // Reserved 243 03CCh
   0, // Reserved 244 03D0h
   0, // Reserved 245 03D4h
   RIIC0_ICEEI0, // RIIC0_ICEEI0 246 03D8h
   RIIC0_ICRXI0, // RIIC0_ICRXI0 247 03DCh
   RIIC0_ICTXI0, // RIIC0_ICTXI0 248 03E0h
   RIIC0_ICTEI0, // RIIC0_ICTEI0 249 03E4h
   0, // Reserved 250 03E8h
   0, // Reserved 251 03ECh
   0, // Reserved 252 03F0h
   0, // Reserved 253 03F4h
   LIN0_LIN0, // LIN0_LIN0 254 03F8h
   0 // Reserved 255 03FCh
};

static void Default_Handler(void) __attribute__((interrupt));
static void Default_Handler(void)
{
   Reset();
}

static void Ignore(void) __attribute__((interrupt));
static void Ignore(void)
{
}
