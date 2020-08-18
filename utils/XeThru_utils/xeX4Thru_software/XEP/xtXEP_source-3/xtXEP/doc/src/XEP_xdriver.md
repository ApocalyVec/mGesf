# XeThru X4 radar driver - X4Driver {#xep_x4driver}

X4Driver is a driver module used to interact with the XeThru X4 radar chip. X4Driver is implemented as a platform independent component, that is designed to be portable to a variety of platforms. It contains all elements needed to initialize, configure and read data from the X4 radar chip.

Most of the X4Driver methods are used to configure the X4 chip directly. Reference on the different X4 registers and values can be found in the X4 datasheet.


## X4Driver overall features

- X4 internal MCU firmware
- Low level configuration of X4
- Higher level abstract methods for X4

## Typical use of X4Driver

Here is a simple example on how to use X4Driver to configure the XeThru X4 radar chip, and start radar sweeping:

```C
x4driver_init(x4driver);
x4driver_set_dac_min(x4driver, 500);
x4driver_set_dac_max(x4driver, 1500);
x4driver_set_iterations(x4driver, 16);
x4driver_set_pulses_per_step(x4driver, 10);
x4driver_set_downconversion(x4driver, 1); // Radar data as downconverted baseband IQ, not RF.
x4driver_set_frame_area_offset(x4driver, 0.6); // Given by module HW. Makes frame_area start = 0 at front of module.
x4driver_set_frame_area(x4driver, 0.5, 4.0); // Observe from 0.5m to 4.0m.
x4driver_set_fps(x4driver, 20); // Generate 20 frames per second
```


## API documentation

API functions and macros are documented in x4driver.h.

## XEP use of X4Driver

X4Driver is an integral part of the XeThru Embedded Platform, providing XEP with a full-featured direct access to the X4 radar chip.
In XEP, this is typically used in task_radar.c, which does the initial setup of X4Driver and also facilitates the asynchronous access to X4Driver, such as event-driven read from X4. Also, task_radar.c contains the XEP port of X4Driver dependencies, e.g. semaphores, timers etc.
Another use of X4Driver in XEP is demonstrated in xep_application_mcp_callbacks.c, where all external serial host commands are implemented.


## Custom port of X4Driver

X4Driver is implemented in a way that enables it to be used on any device or platform providing the necessary dependencies. X4Driver itself is written in C, with a small set of abstractions that needs to be implemented specifically for the running platform.

The features that need to be ported to a custom platform are:

- X4DriverLock_t: Semaphore mechanism
- X4DriverTimer_t: Timers used by X4Driver
- X4DriverCallbacks_t: Typically hardware callbacks, e.g. SPI and physical pin use

For examples on how this is implemented for XEP, have a look at task_radar.c.

The following code illustrates how this is done in task_radar.c, in the task_radar_init() method:

@snippet Radar/task_radar.c X4Driver Platform Dependencies
