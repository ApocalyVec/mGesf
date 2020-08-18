Hardware Abstraction Layer - HAL API {#xep_hal_api}
====================================

XEP HAL is an Hardware Abstracion Layer used in XeThru Radar embedded modules.
The HAL is modular and can be extended or modified to support other
PCB boards or MCUs.

For Atmel Smart ARM microcontrollers, Atmel Software Framework and Atmel Software Package (Softpack) is used.

Peripherals supported
---------------------

- IO pins available on the 16 pin XeThru connector
- IO pins to/from the X4 Radar IC
- RGB LED on the Radar Module
- SPI communication to/from the X4 Radar IC
- UART/USB communication to host

API documentation
-----------------

API functions and macros are documented in xep_hal.h.

Tweaks for hardware platforms using ASF
--------------------------------------------

### No source files except in HAL should include "asf.h" or "chip.h".

No ASF or Softpack functions should be called other than in HAL. If there is need for a
function implemented in ASF or Softpack, it should be implemented through HAL.

Include of "asf.h" and "chip.h" will probably trigger compiler warnings.

### Using types or functions that are defined in arm-math.h ###

Most definitions and functions defined in arm_math.h should be defined in
xt_XEP_HAL.h. If you need access to definitions and/or functions not defined in
xt_XEP_HAL.h, you should consider implementing them in the xt_XEP_HAL instead
of using arm-math.h. If it is not possible to implement them in xt_XEP_HAL and
you have to include the arm-math.h header file, including arm-math.h will
generate warnings for cast alignment. To set the compiler to ignore this waring,
give GCC instructions to ignore this like below, when including the header file:

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    #include <arm_math.h>
    #pragma GCC diagnostic pop
