/************************************************************************/
/* Header file generated from device file:                              */
/*    DR5F1026A.DVF                                                     */
/*    V1.12 (2012/04/03)                                                */
/*    Copyright(C) 2012 Renesas                                         */
/* Tool Version: 4.0.0                                                  */
/* Date Generated: 2020/01/17                                           */
/************************************************************************/

#ifndef __INTRINSIC_FUNCTIONS
#define __INTRINSIC_FUNCTIONS

#define DI() asm("di")
#define EI() asm("ei")
#define HALT() asm("halt")
#define NOP() asm("nop")
#define STOP() asm("stop")

#endif

#ifndef __IOREG_BIT_STRUCTURES
#define __IOREG_BIT_STRUCTURES
typedef struct
{
   unsigned char no0 : 1;
   unsigned char no1 : 1;
   unsigned char no2 : 1;
   unsigned char no3 : 1;
   unsigned char no4 : 1;
   unsigned char no5 : 1;
   unsigned char no6 : 1;
   unsigned char no7 : 1;
} __BITS8;

typedef struct
{
   unsigned short no0 : 1;
   unsigned short no1 : 1;
   unsigned short no2 : 1;
   unsigned short no3 : 1;
   unsigned short no4 : 1;
   unsigned short no5 : 1;
   unsigned short no6 : 1;
   unsigned short no7 : 1;
   unsigned short no8 : 1;
   unsigned short no9 : 1;
   unsigned short no10 : 1;
   unsigned short no11 : 1;
   unsigned short no12 : 1;
   unsigned short no13 : 1;
   unsigned short no14 : 1;
   unsigned short no15 : 1;
} __BITS16;

#endif

#ifndef IODEFINE_H
#define IODEFINE_H

/*
 IO Registers
 */
union un_p1
{
   unsigned char p1;
   __BITS8 BIT;
};
union un_p2
{
   unsigned char p2;
   __BITS8 BIT;
};
union un_p4
{
   unsigned char p4;
   __BITS8 BIT;
};
union un_p6
{
   unsigned char p6;
   __BITS8 BIT;
};
union un_p12
{
   unsigned char p12;
   __BITS8 BIT;
};
union un_p13
{
   unsigned char p13;
   __BITS8 BIT;
};
union un_pm1
{
   unsigned char pm1;
   __BITS8 BIT;
};
union un_pm2
{
   unsigned char pm2;
   __BITS8 BIT;
};
union un_pm4
{
   unsigned char pm4;
   __BITS8 BIT;
};
union un_pm6
{
   unsigned char pm6;
   __BITS8 BIT;
};
union un_adm0
{
   unsigned char adm0;
   __BITS8 BIT;
};
union un_ads
{
   unsigned char ads;
   __BITS8 BIT;
};
union un_adm1
{
   unsigned char adm1;
   __BITS8 BIT;
};
union un_krctl
{
   unsigned char krctl;
   __BITS8 BIT;
};
union un_krm0
{
   unsigned char krm0;
   __BITS8 BIT;
};
union un_egp0
{
   unsigned char egp0;
   __BITS8 BIT;
};
union un_egn0
{
   unsigned char egn0;
   __BITS8 BIT;
};
union un_iics0
{
   unsigned char iics0;
   __BITS8 BIT;
};
union un_iicf0
{
   unsigned char iicf0;
   __BITS8 BIT;
};
union un_csc
{
   unsigned char csc;
   __BITS8 BIT;
};
union un_ostc
{
   unsigned char ostc;
   __BITS8 BIT;
};
union un_ckc
{
   unsigned char ckc;
   __BITS8 BIT;
};
union un_cks0
{
   unsigned char cks0;
   __BITS8 BIT;
};
union un_lvim
{
   unsigned char lvim;
   __BITS8 BIT;
};
union un_lvis
{
   unsigned char lvis;
   __BITS8 BIT;
};
union un_dmc0
{
   unsigned char dmc0;
   __BITS8 BIT;
};
union un_dmc1
{
   unsigned char dmc1;
   __BITS8 BIT;
};
union un_drc0
{
   unsigned char drc0;
   __BITS8 BIT;
};
union un_drc1
{
   unsigned char drc1;
   __BITS8 BIT;
};
union un_if0
{
   unsigned short if0;
   __BITS16 BIT;
};
union un_if0l
{
   unsigned char if0l;
   __BITS8 BIT;
};
union un_if0h
{
   unsigned char if0h;
   __BITS8 BIT;
};
union un_if1
{
   unsigned short if1;
   __BITS16 BIT;
};
union un_if1l
{
   unsigned char if1l;
   __BITS8 BIT;
};
union un_mk0
{
   unsigned short mk0;
   __BITS16 BIT;
};
union un_mk0l
{
   unsigned char mk0l;
   __BITS8 BIT;
};
union un_mk0h
{
   unsigned char mk0h;
   __BITS8 BIT;
};
union un_mk1
{
   unsigned short mk1;
   __BITS16 BIT;
};
union un_mk1l
{
   unsigned char mk1l;
   __BITS8 BIT;
};
union un_pr00
{
   unsigned short pr00;
   __BITS16 BIT;
};
union un_pr00l
{
   unsigned char pr00l;
   __BITS8 BIT;
};
union un_pr00h
{
   unsigned char pr00h;
   __BITS8 BIT;
};
union un_pr01
{
   unsigned short pr01;
   __BITS16 BIT;
};
union un_pr01l
{
   unsigned char pr01l;
   __BITS8 BIT;
};
union un_pr10
{
   unsigned short pr10;
   __BITS16 BIT;
};
union un_pr10l
{
   unsigned char pr10l;
   __BITS8 BIT;
};
union un_pr10h
{
   unsigned char pr10h;
   __BITS8 BIT;
};
union un_pr11
{
   unsigned short pr11;
   __BITS16 BIT;
};
union un_pr11l
{
   unsigned char pr11l;
   __BITS8 BIT;
};
union un_pmc
{
   unsigned char pmc;
   __BITS8 BIT;
};

#define P1 (*(volatile union un_p1 *)0xFFF01).p1
#define P1_bit (*(volatile union un_p1 *)0xFFF01).BIT
#define P2 (*(volatile union un_p2 *)0xFFF02).p2
#define P2_bit (*(volatile union un_p2 *)0xFFF02).BIT
#define P4 (*(volatile union un_p4 *)0xFFF04).p4
#define P4_bit (*(volatile union un_p4 *)0xFFF04).BIT
#define P6 (*(volatile union un_p6 *)0xFFF06).p6
#define P6_bit (*(volatile union un_p6 *)0xFFF06).BIT
#define P12 (*(volatile union un_p12 *)0xFFF0C).p12
#define P12_bit (*(volatile union un_p12 *)0xFFF0C).BIT
#define P13 (*(volatile union un_p13 *)0xFFF0D).p13
#define P13_bit (*(volatile union un_p13 *)0xFFF0D).BIT
#define SDR00 (*(volatile unsigned short *)0xFFF10)
#define SIO00 (*(volatile unsigned char *)0xFFF10)
#define TXD0 (*(volatile unsigned char *)0xFFF10)
#define SDR01 (*(volatile unsigned short *)0xFFF12)
#define RXD0 (*(volatile unsigned char *)0xFFF12)
#define SIO01 (*(volatile unsigned char *)0xFFF12)
#define TDR00 (*(volatile unsigned short *)0xFFF18)
#define TDR01 (*(volatile unsigned short *)0xFFF1A)
#define TDR01L (*(volatile unsigned char *)0xFFF1A)
#define TDR01H (*(volatile unsigned char *)0xFFF1B)
#define ADCR (*(volatile unsigned short *)0xFFF1E)
#define ADCRH (*(volatile unsigned char *)0xFFF1F)
#define PM1 (*(volatile union un_pm1 *)0xFFF21).pm1
#define PM1_bit (*(volatile union un_pm1 *)0xFFF21).BIT
#define PM2 (*(volatile union un_pm2 *)0xFFF22).pm2
#define PM2_bit (*(volatile union un_pm2 *)0xFFF22).BIT
#define PM4 (*(volatile union un_pm4 *)0xFFF24).pm4
#define PM4_bit (*(volatile union un_pm4 *)0xFFF24).BIT
#define PM6 (*(volatile union un_pm6 *)0xFFF26).pm6
#define PM6_bit (*(volatile union un_pm6 *)0xFFF26).BIT
#define ADM0 (*(volatile union un_adm0 *)0xFFF30).adm0
#define ADM0_bit (*(volatile union un_adm0 *)0xFFF30).BIT
#define ADS (*(volatile union un_ads *)0xFFF31).ads
#define ADS_bit (*(volatile union un_ads *)0xFFF31).BIT
#define ADM1 (*(volatile union un_adm1 *)0xFFF32).adm1
#define ADM1_bit (*(volatile union un_adm1 *)0xFFF32).BIT
#define KRCTL (*(volatile union un_krctl *)0xFFF34).krctl
#define KRCTL_bit (*(volatile union un_krctl *)0xFFF34).BIT
#define KRF (*(volatile unsigned char *)0xFFF35)
#define KRM0 (*(volatile union un_krm0 *)0xFFF37).krm0
#define KRM0_bit (*(volatile union un_krm0 *)0xFFF37).BIT
#define EGP0 (*(volatile union un_egp0 *)0xFFF38).egp0
#define EGP0_bit (*(volatile union un_egp0 *)0xFFF38).BIT
#define EGN0 (*(volatile union un_egn0 *)0xFFF39).egn0
#define EGN0_bit (*(volatile union un_egn0 *)0xFFF39).BIT
#define IICA0 (*(volatile unsigned char *)0xFFF50)
#define IICS0 (*(volatile union un_iics0 *)0xFFF51).iics0
#define IICS0_bit (*(volatile union un_iics0 *)0xFFF51).BIT
#define IICF0 (*(volatile union un_iicf0 *)0xFFF52).iicf0
#define IICF0_bit (*(volatile union un_iicf0 *)0xFFF52).BIT
#define TDR02 (*(volatile unsigned short *)0xFFF64)
#define TDR03 (*(volatile unsigned short *)0xFFF66)
#define TDR03L (*(volatile unsigned char *)0xFFF66)
#define TDR03H (*(volatile unsigned char *)0xFFF67)
#define ITMC (*(volatile unsigned short *)0xFFF90)
#define CMC (*(volatile unsigned char *)0xFFFA0)
#define CSC (*(volatile union un_csc *)0xFFFA1).csc
#define CSC_bit (*(volatile union un_csc *)0xFFFA1).BIT
#define OSTC (*(volatile union un_ostc *)0xFFFA2).ostc
#define OSTC_bit (*(volatile union un_ostc *)0xFFFA2).BIT
#define OSTS (*(volatile unsigned char *)0xFFFA3)
#define CKC (*(volatile union un_ckc *)0xFFFA4).ckc
#define CKC_bit (*(volatile union un_ckc *)0xFFFA4).BIT
#define CKS0 (*(volatile union un_cks0 *)0xFFFA5).cks0
#define CKS0_bit (*(volatile union un_cks0 *)0xFFFA5).BIT
#define RESF (*(volatile unsigned char *)0xFFFA8)
#define LVIM (*(volatile union un_lvim *)0xFFFA9).lvim
#define LVIM_bit (*(volatile union un_lvim *)0xFFFA9).BIT
#define LVIS (*(volatile union un_lvis *)0xFFFAA).lvis
#define LVIS_bit (*(volatile union un_lvis *)0xFFFAA).BIT
#define WDTE (*(volatile unsigned char *)0xFFFAB)
#define CRCIN (*(volatile unsigned char *)0xFFFAC)
#define DSA0 (*(volatile unsigned char *)0xFFFB0)
#define DSA1 (*(volatile unsigned char *)0xFFFB1)
#define DRA0 (*(volatile unsigned short *)0xFFFB2)
#define DRA0L (*(volatile unsigned char *)0xFFFB2)
#define DRA0H (*(volatile unsigned char *)0xFFFB3)
#define DRA1 (*(volatile unsigned short *)0xFFFB4)
#define DRA1L (*(volatile unsigned char *)0xFFFB4)
#define DRA1H (*(volatile unsigned char *)0xFFFB5)
#define DBC0 (*(volatile unsigned short *)0xFFFB6)
#define DBC0L (*(volatile unsigned char *)0xFFFB6)
#define DBC0H (*(volatile unsigned char *)0xFFFB7)
#define DBC1 (*(volatile unsigned short *)0xFFFB8)
#define DBC1L (*(volatile unsigned char *)0xFFFB8)
#define DBC1H (*(volatile unsigned char *)0xFFFB9)
#define DMC0 (*(volatile union un_dmc0 *)0xFFFBA).dmc0
#define DMC0_bit (*(volatile union un_dmc0 *)0xFFFBA).BIT
#define DMC1 (*(volatile union un_dmc1 *)0xFFFBB).dmc1
#define DMC1_bit (*(volatile union un_dmc1 *)0xFFFBB).BIT
#define DRC0 (*(volatile union un_drc0 *)0xFFFBC).drc0
#define DRC0_bit (*(volatile union un_drc0 *)0xFFFBC).BIT
#define DRC1 (*(volatile union un_drc1 *)0xFFFBD).drc1
#define DRC1_bit (*(volatile union un_drc1 *)0xFFFBD).BIT
#define IF0 (*(volatile union un_if0 *)0xFFFE0).if0
#define IF0_bit (*(volatile union un_if0 *)0xFFFE0).BIT
#define IF0L (*(volatile union un_if0l *)0xFFFE0).if0l
#define IF0L_bit (*(volatile union un_if0l *)0xFFFE0).BIT
#define IF0H (*(volatile union un_if0h *)0xFFFE1).if0h
#define IF0H_bit (*(volatile union un_if0h *)0xFFFE1).BIT
#define IF1 (*(volatile union un_if1 *)0xFFFE2).if1
#define IF1_bit (*(volatile union un_if1 *)0xFFFE2).BIT
#define IF1L (*(volatile union un_if1l *)0xFFFE2).if1l
#define IF1L_bit (*(volatile union un_if1l *)0xFFFE2).BIT
#define MK0 (*(volatile union un_mk0 *)0xFFFE4).mk0
#define MK0_bit (*(volatile union un_mk0 *)0xFFFE4).BIT
#define MK0L (*(volatile union un_mk0l *)0xFFFE4).mk0l
#define MK0L_bit (*(volatile union un_mk0l *)0xFFFE4).BIT
#define MK0H (*(volatile union un_mk0h *)0xFFFE5).mk0h
#define MK0H_bit (*(volatile union un_mk0h *)0xFFFE5).BIT
#define MK1 (*(volatile union un_mk1 *)0xFFFE6).mk1
#define MK1_bit (*(volatile union un_mk1 *)0xFFFE6).BIT
#define MK1L (*(volatile union un_mk1l *)0xFFFE6).mk1l
#define MK1L_bit (*(volatile union un_mk1l *)0xFFFE6).BIT
#define PR00 (*(volatile union un_pr00 *)0xFFFE8).pr00
#define PR00_bit (*(volatile union un_pr00 *)0xFFFE8).BIT
#define PR00L (*(volatile union un_pr00l *)0xFFFE8).pr00l
#define PR00L_bit (*(volatile union un_pr00l *)0xFFFE8).BIT
#define PR00H (*(volatile union un_pr00h *)0xFFFE9).pr00h
#define PR00H_bit (*(volatile union un_pr00h *)0xFFFE9).BIT
#define PR01 (*(volatile union un_pr01 *)0xFFFEA).pr01
#define PR01_bit (*(volatile union un_pr01 *)0xFFFEA).BIT
#define PR01L (*(volatile union un_pr01l *)0xFFFEA).pr01l
#define PR01L_bit (*(volatile union un_pr01l *)0xFFFEA).BIT
#define PR10 (*(volatile union un_pr10 *)0xFFFEC).pr10
#define PR10_bit (*(volatile union un_pr10 *)0xFFFEC).BIT
#define PR10L (*(volatile union un_pr10l *)0xFFFEC).pr10l
#define PR10L_bit (*(volatile union un_pr10l *)0xFFFEC).BIT
#define PR10H (*(volatile union un_pr10h *)0xFFFED).pr10h
#define PR10H_bit (*(volatile union un_pr10h *)0xFFFED).BIT
#define PR11 (*(volatile union un_pr11 *)0xFFFEE).pr11
#define PR11_bit (*(volatile union un_pr11 *)0xFFFEE).BIT
#define PR11L (*(volatile union un_pr11l *)0xFFFEE).pr11l
#define PR11L_bit (*(volatile union un_pr11l *)0xFFFEE).BIT
#define MDAL (*(volatile unsigned short *)0xFFFF0)
#define MULA (*(volatile unsigned short *)0xFFFF0)
#define MDAH (*(volatile unsigned short *)0xFFFF2)
#define MULB (*(volatile unsigned short *)0xFFFF2)
#define MDBH (*(volatile unsigned short *)0xFFFF4)
#define MULOH (*(volatile unsigned short *)0xFFFF4)
#define MDBL (*(volatile unsigned short *)0xFFFF6)
#define MULOL (*(volatile unsigned short *)0xFFFF6)
#define PMC (*(volatile union un_pmc *)0xFFFFE).pmc
#define PMC_bit (*(volatile union un_pmc *)0xFFFFE).BIT

/*
 Sfr bits
 */
#define ADCE ADM0_bit.no0
#define ADCS ADM0_bit.no7
#define SPD0 IICS0_bit.no0
#define STD0 IICS0_bit.no1
#define ACKD0 IICS0_bit.no2
#define TRC0 IICS0_bit.no3
#define COI0 IICS0_bit.no4
#define EXC0 IICS0_bit.no5
#define ALD0 IICS0_bit.no6
#define MSTS0 IICS0_bit.no7
#define IICRSV0 IICF0_bit.no0
#define STCEN0 IICF0_bit.no1
#define IICBSY0 IICF0_bit.no6
#define STCF0 IICF0_bit.no7
#define HIOSTOP CSC_bit.no0
#define MSTOP CSC_bit.no7
#define MCM0 CKC_bit.no4
#define MCS CKC_bit.no5
#define PCLOE0 CKS0_bit.no7
#define LVIF LVIM_bit.no0
#define LVIOMSK LVIM_bit.no1
#define LVISEN LVIM_bit.no7
#define LVILV LVIS_bit.no0
#define LVIMD LVIS_bit.no7
#define DWAIT0 DMC0_bit.no4
#define DS0 DMC0_bit.no5
#define DRS0 DMC0_bit.no6
#define STG0 DMC0_bit.no7
#define DWAIT1 DMC1_bit.no4
#define DS1 DMC1_bit.no5
#define DRS1 DMC1_bit.no6
#define STG1 DMC1_bit.no7
#define DST0 DRC0_bit.no0
#define DEN0 DRC0_bit.no7
#define DST1 DRC1_bit.no0
#define DEN1 DRC1_bit.no7
#define WDTIIF IF0_bit.no0
#define LVIIF IF0_bit.no1
#define PIF0 IF0_bit.no2
#define PIF1 IF0_bit.no3
#define PIF2 IF0_bit.no4
#define PIF3 IF0_bit.no5
#define DMAIF0 IF0_bit.no6
#define DMAIF1 IF0_bit.no7
#define CSIIF00 IF0H_bit.no0
#define IICIF00 IF0H_bit.no0
#define STIF0 IF0H_bit.no0
#define CSIIF01 IF0H_bit.no1
#define IICIF01 IF0H_bit.no1
#define SRIF0 IF0H_bit.no1
#define SREIF0 IF0H_bit.no2
#define TMIF01H IF0H_bit.no3
#define TMIF03H IF0H_bit.no4
#define IICAIF0 IF0H_bit.no5
#define TMIF00 IF0H_bit.no6
#define TMIF01 IF0H_bit.no7
#define TMIF02 IF1_bit.no0
#define TMIF03 IF1_bit.no1
#define ADIF IF1_bit.no2
#define TMKAIF IF1_bit.no3
#define KRIF IF1_bit.no4
#define MDIF IF1_bit.no5
#define FLIF IF1_bit.no6
#define WDTIMK MK0_bit.no0
#define LVIMK MK0_bit.no1
#define PMK0 MK0_bit.no2
#define PMK1 MK0_bit.no3
#define PMK2 MK0_bit.no4
#define PMK3 MK0_bit.no5
#define DMAMK0 MK0_bit.no6
#define DMAMK1 MK0_bit.no7
#define CSIMK00 MK0H_bit.no0
#define IICMK00 MK0H_bit.no0
#define STMK0 MK0H_bit.no0
#define CSIMK01 MK0H_bit.no1
#define IICMK01 MK0H_bit.no1
#define SRMK0 MK0H_bit.no1
#define SREMK0 MK0H_bit.no2
#define TMMK01H MK0H_bit.no3
#define TMMK03H MK0H_bit.no4
#define IICAMK0 MK0H_bit.no5
#define TMMK00 MK0H_bit.no6
#define TMMK01 MK0H_bit.no7
#define TMMK02 MK1_bit.no0
#define TMMK03 MK1_bit.no1
#define ADMK MK1_bit.no2
#define TMKAMK MK1_bit.no3
#define KRMK MK1_bit.no4
#define MDMK MK1_bit.no5
#define FLMK MK1_bit.no6
#define WDTIPR0 PR00_bit.no0
#define LVIPR0 PR00_bit.no1
#define PPR00 PR00_bit.no2
#define PPR01 PR00_bit.no3
#define PPR02 PR00_bit.no4
#define PPR03 PR00_bit.no5
#define DMAPR00 PR00_bit.no6
#define DMAPR01 PR00_bit.no7
#define CSIPR000 PR00H_bit.no0
#define IICPR000 PR00H_bit.no0
#define STPR00 PR00H_bit.no0
#define CSIPR001 PR00H_bit.no1
#define IICPR001 PR00H_bit.no1
#define SRPR00 PR00H_bit.no1
#define SREPR00 PR00H_bit.no2
#define TMPR001H PR00H_bit.no3
#define TMPR003H PR00H_bit.no4
#define IICAPR00 PR00H_bit.no5
#define TMPR000 PR00H_bit.no6
#define TMPR001 PR00H_bit.no7
#define TMPR002 PR01_bit.no0
#define TMPR003 PR01_bit.no1
#define ADPR0 PR01_bit.no2
#define TMKAPR0 PR01_bit.no3
#define KRPR0 PR01_bit.no4
#define MDPR0 PR01_bit.no5
#define FLPR0 PR01_bit.no6
#define WDTIPR1 PR10_bit.no0
#define LVIPR1 PR10_bit.no1
#define PPR10 PR10_bit.no2
#define PPR11 PR10_bit.no3
#define PPR12 PR10_bit.no4
#define PPR13 PR10_bit.no5
#define DMAPR10 PR10_bit.no6
#define DMAPR11 PR10_bit.no7
#define CSIPR100 PR10H_bit.no0
#define IICPR100 PR10H_bit.no0
#define STPR10 PR10H_bit.no0
#define CSIPR101 PR10H_bit.no1
#define IICPR101 PR10H_bit.no1
#define SRPR10 PR10H_bit.no1
#define SREPR10 PR10H_bit.no2
#define TMPR101H PR10H_bit.no3
#define TMPR103H PR10H_bit.no4
#define IICAPR10 PR10H_bit.no5
#define TMPR100 PR10H_bit.no6
#define TMPR101 PR10H_bit.no7
#define TMPR102 PR11_bit.no0
#define TMPR103 PR11_bit.no1
#define ADPR1 PR11_bit.no2
#define TMKAPR1 PR11_bit.no3
#define KRPR1 PR11_bit.no4
#define MDPR1 PR11_bit.no5
#define FLPR1 PR11_bit.no6
#define MAA PMC_bit.no0

/*
 Interrupt vector addresses
 */
#define RST_vect (0x0)
#define INTDBG_vect (0x2)
#define INTWDTI_vect (0x4)
#define INTLVI_vect (0x6)
#define INTP0_vect (0x8)
#define INTP1_vect (0xA)
#define INTP2_vect (0xC)
#define INTP3_vect (0xE)
#define INTDMA0_vect (0x10)
#define INTDMA1_vect (0x12)
#define INTCSI00_vect (0x14)
#define INTIIC00_vect (0x14)
#define INTST0_vect (0x14)
#define INTCSI01_vect (0x16)
#define INTIIC01_vect (0x16)
#define INTSR0_vect (0x16)
#define INTSRE0_vect (0x18)
#define INTTM01H_vect (0x1A)
#define INTTM03H_vect (0x1C)
#define INTIICA0_vect (0x1E)
#define INTTM00_vect (0x20)
#define INTTM01_vect (0x22)
#define INTTM02_vect (0x24)
#define INTTM03_vect (0x26)
#define INTAD_vect (0x28)
#define INTIT_vect (0x2A)
#define INTKR_vect (0x2C)
#define INTMD_vect (0x2E)
#define INTFL_vect (0x30)
#define BRK_I_vect (0x7E)
#endif
