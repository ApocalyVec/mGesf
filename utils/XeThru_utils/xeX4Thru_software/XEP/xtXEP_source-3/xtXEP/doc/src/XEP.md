XeThru Embedded Platform - XEP  {#mainpage}
==============================

XeThru Embedded Platform (XEP) provides the low level building blocks of a XeThru
Embedded application and serves as a firmware platform for the XeThru module products. It provides all necessary features to interact with the XeThru radar X4, running application code, and interact with a host through serial communication.

XEP can be used standalone or as a library. Standalone it configures the radar and provides common interaction such as serial communication bridge to the X4Driver. It will not do any actions by default, but wait for instructions from host. The host can typically use the serial communication, i.e. **Module Connector**, to send instructions such as setting X4Driver configuration and start radar frame streaming. This will in turn result in streaming of radar data to the host.

XEP does not come with any configuration specific to the module product, but only holds generic features. Application or product specific configuration is done when adding a product layer, using XEP as a library. In this case, XEP provides an interface and a set of weak methods that can be overridden by the custom application, in order to configure XEP and interact with XEP features.

## XEP Modules

XEP consists of several modules:

- [XEP top level application layer and interface - XEPA](@ref xep_application)
- [Hardware Abstraction Layer - hal](@ref xep_hal_api)
- [Radar Hardware Abstraction Layer - radar_hal](@ref xep_radar_hal_api)
- [XeThru X4 radar driver - X4Driver](@ref xep_x4driver)
- [Host communication - HostCom](@ref xep_hostcom)
- [Dispatcher for inter-task message distribution - Dispatch](@ref xep_dispatch)
- [System health monitoring](@ref xep_monitor)
- [File system](@ref xep_filesystem)

## XEP Modules Overview

<CENTER>
\dotfile XEP_block_diagram.dot
</CENTER>

## Version History
See [XEP Version History](@ref xep_version_history) for an outline of the changes.