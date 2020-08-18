/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/*                       SAM Software Package License                           */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2015, Atmel Corporation                                        */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */

#ifndef _SAMS70_MATRIX_COMPONENT_
#define _SAMS70_MATRIX_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR AHB Bus Matrix */
/* ============================================================================= */
/** \addtogroup SAMS70_MATRIX AHB Bus Matrix */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Matrix hardware registers */
typedef struct {
  __IO uint32_t MATRIX_MCFG0;   /**< \brief (Matrix Offset: 0x0000) Master Configuration Register 0 */
  __IO uint32_t MATRIX_MCFG1;   /**< \brief (Matrix Offset: 0x0004) Master Configuration Register 1 */
  __IO uint32_t MATRIX_MCFG2;   /**< \brief (Matrix Offset: 0x0008) Master Configuration Register 2 */
  __IO uint32_t MATRIX_MCFG3;   /**< \brief (Matrix Offset: 0x000C) Master Configuration Register 3 */
  __IO uint32_t MATRIX_MCFG4;   /**< \brief (Matrix Offset: 0x0010) Master Configuration Register 4 */
  __IO uint32_t MATRIX_MCFG5;   /**< \brief (Matrix Offset: 0x0014) Master Configuration Register 5 */
  __IO uint32_t MATRIX_MCFG6;   /**< \brief (Matrix Offset: 0x0018) Master Configuration Register 6 */
  __I  uint32_t Reserved1[1];
  __IO uint32_t MATRIX_MCFG8;   /**< \brief (Matrix Offset: 0x0020) Master Configuration Register 8 */
  __I  uint32_t Reserved2[7];
  __IO uint32_t MATRIX_SCFG[9]; /**< \brief (Matrix Offset: 0x0040) Slave Configuration Register */
  __I  uint32_t Reserved3[7];
  __IO uint32_t MATRIX_PRAS0;   /**< \brief (Matrix Offset: 0x0080) Priority Register A for Slave 0 */
  __IO uint32_t MATRIX_PRBS0;   /**< \brief (Matrix Offset: 0x0084) Priority Register B for Slave 0 */
  __IO uint32_t MATRIX_PRAS1;   /**< \brief (Matrix Offset: 0x0088) Priority Register A for Slave 1 */
  __IO uint32_t MATRIX_PRBS1;   /**< \brief (Matrix Offset: 0x008C) Priority Register B for Slave 1 */
  __IO uint32_t MATRIX_PRAS2;   /**< \brief (Matrix Offset: 0x0090) Priority Register A for Slave 2 */
  __IO uint32_t MATRIX_PRBS2;   /**< \brief (Matrix Offset: 0x0094) Priority Register B for Slave 2 */
  __IO uint32_t MATRIX_PRAS3;   /**< \brief (Matrix Offset: 0x0098) Priority Register A for Slave 3 */
  __IO uint32_t MATRIX_PRBS3;   /**< \brief (Matrix Offset: 0x009C) Priority Register B for Slave 3 */
  __IO uint32_t MATRIX_PRAS4;   /**< \brief (Matrix Offset: 0x00A0) Priority Register A for Slave 4 */
  __IO uint32_t MATRIX_PRBS4;   /**< \brief (Matrix Offset: 0x00A4) Priority Register B for Slave 4 */
  __IO uint32_t MATRIX_PRAS5;   /**< \brief (Matrix Offset: 0x00A8) Priority Register A for Slave 5 */
  __IO uint32_t MATRIX_PRBS5;   /**< \brief (Matrix Offset: 0x00AC) Priority Register B for Slave 5 */
  __IO uint32_t MATRIX_PRAS6;   /**< \brief (Matrix Offset: 0x00B0) Priority Register A for Slave 6 */
  __IO uint32_t MATRIX_PRBS6;   /**< \brief (Matrix Offset: 0x00B4) Priority Register B for Slave 6 */
  __IO uint32_t MATRIX_PRAS7;   /**< \brief (Matrix Offset: 0x00B8) Priority Register A for Slave 7 */
  __IO uint32_t MATRIX_PRBS7;   /**< \brief (Matrix Offset: 0x00BC) Priority Register B for Slave 7 */
  __IO uint32_t MATRIX_PRAS8;   /**< \brief (Matrix Offset: 0x00C0) Priority Register A for Slave 8 */
  __IO uint32_t MATRIX_PRBS8;   /**< \brief (Matrix Offset: 0x00C4) Priority Register B for Slave 8 */
  __I  uint32_t Reserved4[14];
  __IO uint32_t MATRIX_MRCR;    /**< \brief (Matrix Offset: 0x0100) Master Remap Control Register */
  __I  uint32_t Reserved5[4];
  __IO uint32_t CCFG_SYSIO;     /**< \brief (Matrix Offset: 0x0114) System I/O Configuration Register */
  __I  uint32_t Reserved6[3];
  __IO uint32_t CCFG_SMCNFCS;   /**< \brief (Matrix Offset: 0x0124) SMC NAND Flash Chip Select Configuration Register */
  __I  uint32_t Reserved7[47];
  __IO uint32_t MATRIX_WPMR;    /**< \brief (Matrix Offset: 0x01E4) Write Protection Mode Register */
  __I  uint32_t MATRIX_WPSR;    /**< \brief (Matrix Offset: 0x01E8) Write Protection Status Register */
} Matrix;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- MATRIX_MCFG0 : (MATRIX Offset: 0x0000) Master Configuration Register 0 -------- */
#define MATRIX_MCFG0_ULBT_Pos 0
#define MATRIX_MCFG0_ULBT_Msk (0x7u << MATRIX_MCFG0_ULBT_Pos) /**< \brief (MATRIX_MCFG0) Undefined Length Burst Type */
#define MATRIX_MCFG0_ULBT(value) ((MATRIX_MCFG0_ULBT_Msk & ((value) << MATRIX_MCFG0_ULBT_Pos)))
#define   MATRIX_MCFG0_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG0) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG0_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG0) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG0_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG0) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG0_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG0) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG0_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG0) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG0_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG0) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG0_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG0) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG0_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG0) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG1 : (MATRIX Offset: 0x0004) Master Configuration Register 1 -------- */
#define MATRIX_MCFG1_ULBT_Pos 0
#define MATRIX_MCFG1_ULBT_Msk (0x7u << MATRIX_MCFG1_ULBT_Pos) /**< \brief (MATRIX_MCFG1) Undefined Length Burst Type */
#define MATRIX_MCFG1_ULBT(value) ((MATRIX_MCFG1_ULBT_Msk & ((value) << MATRIX_MCFG1_ULBT_Pos)))
#define   MATRIX_MCFG1_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG1) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG1_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG1) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG1_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG1) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG1_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG1) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG1_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG1) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG1_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG1) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG1_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG1) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG1_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG1) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG2 : (MATRIX Offset: 0x0008) Master Configuration Register 2 -------- */
#define MATRIX_MCFG2_ULBT_Pos 0
#define MATRIX_MCFG2_ULBT_Msk (0x7u << MATRIX_MCFG2_ULBT_Pos) /**< \brief (MATRIX_MCFG2) Undefined Length Burst Type */
#define MATRIX_MCFG2_ULBT(value) ((MATRIX_MCFG2_ULBT_Msk & ((value) << MATRIX_MCFG2_ULBT_Pos)))
#define   MATRIX_MCFG2_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG2) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG2_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG2) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG2_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG2) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG2_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG2) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG2_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG2) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG2_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG2) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG2_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG2) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG2_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG2) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG3 : (MATRIX Offset: 0x000C) Master Configuration Register 3 -------- */
#define MATRIX_MCFG3_ULBT_Pos 0
#define MATRIX_MCFG3_ULBT_Msk (0x7u << MATRIX_MCFG3_ULBT_Pos) /**< \brief (MATRIX_MCFG3) Undefined Length Burst Type */
#define MATRIX_MCFG3_ULBT(value) ((MATRIX_MCFG3_ULBT_Msk & ((value) << MATRIX_MCFG3_ULBT_Pos)))
#define   MATRIX_MCFG3_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG3) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG3_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG3) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG3_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG3) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG3_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG3) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG3_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG3) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG3_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG3) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG3_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG3) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG3_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG3) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG4 : (MATRIX Offset: 0x0010) Master Configuration Register 4 -------- */
#define MATRIX_MCFG4_ULBT_Pos 0
#define MATRIX_MCFG4_ULBT_Msk (0x7u << MATRIX_MCFG4_ULBT_Pos) /**< \brief (MATRIX_MCFG4) Undefined Length Burst Type */
#define MATRIX_MCFG4_ULBT(value) ((MATRIX_MCFG4_ULBT_Msk & ((value) << MATRIX_MCFG4_ULBT_Pos)))
#define   MATRIX_MCFG4_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG4) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG4_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG4) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG4_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG4) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG4_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG4) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG4_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG4) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG4_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG4) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG4_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG4) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG4_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG4) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG5 : (MATRIX Offset: 0x0014) Master Configuration Register 5 -------- */
#define MATRIX_MCFG5_ULBT_Pos 0
#define MATRIX_MCFG5_ULBT_Msk (0x7u << MATRIX_MCFG5_ULBT_Pos) /**< \brief (MATRIX_MCFG5) Undefined Length Burst Type */
#define MATRIX_MCFG5_ULBT(value) ((MATRIX_MCFG5_ULBT_Msk & ((value) << MATRIX_MCFG5_ULBT_Pos)))
#define   MATRIX_MCFG5_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG5) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG5_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG5) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG5_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG5) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG5_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG5) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG5_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG5) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG5_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG5) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG5_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG5) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG5_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG5) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG6 : (MATRIX Offset: 0x0018) Master Configuration Register 6 -------- */
#define MATRIX_MCFG6_ULBT_Pos 0
#define MATRIX_MCFG6_ULBT_Msk (0x7u << MATRIX_MCFG6_ULBT_Pos) /**< \brief (MATRIX_MCFG6) Undefined Length Burst Type */
#define MATRIX_MCFG6_ULBT(value) ((MATRIX_MCFG6_ULBT_Msk & ((value) << MATRIX_MCFG6_ULBT_Pos)))
#define   MATRIX_MCFG6_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG6) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG6_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG6) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG6_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG6) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG6_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG6) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG6_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG6) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG6_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG6) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG6_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG6) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG6_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG6) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG8 : (MATRIX Offset: 0x0020) Master Configuration Register 8 -------- */
#define MATRIX_MCFG8_ULBT_Pos 0
#define MATRIX_MCFG8_ULBT_Msk (0x7u << MATRIX_MCFG8_ULBT_Pos) /**< \brief (MATRIX_MCFG8) Undefined Length Burst Type */
#define MATRIX_MCFG8_ULBT(value) ((MATRIX_MCFG8_ULBT_Msk & ((value) << MATRIX_MCFG8_ULBT_Pos)))
#define   MATRIX_MCFG8_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG8) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG8_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG8) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG8_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG8) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG8_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG8) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG8_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG8) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG8_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG8) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG8_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG8) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG8_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG8) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_SCFG[9] : (MATRIX Offset: 0x0040) Slave Configuration Register -------- */
#define MATRIX_SCFG_SLOT_CYCLE_Pos 0
#define MATRIX_SCFG_SLOT_CYCLE_Msk (0x1ffu << MATRIX_SCFG_SLOT_CYCLE_Pos) /**< \brief (MATRIX_SCFG[9]) Maximum Bus Grant Duration for Masters */
#define MATRIX_SCFG_SLOT_CYCLE(value) ((MATRIX_SCFG_SLOT_CYCLE_Msk & ((value) << MATRIX_SCFG_SLOT_CYCLE_Pos)))
#define MATRIX_SCFG_DEFMSTR_TYPE_Pos 16
#define MATRIX_SCFG_DEFMSTR_TYPE_Msk (0x3u << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< \brief (MATRIX_SCFG[9]) Default Master Type */
#define MATRIX_SCFG_DEFMSTR_TYPE(value) ((MATRIX_SCFG_DEFMSTR_TYPE_Msk & ((value) << MATRIX_SCFG_DEFMSTR_TYPE_Pos)))
#define   MATRIX_SCFG_DEFMSTR_TYPE_NONE (0x0u << 16) /**< \brief (MATRIX_SCFG[9]) No Default Master-At the end of the current slave access, if no other master request is pending, the slave is disconnected from all masters.This results in a one clock cycle latency for the first access of a burst transfer or for a single access. */
#define   MATRIX_SCFG_DEFMSTR_TYPE_LAST (0x1u << 16) /**< \brief (MATRIX_SCFG[9]) Last Default Master-At the end of the current slave access, if no other master request is pending, the slave stays connected to the last master having accessed it.This results in not having one clock cycle latency when the last master tries to access the slave again. */
#define   MATRIX_SCFG_DEFMSTR_TYPE_FIXED (0x2u << 16) /**< \brief (MATRIX_SCFG[9]) Fixed Default Master-At the end of the current slave access, if no other master request is pending, the slave connects to the fixed master the number that has been written in the FIXED_DEFMSTR field.This results in not having one clock cycle latency when the fixed master tries to access the slave again. */
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
/* -------- MATRIX_PRBS0 : (MATRIX Offset: 0x0084) Priority Register B for Slave 0 -------- */
#define MATRIX_PRBS0_M8PR_Pos 0
#define MATRIX_PRBS0_M8PR_Msk (0x3u << MATRIX_PRBS0_M8PR_Pos) /**< \brief (MATRIX_PRBS0) Master 8 Priority */
#define MATRIX_PRBS0_M8PR(value) ((MATRIX_PRBS0_M8PR_Msk & ((value) << MATRIX_PRBS0_M8PR_Pos)))
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
/* -------- MATRIX_PRBS1 : (MATRIX Offset: 0x008C) Priority Register B for Slave 1 -------- */
#define MATRIX_PRBS1_M8PR_Pos 0
#define MATRIX_PRBS1_M8PR_Msk (0x3u << MATRIX_PRBS1_M8PR_Pos) /**< \brief (MATRIX_PRBS1) Master 8 Priority */
#define MATRIX_PRBS1_M8PR(value) ((MATRIX_PRBS1_M8PR_Msk & ((value) << MATRIX_PRBS1_M8PR_Pos)))
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
/* -------- MATRIX_PRBS2 : (MATRIX Offset: 0x0094) Priority Register B for Slave 2 -------- */
#define MATRIX_PRBS2_M8PR_Pos 0
#define MATRIX_PRBS2_M8PR_Msk (0x3u << MATRIX_PRBS2_M8PR_Pos) /**< \brief (MATRIX_PRBS2) Master 8 Priority */
#define MATRIX_PRBS2_M8PR(value) ((MATRIX_PRBS2_M8PR_Msk & ((value) << MATRIX_PRBS2_M8PR_Pos)))
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
/* -------- MATRIX_PRBS3 : (MATRIX Offset: 0x009C) Priority Register B for Slave 3 -------- */
#define MATRIX_PRBS3_M8PR_Pos 0
#define MATRIX_PRBS3_M8PR_Msk (0x3u << MATRIX_PRBS3_M8PR_Pos) /**< \brief (MATRIX_PRBS3) Master 8 Priority */
#define MATRIX_PRBS3_M8PR(value) ((MATRIX_PRBS3_M8PR_Msk & ((value) << MATRIX_PRBS3_M8PR_Pos)))
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
/* -------- MATRIX_PRBS4 : (MATRIX Offset: 0x00A4) Priority Register B for Slave 4 -------- */
#define MATRIX_PRBS4_M8PR_Pos 0
#define MATRIX_PRBS4_M8PR_Msk (0x3u << MATRIX_PRBS4_M8PR_Pos) /**< \brief (MATRIX_PRBS4) Master 8 Priority */
#define MATRIX_PRBS4_M8PR(value) ((MATRIX_PRBS4_M8PR_Msk & ((value) << MATRIX_PRBS4_M8PR_Pos)))
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
/* -------- MATRIX_PRBS5 : (MATRIX Offset: 0x00AC) Priority Register B for Slave 5 -------- */
#define MATRIX_PRBS5_M8PR_Pos 0
#define MATRIX_PRBS5_M8PR_Msk (0x3u << MATRIX_PRBS5_M8PR_Pos) /**< \brief (MATRIX_PRBS5) Master 8 Priority */
#define MATRIX_PRBS5_M8PR(value) ((MATRIX_PRBS5_M8PR_Msk & ((value) << MATRIX_PRBS5_M8PR_Pos)))
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
/* -------- MATRIX_PRBS6 : (MATRIX Offset: 0x00B4) Priority Register B for Slave 6 -------- */
#define MATRIX_PRBS6_M8PR_Pos 0
#define MATRIX_PRBS6_M8PR_Msk (0x3u << MATRIX_PRBS6_M8PR_Pos) /**< \brief (MATRIX_PRBS6) Master 8 Priority */
#define MATRIX_PRBS6_M8PR(value) ((MATRIX_PRBS6_M8PR_Msk & ((value) << MATRIX_PRBS6_M8PR_Pos)))
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
/* -------- MATRIX_PRBS7 : (MATRIX Offset: 0x00BC) Priority Register B for Slave 7 -------- */
#define MATRIX_PRBS7_M8PR_Pos 0
#define MATRIX_PRBS7_M8PR_Msk (0x3u << MATRIX_PRBS7_M8PR_Pos) /**< \brief (MATRIX_PRBS7) Master 8 Priority */
#define MATRIX_PRBS7_M8PR(value) ((MATRIX_PRBS7_M8PR_Msk & ((value) << MATRIX_PRBS7_M8PR_Pos)))
/* -------- MATRIX_PRAS8 : (MATRIX Offset: 0x00C0) Priority Register A for Slave 8 -------- */
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
/* -------- MATRIX_PRBS8 : (MATRIX Offset: 0x00C4) Priority Register B for Slave 8 -------- */
#define MATRIX_PRBS8_M8PR_Pos 0
#define MATRIX_PRBS8_M8PR_Msk (0x3u << MATRIX_PRBS8_M8PR_Pos) /**< \brief (MATRIX_PRBS8) Master 8 Priority */
#define MATRIX_PRBS8_M8PR(value) ((MATRIX_PRBS8_M8PR_Msk & ((value) << MATRIX_PRBS8_M8PR_Pos)))
/* -------- MATRIX_MRCR : (MATRIX Offset: 0x0100) Master Remap Control Register -------- */
#define MATRIX_MRCR_RCB0 (0x1u << 0) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 0 */
#define MATRIX_MRCR_RCB1 (0x1u << 1) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 1 */
#define MATRIX_MRCR_RCB2 (0x1u << 2) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 2 */
#define MATRIX_MRCR_RCB3 (0x1u << 3) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 3 */
#define MATRIX_MRCR_RCB4 (0x1u << 4) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 4 */
#define MATRIX_MRCR_RCB5 (0x1u << 5) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 5 */
#define MATRIX_MRCR_RCB6 (0x1u << 6) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 6 */
#define MATRIX_MRCR_RCB8 (0x1u << 8) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 8 */
/* -------- CCFG_SYSIO : (MATRIX Offset: 0x0114) System I/O Configuration Register -------- */
#define CCFG_SYSIO_SYSIO4 (0x1u << 4) /**< \brief (CCFG_SYSIO) PB4 or TDI Assignment */
#define CCFG_SYSIO_SYSIO5 (0x1u << 5) /**< \brief (CCFG_SYSIO) PB5 or TDO/TRACESWO Assignment */
#define CCFG_SYSIO_SYSIO6 (0x1u << 6) /**< \brief (CCFG_SYSIO) PB6 or TMS/SWDIO Assignment */
#define CCFG_SYSIO_SYSIO7 (0x1u << 7) /**< \brief (CCFG_SYSIO) PB7 or TCK/SWCLK Assignment */
#define CCFG_SYSIO_SYSIO12 (0x1u << 12) /**< \brief (CCFG_SYSIO) PB12 or ERASE Assignment */
/* -------- CCFG_SMCNFCS : (MATRIX Offset: 0x0124) SMC NAND Flash Chip Select Configuration Register -------- */
#define CCFG_SMCNFCS_SMC_NFCS0 (0x1u << 0) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 0 Assignment */
#define CCFG_SMCNFCS_SMC_NFCS1 (0x1u << 1) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 1 Assignment */
#define CCFG_SMCNFCS_SMC_NFCS2 (0x1u << 2) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 2 Assignment */
#define CCFG_SMCNFCS_SMC_NFCS3 (0x1u << 3) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 3 Assignment */
#define CCFG_SMCNFCS_SDRAMEN (0x1u << 4) /**< \brief (CCFG_SMCNFCS) SDRAM Enable */
/* -------- MATRIX_WPMR : (MATRIX Offset: 0x01E4) Write Protection Mode Register -------- */
#define MATRIX_WPMR_WPEN (0x1u << 0) /**< \brief (MATRIX_WPMR) Write Protection Enable */
#define MATRIX_WPMR_WPKEY_Pos 8
#define MATRIX_WPMR_WPKEY_Msk (0xffffffu << MATRIX_WPMR_WPKEY_Pos) /**< \brief (MATRIX_WPMR) Write Protection Key */
#define MATRIX_WPMR_WPKEY(value) ((MATRIX_WPMR_WPKEY_Msk & ((value) << MATRIX_WPMR_WPKEY_Pos)))
#define   MATRIX_WPMR_WPKEY_PASSWD (0x4D4154u << 8) /**< \brief (MATRIX_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. */
/* -------- MATRIX_WPSR : (MATRIX Offset: 0x01E8) Write Protection Status Register -------- */
#define MATRIX_WPSR_WPVS (0x1u << 0) /**< \brief (MATRIX_WPSR) Write Protection Violation Status */
#define MATRIX_WPSR_WPVSRC_Pos 8
#define MATRIX_WPSR_WPVSRC_Msk (0xffffu << MATRIX_WPSR_WPVSRC_Pos) /**< \brief (MATRIX_WPSR) Write Protection Violation Source */

/*@}*/


#endif /* _SAMS70_MATRIX_COMPONENT_ */
