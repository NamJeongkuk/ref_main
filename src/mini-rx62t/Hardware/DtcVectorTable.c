/*****************************************************************************\
$RCSfile$

$Author$

$Revision$

$Date$

DESCRIPTION:
<Present a general "implementation view" for the entire module. If there are
multiple paragraphs in the description, they should  be separated by a blank
line.>

Copyright GE Appliances - Confidential - All rights reserved.
\*****************************************************************************/

/***************************** Header Files **********************************/
#include "Dtc.h"
/*************************** Global Variables ********************************/
#define DUMMY_ADDR (void *)0xFFFFFFFF // dummy address
/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void *const dtcVectorTable[] __attribute__((section(".dtcVectorTable"))) = {
   DUMMY_ADDR, /*   0 INTB+0000h                   */
   DUMMY_ADDR, /*   1 INTB+0004h                   */
   DUMMY_ADDR, /*   2 INTB+0008h                   */
   DUMMY_ADDR, /*   3 INTB+000Ch                   */
   DUMMY_ADDR, /*   4 INTB+0010h                   */
   DUMMY_ADDR, /*   5 INTB+0014h                   */
   DUMMY_ADDR, /*   6 INTB+0018h                   */
   DUMMY_ADDR, /*   7 INTB+001Ch                   */
   DUMMY_ADDR, /*   8 INTB+0020h                   */
   DUMMY_ADDR, /*   9 INTB+0024h                   */
   DUMMY_ADDR, /*  10 INTB+0028h                   */
   DUMMY_ADDR, /*  11 INTB+002Ch                   */
   DUMMY_ADDR, /*  12 INTB+0030h                   */
   DUMMY_ADDR, /*  13 INTB+0034h                   */
   DUMMY_ADDR, /*  14 INTB+0038h                   */
   DUMMY_ADDR, /*  15 INTB+003Ch                   */
   DUMMY_ADDR, /*  16 INTB+0040h                   */
   DUMMY_ADDR, /*  17 INTB+0044h                   */
   DUMMY_ADDR, /*  18 INTB+0048h                   */
   DUMMY_ADDR, /*  19 INTB+004Ch                   */
   DUMMY_ADDR, /*  20 INTB+0050h                   */
   DUMMY_ADDR, /*  21 INTB+0054h                   */
   DUMMY_ADDR, /*  22 INTB+0058h                   */
   DUMMY_ADDR, /*  23 INTB+005Ch                   */
   DUMMY_ADDR, /*  24 INTB+0060h                   */
   DUMMY_ADDR, /*  25 INTB+0064h                   */
   DUMMY_ADDR, /*  26 INTB+0068h                   */
   DUMMY_ADDR, /*  27 INTB+006Ch       SWINT       */
   DUMMY_ADDR, /*  28 INTB+0070h       CMI0        */
   DUMMY_ADDR, /*  29 INTB+0074h       CMI1        */
   DUMMY_ADDR, /*  30 INTB+0078h       CMI2        */
   DUMMY_ADDR, /*  31 INTB+007Ch       CMI3        */
   DUMMY_ADDR, /*  32 INTB+0080h                   */
   DUMMY_ADDR, /*  33 INTB+0084h                   */
   DUMMY_ADDR, /*  34 INTB+0088h                   */
   DUMMY_ADDR, /*  35 INTB+008Ch                   */
   DUMMY_ADDR, /*  36 INTB+0090h                   */
   DUMMY_ADDR, /*  37 INTB+0094h                   */
   DUMMY_ADDR, /*  38 INTB+0098h                   */
   DUMMY_ADDR, /*  39 INTB+009Ch                   */
   DUMMY_ADDR, /*  40 INTB+00A0h                   */
   DUMMY_ADDR, /*  41 INTB+00A4h                   */
   DUMMY_ADDR, /*  42 INTB+00A8h                   */
   DUMMY_ADDR, /*  43 INTB+00ACh                   */
   DUMMY_ADDR, /*  44 INTB+00B0h                   */
   DUMMY_ADDR, /*  45 INTB+00B4h       SPRI0       */
   DUMMY_ADDR, /*  46 INTB+00B8h       SPTI0       */
   DUMMY_ADDR, /*  47 INTB+00BCh                   */
   DUMMY_ADDR, /*  48 INTB+00C0h                   */
   DUMMY_ADDR, /*  49 INTB+00C4h                   */
   DUMMY_ADDR, /*  50 INTB+00C8h                   */
   DUMMY_ADDR, /*  51 INTB+00CCh                   */
   DUMMY_ADDR, /*  52 INTB+00D0h                   */
   DUMMY_ADDR, /*  53 INTB+00D4h                   */
   DUMMY_ADDR, /*  54 INTB+00D8h                   */
   DUMMY_ADDR, /*  55 INTB+00DCh                   */
   DUMMY_ADDR, /*  56 INTB+00E0h                   */
   DUMMY_ADDR, /*  57 INTB+00E4h                   */
   DUMMY_ADDR, /*  58 INTB+00E8h                   */
   DUMMY_ADDR, /*  59 INTB+00ECh                   */
   DUMMY_ADDR, /*  60 INTB+00F0h                   */
   DUMMY_ADDR, /*  61 INTB+00F4h                   */
   DUMMY_ADDR, /*  62 INTB+00F8h                   */
   DUMMY_ADDR, /*  63 INTB+00FCh                   */
   DUMMY_ADDR, /*  64 INTB+0100h       IRQ0        */
   DUMMY_ADDR, /*  65 INTB+0104h       IRQ1        */
   DUMMY_ADDR, /*  66 INTB+0108h       IRQ2        */
   DUMMY_ADDR, /*  67 INTB+010Ch       IRQ3        */
   DUMMY_ADDR, /*  68 INTB+0110h       IRQ4        */
   DUMMY_ADDR, /*  69 INTB+0114h       IRQ5        */
   DUMMY_ADDR, /*  70 INTB+0118h       IRQ6        */
   DUMMY_ADDR, /*  71 INTB+011Ch       IRQ7        */
   DUMMY_ADDR, /*  72 INTB+0120h                   */
   DUMMY_ADDR, /*  73 INTB+0124h                   */
   DUMMY_ADDR, /*  74 INTB+0128h                   */
   DUMMY_ADDR, /*  75 INTB+012Ch                   */
   DUMMY_ADDR, /*  76 INTB+0130h                   */
   DUMMY_ADDR, /*  77 INTB+0134h                   */
   DUMMY_ADDR, /*  78 INTB+0138h                   */
   DUMMY_ADDR, /*  79 INTB+013Ch                   */
   DUMMY_ADDR, /*  80 INTB+0140h                   */
   DUMMY_ADDR, /*  81 INTB+0144h                   */
   DUMMY_ADDR, /*  82 INTB+0148h                   */
   DUMMY_ADDR, /*  83 INTB+014Ch                   */
   DUMMY_ADDR, /*  84 INTB+0150h                   */
   DUMMY_ADDR, /*  85 INTB+0154h                   */
   DUMMY_ADDR, /*  86 INTB+0158h                   */
   DUMMY_ADDR, /*  87 INTB+015Ch                   */
   DUMMY_ADDR, /*  88 INTB+0160h                   */
   DUMMY_ADDR, /*  89 INTB+0164h                   */
   DUMMY_ADDR, /*  90 INTB+0168h                   */
   DUMMY_ADDR, /*  91 INTB+016Ch                   */
   DUMMY_ADDR, /*  92 INTB+0170h                   */
   DUMMY_ADDR, /*  93 INTB+0174h                   */
   DUMMY_ADDR, /*  94 INTB+0178h                   */
   DUMMY_ADDR, /*  95 INTB+017Ch                   */
   DUMMY_ADDR, /*  96 INTB+0180h                   */
   DUMMY_ADDR, /*  97 INTB+0184h                   */
   DUMMY_ADDR, /*  98 INTB+0188h       ADI0        */
   DUMMY_ADDR, /*  99 INTB+018Ch                   */
   DUMMY_ADDR, /* 100 INTB+0190h                   */
   DUMMY_ADDR, /* 101 INTB+0194h                   */
   DUMMY_ADDR, /* 102 INTB+0198h       S12ADI0     */
   DUMMY_ADDR, /* 103 INTB+019Ch       S12ADI1     */
   DUMMY_ADDR, /* 104 INTB+01A0h                   */
   DUMMY_ADDR, /* 105 INTB+01A4h                   */
   DUMMY_ADDR, /* 106 INTB+01A8h       CMPI        */
   DUMMY_ADDR, /* 107 INTB+01ACh                   */
   DUMMY_ADDR, /* 108 INTB+01B0h                   */
   DUMMY_ADDR, /* 109 INTB+01B4h                   */
   DUMMY_ADDR, /* 110 INTB+01B8h                   */
   DUMMY_ADDR, /* 111 INTB+01BCh                   */
   DUMMY_ADDR, /* 112 INTB+01C0h                   */
   DUMMY_ADDR, /* 113 INTB+01C4h                   */
   DUMMY_ADDR, /* 114 INTB+01C8h       TGIA0       */
   DUMMY_ADDR, /* 115 INTB+01CCh       TGIB0       */
   DUMMY_ADDR, /* 116 INTB+01D0h       TGIC0       */
   DUMMY_ADDR, /* 117 INTB+01D4h       TGID0       */
   DUMMY_ADDR, /* 118 INTB+01D8h                   */
   DUMMY_ADDR, /* 119 INTB+01DCh                   */
   DUMMY_ADDR, /* 120 INTB+01E0h                   */
   DUMMY_ADDR, /* 121 INTB+01E4h       TGIA1       */
   DUMMY_ADDR, /* 122 INTB+01E8h       TGIB1       */
   DUMMY_ADDR, /* 123 INTB+01ECh                   */
   DUMMY_ADDR, /* 124 INTB+01F0h                   */
   DUMMY_ADDR, /* 125 INTB+01F4h       TGIA2       */
   DUMMY_ADDR, /* 126 INTB+01F8h       TGIB2       */
   DUMMY_ADDR, /* 127 INTB+01FCh                   */
   DUMMY_ADDR, /* 128 INTB+0200h                   */
   DUMMY_ADDR, /* 129 INTB+0204h       TGIA3       */
   DUMMY_ADDR, /* 130 INTB+0208h       TGIB3       */
   DUMMY_ADDR, /* 131 INTB+020Ch       TGIC3       */
   DUMMY_ADDR, /* 132 INTB+0210h       TGID3       */
   DUMMY_ADDR, /* 133 INTB+0214h                   */
   DUMMY_ADDR, /* 134 INTB+0218h       TGIA4       */
   DUMMY_ADDR, /* 135 INTB+021Ch       TGIB4       */
   DUMMY_ADDR, /* 136 INTB+0220h       TGIC4       */
   DUMMY_ADDR, /* 137 INTB+0224h       TGID4       */
   DUMMY_ADDR, /* 138 INTB+0228h       TCIV4       */
   DUMMY_ADDR, /* 139 INTB+022Ch       TGIU5       */
   DUMMY_ADDR, /* 140 INTB+0230h       TGIV5       */
   DUMMY_ADDR, /* 141 INTB+0234h       TGIW5       */
   DUMMY_ADDR, /* 142 INTB+0238h       TGIA6       */
   DUMMY_ADDR, /* 143 INTB+023Ch       TGIB6       */
   DUMMY_ADDR, /* 144 INTB+0240h       TGIC6       */
   DUMMY_ADDR, /* 145 INTB+0244h       TGID6       */
   DUMMY_ADDR, /* 146 INTB+0248h                   */
   DUMMY_ADDR, /* 147 INTB+024Ch                   */
   DUMMY_ADDR, /* 148 INTB+0250h                   */
   DUMMY_ADDR, /* 149 INTB+0254h       TGIA7       */
   DUMMY_ADDR, /* 150 INTB+0258h       TGIB7       */
   DUMMY_ADDR, /* 151 INTB+025Ch       TGIC7       */
   DUMMY_ADDR, /* 152 INTB+0260h       TGID7       */
   DUMMY_ADDR, /* 153 INTB+0264h       TCIV7       */
   DUMMY_ADDR, /* 154 INTB+0268h                   */
   DUMMY_ADDR, /* 155 INTB+026Ch                   */
   DUMMY_ADDR, /* 156 INTB+0270h                   */
   DUMMY_ADDR, /* 157 INTB+0274h                   */
   DUMMY_ADDR, /* 158 INTB+0278h                   */
   DUMMY_ADDR, /* 159 INTB+027Ch                   */
   DUMMY_ADDR, /* 160 INTB+0280h                   */
   DUMMY_ADDR, /* 161 INTB+0284h                   */
   DUMMY_ADDR, /* 162 INTB+0288h                   */
   DUMMY_ADDR, /* 163 INTB+028Ch                   */
   DUMMY_ADDR, /* 164 INTB+0290h                   */
   DUMMY_ADDR, /* 165 INTB+0294h                   */
   DUMMY_ADDR, /* 166 INTB+0298h                   */
   DUMMY_ADDR, /* 167 INTB+029Ch                   */
   DUMMY_ADDR, /* 168 INTB+02A0h                   */
   DUMMY_ADDR, /* 169 INTB+02A4h                   */
   DUMMY_ADDR, /* 170 INTB+02A8h                   */
   DUMMY_ADDR, /* 171 INTB+02ACh                   */
   DUMMY_ADDR, /* 172 INTB+02B0h                   */
   DUMMY_ADDR, /* 173 INTB+02B4h                   */
   DUMMY_ADDR, /* 174 INTB+02B8h       GTCIA0      */
   DUMMY_ADDR, /* 175 INTB+02BCh       GTCIB0      */
   DUMMY_ADDR, /* 176 INTB+02C0h       GTCIC0      */
   DUMMY_ADDR, /* 177 INTB+02C4h       GTCIE0      */
   DUMMY_ADDR, /* 178 INTB+02C8h       GTCIV0      */
   DUMMY_ADDR, /* 179 INTB+02CCh       LOCO1       */
   DUMMY_ADDR, /* 180 INTB+02D0h       GTCIA1      */
   DUMMY_ADDR, /* 181 INTB+02D4h       GTCIB1      */
   DUMMY_ADDR, /* 182 INTB+02D8h       GTCIC1      */
   DUMMY_ADDR, /* 183 INTB+02DCh       GTCIE1      */
   DUMMY_ADDR, /* 184 INTB+02E0h       GTCIV1      */
   DUMMY_ADDR, /* 185 INTB+02E4h                   */
   DUMMY_ADDR, /* 186 INTB+02E8h       GTCIA2      */
   DUMMY_ADDR, /* 187 INTB+02ECh       GTCIB2      */
   DUMMY_ADDR, /* 188 INTB+02F0h       GTCIC2      */
   DUMMY_ADDR, /* 189 INTB+02F4h       GTCIE2      */
   DUMMY_ADDR, /* 190 INTB+02F8h       GTCIV2      */
   DUMMY_ADDR, /* 191 INTB+02FCh                   */
   DUMMY_ADDR, /* 192 INTB+0300h       GTCIA3      */
   DUMMY_ADDR, /* 193 INTB+0304h       GTCIB3      */
   DUMMY_ADDR, /* 194 INTB+0308h       GTCIC3      */
   DUMMY_ADDR, /* 195 INTB+030Ch       GTCIE3      */
   DUMMY_ADDR, /* 196 INTB+0310h       GTCIV3      */
   DUMMY_ADDR, /* 197 INTB+0314h                   */
   DUMMY_ADDR, /* 198 INTB+0318h                   */
   DUMMY_ADDR, /* 199 INTB+031Ch                   */
   DUMMY_ADDR, /* 200 INTB+0320h                   */
   DUMMY_ADDR, /* 201 INTB+0324h                   */
   DUMMY_ADDR, /* 202 INTB+0328h                   */
   DUMMY_ADDR, /* 203 INTB+032Ch                   */
   DUMMY_ADDR, /* 204 INTB+0330h                   */
   DUMMY_ADDR, /* 205 INTB+0334h                   */
   DUMMY_ADDR, /* 206 INTB+0338h                   */
   DUMMY_ADDR, /* 207 INTB+033Ch                   */
   DUMMY_ADDR, /* 208 INTB+0340h                   */
   DUMMY_ADDR, /* 209 INTB+0344h                   */
   DUMMY_ADDR, /* 210 INTB+0348h                   */
   DUMMY_ADDR, /* 211 INTB+034Ch                   */
   DUMMY_ADDR, /* 212 INTB+0350h                   */
   DUMMY_ADDR, /* 213 INTB+0354h                   */
   DUMMY_ADDR, /* 214 INTB+0358h                   */
   (void *)&uart0Receive, /* 215 INTB+035Ch       SCI0_RXI0   */
   (void *)&uart0Transmit, /* 216 INTB+0360h       SCI0_TXI0   */
   DUMMY_ADDR, /* 217 INTB+0364h                   */
   DUMMY_ADDR, /* 218 INTB+0368h                   */
   DUMMY_ADDR, /* 219 INTB+036Ch       SCI1_RXI1   */
   DUMMY_ADDR, /* 220 INTB+0370h       SCI1_TXI1   */
   DUMMY_ADDR, /* 221 INTB+0374h                   */
   DUMMY_ADDR, /* 222 INTB+0378h                   */
   DUMMY_ADDR, /* 223 INTB+037Ch       SCI2_RXI2   */
   DUMMY_ADDR, /* 224 INTB+0380h       SCI2_TXI2   */
   DUMMY_ADDR, /* 225 INTB+0384h                   */
   DUMMY_ADDR, /* 226 INTB+0388h                   */
   DUMMY_ADDR, /* 227 INTB+038Ch                   */
   DUMMY_ADDR, /* 228 INTB+0390h                   */
   DUMMY_ADDR, /* 229 INTB+0394h                   */
   DUMMY_ADDR, /* 230 INTB+0398h                   */
   DUMMY_ADDR, /* 231 INTB+039Ch                   */
   DUMMY_ADDR, /* 232 INTB+03A0h                   */
   DUMMY_ADDR, /* 233 INTB+03A4h                   */
   DUMMY_ADDR, /* 234 INTB+03A8h                   */
   DUMMY_ADDR, /* 235 INTB+03ACh                   */
   DUMMY_ADDR, /* 236 INTB+03B0h                   */
   DUMMY_ADDR, /* 237 INTB+03B4h                   */
   DUMMY_ADDR, /* 238 INTB+03B8h                   */
   DUMMY_ADDR, /* 239 INTB+03BCh                   */
   DUMMY_ADDR, /* 240 INTB+03C0h                   */
   DUMMY_ADDR, /* 241 INTB+03C4h                   */
   DUMMY_ADDR, /* 242 INTB+03C8h                   */
   DUMMY_ADDR, /* 243 INTB+03CCh                   */
   DUMMY_ADDR, /* 244 INTB+03D0h                   */
   DUMMY_ADDR, /* 245 INTB+03D4h                   */
   DUMMY_ADDR, /* 246 INTB+03D8h                   */
   DUMMY_ADDR, /* 247 INTB+03DCh       ICRXI0      */
   DUMMY_ADDR, /* 248 INTB+03E0h       ICTXI0      */
   DUMMY_ADDR, /* 249 INTB+03E4h                   */
   DUMMY_ADDR, /* 250 INTB+03E8h                   */
   DUMMY_ADDR, /* 251 INTB+03ECh                   */
   DUMMY_ADDR, /* 252 INTB+03F0h                   */
   DUMMY_ADDR, /* 253 INTB+03F4h                   */
   DUMMY_ADDR, /* 254 INTB+03F8h                   */
   DUMMY_ADDR /* 255 INTB+03FCh                   */
};
