XEP memory management {#xep_memory}
=====================

Memory regions available
========================

Memory regions are defined in the linker script. The following regions are
defined:

 Name                   | Functionality | How to use | Notes
------------------------|------------------------------------|-------|---------
.bss                    | Uninitialized data | Compiler defined| Zeroed in startup before main()
.data                   | Initialized data | Compiler defined | Copied from Flash in startup before main()
.default_heap_memory    | Heap used by customized FreeRTOS heap_4.c | xtmemory_malloc_default() / xtmemory_free_default(), C++ new / delete, FreeRTOS variables/objects and stacks. Linker script. | Size specified by "variable" in an included linker script.
.slow_heap_memory       | Heap used by customized FreeRTOS heap_4.c | xtmemory_malloc_slow() / xtmemory_free_slow(). Linker script. | Size specified by "variable" in an included linker script. If "slow" memory is not available, objects will be allocated in .default_memory. The xtmemory_malloc_slow() / xtmemory_free_slow() functions will "fall back" to use xtmemory_malloc() / xtmemory_free().
.heap                   | Used by malloc/free | (malloc() / free() ) xtmemory_malloc_bm() / xtmemory_free_bm() | Size is specified by HEAP_SIZE variable in linker script
.stack                  | Used by C-functions before FreeRTOS is started | Compiler defined | Size is specified by HEAP_SIZE variable in linker script. Will not be used by FreeRTOS tasks or functions/interrupts called from/during FreeRTOS tasks
.fast_flash             |  Tightly coupled instruction memory (ITCM) | Linker script | *

[comment]: <> (This is not implemented yet, but will be later: .fast_heap_memory       | Tightly coupled data memory (DTCM) | xtmemory_malloc_fast() / xtmemory_free_fast(). Linker script. | Size specified by set up of TCM. Special buffers/variables placed in this region by linker script or variable attributes. If no is TCM available, objects will be allocated in .default_memory.)