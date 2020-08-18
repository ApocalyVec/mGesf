#
# Copyright (c) 2011 Atmel Corporation. All rights reserved.
#
# \asf_license_start
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
#    Atmel microcontroller product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# \asf_license_stop
#

# Path to top level ASF directory relative to this project directory.
PRJ_PATH = ../../../../../../..

# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m0plus

# Target part: none, sam3n4 or sam4l4aa
PART = samd20j18

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = dbg_print_quick_start_flash.elf
TARGET_SRAM = dbg_print_quick_start_sram.elf

# List of C source files.
CSRCS = \
       common/services/freertos/dbg_print/dbg_print.c     \
       common/services/freertos/dbg_print/quick_start_basic/qs_dbg_print_basic.c \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       sam0/boards/samd20_xplained_pro/board_init.c       \
       sam0/drivers/port/port.c                           \
       sam0/drivers/sercom/sercom.c                       \
       sam0/drivers/sercom/sercom_interrupt.c             \
       sam0/drivers/sercom/usart/usart.c                  \
       sam0/drivers/sercom/usart/usart_interrupt.c        \
       sam0/drivers/system/clock/clock_samd20/clock.c     \
       sam0/drivers/system/clock/clock_samd20/gclk.c      \
       sam0/drivers/system/interrupt/system_interrupt.c   \
       sam0/drivers/system/pinmux/pinmux.c                \
       sam0/drivers/system/system.c                       \
       sam0/utils/cmsis/samd20/source/gcc/startup_samd20.c \
       sam0/utils/cmsis/samd20/source/system_samd20.c     \
       sam0/utils/syscalls/gcc/syscalls.c                 \
       thirdparty/freertos/freertos-8.0.1/Source/croutine.c \
       thirdparty/freertos/freertos-8.0.1/Source/event_groups.c \
       thirdparty/freertos/freertos-8.0.1/Source/list.c   \
       thirdparty/freertos/freertos-8.0.1/Source/portable/GCC/ARM_CM0/port.c \
       thirdparty/freertos/freertos-8.0.1/Source/portable/MemMang/heap_1.c \
       thirdparty/freertos/freertos-8.0.1/Source/queue.c  \
       thirdparty/freertos/freertos-8.0.1/Source/tasks.c  \
       thirdparty/freertos/freertos-8.0.1/Source/timers.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
       common/boards                                      \
       common/services/freertos/dbg_print                 \
       common/services/freertos/dbg_print/quick_start_basic \
       common/services/freertos/dbg_print/quick_start_basic/samd20_xplained_pro \
       common/utils                                       \
       sam0/boards                                        \
       sam0/boards/samd20_xplained_pro                    \
       sam0/drivers/port                                  \
       sam0/drivers/sercom                                \
       sam0/drivers/sercom/usart                          \
       sam0/drivers/system                                \
       sam0/drivers/system/clock                          \
       sam0/drivers/system/clock/clock_samd20             \
       sam0/drivers/system/interrupt                      \
       sam0/drivers/system/interrupt/system_interrupt_samd20 \
       sam0/drivers/system/pinmux                         \
       sam0/drivers/system/power                          \
       sam0/drivers/system/power/power_sam_d_r            \
       sam0/drivers/system/reset                          \
       sam0/drivers/system/reset/reset_sam_d_r            \
       sam0/utils                                         \
       sam0/utils/cmsis/samd20/include                    \
       sam0/utils/cmsis/samd20/source                     \
       sam0/utils/header_files                            \
       sam0/utils/preprocessor                            \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/freertos/freertos-8.0.1/Source/include  \
       thirdparty/freertos/freertos-8.0.1/Source/portable/GCC/ARM_CM0 \
       common/services/freertos/dbg_print/quick_start_basic/samd20_xplained_pro/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                          

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM0l_math                                

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam0/utils/linker_scripts/samd20/gcc/samd20j18_flash.ld
LINKER_SCRIPT_SRAM  = sam0/utils/linker_scripts/samd20/gcc/samd20j18_sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam0/boards/samd20_xplained_pro/debug_scripts/gcc/samd20_xplained_pro_flash.gdb
DEBUG_SCRIPT_SRAM  = sam0/boards/samd20_xplained_pro/debug_scripts/gcc/samd20_xplained_pro_sram.gdb

# Project type parameter: all, sram or flash
PROJECT_TYPE        = flash

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -O1

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
CFLAGS = 

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
       -D ARM_MATH_CM0PLUS=true                           \
       -D BOARD=SAMD20_XPLAINED_PRO                       \
       -D USART_CALLBACK_MODE=true                        \
       -D __FREERTOS__                                    \
       -D __SAMD20J18__

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 