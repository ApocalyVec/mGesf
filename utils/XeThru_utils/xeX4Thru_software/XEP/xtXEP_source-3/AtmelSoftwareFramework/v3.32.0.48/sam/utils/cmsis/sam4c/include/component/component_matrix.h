/**
 * \file
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef _SAM4C_MATRIX_COMPONENT_
#define _SAM4C_MATRIX_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR AHB Bus Matrix */
/* ============================================================================= */
/** \addtogroup SAM4C_MATRIX AHB Bus Matrix */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Matrix hardware registers */
typedef struct {
  __IO uint32_t MATRIX_MCFG[8];    /**< \brief (Matrix Offset: 0x0000) Master Configuration Register */
  __I  uint32_t Reserved1[8];
  __IO uint32_t MATRIX_SCFG[9];    /**< \brief (Matrix Offset: 0x0040) Slave Configuration Register */
  __I  uint32_t Reserved2[7];
  __IO uint32_t MATRIX_PRAS0;      /**< \brief (Matrix Offset: 0x0080) Priority Register A for Slave 0 */
  __I  uint32_t Reserved3[1];
  __IO uint32_t MATRIX_PRAS1;      /**< \brief (Matrix Offset: 0x0088) Priority Register A for Slave 1 */
  __I  uint32_t Reserved4[1];
  __IO uint32_t MATRIX_PRAS2;      /**< \brief (Matrix Offset: 0x0090) Priority Register A for Slave 2 */
  __I  uint32_t Reserved5[1];
  __IO uint32_t MATRIX_PRAS3;      /**< \brief (Matrix Offset: 0x0098) Priority Register A for Slave 3 */
  __I  uint32_t Reserved6[1];
  __IO uint32_t MATRIX_PRAS4;      /**< \brief (Matrix Offset: 0x00A0) Priority Register A for Slave 4 */
  __I  uint32_t Reserved7[1];
  __IO uint32_t MATRIX_PRAS5;      /**< \brief (Matrix Offset: 0x00A8) Priority Register A for Slave 5 */
  __I  uint32_t Reserved8[1];
  __IO uint32_t MATRIX_PRAS6;      /**< \brief (Matrix Offset: 0x00B0) Priority Register A for Slave 6 */
  __I  uint32_t Reserved9[1];
  __IO uint32_t MATRIX_PRAS7;      /**< \brief (Matrix Offset: 0x00B8) Priority Register A for Slave 7 */
  __IO uint32_t MATRIX_PRAS8;      /**< \brief (Matrix Offset: 0x00BC) Priority Register A for Slave 8 */
  __I  uint32_t Reserved10[21];
  __IO uint32_t MATRIX_SYSIO;      /**< \brief (Matrix Offset: 0x0114) System I/O Configuration Register */
  __I  uint32_t Reserved11[1];
  __IO uint32_t MATRIX_SMCNFCS;    /**< \brief (Matrix Offset: 0x011C) SMC Nand Flash Chip Select Configuration Register */
  __I  uint32_t Reserved12[2];
  __IO uint32_t MATRIX_CORE_DEBUG; /**< \brief (Matrix Offset: 0x0128) Core Debug Configuration Register */
  __I  uint32_t Reserved13[46];
  __IO uint32_t MATRIX_WPMR;       /**< \brief (Matrix Offset: 0x01E4) Write Protection Mode Register */
  __I  uint32_t MATRIX_WPSR;       /**< \brief (Matrix Offset: 0x01E8) Write Protection Status Register */
} Matrix;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- MATRIX_MCFG[8] : (MATRIX Offset: 0x0000) Master Configuration Register -------- */
#define MATRIX_MCFG_ULBT_Pos 0
#define MATRIX_MCFG_ULBT_Msk (0x7u << MATRIX_MCFG_ULBT_Pos) /**< \brief (MATRIX_MCFG[8]) Undefined Length Burst Type */
#define MATRIX_MCFG_ULBT(value) ((MATRIX_MCFG_ULBT_Msk & ((value) << MATRIX_MCFG_ULBT_Pos)))
/* -------- MATRIX_SCFG[9] : (MATRIX Offset: 0x0040) Slave Configuration Register -------- */
#define MATRIX_SCFG_SLOT_CYCLE_Pos 0
#define MATRIX_SCFG_SLOT_CYCLE_Msk (0x1ffu << MATRIX_SCFG_SLOT_CYCLE_Pos) /**< \brief (MATRIX_SCFG[9]) Maximum Bus Grant Duration for Masters */
#define MATRIX_SCFG_SLOT_CYCLE(value) ((MATRIX_SCFG_SLOT_CYCLE_Msk & ((value) << MATRIX_SCFG_SLOT_CYCLE_Pos)))
#define MATRIX_SCFG_DEFMSTR_TYPE_Pos 16
#define MATRIX_SCFG_DEFMSTR_TYPE_Msk (0x3u << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< \brief (MATRIX_SCFG[9]) Default Master Type */
#define MATRIX_SCFG_DEFMSTR_TYPE(value) ((MATRIX_SCFG_DEFMSTR_TYPE_Msk & ((value) << MATRIX_SCFG_DEFMSTR_TYPE_Pos)))
#define MATRIX_SCFG_FIXED_DEFMSTR_Pos 18
#define MATRIX_SCFG_FIXED_DEFMSTR_Msk (0xfu << MATRIX_SCFG_FIXED_DEFMSTR_Pos) /**< \brief (MATRIX_SCFG[9]) Fixed Default Master */
#define MATRIX_SCFG_FIXED_DEFMSTR(value) ((MATRIX_SCFG_FIXED_DEFMSTR_Msk & ((value) << MATRIX_SCFG_FIXED_DEFMSTR_Pos)))
/* -------- MATRIX_PRAS0 : (MATRIX Offset: 0x0080) Priority Register A for Slave 0 -------- */
#define MATRIX_PRAS0_M0PR_Pos 0
#define MATRIX_PRAS0_M0PR_Msk (0x3u << MATRIX_PRAS0_M0PR_Pos) /**< \brief (MATRIX_PRAS0) Master 0 Priority */
#define MATRIX_PRAS0_M0PR(value) ((MATRIX_PRAS0_M0PR_Msk & ((value) << MATRIX_PRAS0_M0PR_Pos)))
#define MATRIX_PRAS0_M1PR_Pos 4
#define MATRIX_PRAS0_M1PR_Msk (0x3u << MATRIX_PRAS0_M1PR_Pos) /**< \brief (MATRIX_PRAS0) Master 1 Priority */
#define MATRIX_PRAS0_M1PR(value) ((MATRIX_PRAS0_M1PR_Msk & ((value) << MATRIX_PRAS0_M1PR_Pos)))
#define MATRIX_PRAS0_M2PR_Pos 8
#define MATRIX_PRAS0_M2PR_Msk (0x3u << MATRIX_PRAS0_M2PR_Pos) /**< \brief (MATRIX_PRAS0) Master 2 Priority */
#define MATRIX_PRAS0_M2PR(value) ((MATRIX_PRAS0_M2PR_Msk & ((value) << MATRIX_PRAS0_M2PR_Pos)))
#define MATRIX_PRAS0_M3PR_Pos 12
#define MATRIX_PRAS0_M3PR_Msk (0x3u << MATRIX_PRAS0_M3PR_Pos) /**< \brief (MATRIX_PRAS0) Master 3 Priority */
#define MATRIX_PRAS0_M3PR(value) ((MATRIX_PRAS0_M3PR_Msk & ((value) << MATRIX_PRAS0_M3PR_Pos)))
#define MATRIX_PRAS0_M4PR_Pos 16
#define MATRIX_PRAS0_M4PR_Msk (0x3u << MATRIX_PRAS0_M4PR_Pos) /**< \brief (MATRIX_PRAS0) Master 4 Priority */
#define MATRIX_PRAS0_M4PR(value) ((MATRIX_PRAS0_M4PR_Msk & ((value) << MATRIX_PRAS0_M4PR_Pos)))
#define MATRIX_PRAS0_M5PR_Pos 20
#define MATRIX_PRAS0_M5PR_Msk (0x3u << MATRIX_PRAS0_M5PR_Pos) /**< \brief (MATRIX_PRAS0) Master 5 Priority */
#define MATRIX_PRAS0_M5PR(value) ((MATRIX_PRAS0_M5PR_Msk & ((value) << MATRIX_PRAS0_M5PR_Pos)))
#define MATRIX_PRAS0_M6PR_Pos 24
#define MATRIX_PRAS0_M6PR_Msk (0x3u << MATRIX_PRAS0_M6PR_Pos) /**< \brief (MATRIX_PRAS0) Master 6 Priority */
#define MATRIX_PRAS0_M6PR(value) ((MATRIX_PRAS0_M6PR_Msk & ((value) << MATRIX_PRAS0_M6PR_Pos)))
#define MATRIX_PRAS0_M7PR_Pos 28
#define MATRIX_PRAS0_M7PR_Msk (0x3u << MATRIX_PRAS0_M7PR_Pos) /**< \brief (MATRIX_PRAS0) Master 7 Priority */
#define MATRIX_PRAS0_M7PR(value) ((MATRIX_PRAS0_M7PR_Msk & ((value) << MATRIX_PRAS0_M7PR_Pos)))
/* -------- MATRIX_PRAS1 : (MATRIX Offset: 0x0088) Priority Register A for Slave 1 -------- */
#define MATRIX_PRAS1_M0PR_Pos 0
#define MATRIX_PRAS1_M0PR_Msk (0x3u << MATRIX_PRAS1_M0PR_Pos) /**< \brief (MATRIX_PRAS1) Master 0 Priority */
#define MATRIX_PRAS1_M0PR(value) ((MATRIX_PRAS1_M0PR_Msk & ((value) << MATRIX_PRAS1_M0PR_Pos)))
#define MATRIX_PRAS1_M1PR_Pos 4
#define MATRIX_PRAS1_M1PR_Msk (0x3u << MATRIX_PRAS1_M1PR_Pos) /**< \brief (MATRIX_PRAS1) Master 1 Priority */
#define MATRIX_PRAS1_M1PR(value) ((MATRIX_PRAS1_M1PR_Msk & ((value) << MATRIX_PRAS1_M1PR_Pos)))
#define MATRIX_PRAS1_M2PR_Pos 8
#define MATRIX_PRAS1_M2PR_Msk (0x3u << MATRIX_PRAS1_M2PR_Pos) /**< \brief (MATRIX_PRAS1) Master 2 Priority */
#define MATRIX_PRAS1_M2PR(value) ((MATRIX_PRAS1_M2PR_Msk & ((value) << MATRIX_PRAS1_M2PR_Pos)))
#define MATRIX_PRAS1_M3PR_Pos 12
#define MATRIX_PRAS1_M3PR_Msk (0x3u << MATRIX_PRAS1_M3PR_Pos) /**< \brief (MATRIX_PRAS1) Master 3 Priority */
#define MATRIX_PRAS1_M3PR(value) ((MATRIX_PRAS1_M3PR_Msk & ((value) << MATRIX_PRAS1_M3PR_Pos)))
#define MATRIX_PRAS1_M4PR_Pos 16
#define MATRIX_PRAS1_M4PR_Msk (0x3u << MATRIX_PRAS1_M4PR_Pos) /**< \brief (MATRIX_PRAS1) Master 4 Priority */
#define MATRIX_PRAS1_M4PR(value) ((MATRIX_PRAS1_M4PR_Msk & ((value) << MATRIX_PRAS1_M4PR_Pos)))
#define MATRIX_PRAS1_M5PR_Pos 20
#define MATRIX_PRAS1_M5PR_Msk (0x3u << MATRIX_PRAS1_M5PR_Pos) /**< \brief (MATRIX_PRAS1) Master 5 Priority */
#define MATRIX_PRAS1_M5PR(value) ((MATRIX_PRAS1_M5PR_Msk & ((value) << MATRIX_PRAS1_M5PR_Pos)))
#define MATRIX_PRAS1_M6PR_Pos 24
#define MATRIX_PRAS1_M6PR_Msk (0x3u << MATRIX_PRAS1_M6PR_Pos) /**< \brief (MATRIX_PRAS1) Master 6 Priority */
#define MATRIX_PRAS1_M6PR(value) ((MATRIX_PRAS1_M6PR_Msk & ((value) << MATRIX_PRAS1_M6PR_Pos)))
#define MATRIX_PRAS1_M7PR_Pos 28
#define MATRIX_PRAS1_M7PR_Msk (0x3u << MATRIX_PRAS1_M7PR_Pos) /**< \brief (MATRIX_PRAS1) Master 7 Priority */
#define MATRIX_PRAS1_M7PR(value) ((MATRIX_PRAS1_M7PR_Msk & ((value) << MATRIX_PRAS1_M7PR_Pos)))
/* -------- MATRIX_PRAS2 : (MATRIX Offset: 0x0090) Priority Register A for Slave 2 -------- */
#define MATRIX_PRAS2_M0PR_Pos 0
#define MATRIX_PRAS2_M0PR_Msk (0x3u << MATRIX_PRAS2_M0PR_Pos) /**< \brief (MATRIX_PRAS2) Master 0 Priority */
#define MATRIX_PRAS2_M0PR(value) ((MATRIX_PRAS2_M0PR_Msk & ((value) << MATRIX_PRAS2_M0PR_Pos)))
#define MATRIX_PRAS2_M1PR_Pos 4
#define MATRIX_PRAS2_M1PR_Msk (0x3u << MATRIX_PRAS2_M1PR_Pos) /**< \brief (MATRIX_PRAS2) Master 1 Priority */
#define MATRIX_PRAS2_M1PR(value) ((MATRIX_PRAS2_M1PR_Msk & ((value) << MATRIX_PRAS2_M1PR_Pos)))
#define MATRIX_PRAS2_M2PR_Pos 8
#define MATRIX_PRAS2_M2PR_Msk (0x3u << MATRIX_PRAS2_M2PR_Pos) /**< \brief (MATRIX_PRAS2) Master 2 Priority */
#define MATRIX_PRAS2_M2PR(value) ((MATRIX_PRAS2_M2PR_Msk & ((value) << MATRIX_PRAS2_M2PR_Pos)))
#define MATRIX_PRAS2_M3PR_Pos 12
#define MATRIX_PRAS2_M3PR_Msk (0x3u << MATRIX_PRAS2_M3PR_Pos) /**< \brief (MATRIX_PRAS2) Master 3 Priority */
#define MATRIX_PRAS2_M3PR(value) ((MATRIX_PRAS2_M3PR_Msk & ((value) << MATRIX_PRAS2_M3PR_Pos)))
#define MATRIX_PRAS2_M4PR_Pos 16
#define MATRIX_PRAS2_M4PR_Msk (0x3u << MATRIX_PRAS2_M4PR_Pos) /**< \brief (MATRIX_PRAS2) Master 4 Priority */
#define MATRIX_PRAS2_M4PR(value) ((MATRIX_PRAS2_M4PR_Msk & ((value) << MATRIX_PRAS2_M4PR_Pos)))
#define MATRIX_PRAS2_M5PR_Pos 20
#define MATRIX_PRAS2_M5PR_Msk (0x3u << MATRIX_PRAS2_M5PR_Pos) /**< \brief (MATRIX_PRAS2) Master 5 Priority */
#define MATRIX_PRAS2_M5PR(value) ((MATRIX_PRAS2_M5PR_Msk & ((value) << MATRIX_PRAS2_M5PR_Pos)))
#define MATRIX_PRAS2_M6PR_Pos 24
#define MATRIX_PRAS2_M6PR_Msk (0x3u << MATRIX_PRAS2_M6PR_Pos) /**< \brief (MATRIX_PRAS2) Master 6 Priority */
#define MATRIX_PRAS2_M6PR(value) ((MATRIX_PRAS2_M6PR_Msk & ((value) << MATRIX_PRAS2_M6PR_Pos)))
#define MATRIX_PRAS2_M7PR_Pos 28
#define MATRIX_PRAS2_M7PR_Msk (0x3u << MATRIX_PRAS2_M7PR_Pos) /**< \brief (MATRIX_PRAS2) Master 7 Priority */
#define MATRIX_PRAS2_M7PR(value) ((MATRIX_PRAS2_M7PR_Msk & ((value) << MATRIX_PRAS2_M7PR_Pos)))
/* -------- MATRIX_PRAS3 : (MATRIX Offset: 0x0098) Priority Register A for Slave 3 -------- */
#define MATRIX_PRAS3_M0PR_Pos 0
#define MATRIX_PRAS3_M0PR_Msk (0x3u << MATRIX_PRAS3_M0PR_Pos) /**< \brief (MATRIX_PRAS3) Master 0 Priority */
#define MATRIX_PRAS3_M0PR(value) ((MATRIX_PRAS3_M0PR_Msk & ((value) << MATRIX_PRAS3_M0PR_Pos)))
#define MATRIX_PRAS3_M1PR_Pos 4
#define MATRIX_PRAS3_M1PR_Msk (0x3u << MATRIX_PRAS3_M1PR_Pos) /**< \brief (MATRIX_PRAS3) Master 1 Priority */
#define MATRIX_PRAS3_M1PR(value) ((MATRIX_PRAS3_M1PR_Msk & ((value) << MATRIX_PRAS3_M1PR_Pos)))
#define MATRIX_PRAS3_M2PR_Pos 8
#define MATRIX_PRAS3_M2PR_Msk (0x3u << MATRIX_PRAS3_M2PR_Pos) /**< \brief (MATRIX_PRAS3) Master 2 Priority */
#define MATRIX_PRAS3_M2PR(value) ((MATRIX_PRAS3_M2PR_Msk & ((value) << MATRIX_PRAS3_M2PR_Pos)))
#define MATRIX_PRAS3_M3PR_Pos 12
#define MATRIX_PRAS3_M3PR_Msk (0x3u << MATRIX_PRAS3_M3PR_Pos) /**< \brief (MATRIX_PRAS3) Master 3 Priority */
#define MATRIX_PRAS3_M3PR(value) ((MATRIX_PRAS3_M3PR_Msk & ((value) << MATRIX_PRAS3_M3PR_Pos)))
#define MATRIX_PRAS3_M4PR_Pos 16
#define MATRIX_PRAS3_M4PR_Msk (0x3u << MATRIX_PRAS3_M4PR_Pos) /**< \brief (MATRIX_PRAS3) Master 4 Priority */
#define MATRIX_PRAS3_M4PR(value) ((MATRIX_PRAS3_M4PR_Msk & ((value) << MATRIX_PRAS3_M4PR_Pos)))
#define MATRIX_PRAS3_M5PR_Pos 20
#define MATRIX_PRAS3_M5PR_Msk (0x3u << MATRIX_PRAS3_M5PR_Pos) /**< \brief (MATRIX_PRAS3) Master 5 Priority */
#define MATRIX_PRAS3_M5PR(value) ((MATRIX_PRAS3_M5PR_Msk & ((value) << MATRIX_PRAS3_M5PR_Pos)))
#define MATRIX_PRAS3_M6PR_Pos 24
#define MATRIX_PRAS3_M6PR_Msk (0x3u << MATRIX_PRAS3_M6PR_Pos) /**< \brief (MATRIX_PRAS3) Master 6 Priority */
#define MATRIX_PRAS3_M6PR(value) ((MATRIX_PRAS3_M6PR_Msk & ((value) << MATRIX_PRAS3_M6PR_Pos)))
#define MATRIX_PRAS3_M7PR_Pos 28
#define MATRIX_PRAS3_M7PR_Msk (0x3u << MATRIX_PRAS3_M7PR_Pos) /**< \brief (MATRIX_PRAS3) Master 7 Priority */
#define MATRIX_PRAS3_M7PR(value) ((MATRIX_PRAS3_M7PR_Msk & ((value) << MATRIX_PRAS3_M7PR_Pos)))
/* -------- MATRIX_PRAS4 : (MATRIX Offset: 0x00A0) Priority Register A for Slave 4 -------- */
#define MATRIX_PRAS4_M0PR_Pos 0
#define MATRIX_PRAS4_M0PR_Msk (0x3u << MATRIX_PRAS4_M0PR_Pos) /**< \brief (MATRIX_PRAS4) Master 0 Priority */
#define MATRIX_PRAS4_M0PR(value) ((MATRIX_PRAS4_M0PR_Msk & ((value) << MATRIX_PRAS4_M0PR_Pos)))
#define MATRIX_PRAS4_M1PR_Pos 4
#define MATRIX_PRAS4_M1PR_Msk (0x3u << MATRIX_PRAS4_M1PR_Pos) /**< \brief (MATRIX_PRAS4) Master 1 Priority */
#define MATRIX_PRAS4_M1PR(value) ((MATRIX_PRAS4_M1PR_Msk & ((value) << MATRIX_PRAS4_M1PR_Pos)))
#define MATRIX_PRAS4_M2PR_Pos 8
#define MATRIX_PRAS4_M2PR_Msk (0x3u << MATRIX_PRAS4_M2PR_Pos) /**< \brief (MATRIX_PRAS4) Master 2 Priority */
#define MATRIX_PRAS4_M2PR(value) ((MATRIX_PRAS4_M2PR_Msk & ((value) << MATRIX_PRAS4_M2PR_Pos)))
#define MATRIX_PRAS4_M3PR_Pos 12
#define MATRIX_PRAS4_M3PR_Msk (0x3u << MATRIX_PRAS4_M3PR_Pos) /**< \brief (MATRIX_PRAS4) Master 3 Priority */
#define MATRIX_PRAS4_M3PR(value) ((MATRIX_PRAS4_M3PR_Msk & ((value) << MATRIX_PRAS4_M3PR_Pos)))
#define MATRIX_PRAS4_M4PR_Pos 16
#define MATRIX_PRAS4_M4PR_Msk (0x3u << MATRIX_PRAS4_M4PR_Pos) /**< \brief (MATRIX_PRAS4) Master 4 Priority */
#define MATRIX_PRAS4_M4PR(value) ((MATRIX_PRAS4_M4PR_Msk & ((value) << MATRIX_PRAS4_M4PR_Pos)))
#define MATRIX_PRAS4_M5PR_Pos 20
#define MATRIX_PRAS4_M5PR_Msk (0x3u << MATRIX_PRAS4_M5PR_Pos) /**< \brief (MATRIX_PRAS4) Master 5 Priority */
#define MATRIX_PRAS4_M5PR(value) ((MATRIX_PRAS4_M5PR_Msk & ((value) << MATRIX_PRAS4_M5PR_Pos)))
#define MATRIX_PRAS4_M6PR_Pos 24
#define MATRIX_PRAS4_M6PR_Msk (0x3u << MATRIX_PRAS4_M6PR_Pos) /**< \brief (MATRIX_PRAS4) Master 6 Priority */
#define MATRIX_PRAS4_M6PR(value) ((MATRIX_PRAS4_M6PR_Msk & ((value) << MATRIX_PRAS4_M6PR_Pos)))
#define MATRIX_PRAS4_M7PR_Pos 28
#define MATRIX_PRAS4_M7PR_Msk (0x3u << MATRIX_PRAS4_M7PR_Pos) /**< \brief (MATRIX_PRAS4) Master 7 Priority */
#define MATRIX_PRAS4_M7PR(value) ((MATRIX_PRAS4_M7PR_Msk & ((value) << MATRIX_PRAS4_M7PR_Pos)))
/* -------- MATRIX_PRAS5 : (MATRIX Offset: 0x00A8) Priority Register A for Slave 5 -------- */
#define MATRIX_PRAS5_M0PR_Pos 0
#define MATRIX_PRAS5_M0PR_Msk (0x3u << MATRIX_PRAS5_M0PR_Pos) /**< \brief (MATRIX_PRAS5) Master 0 Priority */
#define MATRIX_PRAS5_M0PR(value) ((MATRIX_PRAS5_M0PR_Msk & ((value) << MATRIX_PRAS5_M0PR_Pos)))
#define MATRIX_PRAS5_M1PR_Pos 4
#define MATRIX_PRAS5_M1PR_Msk (0x3u << MATRIX_PRAS5_M1PR_Pos) /**< \brief (MATRIX_PRAS5) Master 1 Priority */
#define MATRIX_PRAS5_M1PR(value) ((MATRIX_PRAS5_M1PR_Msk & ((value) << MATRIX_PRAS5_M1PR_Pos)))
#define MATRIX_PRAS5_M2PR_Pos 8
#define MATRIX_PRAS5_M2PR_Msk (0x3u << MATRIX_PRAS5_M2PR_Pos) /**< \brief (MATRIX_PRAS5) Master 2 Priority */
#define MATRIX_PRAS5_M2PR(value) ((MATRIX_PRAS5_M2PR_Msk & ((value) << MATRIX_PRAS5_M2PR_Pos)))
#define MATRIX_PRAS5_M3PR_Pos 12
#define MATRIX_PRAS5_M3PR_Msk (0x3u << MATRIX_PRAS5_M3PR_Pos) /**< \brief (MATRIX_PRAS5) Master 3 Priority */
#define MATRIX_PRAS5_M3PR(value) ((MATRIX_PRAS5_M3PR_Msk & ((value) << MATRIX_PRAS5_M3PR_Pos)))
#define MATRIX_PRAS5_M4PR_Pos 16
#define MATRIX_PRAS5_M4PR_Msk (0x3u << MATRIX_PRAS5_M4PR_Pos) /**< \brief (MATRIX_PRAS5) Master 4 Priority */
#define MATRIX_PRAS5_M4PR(value) ((MATRIX_PRAS5_M4PR_Msk & ((value) << MATRIX_PRAS5_M4PR_Pos)))
#define MATRIX_PRAS5_M5PR_Pos 20
#define MATRIX_PRAS5_M5PR_Msk (0x3u << MATRIX_PRAS5_M5PR_Pos) /**< \brief (MATRIX_PRAS5) Master 5 Priority */
#define MATRIX_PRAS5_M5PR(value) ((MATRIX_PRAS5_M5PR_Msk & ((value) << MATRIX_PRAS5_M5PR_Pos)))
#define MATRIX_PRAS5_M6PR_Pos 24
#define MATRIX_PRAS5_M6PR_Msk (0x3u << MATRIX_PRAS5_M6PR_Pos) /**< \brief (MATRIX_PRAS5) Master 6 Priority */
#define MATRIX_PRAS5_M6PR(value) ((MATRIX_PRAS5_M6PR_Msk & ((value) << MATRIX_PRAS5_M6PR_Pos)))
#define MATRIX_PRAS5_M7PR_Pos 28
#define MATRIX_PRAS5_M7PR_Msk (0x3u << MATRIX_PRAS5_M7PR_Pos) /**< \brief (MATRIX_PRAS5) Master 7 Priority */
#define MATRIX_PRAS5_M7PR(value) ((MATRIX_PRAS5_M7PR_Msk & ((value) << MATRIX_PRAS5_M7PR_Pos)))
/* -------- MATRIX_PRAS6 : (MATRIX Offset: 0x00B0) Priority Register A for Slave 6 -------- */
#define MATRIX_PRAS6_M0PR_Pos 0
#define MATRIX_PRAS6_M0PR_Msk (0x3u << MATRIX_PRAS6_M0PR_Pos) /**< \brief (MATRIX_PRAS6) Master 0 Priority */
#define MATRIX_PRAS6_M0PR(value) ((MATRIX_PRAS6_M0PR_Msk & ((value) << MATRIX_PRAS6_M0PR_Pos)))
#define MATRIX_PRAS6_M1PR_Pos 4
#define MATRIX_PRAS6_M1PR_Msk (0x3u << MATRIX_PRAS6_M1PR_Pos) /**< \brief (MATRIX_PRAS6) Master 1 Priority */
#define MATRIX_PRAS6_M1PR(value) ((MATRIX_PRAS6_M1PR_Msk & ((value) << MATRIX_PRAS6_M1PR_Pos)))
#define MATRIX_PRAS6_M2PR_Pos 8
#define MATRIX_PRAS6_M2PR_Msk (0x3u << MATRIX_PRAS6_M2PR_Pos) /**< \brief (MATRIX_PRAS6) Master 2 Priority */
#define MATRIX_PRAS6_M2PR(value) ((MATRIX_PRAS6_M2PR_Msk & ((value) << MATRIX_PRAS6_M2PR_Pos)))
#define MATRIX_PRAS6_M3PR_Pos 12
#define MATRIX_PRAS6_M3PR_Msk (0x3u << MATRIX_PRAS6_M3PR_Pos) /**< \brief (MATRIX_PRAS6) Master 3 Priority */
#define MATRIX_PRAS6_M3PR(value) ((MATRIX_PRAS6_M3PR_Msk & ((value) << MATRIX_PRAS6_M3PR_Pos)))
#define MATRIX_PRAS6_M4PR_Pos 16
#define MATRIX_PRAS6_M4PR_Msk (0x3u << MATRIX_PRAS6_M4PR_Pos) /**< \brief (MATRIX_PRAS6) Master 4 Priority */
#define MATRIX_PRAS6_M4PR(value) ((MATRIX_PRAS6_M4PR_Msk & ((value) << MATRIX_PRAS6_M4PR_Pos)))
#define MATRIX_PRAS6_M5PR_Pos 20
#define MATRIX_PRAS6_M5PR_Msk (0x3u << MATRIX_PRAS6_M5PR_Pos) /**< \brief (MATRIX_PRAS6) Master 5 Priority */
#define MATRIX_PRAS6_M5PR(value) ((MATRIX_PRAS6_M5PR_Msk & ((value) << MATRIX_PRAS6_M5PR_Pos)))
#define MATRIX_PRAS6_M6PR_Pos 24
#define MATRIX_PRAS6_M6PR_Msk (0x3u << MATRIX_PRAS6_M6PR_Pos) /**< \brief (MATRIX_PRAS6) Master 6 Priority */
#define MATRIX_PRAS6_M6PR(value) ((MATRIX_PRAS6_M6PR_Msk & ((value) << MATRIX_PRAS6_M6PR_Pos)))
#define MATRIX_PRAS6_M7PR_Pos 28
#define MATRIX_PRAS6_M7PR_Msk (0x3u << MATRIX_PRAS6_M7PR_Pos) /**< \brief (MATRIX_PRAS6) Master 7 Priority */
#define MATRIX_PRAS6_M7PR(value) ((MATRIX_PRAS6_M7PR_Msk & ((value) << MATRIX_PRAS6_M7PR_Pos)))
/* -------- MATRIX_PRAS7 : (MATRIX Offset: 0x00B8) Priority Register A for Slave 7 -------- */
#define MATRIX_PRAS7_M0PR_Pos 0
#define MATRIX_PRAS7_M0PR_Msk (0x3u << MATRIX_PRAS7_M0PR_Pos) /**< \brief (MATRIX_PRAS7) Master 0 Priority */
#define MATRIX_PRAS7_M0PR(value) ((MATRIX_PRAS7_M0PR_Msk & ((value) << MATRIX_PRAS7_M0PR_Pos)))
#define MATRIX_PRAS7_M1PR_Pos 4
#define MATRIX_PRAS7_M1PR_Msk (0x3u << MATRIX_PRAS7_M1PR_Pos) /**< \brief (MATRIX_PRAS7) Master 1 Priority */
#define MATRIX_PRAS7_M1PR(value) ((MATRIX_PRAS7_M1PR_Msk & ((value) << MATRIX_PRAS7_M1PR_Pos)))
#define MATRIX_PRAS7_M2PR_Pos 8
#define MATRIX_PRAS7_M2PR_Msk (0x3u << MATRIX_PRAS7_M2PR_Pos) /**< \brief (MATRIX_PRAS7) Master 2 Priority */
#define MATRIX_PRAS7_M2PR(value) ((MATRIX_PRAS7_M2PR_Msk & ((value) << MATRIX_PRAS7_M2PR_Pos)))
#define MATRIX_PRAS7_M3PR_Pos 12
#define MATRIX_PRAS7_M3PR_Msk (0x3u << MATRIX_PRAS7_M3PR_Pos) /**< \brief (MATRIX_PRAS7) Master 3 Priority */
#define MATRIX_PRAS7_M3PR(value) ((MATRIX_PRAS7_M3PR_Msk & ((value) << MATRIX_PRAS7_M3PR_Pos)))
#define MATRIX_PRAS7_M4PR_Pos 16
#define MATRIX_PRAS7_M4PR_Msk (0x3u << MATRIX_PRAS7_M4PR_Pos) /**< \brief (MATRIX_PRAS7) Master 4 Priority */
#define MATRIX_PRAS7_M4PR(value) ((MATRIX_PRAS7_M4PR_Msk & ((value) << MATRIX_PRAS7_M4PR_Pos)))
#define MATRIX_PRAS7_M5PR_Pos 20
#define MATRIX_PRAS7_M5PR_Msk (0x3u << MATRIX_PRAS7_M5PR_Pos) /**< \brief (MATRIX_PRAS7) Master 5 Priority */
#define MATRIX_PRAS7_M5PR(value) ((MATRIX_PRAS7_M5PR_Msk & ((value) << MATRIX_PRAS7_M5PR_Pos)))
#define MATRIX_PRAS7_M6PR_Pos 24
#define MATRIX_PRAS7_M6PR_Msk (0x3u << MATRIX_PRAS7_M6PR_Pos) /**< \brief (MATRIX_PRAS7) Master 6 Priority */
#define MATRIX_PRAS7_M6PR(value) ((MATRIX_PRAS7_M6PR_Msk & ((value) << MATRIX_PRAS7_M6PR_Pos)))
#define MATRIX_PRAS7_M7PR_Pos 28
#define MATRIX_PRAS7_M7PR_Msk (0x3u << MATRIX_PRAS7_M7PR_Pos) /**< \brief (MATRIX_PRAS7) Master 7 Priority */
#define MATRIX_PRAS7_M7PR(value) ((MATRIX_PRAS7_M7PR_Msk & ((value) << MATRIX_PRAS7_M7PR_Pos)))
/* -------- MATRIX_PRAS8 : (MATRIX Offset: 0x00BC) Priority Register A for Slave 8 -------- */
#define MATRIX_PRAS8_M0PR_Pos 0
#define MATRIX_PRAS8_M0PR_Msk (0x3u << MATRIX_PRAS8_M0PR_Pos) /**< \brief (MATRIX_PRAS8) Master 0 Priority */
#define MATRIX_PRAS8_M0PR(value) ((MATRIX_PRAS8_M0PR_Msk & ((value) << MATRIX_PRAS8_M0PR_Pos)))
#define MATRIX_PRAS8_M1PR_Pos 4
#define MATRIX_PRAS8_M1PR_Msk (0x3u << MATRIX_PRAS8_M1PR_Pos) /**< \brief (MATRIX_PRAS8) Master 1 Priority */
#define MATRIX_PRAS8_M1PR(value) ((MATRIX_PRAS8_M1PR_Msk & ((value) << MATRIX_PRAS8_M1PR_Pos)))
#define MATRIX_PRAS8_M2PR_Pos 8
#define MATRIX_PRAS8_M2PR_Msk (0x3u << MATRIX_PRAS8_M2PR_Pos) /**< \brief (MATRIX_PRAS8) Master 2 Priority */
#define MATRIX_PRAS8_M2PR(value) ((MATRIX_PRAS8_M2PR_Msk & ((value) << MATRIX_PRAS8_M2PR_Pos)))
#define MATRIX_PRAS8_M3PR_Pos 12
#define MATRIX_PRAS8_M3PR_Msk (0x3u << MATRIX_PRAS8_M3PR_Pos) /**< \brief (MATRIX_PRAS8) Master 3 Priority */
#define MATRIX_PRAS8_M3PR(value) ((MATRIX_PRAS8_M3PR_Msk & ((value) << MATRIX_PRAS8_M3PR_Pos)))
#define MATRIX_PRAS8_M4PR_Pos 16
#define MATRIX_PRAS8_M4PR_Msk (0x3u << MATRIX_PRAS8_M4PR_Pos) /**< \brief (MATRIX_PRAS8) Master 4 Priority */
#define MATRIX_PRAS8_M4PR(value) ((MATRIX_PRAS8_M4PR_Msk & ((value) << MATRIX_PRAS8_M4PR_Pos)))
#define MATRIX_PRAS8_M5PR_Pos 20
#define MATRIX_PRAS8_M5PR_Msk (0x3u << MATRIX_PRAS8_M5PR_Pos) /**< \brief (MATRIX_PRAS8) Master 5 Priority */
#define MATRIX_PRAS8_M5PR(value) ((MATRIX_PRAS8_M5PR_Msk & ((value) << MATRIX_PRAS8_M5PR_Pos)))
#define MATRIX_PRAS8_M6PR_Pos 24
#define MATRIX_PRAS8_M6PR_Msk (0x3u << MATRIX_PRAS8_M6PR_Pos) /**< \brief (MATRIX_PRAS8) Master 6 Priority */
#define MATRIX_PRAS8_M6PR(value) ((MATRIX_PRAS8_M6PR_Msk & ((value) << MATRIX_PRAS8_M6PR_Pos)))
#define MATRIX_PRAS8_M7PR_Pos 28
#define MATRIX_PRAS8_M7PR_Msk (0x3u << MATRIX_PRAS8_M7PR_Pos) /**< \brief (MATRIX_PRAS8) Master 7 Priority */
#define MATRIX_PRAS8_M7PR(value) ((MATRIX_PRAS8_M7PR_Msk & ((value) << MATRIX_PRAS8_M7PR_Pos)))
/* -------- MATRIX_SYSIO : (MATRIX Offset: 0x0114) System I/O Configuration Register -------- */
#define MATRIX_SYSIO_SYSIO0 (0x1u << 0) /**< \brief (MATRIX_SYSIO) PB0 or TDI Assignment */
#define MATRIX_SYSIO_SYSIO1 (0x1u << 1) /**< \brief (MATRIX_SYSIO) PB1 or TDO/TRACESWO Assignment */
#define MATRIX_SYSIO_SYSIO2 (0x1u << 2) /**< \brief (MATRIX_SYSIO) PB2 or TMS/SWDIO Assignment */
#define MATRIX_SYSIO_SYSIO3 (0x1u << 3) /**< \brief (MATRIX_SYSIO) PB3 or TCK/SWCLK Assignment */
#define MATRIX_SYSIO_SYSIO9 (0x1u << 9) /**< \brief (MATRIX_SYSIO) PC9 or ERASE Assignment */
#define MATRIX_SYSIO_SYSIO10 (0x1u << 10) /**< \brief (MATRIX_SYSIO) PD0 or DDM Assignment */
#define MATRIX_SYSIO_SYSIO11 (0x1u << 11) /**< \brief (MATRIX_SYSIO) PD1 or DDP Assignment */
/* -------- MATRIX_SMCNFCS : (MATRIX Offset: 0x011C) SMC Nand Flash Chip Select Configuration Register -------- */
#define MATRIX_SMCNFCS_SMC_NFCS0 (0x1u << 0) /**< \brief (MATRIX_SMCNFCS) SMC NAND Flash Chip Select 0 Assignment */
#define MATRIX_SMCNFCS_SMC_NFCS1 (0x1u << 1) /**< \brief (MATRIX_SMCNFCS) SMC NAND Flash Chip Select 1 Assignment */
#define MATRIX_SMCNFCS_SMC_NFCS2 (0x1u << 2) /**< \brief (MATRIX_SMCNFCS) SMC NAND Flash Chip Select 2 Assignment */
#define MATRIX_SMCNFCS_SMC_NFCS3 (0x1u << 3) /**< \brief (MATRIX_SMCNFCS) SMC NAND Flash Chip Select 3 Assignment */
#define MATRIX_SMCNFCS_SMC_SEL (0x1u << 31) /**< \brief (MATRIX_SMCNFCS) SMC Selection for EBI pins */
/* -------- MATRIX_CORE_DEBUG : (MATRIX Offset: 0x0128) Core Debug Configuration Register -------- */
#define MATRIX_CORE_DEBUG_CROSS_TRG1 (0x1u << 1) /**< \brief (MATRIX_CORE_DEBUG)  */
#define MATRIX_CORE_DEBUG_CROSS_TRG0 (0x1u << 2) /**< \brief (MATRIX_CORE_DEBUG)  */
/* -------- MATRIX_WPMR : (MATRIX Offset: 0x01E4) Write Protection Mode Register -------- */
#define MATRIX_WPMR_WPEN (0x1u << 0) /**< \brief (MATRIX_WPMR) Write Protect Enable */
#define MATRIX_WPMR_WPKEY_Pos 8
#define MATRIX_WPMR_WPKEY_Msk (0xffffffu << MATRIX_WPMR_WPKEY_Pos) /**< \brief (MATRIX_WPMR) Write Protect Key */
#define   MATRIX_WPMR_WPKEY_PASSWD (0x4D4154u << 8) /**< \brief (MATRIX_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. */
/* -------- MATRIX_WPSR : (MATRIX Offset: 0x01E8) Write Protection Status Register -------- */
#define MATRIX_WPSR_WPVS (0x1u << 0) /**< \brief (MATRIX_WPSR) Write Protection Violation Status */
#define MATRIX_WPSR_WPVSRC_Pos 8
#define MATRIX_WPSR_WPVSRC_Msk (0xffffu << MATRIX_WPSR_WPVSRC_Pos) /**< \brief (MATRIX_WPSR) Write Protection Violation Source */

/*@}*/


#endif /* _SAM4C_MATRIX_COMPONENT_ */
