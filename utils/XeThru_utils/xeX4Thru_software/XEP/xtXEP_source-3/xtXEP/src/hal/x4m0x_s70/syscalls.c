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

/**
 * \file
 *
 * Implementation of newlib syscall.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/


#include "board.h"
#include "xep_hal.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

/*----------------------------------------------------------------------------
 *        Exported variables
 *----------------------------------------------------------------------------*/

#undef errno
extern int errno;
extern int _sheap;
extern int _eheap;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
extern void _exit(int status);
extern void _kill(int pid, int sig);
extern int _getpid (void);

extern caddr_t _sbrk (int incr)
{
	static unsigned char *heap = NULL;
	unsigned char *prev_sheap;
    int heapend = (int)&_eheap;

	if (heap == NULL)
		heap = (unsigned char *)&_sheap;

	prev_sheap = heap;

    if (((int)prev_sheap + incr) > heapend) {
		return (caddr_t) -1;
	}

	heap += incr;

	return (caddr_t) prev_sheap;
}

extern int link(char *old, char *new)
{
	return -1;
}

extern int _close(int file)
{
	return -1;
}

extern int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;

	return 0;
}

extern int _isatty(int file)
{
	return 1;
}

extern int _lseek(int file, int ptr, int dir)
{
	return 0;
}

extern int _read(int file, char *ptr, int len)
{
	return 0;
}

extern int _write(int file, char *ptr, int len)
{
	int iIndex;

	//    for (; *ptr != 0; ptr++)
	for (iIndex = 0; iIndex < len; iIndex++, ptr++)
	{
		//DBG_PutChar(*ptr);
	}

	return iIndex;
}

extern void __assert_func(const char *file, int line, const char *function,
        const char *expr)
{
    char buf[101];
    snprintf(buf, 101, "assert(%s); %s:%d %s(..)", expr, file, line, function);
    xt_trigger_crash(XT_SWRST_ASSERT, buf, true);
    for (;;);
}

extern void _exit(int status)
{
	printf("Exiting with status %d.\n", status);

	for (;;);
}

extern void _kill(int pid, int sig)
{
	return;
}

extern int _getpid (void)
{
	return -1;
}
