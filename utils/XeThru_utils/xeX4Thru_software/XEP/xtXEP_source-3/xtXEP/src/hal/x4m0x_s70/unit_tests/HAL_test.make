################################################################################
# Determine platform
################################################################################
OS := $(shell uname -o)
$(info OS: $(OS))
ifeq ($(OS), GNU/Linux)
GCC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size
#####GEN_CERT = ../../gencert/gen-cert
#####GEN_CERT_CONF = ../xtProduct/production/common/cert_x2m04.opt
make_dir = mkdir -p $(1)
else # Windows
GCC = "arm-none-eabi-gcc.exe"
CXX = "arm-none-eabi-g++.exe"
AR = "arm-none-eabi-ar.exe"
OBJCOPY = "arm-none-eabi-objcopy.exe"
OBJDUMP = "arm-none-eabi-objdump.exe"
SIZE = "arm-none-eabi-size.exe"
SHELL := cmd.exe
make_dir = if not exist $(subst /,\\,$(1)) mkdir $(subst /,\\,$(1))
#####GEN_CERT = "..\..\gencert\gen-cert.exe"
#####GEN_CERT_CONF = "..\xtProduct\production\common\cert_x2m04.opt"

endif

#SHELL = bash

$(info GCC: $(GCC))

################################################################################
# The source
################################################################################

VPATH += $(TOP_LEVEL_DIR)/src

################################################################################
# add ASF
ASF_VERSION = v3.32.0.48
PATH_TO_ASF = $(TOP_LEVEL_DIR)/../AtmelSoftwareFramework/$(ASF_VERSION)

VPATH += $(PATH_TO_ASF)
$(info VPATH: $(VPATH))

################################################################################
# add FreeRTOS
FREERTOS_VERSION = v9.0.0
PATH_TO_FREERTOS = $(TOP_LEVEL_DIR)/../FreeRTOS

VPATH += $(PATH_TO_FREERTOS)
$(info VPATH: $(VPATH))



C_SRCS += \
    HAL/$(HW_PLATFORM)/unit_tests/HAL_test_main.c \
    FreeRTOS_hooks.c \
    HAL/$(HW_PLATFORM)/HAL_board_init.c \
    HAL/$(HW_PLATFORM)/xtio_gpio.c \
    HAL/$(HW_PLATFORM)/xtio_led.c \
    HAL/$(HW_PLATFORM)/xt_mcu_handlers.c \
	Dispatch/xep_dispatch.c \
	XEPA/module_application_instance.c \
	System/task_system.c \
	HostCom/task_hostcom.c \
	Radar/task_radar.c \
	XDriver/xdriver.c \


C_ASF_SRCS += \
    common/services/clock/samv70/sysclk.c \
    common/services/delay/sam/cycle_counter.c \
    common/services/serial/usart_serial.c \
    common/services/sleepmgr/sam/sleepmgr.c \
    common/utils/interrupt/interrupt_sam_nvic.c \
    sam/drivers/ebi/smc/smc.c \
    sam/drivers/efc/efc.c \
    sam/drivers/gpbr/gpbr.c \
    sam/drivers/xdmac/xdmac.c \
    sam/drivers/pio/pio.c \
    sam/drivers/pio/pio_handler.c \
    sam/drivers/pmc/pmc.c \
    sam/drivers/pmc/sleep.c \
    sam/drivers/rstc/rstc.c \
    sam/drivers/qspi/qspi.c \
    sam/drivers/supc/supc.c \
    sam/drivers/twihs/twihs.c \
    sam/drivers/uart/uart.c \
    sam/drivers/usart/usart.c \
    sam/drivers/wdt/wdt.c \
    sam/services/flash_efc/flash_efc.c \
    sam/utils/cmsis/sams70/source/templates/gcc/startup_sams70.c \
    sam/utils/cmsis/sams70/source/templates/system_sams70.c \
    sam/utils/syscalls/gcc/syscalls.c \
    sam/drivers/mpu/mpu.c \

###    common/services/usb/class/cdc/device/udi_cdc.c \
###    common/services/usb/class/cdc/device/udi_cdc_desc.c \
###    common/services/usb/udc/udc.c \
###    sam/drivers/usbhs/usbhs_device.c \


C_FREERTOS_SRCS += \
    $(FREERTOS_VERSION)/Source/croutine.c \
    $(FREERTOS_VERSION)/Source/list.c \
    $(FREERTOS_VERSION)/Source/portable/GCC/ARM_CM7/r0p1/port.c \
    $(FREERTOS_VERSION)/Source/portable/memmang/heap_4.c \
    $(FREERTOS_VERSION)/Source/queue.c \
    $(FREERTOS_VERSION)/Source/tasks.c \
    $(FREERTOS_VERSION)/Source/timers.c \


###    $(FREERTOS_VERSION)/Source/FreeRTOS_CLI.c \

###../xtEmbedded/xteBackbone/ASF_man/common/services/freertos/samv70/freertos_peripheral_control.c \
###../xtEmbedded/xteBackbone/ASF_man/common/services/freertos/samv70/freertos_spi_master.c \
###../xtEmbedded/xteBackbone/ASF_man/common/services/freertos/samv70/freertos_uart_serial.c \
###../xtEmbedded/xteBackbone/ASF_man/common/services/freertos/samv70/freertos_usart_serial.c \



INCLUDES = \
    -I"$(TOP_LEVEL_DIR)/src" \
    -I"$(TOP_LEVEL_DIR)/src/HAL/$(HW_PLATFORM)" \
	-I"$(TOP_LEVEL_DIR)/src/Dispatch" \
	-I"$(TOP_LEVEL_DIR)/src/XEPA" \
	-I"$(TOP_LEVEL_DIR)/src/System" \
	-I"$(TOP_LEVEL_DIR)/src/HostCom" \
	-I"$(TOP_LEVEL_DIR)/src/Radar" \
	-I"$(TOP_LEVEL_DIR)/src/XDriver" \
    -I"$(TOP_LEVEL_DIR)/include" \



ASF_INCLUDES += \
    -I"sam/utils" \
    -I"common/utils" \
    -I"sam/utils/preprocessor" \
    -I"sam/utils/header_files" \
    -I"common/services/delay" \
    -I"common/services/clock" \
    -I"sam/drivers/pmc" \
    -I"sam/drivers/efc" \
    -I"sam/services/flash_efc" \
    -I"sam/drivers/gpbr" \
    -I"common/services/ioport/sam" \
    -I"common/services/ioport" \
    -I"sam/drivers/xdmac" \
    -I"sam/drivers/pio" \
    -I"sam/drivers/rstc" \
    -I"sam/utils/fpu" \
    -I"sam/utils/cmsis/sams70/include" \
    -I"sam/utils/cmsis/sams70/source/templates" \
    -I"sam/drivers/ebi/smc" \
    -I"sam/drivers/qspi" \
    -I"sam/drivers/supc" \
    -I"common/services/twi" \
    -I"sam/drivers/twihs" \
    -I"sam/drivers/uart" \
    -I"common/services/serial" \
    -I"sam/drivers/usart" \
    -I"sam/drivers/wdt" \
    -I"thirdparty/CMSIS/Include" \
    -I"sam/utils/cmsis/samv70/include" \
    -I"common/services/usb" \
    -I"common/services/usb/udc" \
    -I"common/services/usb/class/cdc" \
    -I"common/services/usb/class/cdc/device" \
    -I"sam/drivers/usbhs" \
    -I"common/services/sleepmgr" \
    -I"common/services/sleepmgr/sam" \
    -I"common/services/freertos/sam" \
    -I"sam/drivers/mpu" \


INCLUDES += $(patsubst -I"%",-I"$(PATH_TO_ASF)/%", $(ASF_INCLUDES))




INCLUDES += \
    -I"$(PATH_TO_FREERTOS)/$(FREERTOS_VERSION)/Source/include" \
    -I"$(PATH_TO_FREERTOS)/$(FREERTOS_VERSION)/Source/portable/GCC/ARM_CM7/r0p1" \


##    -I"src/HAL/$(HW_PLATFORM)/ASF_man/common/services/freertos/sams70" \



###	-I"../xtEmbedded/xteBackbone/ASF/sam/utils" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/utils" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/utils/preprocessor" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/utils/header_files" \
###	-I"../xtEmbedded/xteBackbone/ASF/thirdparty/freertos/freertos-8.2.3/source/include" \
###	-I"../xtEmbedded/xteBackbone/ASF/thirdparty/freertos/freertos-8.2.3/source/portable/gcc/arm_cm7/r0p1" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/delay" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/clock" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/pmc" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/efc" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/services/flash_efc" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/gpbr" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/ioport/sam" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/ioport" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/xdmac" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/pio" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/rstc" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/utils/fpu" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/utils/cmsis/samv70/source/templates" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/ebi/smc" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/qspi" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/supc" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/twi" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/twihs" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/uart" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/serial" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/usart" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/wdt" \
###	-I"../xtEmbedded/xteBackbone/ASF/thirdparty/CMSIS/Include" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/utils/cmsis/samv70/include" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/usb" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/usb/udc" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/usb/class/cdc" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/usb/class/cdc/device" \
###	-I"../xtEmbedded/xteBackbone/ASF/sam/drivers/usbhs" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/sleepmgr" \
###	-I"../xtEmbedded/xteBackbone/ASF/common/services/sleepmgr/sam" \
###	-I"../xtEmbedded/xteBackbone/ASF_man/common/services/freertos/samv70" \
###	-I"../xtEmbedded/xteBoard/X2M04" \


C_SOURCES := $(filter %.c ,$(C_SRCS))
C_ASF_SOURCES := $(filter %.c ,$(C_ASF_SRCS))
C_FREERTOS_SOURCES := $(filter %.c ,$(C_FREERTOS_SRCS))
CPP_SOURCES := $(filter %.cpp ,$(C_SRCS))


C_OBJECTS := $(patsubst %.c,$(TOP_LEVEL_DIR)/_build/%.o, $(C_SOURCES))
C_ASF_OBJECTS := $(patsubst %.c,$(TOP_LEVEL_DIR)/_build/ASF/%.o, $(C_ASF_SOURCES))
C_FREERTOS_OBJECTS := $(patsubst %.c,$(TOP_LEVEL_DIR)/_build/FreeRTOS/%.o, $(C_FREERTOS_SOURCES))
CPP_OBJECTS := $(patsubst ../%.cpp,$(TOP_LEVEL_DIR)/_build/%.o, $(CPP_SOURCES))
PARAM_OBJECTS = $(patsubst ../%.c,$(TOP_LEVEL_DIR)/_build/%.o, $(PARAM_SRC_FILES))


C_DEPS := $(patsubst %.o,%.d, $(C_OBJECTS))
C_DEPS += $(patsubst %.o,%.d, $(C_ASF_OBJECTS))
C_DEPS += $(patsubst %.o,%.d, $(C_FREERTOS_OBJECTS))
C_DEPS += $(patsubst %.o,%.d, $(CPP_OBJECTS))


COMMON_DEFINES = -march=armv7e-m -mthumb -D__SAMS70Q20__ -DARM_MATH_CM7=true -DBOARD=USER_BOARD -DFREERTOS_USED -DMPU6500 -DEMPL_TARGET_SAM7 -D__FPU_PRESENT=1

GCC_DEFINES = -Dprintf=iprintf -Dscanf=iscanf -DVSDM_HW_MODULE -DVSDM_FW_DEV -DVSDM_HW_PMIC_YES
CXX_DEFINES =

GCC_FLAGS = -std=gnu99
CXX_FLAGS = -fno-rtti -fno-exceptions -std=gnu++11
LD_FLAGS = -mfpu=fpv5-sp-d16 -mfloat-abi=hard -L$(PATH_TO_ASF)/thirdparty/CMSIS/Lib/GCC

COMMON_FLAGS = -march=armv7e-m -fdata-sections -ffunction-sections -mlong-calls -Wall -c -pipe -fno-strict-aliasing -Wall -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -ffunction-sections -fdata-sections -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wbad-function-cast -Wwrite-strings -Wsign-compare -Waggregate-return -Wmissing-declarations -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs -Winline -Wlong-long -Wunreachable-code -Wcast-align --param max-inline-insns-single=500 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"


################################################################################
# Rules
################################################################################

PARAMS_IN_SOURCE_SRC = \
###	../xtEmbedded/xteBoard/X2M04/application/RO_Parameters_in_source.c \
###	../xtEmbedded/xteBoard/X2M04/application/RW_Parameters_in_source.c


PARAMS_IN_SOURCE_OBJECTS := $(patsubst ../%.c,_build/%.o, $(PARAMS_IN_SOURCE_SRC))

PARAMS_IN_FLASH_SRC = \
###	../xtEmbedded/xteBoard/X2M04/application/RO_Parameters_in_flash.c \
###	../xtEmbedded/xteBoard/X2M04/application/RW_Parameters_in_flash.c

PARAMS_IN_FLASH_OBJECTS := $(patsubst ../%.c,_build/%.o, $(PARAMS_IN_FLASH_SRC))


ALL_LOCAL_C_OBJECTS = $(C_OBJECTS) $(PARAMS_IN_FLASH_OBJECTS) $(PARAMS_IN_SOURCE_OBJECTS)
###ALL_LOCAL_C_OBJECTS = $(C_OBJECTS) $(PARAMS_IN_FLASH_OBJECTS) $(PARAMS_IN_SOURCE_OBJECTS)
### $(info C_ASF_OBJECTS: $(C_ASF_OBJECTS))
### $(info ALL_LOCAL_C_OBJECTS: $(ALL_LOCAL_C_OBJECTS))

###$(ALL_LOCAL_C_OBJECTS): _build/%.o: %.c Makefile

###../xtapplication_m7.make
$(ALL_LOCAL_C_OBJECTS): $(TOP_LEVEL_DIR)/_build/%.o: %.c Makefile
	@echo Building file : $<
	$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : 4.8.4
	$(GCC) -x c $(DEBUG_FLAGS) $(COMMON_DEFINES) $(GCC_DEFINES)  $(INCLUDES)  $(COMMON_FLAGS) $(GCC_FLAGS) -o "$@" "$<"
	@echo Finished building: $<

## build C source from other repositories
$(C_ASF_OBJECTS): $(TOP_LEVEL_DIR)/_build/ASF/%.o: %.c Makefile
	@echo Building file : $<
	$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : 4.8.4
	$(GCC) -x c $(DEBUG_FLAGS) $(COMMON_DEFINES) $(GCC_DEFINES)  $(INCLUDES)  $(COMMON_FLAGS) $(GCC_FLAGS) -o "$@" "$<"
	@echo Finished building: $<

$(C_FREERTOS_OBJECTS): $(TOP_LEVEL_DIR)/_build/FreeRTOS/%.o: %.c Makefile
	@echo Building file : $<
	$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : 4.8.4
	$(GCC) -x c $(DEBUG_FLAGS) $(COMMON_DEFINES) $(GCC_DEFINES)  $(INCLUDES)  $(COMMON_FLAGS) $(GCC_FLAGS) -o "$@" "$<"
	@echo Finished building: $<

## build CPP source
$(CPP_OBJECTS): $(TOP_LEVEL_DIR)/_build/%.o: ../%.cpp Makefile ###../xtapplication_m7.make
	@echo Building file: $<
	$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU C Compiler : 4.8.4
	$(CXX) $(DEBUG_FLAGS) $(COMMON_DEFINES) $(CXX_DEFINES) $(INCLUDES) $(COMMON_FLAGS) $(CXX_FLAGS) -o "$@" "$<"
	@echo Finished building: $<



ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf: $(C_OBJECTS) $(C_ASF_OBJECTS) $(C_FREERTOS_OBJECTS) $(CPP_OBJECTS) $(PARAMS_IN_SOURCE_OBJECTS)
	@echo Building target: $@
	$(call make_dir, $(@D))
	@echo Invoking: ARM/GNU Linker : 4.8.4
###	$(CXX) $^ -march=armv7e-m -mthumb -Wl,-Map=$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).map -Wl,--start-group -larm_cortexM7lfsp_math_softfp -lxtradarhal_atmel_sam7  -Wl,--end-group -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--cref -mthumb -Tsamv70q20_flash.ld -o $@
	$(CXX) $^ -march=armv7e-m -mthumb -Wl,-Map=$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).map $(LD_FLAGS) -Wl,--start-group -larm_cortexM7lfsp_math -lm -Wl,--end-group -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--cref -mthumb -T$(TOP_LEVEL_DIR)/src/HAL/$(HW_PLATFORM)/application_without_bl.ld -o $@
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

.PHONY: show_size

show_size: $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf
	$(SIZE) $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).elf


###$(PRODUCT_NAME)_4bl.elf: $(C_OBJECTS) $(CPP_OBJECTS) $(PARAMS_IN_FLASH_OBJECTS)
###	$(CXX) $^ -nostartfiles -march=armv7e-m -mthumb -Wl,-Map=$(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).map -Wl,--start-group -larm_cortexM7lfsp_math_softfp -lxtradarhal_atmel_sam7  -Wl,--end-group -L../../../xtradaraccess/xtRadarHAL/$(RADAR_LIB_BUILD) -L"../xtEmbedded/xteBackbone/ASF/thirdparty/CMSIS/Lib/GCC"  -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--cref -mthumb -T../xtEmbedded/xteBoard/X2M04/application/application_w_bootloader.ld -o $@


###$(PRODUCT_NAME)_4bl.hex: $(PRODUCT_NAME)_4bl.elf
###	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $< $@


###$(PRODUCT_NAME)_4bl_cert.hex: $(PRODUCT_NAME)_4bl.hex
###	 $(GEN_CERT) -o $@ -i $< -conf $(GEN_CERT_CONF) -cout $(PRODUCT_NAME)_4bl.cout



# Other Targets

doc:
	cd $(TOP_LEVEL_DIR)/doc && doxygen config/Doxyfile

clean:
	-rm $(C_OBJECTS) $(C_ASF_OBJECTS) $(C_FREERTOS_OBJECTS) $(CPP_OBJECTS) $(TARGET_FOLDER)/$(PRODUCT_NAME)_$(HW_PLATFORM).map $(PRODUCT_NAME)__$(HW_PLATFORM)_4bl.cout
	-rm $(C_DEPS)
	-rm -r $(TOP_LEVEL_DIR)/_build
	-rm $(TARGETS)
	-rm -r $(TOP_LEVEL_DIR)/doc/output

.PHONY: doc
