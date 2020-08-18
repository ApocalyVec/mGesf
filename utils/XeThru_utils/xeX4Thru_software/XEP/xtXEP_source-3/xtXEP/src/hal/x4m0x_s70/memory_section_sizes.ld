/**
 * @file
 *
 * @brief Linker script snippet for defining size of memory sections.
 */

/* NOTE: you need to adjust memory section sizes according to your application. */

/**
 * The Size of the DTCM and the ITCM individually (not put together).
 */
TCM_SIZE = 0x00008000;

/**
 * The size of the internal SRAM. Memory used by the TCM gets subtracted
 * automatically.
 */
SRAM_SIZE = 0x00060000;

/**
 * Available external SDRAM.
 */
SDRAM_SIZE = 0x00800000;

/* Size of .default_heap_memory section.
   This memory section is used by xtmemory_malloc_default().
   By default in XEP the C++ new operator also uses this memory. */
DEFAULT_HEAP_MEMORY_SIZE = DEFINED(DEFAULT_HEAP_MEMORY_SIZE) ? DEFAULT_HEAP_MEMORY_SIZE : 256K;

/* Size of .slow_heap_memory section.
   This memory section is used by xtmemory_malloc_slow().*/
SLOW_HEAP_MEMORY_SIZE = DEFINED(SLOW_HEAP_MEMORY_SIZE) ? SLOW_HEAP_MEMORY_SIZE : 2M;

/* Size of .fast_heap_memory section.
   This memory section is used by xtmemory_malloc_fast(). */
FAST_HEAP_MEMORY_SIZE = DEFINED(FAST_HEAP_MEMORY_SIZE) ? FAST_HEAP_MEMORY_SIZE : 32K;

/* The stack size used by the application.
   When FreeRTOS is used, only function calls and local variables allocated
   before starting FreeRTOS tasks needs stack space.
   Stack space for FreeRTOS tasks are allocated when creating the tasks. */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : 3K;

/* The heap size used by the application.
   This heap space is only used by malloc/free. Normally xtmemory_malloc_....
   functions should be used to allocate memory. */
HEAP_SIZE = DEFINED(HEAP_SIZE) ? HEAP_SIZE : 2K;

APP_STORAGE_SIZE = DEFINED(APP_STORAGE_SIZE) ? APP_STORAGE_SIZE : 96K;