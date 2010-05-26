/*****************************************************************************
 * quant.c: xavs encoder library
 *****************************************************************************
 * Copyright (C) 2009 xavs project
 *
 * Authors:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

#include "common.h"
#include <stdio.h>
#include <string.h>

#ifdef HAVE_MMXEXT
#include "i386/quant.h"
#endif

/*Spec. 9.6.2 Table 23*/
static const uint16_t dequant_shifttable[64] = {
  14, 14, 14, 14, 14, 14, 14, 14,
  13, 13, 13, 13, 13, 13, 13, 13,
  13, 12, 12, 12, 12, 12, 12, 12,
  11, 11, 11, 11, 11, 11, 11, 11,
  11, 10, 10, 10, 10, 10, 10, 10,
  10, 9, 9, 9, 9, 9, 9, 9,
  9, 8, 8, 8, 8, 8, 8, 8,
  7, 7, 7, 7, 7, 7, 7, 7
};

static const int quant8_table[64] = {
  32768, 29775, 27554, 25268, 23170, 21247, 19369, 17770,
  16302, 15024, 13777, 12634, 11626, 10624, 9742, 8958,
  8192, 7512, 6889, 6305, 5793, 5303, 4878, 4467,
  4091, 3756, 3444, 3161, 2894, 2654, 2435, 2235,
  2048, 1878, 1722, 1579, 1449, 1329, 1218, 1117,
  1024, 939, 861, 790, 724, 664, 609, 558,
  512, 470, 430, 395, 362, 332, 304, 279,
  256, 235, 215, 197, 181, 166, 152, 140
};


/*
*************************************************************************
* Function: quantization 8x8 block 
* Input:    dct[8][8] 
*           mf  quantization matrix for 8x8 block
*           f   quantization deadzone for 8x8 block  
*           qp  quantization parameter for 8x8 block
* Output:   dct[8][8] quantizated data
* Return:   
* Attention: The data address of the input 8x8 block should be continuous
*  
*************************************************************************
*/
#define QUANT_ONE( coef, mf, qtable, f )\
{ \
    if( (coef) > 0 ) \
        (coef) = (f + ( ((coef) * (mf) + (1<<18)) >>19 ) * (qtable)) >> 15; \
    else \
        (coef) = - ((f + ( ( ((-coef) * (mf)  + (1<<18))>>19 ) * (qtable))) >> 15); \
    nz |= (coef);\
}

int
quant_8x8 (int16_t dct[8][8], int mf[64], uint16_t bias[64], int qp)
{
  int i, nz = 0;
  int qptable;
  qptable = quant8_table[qp];

  for (i = 0; i < 64; i++)
    QUANT_ONE (dct[0][i], mf[i], qptable, bias[i]);
  return !!nz;
}

/*
*************************************************************************
* Function: dequantization 8x8 block 
* Input:    dct[8][8] 
*           dequant_mf  dequantization matrix for 8x8 block 
*           iq   quantization parameter
* Output:   dct[8][8] dequantizated data
* Return: 
* Attention: 
*************************************************************************
*/
#define DEQUANT_SHR( x ) \
    dct[y][x] = ( dct[y][x] * dequant_mf[i_qp][y][x] + f ) >> (shift_bits);\
    dct[y][x] = ( dct[y][x] < (-32768))?(-32768):(dct[y][x]>32767)?32767:(dct[y][x]);

void
dequant_8x8 (int16_t dct[8][8], int dequant_mf[64][8][8], int i_qp)
{
  int y;
  const int shift_bits = dequant_shifttable[i_qp];
  const int f = 1 << (shift_bits - 1);
  for (y = 0; y < 8; y++)
  {
  DEQUANT_SHR (0) DEQUANT_SHR (1) DEQUANT_SHR (2) DEQUANT_SHR (3) DEQUANT_SHR (4) DEQUANT_SHR (5) DEQUANT_SHR (6) DEQUANT_SHR (7)}
}

void
xavs_quant_init (xavs_t * h, int cpu, xavs_quant_function_t * pf)
{
  pf->quant_8x8 = quant_8x8;
  pf->dequant_8x8 = dequant_8x8;

#ifdef HAVE_MMX
  if (cpu & XAVS_CPU_MMX)
  {
#ifdef ARCH_X86
    pf->quant_8x8 = xavs_quant_8x8_mmx;
    pf->dequant_8x8 = xavs_dequant_8x8_mmx;
    if (h->param.i_cqm_preset == XAVS_CQM_FLAT)
    {
      pf->dequant_8x8 = xavs_dequant_8x8_flat16_mmx;
    }
#endif
  }

  if (cpu & XAVS_CPU_SSE2)
  {
    pf->quant_8x8 = xavs_quant_8x8_sse2;
    pf->dequant_8x8 = xavs_dequant_8x8_sse2;
  }

  if (cpu & XAVS_CPU_SSSE3)
  {
    pf->quant_8x8 = xavs_quant_8x8_ssse3;
  }

  if (cpu & XAVS_CPU_SSE4)
  {
    pf->quant_8x8 = xavs_quant_8x8_sse4;
  }
#endif // HAVE_MMX

#ifdef ARCH_PPC
  if (cpu & XAVS_CPU_ALTIVEC)
  {
    pf->quant_8x8 = xavs_quant_8x8_altivec;
    pf->dequant_8x8 = xavs_dequant_8x8_altivec;
  }
#endif
}