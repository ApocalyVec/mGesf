# Change Log
All notable changes to this project will be documented in this file.

## 3.4.7 - 2018-07-31
### Fixed
    Bug with communication buffer cache invalidation
    Bug in UART receive crash after 1024B
    Bug in buffer overflow handling for radar buffers

### Changed
    Flash writing memory optimizations

## 3.3.0 - 2018-05-30
### Fixed
    USB enumeration issues
    Bug in X4Driver_init causing crash
    Bug in X4 frame overflow handling
    Bug in Certification LED behaviour after protocol activation
    Bug with memorypools causing memory corruption when full

### Added
    Implemented missing get calls in X4Driver API
    Adding unique ID to get_system_info
    Adding build for bootloader

## 3.0.0 - 2017-12-18
### Added
- USART communication mode
- Certification modes through protocol interface
- Secondary interface as debug channel

### Changed
- API change for dispatching messages and route messages to correct interface

## 2.2.0 - 2017-11-23
### Fixed
- Bug in safe mode reset counting

### Added
- Hardware specific configuration from user signature area

## 2.1.8 - 2017-10-31
### Fixed
- Bug with some frame areas giving a corrupt RF sample at end

## 2.1.7 - 2017-09-22
### Fixed
- Bug in x4driver clock initialization not disabling internal decoupling

## 2.1.6 - 2017-09-15
### Fixed
- Bug in x4driver causing issues when using multiple radars
- Bug in x4driver that could lead to corrupted register reads
- Wrong definition of X4 register MISC_CTRL
- Bug in init test of X4 communication

## 2.1.5 - 2017-06-26
### Added
- Monitoring of host communication task

### Changed
- Increased application task timeout from 1 to 5 seconds

## 2.1.4 - 2017-06-23
### Added
- Ping response indicates whether or not in safe mode

## 2.1.3 - 2017-06-20
### Fixed
- Workaround for duplicate radar frames

## 2.1.1 - 2017-06-16
### Added
- Bypass oscillator mode for updated hardware revision 

## 2.1.0 - 2017-06-12
### Changed
- Correcting Q data by negation

### Added
- Certification test modes

## 2.0.0 - 2017-06-02
### Changed
- Naming of function call corrected from x4driver_get_downconvertion to x4driver_get_downconversion

### Added
- Calculation of CPU load by measuring the time waiting in the FreeRTOS idle task. Pollable by xt_get_cpu_load.
- Host command for formatting the application data file system in flash. 
- Linker script for 2MB flash size; application_without_bl_2mb.ld. To use it, change xep.make to use it when invoking the linker. 

### Fixed
- Frame normalizations bug. The frame data is now scaled correctly between -1 and 1.
- Upper band pulse generator downconvertion filter coefficients. Previous versions filtered away signal energy, resulting in lower SNR.

## 1.1.4-beta - 2017-05-29
### Changed
- Dispatch queue size increased
- Optimizations for X4 frame read

## 1.1.3 - 2017-05-22
### Changed
- Start bootloader key to avoid unintentional downgrades

### Fixed
- Mutex for flash writing

## 1.1.2-beta - 2017-05-12
### Added
- Enabling re-inforced WDT
- Support for 2MB (S70Q21) MCU
- File system API

### Changed
- Allocating location for file system

### Fixed
- Initializing TCM early
- Test system fixes 

## 1.1.1-beta - 2017-05-03
### Added
- Trigger crash dump for failing malloc

## 1.1.0-beta - 2017-05-03
### Added
- File system for application data
- Using idle sleep when possible to conserve power

### Changed
- Enabled TCM and Data Cache for optimal performance
- Changed crash dump storage location to use file system

### Fixed
- Can compile with optimization, release build

## 1.0.7-beta - 2017-04-17
### Added
- Adjustable baudrate for UART

### Changed
- Using no-escape packets for baseband data
- Increased buffer size for host communication

### Fixed
- Possible lock in DMA

## 1.0.6-beta - 2017-03-08
### Changed
- Crash dump improvements

## 1.0.5 - 2017-03-06
### Added
- Crash dump is stored to flash on critical system failures
- Mechanism to reset in case of system failure

## 1.0.3-rc.2 - 2017-02-24
### Changed
- Improvements on X4 internal firmware

## 1.0.3-rc.1 - 2017-02-23
### Changed
- Set and get FPS function parameter from integer to floating point
- Improved X4driver limits

### Fixed
- Memory alignment and DMA operation

## 1.0.1-beta - 2017-02-15
### Fixed
- Possible corruption of response in case of busy communication

## 1.0.0-beta - 2017-02-10
### Added
- First release of XEP
