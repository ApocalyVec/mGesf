################################################################################
# Determine platform
################################################################################
OS := $(shell uname -s)

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
    AR = "arm-none-eabi-ar.exe"
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
    AR = arm-none-eabi-ar
    OBJCOPY = arm-none-eabi-objcopy
    OBJDUMP = arm-none-eabi-objdump
    SIZE = arm-none-eabi-size
endif



INCLUDES += -I"$(TOP_LEVEL_DIR)/src"

VPATH += $(TOP_LEVEL_DIR)/src

################################################################################
# Add ASF source
################################################################################
PATH_TO_ASF = $(TOP_LEVEL_DIR)/src/ASF

VPATH += $(PATH_TO_ASF)

C_SRCS +=  \
	ASF/common/services/delay/sam/cycle_counter.c \
	ASF/common/services/fifo/fifo.c \
	init.c \
	led_ctrl.c \
	comm/comm_iface.c \
	nvm/nvm_sections.c \
	xt_config/xt_config.c \
	ASF/common/services/sleepmgr/sam/sleepmgr.c \
	ASF/common/services/usb/class/cdc/device/udi_cdc.c \
	ASF/common/services/usb/udc/udc.c \
	ASF/common/utils/stdio/read.c \
	ASF/sam/drivers/pio/pio_handler.c \
	ASF/sam/drivers/usart/usart.c \
	ASF/sam/drivers/usbhs/usbhs_device.c \
	comm/cdc.c \
	comm/ftdi.c \
	comm/protocol_handlers.c \
	nvm/nvm.c \
	ASF/common/utils/stdio/write.c \
	ASF/common/services/usb/class/cdc/device/udi_cdc_desc.c \
	ASF/common/services/serial/usart_serial.c \
	ASF/common/services/clock/sams70/sysclk.c \
	ASF/sam/drivers/aes/aes.c \
	ASF/sam/drivers/efc/efc.c \
	ASF/sam/drivers/gpbr/gpbr.c \
	ASF/sam/drivers/pio/pio.c \
	ASF/sam/drivers/pmc/pmc.c \
	ASF/sam/drivers/twihs/twihs.c \
	ASF/sam/drivers/uart/uart.c \
	ASF/sam/services/flash_efc/flash_efc.c \
	ASF/sam/drivers/pmc/sleep.c \
	ASF/common/utils/interrupt/interrupt_sam_nvic.c \
	ASF/sam/utils/cmsis/sams70/source/templates/gcc/startup_sams70.c \
	ASF/sam/utils/cmsis/sams70/source/templates/system_sams70.c \
	ASF/sam/utils/syscalls/gcc/syscalls.c \
	main.c 

INCLUDES = \
    -I"$(TOP_LEVEL_DIR)/src" \
	-I"$(TOP_LEVEL_DIR)/src/config/" \
	-I"$(TOP_LEVEL_DIR)/src/comm" \
	-I"$(TOP_LEVEL_DIR)/src/xt_config" \
	-I"$(TOP_LEVEL_DIR)/src/nvm"

ASF_INCLUDES += \
	-I"$(TOP_LEVEL_DIR)/src/ASF/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/boards/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/boards/user_board/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/clock/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/clock/sams70/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/delay/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/delay/sam/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/fifo/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/ioport/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/ioport/sam/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/serial/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/serial/sam_uart/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/sleepmgr/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/sleepmgr/sam/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/usb/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/usb/class/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/usb/class/cdc/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/usb/class/cdc/device/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/services/usb/udc/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/utils/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/utils/interrupt/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/utils/stdio/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/common/utils/stdio/stdio_serial/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/aes/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/aes/example/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/chipid/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/efc/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/gpbr/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/pio/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/pmc/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/twihs/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/uart/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/usart/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/drivers/usbhs/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/services/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/services/flash_efc/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/cmsis/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/cmsis/sams70/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/cmsis/sams70/include/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/cmsis/sams70/include/component/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/cmsis/sams70/include/instance/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/cmsis/sams70/include/pio/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/cmsis/sams70/source/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/cmsis/sams70/source/templates/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/cmsis/sams70/source/templates/gcc/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/fpu/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/header_files/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/linker_scripts/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/linker_scripts/sams70/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/linker_scripts/sams70/sams70q21/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/linker_scripts/sams70/sams70q21/gcc/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/make/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/preprocessor/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/syscalls/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/sam/utils/syscalls/gcc/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/thirdparty/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/thirdparty/CMSIS/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/thirdparty/CMSIS/Include/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/thirdparty/CMSIS/Lib/" \
	-I"$(TOP_LEVEL_DIR)/src/ASF/thirdparty/CMSIS/Lib/GCC/"

INCLUDES += $(ASF_INCLUDES)

################################################################################
# Add xtserial
################################################################################
PATH_TO_XTSERIAL = $(TOP_LEVEL_DIR)/../xtSerial/src
VPATH += $(PATH_TO_XTSERIAL)

C_SRCS += \
	protocol.c

INCLUDES += -I"$(PATH_TO_XTSERIAL)"


C_OBJECTS = $(patsubst %.c,$(TOP_LEVEL_DIR)/_build/%.o, $(C_SRCS))
C_DEPS = $(patsubst %.o,%.d, $(C_OBJECTS))


GCC_FLAGS = \
	-x c -mthumb -DVSDM_HW_XTI -DVSDM_HW_MODULE -ffunction-sections \
	-mlong-calls -Wall -c -std=gnu99 -MD -MP \
	-MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" \
	$(GCC_TARGET_FLAGS)


$(C_OBJECTS): $(TOP_LEVEL_DIR)/_build/%.o: %.c Makefile
	$(call make_dir, $(@D))
	$(GCC)  $(GCC_FLAGS) $(DEBUG_FLAGS) $(INCLUDES) -o "$@" "$<"



ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


LINKER_FLAGS = \
	-mthumb \
	-Wl,-Map=$(PRODUCT_NAME).map \
	-Wl,--start-group \
	-lm \
	-Wl,--end-group -L"../cmsis/linkerScripts"  \
	-Wl,--gc-sections \
	$(LINKER_TARGET_FLAGS) \
	-Wl,--entry=Reset_Handler \
	-Wl,--cref \
	-mthumb \
	-T../src/bootloader.ld



$(PRODUCT_NAME).elf: $(C_OBJECTS)
	echo C_OBJECTS $(C_OBJECTS)
	@echo Building target: $@
	@echo Invoking: ARM/GNU Linker : $(GCC_VERSION)
	$(GCC) $(LINKER_FLAGS) -o $@ $^
	@echo Finished building target: $@

$(PRODUCT_NAME).bin: $(PRODUCT_NAME).elf
	$(OBJCOPY) -O binary $< $@

$(PRODUCT_NAME).hex: $(PRODUCT_NAME).elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $< $@

$(PRODUCT_NAME).eep: $(PRODUCT_NAME).elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary $< $@

$(PRODUCT_NAME).lss: $(PRODUCT_NAME).elf
	$(OBJDUMP) -h -S $< > $@


$(PRODUCT_NAME).srec: $(PRODUCT_NAME).elf
	$(OBJCOPY) -O srec -R .eeprom -R .fuse -R .lock -R .signature  $< $@


.PHONY: show_size
show_size: $(PRODUCT_NAME).elf
	$(SIZE) $<


.PHONY: clean
clean:
	-rm -rf $(TARGETS)
	-rm $(C_OBJECTS) $(PRODUCT_NAME).map
	-rm $(C_DEPS)
