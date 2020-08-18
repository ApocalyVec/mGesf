Module Communication Protocol Wrapper - MCPW  {#mainpage}
============================================

# Introduction

Module Communication Protocol (MCP) is the lowest level communication protocol implementation for the XeThru modules, used on the module FW and XeThru host components, e.g. Module Connector.

For details on MCP format and implementation, look at the [XeThru Module Communication Protocol document](@ref mcp).

Building on MCP, MCP Wrapper (MCPW) goes one step further for embedded host implementation adding a wrapper with convenience methods around the MCP. Compared to Module Connector, which is a more feature rich component with logging and buffering capabilities, MCWP offers a similar interface with only the basic transport methods wrapped in a more easy-to-use interface.

Typical use for MCPW is when implementing a host application communicating with a XeThru module on an embedded target that does not need or have available the Module Connector.

MCPW needs some platform dependent / ported methods, that provide some basic features:
- serial port transport layer
- mutex / synchronization
- data ready callbacks
- delay

Here are the methods that need to be implemented on the target platform:
@snippet generic/src/main.cpp MCPW Platform Methods

In addition, this method must be called when host receives bytes from the XeThru module: mcpw_process_rx_data().

# Example
In the examples folder, there is an example mcpw_demo on how to use MCPW. This example has two modes, one that uses an X4M300 module to provide Presence output, and one that uses an X2M200 module to provide Sleep and Respiration output.

Take a look at generic/src/main.cpp for more details.

Typical console output:

```

$ ./mcpw_demo.exe COM15 X4M300
Starting mcpw_demo_x4m300.
Connecting to XeThru module on COM15.
Starting serial port read thread.
...
Counter=1938, State=Initializing
Counter=1955, State=Initializing
Counter=1972, State=Initializing
Counter=1989, State=Initializing
Counter=2006, State=Initializing
Counter=2023, State=Initializing
Counter=2040, State=Initializing
Counter=2057, State=Initializing
...
Counter=5967, State=No presence
Counter=5984, State=No presence
Counter=6001, State=No presence
Counter=6018, State=No presence
Counter=6035, State=No presence
Counter=6052, State=No presence
Counter=6086, State=Presence, Distance=1.2677
Counter=6103, State=Presence, Distance=0.697288
Counter=6120, State=Presence, Distance=0.713739
Counter=6137, State=Presence, Distance=0.726641
Counter=6154, State=Presence, Distance=0.693994
Counter=6171, State=Presence, Distance=0.679309

```

The following targets are supported.

## Generic
Generic project and makefile, supporting (at least) the following targets:
- Windows
- Raspberry Pi (Raspbian Jessie)

Implemented in generic/src/main.cpp.

To start demo, enter:

`mcpw_demo COM1 x4m300` or `mcpw_demo COM1 x2m200` on Windows

or

`./mcpw_demo /dev/ttyAMA0 x4m300` or `./mcpw_demo /dev/ttyAMA0 x2m200` on Linux

This project also contains serial port implementation for Windows and Linux. These are included for the purpose of example.

## Atmel SAMG55
Atmel Studio project set up for Atmel SAMG55 XPlained Pro. Provides console output through the EDBG serial port from the XPlained USB connector.

- Connect the X4M300 module to the Atmel XPlained board according to connection table
- Open the Atmel Studio project at examples/atmel_samg55/mcpw_demo.cproj
- Build and run the project on an Atmel SAMG55 XPlained board
- To change between X4M300 and X2M200 example, look for switch in `xethru_task()`.

| Function     | X4M300 pin     | XPlained pin   |
| :----------- | :------------- | :------------- |
| VCC (3.3V)   | 1              | VCC            |
| GND          | 2              | GND            |
| Uart         | 3              | PA03           |
| Uart         | 4              | PA04           |

Implemented in atmel_samg55/src/task_xethru.c.
