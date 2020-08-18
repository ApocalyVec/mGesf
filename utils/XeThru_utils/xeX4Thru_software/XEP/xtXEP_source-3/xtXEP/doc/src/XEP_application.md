# XEP Application (XEPA) {#xep_application}

XEP Application is the top level that ties all the different parts of XEP together.

The most common use of XEP is as a library, but it is also possible to execute "bare" XEP for simplistic and testing purposes.

When using XEP as a library, xep_application.c has methods that are implemented as *weak* methods that can be overridden, in order to re-implement the functions suitable for the new application. An example of this can be found in the **X4XEPDistanceDemo** project.

## XEP interface

The top level interface of XEP is given in the header files found in the header folder. The most used interfaces are:

- XEP main interface: xep.h
- XeThru radar chip driver: x4driver.h
- Host commands: xep_dispatch_messages.h
- Module UI methods, e.g. LED control: module_ui.h
