################################################################################
# Determine platform and set platform dependant variables and commands
################################################################################
ifdef ComSpec
    $(info On a Windows system using cmd.exe)
    SHELL := cmd.exe
    PATHSEP=$(strip \\)
    FIXPATH = $(subst /,\,$1)
    CP = COPY
    RM = DEL /Q
    RM_RECURSIVE = DEL /S /Q
    make_dir = if not exist $(subst /,\\,$(1)) mkdir $(subst /,\\,$(1))
    CAT := type
    GCC = "arm-none-eabi-gcc.exe"
    CXX = "arm-none-eabi-g++.exe"
    AR = "arm-none-eabi-gcc-ar.exe"
    OBJCOPY = "arm-none-eabi-objcopy.exe"
    OBJDUMP = "arm-none-eabi-objdump.exe"
    SIZE = "arm-none-eabi-size.exe"
else
    $(info On a POSIX like system)
    PATHSEP=$(strip /)
    FIXPATH = $1
    CP = cp
    RM = rm
    RM_RECURSIVE = rm -r
    make_dir = mkdir -p $(1)
    CAT := cat
    GCC = arm-none-eabi-gcc
    CXX = arm-none-eabi-g++
    AR = arm-none-eabi-gcc-ar
    OBJCOPY = arm-none-eabi-objcopy
    OBJDUMP = arm-none-eabi-objdump
    SIZE = arm-none-eabi-size
endif

$(info PATHSEP: $(PATHSEP))
## Get project version from build.properties file
VERSION := $(subst version=,,$(filter version=%,$(shell $(CAT) $(call FIXPATH,$(TOP_LEVEL_DIR)/build.properties))))

$(info Building Project "$(PRODUCT_NAME)_$(HW_PLATFORM)" version "$(VERSION)")
$(info SHELL: $(SHELL))
$(info GCC: $(GCC))

################################################################################
# The source
################################################################################

VPATH += $(TOP_LEVEL_DIR)/src

################################################################################
# add xtSerial
PATH_TO_XTSERIAL = $(TOP_LEVEL_DIR)/../xtSerial

VPATH += $(PATH_TO_XTSERIAL)

################################################################################
# add ASF
ASF_VERSION = v3.32.0.48
PATH_TO_ASF = $(TOP_LEVEL_DIR)/../AtmelSoftwareFramework/$(ASF_VERSION)

VPATH += $(PATH_TO_ASF)

################################################################################
# add FreeRTOS
FREERTOS_VERSION = v9.0.0
PATH_TO_FREERTOS = $(TOP_LEVEL_DIR)/../FreeRTOS

VPATH += $(PATH_TO_FREERTOS)


$(info VPATH: $(VPATH))


C_SRCS += \
    main.c \
    FreeRTOS_hooks.c \
	Dispatch/xep_dispatch.c \
    Dispatch/xep_dispatch_messages.c \
    Dispatch/memorypool.c \
    Dispatch/memorypoolset.c \
    System/task_monitor.c \
    System/Housekeeping/xthousekeeping.c \
    System/File/xfs.c \
    System/File/crc.c \
    System/File/xtfile.c \
    HostCom/task_hostcom.c \
    Radar/task_radar.c \
    XDriver/x4driver.c \
    XEPA/xep_application.c \
    XEPA/xep_application_mcp_callbacks.c \
    XEPA/module_ui.c \
    Tools/xtsemaphore_xep.c \
    Tools/xtmemory_xep.c \
    Tools/xttoolbox.c \
    hal/xt_new_delete.cpp \

C_XTSERIAL_SRCS += \
	src/protocol.c \
	src/protocol_target.c \
	src/protocol_parser.c \

C_HAL_SRCS += \
    hal/$(HW_PLATFORM)/xt_config.c \
    hal/$(HW_PLATFORM)/xt_system.c \
    hal/$(HW_PLATFORM)/xt_selftest.c \
    hal/$(HW_PLATFORM)/xtio_gpio.c \
    hal/$(HW_PLATFORM)/xtio_led.c \
    hal/$(HW_PLATFORM)/xt_mcu_handlers.c \
    hal/$(HW_PLATFORM)/syscalls.c \
    hal/$(HW_PLATFORM)/xtio_serial_com.c \
    hal/$(HW_PLATFORM)/startup_sams70.c \
    hal/$(HW_PLATFORM)/system_sams70.c \
    hal/$(HW_PLATFORM)/xtio_usb.c \
    hal/$(HW_PLATFORM)/board_lowlevel.c \
    hal/$(HW_PLATFORM)/board_memories.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/mpu.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/pio.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/pio_it.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/pmc.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/wdt.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/qspi.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/qspi_dma.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/rstc.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/timetick.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/usart.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/usart_dma.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/xdmac.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/xdmad.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/xdma_hardware_interface.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/flashd.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/efc.c \
    hal/$(HW_PLATFORM)/libraries/libchip/source/twi.c \
    hal/$(HW_PLATFORM)/USBDDriverDescriptors.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/cdc-serial/CDCDSerialDriver.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/core/USBD.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/core/USBDDriver.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/cdc-serial/CDCDSerial.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/cdc-serial/CDCDSerial_Callbacks.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/cdc-serial/CDCDSerialPort.c \
    hal/$(HW_PLATFORM)/libraries/libusb/common/cdc/CDCLineCoding.c \
    hal/$(HW_PLATFORM)/libraries/libusb/common/core/USBD_HAL.c \
    hal/$(HW_PLATFORM)/libraries/libusb/common/core/USBDescriptors.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/core/USBDCallbacks_Suspended.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/core/USBDCallbacks_Resumed.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/core/USBDCallbacks.c \
    hal/$(HW_PLATFORM)/libraries/libusb/device/core/USBDDriverCallbacks.c \
    hal/$(HW_PLATFORM)/libraries/libusb/common/core/USBRequests.c \
    hal/radar_hal.c \
    hal/$(HW_PLATFORM)/xtio_spi.c \
    hal/$(HW_PLATFORM)/xtio_uart.c \
    hal/$(HW_PLATFORM)/xtio_i2c.c \
    hal/$(HW_PLATFORM)/xtio_flash.c \
    

##    hal/$(HW_PLATFORM)/xtio_extram.c \


C_ASF_SRCS += \
##    common/services/clock/sams70/sysclk.c \
##    common/services/delay/sam/cycle_counter.c \
##    common/services/serial/usart_serial.c \
##    common/services/sleepmgr/sam/sleepmgr.c \
##    common/utils/interrupt/interrupt_sam_nvic.c \
##    sam/drivers/ebi/smc/smc.c \
##    sam/drivers/efc/efc.c \
##    sam/drivers/gpbr/gpbr.c \
##    sam/drivers/xdmac/xdmac.c \
##    sam/drivers/pio/pio.c \
##    sam/drivers/pio/pio_handler.c \
##    sam/drivers/pmc/pmc.c \
##    sam/drivers/pmc/sleep.c \
##    sam/drivers/rstc/rstc.c \
##    sam/drivers/supc/supc.c \
##    sam/drivers/twihs/twihs.c \
##    sam/drivers/uart/uart.c \
##    sam/drivers/usart/usart.c \
##    sam/drivers/wdt/wdt.c \
##    sam/services/flash_efc/flash_efc.c \
##    sam/utils/cmsis/sams70/source/templates/system_sams70.c \
##    sam/drivers/mpu/mpu.c \
##    sam/drivers/tc/tc.c \
##    common/services/usb/class/cdc/device/udi_cdc.c \
##    common/services/usb/class/cdc/device/udi_cdc_desc.c \
##    common/services/usb/udc/udc.c \


C_FREERTOS_SRCS += \
    $(FREERTOS_VERSION)/Source/croutine.c \
    $(FREERTOS_VERSION)/Source/list.c \
    $(FREERTOS_VERSION)/Source/queue.c \
    $(FREERTOS_VERSION)/Source/tasks.c \
    $(FREERTOS_VERSION)/Source/timers.c \
    $(FREERTOS_VERSION)/Source/portable/GCC/ARM_CM7/r0p1/port.c \
    hal/$(HW_PLATFORM)/FreeRTOS/heap_4_default.c \
    hal/$(HW_PLATFORM)/FreeRTOS/heap_4_slow.c \
    hal/$(HW_PLATFORM)/FreeRTOS/heap_4_fast.c \

##    hal/$(HW_PLATFORM)/FreeRTOS/freertos_xdma.c \

###    $(FREERTOS_VERSION)/Source/FreeRTOS_CLI.c \
##    hal/$(HW_PLATFORM)/FreeRTOS/freertos_usart_serial.c \
##    hal/$(HW_PLATFORM)/FreeRTOS/freertos_peripheral_control.c \


INCLUDES = \
    -I"$(TOP_LEVEL_DIR)/src" \
    -I"$(TOP_LEVEL_DIR)/src/hal" \
    -I"$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)" \
    -I"$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/FreeRTOS" \
    -I"$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/libraries/libchip" \
    -I"$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/libraries/libchip/include" \
    -I"$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/libraries/libchip/include/cmsis/CMSIS/Include" \
    -I"$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/libraries/libchip/include/sams70" \
    -I"$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/libraries/libboard" \
    -I"$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/utils" \
    -I"$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/libraries/libusb/include" \
	-I"$(TOP_LEVEL_DIR)/src/Dispatch" \
	-I"$(TOP_LEVEL_DIR)/src/XEPA" \
	-I"$(TOP_LEVEL_DIR)/src/System" \
	-I"$(TOP_LEVEL_DIR)/src/System/File" \
	-I"$(TOP_LEVEL_DIR)/src/HostCom" \
	-I"$(TOP_LEVEL_DIR)/src/Radar" \
    -I"$(TOP_LEVEL_DIR)/src/XDriver" \
	-I"$(TOP_LEVEL_DIR)/src/Tools" \
    -I"$(TOP_LEVEL_DIR)/include" \

INCLUDES += \
	-I"$(PATH_TO_XTSERIAL)/src/" \

ASF_INCLUDES += \
##    -I"sam/utils" \
##    -I"common/utils" \
##    -I"sam/utils/preprocessor" \
##    -I"sam/utils/header_files" \
##    -I"common/services/delay" \
##    -I"common/services/clock" \
##    -I"sam/drivers/pmc" \
##    -I"sam/drivers/efc" \
##    -I"sam/services/flash_efc" \
##    -I"sam/drivers/gpbr" \
##    -I"common/services/ioport/sam" \
##    -I"common/services/ioport" \
##    -I"sam/drivers/xdmac" \
##    -I"sam/drivers/pio" \
##    -I"sam/drivers/rstc" \
##    -I"sam/utils/fpu" \
##    -I"sam/utils/cmsis/sams70/include" \
##    -I"sam/utils/cmsis/sams70/source/templates" \
##    -I"sam/drivers/ebi/smc" \
##    -I"sam/drivers/qspi" \
##    -I"sam/drivers/supc" \
##    -I"common/services/twi" \
##    -I"sam/drivers/twihs" \
##    -I"sam/drivers/uart" \
##    -I"common/services/serial" \
##    -I"sam/drivers/usart" \
##    -I"sam/drivers/wdt" \
##    -I"thirdparty/CMSIS/Include" \
##    -I"sam/utils/cmsis/samv70/include" \
##    -I"common/services/usb" \
##    -I"common/services/usb/udc" \
##    -I"common/services/usb/class/cdc" \
##    -I"common/services/usb/class/cdc/device" \
##    -I"sam/drivers/usbhs" \
##    -I"common/services/sleepmgr" \
##    -I"common/services/sleepmgr/sam" \
##    -I"common/services/freertos/sam" \
##    -I"sam/drivers/mpu" \
##    -I"common/services/delay/sam" \
##	-I"sam/drivers/sdramc" \


## INCLUDES += $(patsubst -I"%",-I"$(PATH_TO_ASF)/%", $(ASF_INCLUDES))

INCLUDES += \
    -I"$(PATH_TO_FREERTOS)/$(FREERTOS_VERSION)/Source/include" \
    -I"$(PATH_TO_FREERTOS)/$(FREERTOS_VERSION)/Source/portable/GCC/ARM_CM7/r0p1" \


## Set up different categories of source
C_SOURCES := $(filter %.c ,$(C_SRCS))
C_XTSERIAL_SOURCES := $(filter %.c ,$(C_XTSERIAL_SRCS))
C_HAL_SOURCES := $(filter %.c ,$(C_HAL_SRCS))
C_ASF_SOURCES := $(filter %.c ,$(C_ASF_SRCS))
C_FREERTOS_SOURCES := $(filter %.c ,$(C_FREERTOS_SRCS))
CPP_SOURCES := $(filter %.cpp ,$(C_SRCS))

## Compute objects files from source files
C_OBJECTS := $(patsubst %.c,$(TOP_LEVEL_DIR)/_build/%.o, $(C_SOURCES))
C_XTSERIAL_OBJECTS := $(patsubst %.c,$(TOP_LEVEL_DIR)/_build/xtSerial/%.o, $(C_XTSERIAL_SOURCES))
C_HAL_OBJECTS := $(patsubst %.c,$(TOP_LEVEL_DIR)/_build/%.o, $(C_HAL_SOURCES))
C_ASF_OBJECTS := $(patsubst %.c,$(TOP_LEVEL_DIR)/_build/ASF/%.o, $(C_ASF_SOURCES))
C_FREERTOS_OBJECTS := $(patsubst %.c,$(TOP_LEVEL_DIR)/_build/FreeRTOS/%.o, $(C_FREERTOS_SOURCES))
CPP_OBJECTS := $(patsubst %.cpp,$(TOP_LEVEL_DIR)/_build/%.o, $(CPP_SOURCES))
PARAM_OBJECTS = $(patsubst ../%.c,$(TOP_LEVEL_DIR)/_build/%.o, $(PARAM_SRC_FILES))

## Compute dependency files
C_DEPS := $(patsubst %.o,%.d, $(C_OBJECTS))
C_DEPS += $(patsubst %.o,%.d, $(C_XTSERIAL_OBJECTS))
C_DEPS += $(patsubst %.o,%.d, $(C_HAL_OBJECTS))
C_DEPS += $(patsubst %.o,%.d, $(C_ASF_OBJECTS))
C_DEPS += $(patsubst %.o,%.d, $(C_FREERTOS_OBJECTS))
C_DEPS += $(patsubst %.o,%.d, $(CPP_OBJECTS))

CMSIS_LIB = $(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/libraries/libchip/include/cmsis/CMSIS/Lib/ARM/arm_cortexM7lfsp_math.lib

## Set up compiler flags
COMMON_DEFINES = -march=armv7e-m -mthumb -DENABLE_TCM -D__SAMS70Q20__ -DARM_MATH_CM7=true -DBOARD=USER_BOARD -DFREERTOS_USED -DMPU6500 -DEMPL_TARGET_SAM7 -D__FPU_PRESENT=1  -DBOARD_X4M0x_SAMS70 -D__ARMCC_VERSION=0

GCC_DEFINES = -Dprintf=iprintf -Dscanf=iscanf -DVSDM_HW_MODULE -DVSDM_FW_DEV -DVSDM_HW_PMIC_YES
CXX_DEFINES =

GCC_FLAGS = -std=gnu99
CXX_FLAGS = -fno-rtti -fno-exceptions -std=gnu++11
LD_FLAGS = -mfpu=fpv5-sp-d16 -mfloat-abi=hard -L$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM)/libraries/libchip/include/cmsis/CMSIS/Lib/ARM

COMMON_COMPILER_FLAGS = -march=armv7e-m -fdata-sections -ffunction-sections -mlong-calls \
    -c -pipe -fno-strict-aliasing -ffunction-sections -fdata-sections \
    --param max-inline-insns-single=500 -mfpu=fpv5-sp-d16 -mfloat-abi=hard \
    -fsingle-precision-constant -MD -MP \
    -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"

C_COMPILER_WARNINGS = \
    -Wall -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration \
    -Wpointer-arith -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain -Wparentheses \
    -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal \
    -Wshadow -Wbad-function-cast -Wwrite-strings -Wsign-compare -Waggregate-return -Wmissing-declarations \
    -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wnested-externs \
    -Winline -Wunreachable-code -Wredundant-decls -Wcast-align -Wundef

CXX_COMPILER_WARNINGS = \
    -Wall -Werror-implicit-function-declaration \
    -Wpointer-arith -Wchar-subscripts -Wcomment -Wformat=2 -Wmain -Wparentheses \
    -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal \
    -Wshadow -Wwrite-strings -Wsign-compare -Waggregate-return -Wmissing-declarations \
    -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked \
    -Winline -Wlong-long -Wunreachable-code -Wredundant-decls -Wcast-align -Wundef


DEFAULT_C_FLAGS = $(COMMON_COMPILER_FLAGS) $(C_COMPILER_WARNINGS)

DEFAULT_CXX_FLAGS = $(COMMON_COMPILER_FLAGS) $(CXX_COMPILER_WARNINGS)

ASF_C_FLAGS = $(COMMON_COMPILER_FLAGS) $(C_COMPILER_WARNINGS) \
                   -Wno-redundant-decls -Wno-cast-align -Wno-undef \
                   -Wno-unused-but-set-variable

FREERTOS_C_FLAGS = $(COMMON_COMPILER_FLAGS) $(C_COMPILER_WARNINGS) \
                   -Wno-redundant-decls -Wno-nested-externs


## Set up special source objects
PARAMS_IN_SOURCE_SRC = \
###	../xtEmbedded/xteBoard/X2M04/application/RO_Parameters_in_source.c \
###	../xtEmbedded/xteBoard/X2M04/application/RW_Parameters_in_source.c


PARAMS_IN_SOURCE_OBJECTS := $(patsubst ../%.c,_build/%.o, $(PARAMS_IN_SOURCE_SRC))

PARAMS_IN_FLASH_SRC = \
###	../xtEmbedded/xteBoard/X2M04/application/RO_Parameters_in_flash.c \
###	../xtEmbedded/xteBoard/X2M04/application/RW_Parameters_in_flash.c

PARAMS_IN_FLASH_OBJECTS := $(patsubst ../%.c,_build/%.o, $(PARAMS_IN_FLASH_SRC))


ALL_LOCAL_C_OBJECTS = $(C_OBJECTS) $(PARAMS_IN_FLASH_OBJECTS) $(PARAMS_IN_SOURCE_OBJECTS)

FILES_TO_CLEAN = $(C_OBJECTS) $(C_XTESRIAL_OBJECTS) \
                     $(C_FREERTOS_OBJECTS) $(CPP_OBJECTS) \
                     $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).map $(PRODUCT_NAME)__$(HW_PLATFORM)_4bl.cout \
                      $(C_DEPS)

################################################################################
# Rules
################################################################################

## Check which version of GCC is in use
GCC_VERSION = $(wordlist 1, 10, $(shell $(GCC) --version))
$(info	GCC Version: $(GCC_VERSION))

## Compile targets for objects in this repository
$(ALL_LOCAL_C_OBJECTS): $(TOP_LEVEL_DIR)/_build/%.o: %.c Makefile
	@echo Building file : $<
	@$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : "$(GCC_VERSION)"
	$(GCC) -x c $(DEBUG_FLAGS) $(COMMON_DEFINES) $(GCC_DEFINES)  $(INCLUDES)  $(DEFAULT_C_FLAGS) $(GCC_FLAGS) -o "$@" "$<"
	@echo Finished building: $<

$(C_HAL_OBJECTS): $(TOP_LEVEL_DIR)/_build/%.o: %.c Makefile
	@echo Building file : $<
	@$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : "$(GCC_VERSION)"
	$(GCC) -x c $(DEBUG_FLAGS) $(COMMON_DEFINES) $(GCC_DEFINES)  $(INCLUDES)  $(ASF_C_FLAGS) $(GCC_FLAGS) -o "$@" "$<"
	@echo Finished building: $<

## build C source from other repositories
$(C_XTSERIAL_OBJECTS): $(TOP_LEVEL_DIR)/_build/xtSerial/%.o: %.c Makefile
	@echo Building file : $<
	$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : 4.8.4
	$(GCC) -x c $(DEBUG_FLAGS) $(COMMON_DEFINES) $(GCC_DEFINES)  $(INCLUDES)  $(DEFAULT_C_FLAGS) $(GCC_FLAGS) -o "$@" "$<"
	@echo Finished building: $<

$(C_ASF_OBJECTS): $(TOP_LEVEL_DIR)/_build/ASF/%.o: %.c Makefile
	@echo Building file : $<
	@$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : "$(GCC_VERSION)"
	$(GCC) -x c $(DEBUG_FLAGS) $(COMMON_DEFINES) $(GCC_DEFINES)  $(INCLUDES)  $(ASF_C_FLAGS) $(GCC_FLAGS) -o "$@" "$<"
	@echo Finished building: $<

$(C_FREERTOS_OBJECTS): $(TOP_LEVEL_DIR)/_build/FreeRTOS/%.o: %.c Makefile
	@echo Building file : $<
	@$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : "$(GCC_VERSION)"
	$(GCC) -x c $(DEBUG_FLAGS) $(COMMON_DEFINES) $(GCC_DEFINES)  $(INCLUDES)  $(FREERTOS_C_FLAGS) $(GCC_FLAGS) -o "$@" "$<"
	@echo Finished building: $<

## build CPP source
$(CPP_OBJECTS): $(TOP_LEVEL_DIR)/_build/%.o: %.cpp Makefile
	@echo Building file: $<
	@$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : "$(GCC_VERSION)"
	$(CXX) $(DEBUG_FLAGS) $(COMMON_DEFINES) $(CXX_DEFINES) $(INCLUDES) $(DEFAULT_CXX_FLAGS) $(CXX_FLAGS) -o "$@" "$<"
	@echo Finished building: $<


ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

## Targets for different versions of binaries

$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf: $(C_OBJECTS) $(C_HAL_OBJECTS) $(C_XTSERIAL_OBJECTS) $(C_ASF_OBJECTS) $(C_FREERTOS_OBJECTS) $(CPP_OBJECTS) $(PARAMS_IN_SOURCE_OBJECTS)
	@echo Building target: $@
	$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU Linker : 4.8.4
	$(CXX) $^ -march=armv7e-m -mthumb -Wl,-Map=$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).map $(LD_FLAGS) -Wl,--start-group $(CMSIS_LIB) -lm -Wl,--end-group -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--cref -Wl,--undefined=uxTopUsedPriority -mthumb -L$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM) -Tapplication_without_bl.ld -o $@
	@echo Finished building target: $@

$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).bin: $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).hex: $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $< $@

$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).eep: $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary $< $@ || exit 0

$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).lss: $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf
	$(OBJDUMP) -h -S $< > $@

$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).srec: $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf
	$(OBJCOPY) -O srec -R .eeprom -R .fuse -R .lock -R .signature  $< $@

$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM)_4bl.elf: $(C_OBJECTS) $(C_HAL_OBJECTS) $(C_XTSERIAL_OBJECTS) $(C_ASF_OBJECTS) $(C_FREERTOS_OBJECTS) $(CPP_OBJECTS) $(PARAMS_IN_SOURCE_OBJECTS)
	@echo Building target: $@
	$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU Linker : 4.8.4
	$(CXX) $^ -march=armv7e-m -mthumb -Wl,-Map=$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM)_4bl.map $(LD_FLAGS) -Wl,--start-group $(CMSIS_LIB) -lm -Wl,--end-group -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--cref -Wl,--undefined=uxTopUsedPriority -mthumb -L$(TOP_LEVEL_DIR)/src/hal/$(HW_PLATFORM) -Tapplication_with_bl_2mb.ld -o $@
	@echo Finished building target: $@

$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM)_4bl.hex: $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM)_4bl.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $< $@

## Build as lib
$(TARGET_FOLDER)/lib$(PRODUCT_NAME)_$(HW_PLATFORM).a: $(C_OBJECTS) $(C_HAL_OBJECTS) $(C_XTSERIAL_OBJECTS) $(C_ASF_OBJECTS) $(C_FREERTOS_OBJECTS) $(CPP_OBJECTS)
	@echo Building target: $@
	$(AR) crs $@ $(C_OBJECTS) $(C_HAL_OBJECTS) $(C_XTSERIAL_OBJECTS) $(C_ASF_OBJECTS) $(C_FREERTOS_OBJECTS) $(CPP_OBJECTS)


.PHONY: show_size

show_size: $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf
	$(SIZE) $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf

# Other Targets

doc: export PROJECT_VERSION = $(VERSION)
doc:
	cd $(call FIXPATH,$(TOP_LEVEL_DIR)/doc) && \
	doxygen $(call FIXPATH,config/Doxyfile) && \
	$(CP) $(call FIXPATH,config/css/*) $(call FIXPATH,output/html/)

doc_clean:
	-$(RM_RECURSIVE) $(call FIXPATH,$(TOP_LEVEL_DIR)/doc/output)

clean:
	-$(RM) $(call FIXPATH,$(FILES_TO_CLEAN))
	-$(RM_RECURSIVE) $(call FIXPATH,$(TOP_LEVEL_DIR)/_build)
	-$(RM) $(call FIXPATH,$(TARGETS))

info:


.PHONY: doc doc_clean clean info
