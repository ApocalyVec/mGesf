DEBUG_FLAGS = -DDEBUG -O0 -g3

PRODUCT_NAME = xtopenbootloader

TOP_LEVEL_DIR = ..
TARGETS = \
	$(PRODUCT_NAME).elf \
	$(PRODUCT_NAME).srec \
	$(PRODUCT_NAME).lss \
	$(PRODUCT_NAME).eep \
	$(PRODUCT_NAME).hex \
	$(PRODUCT_NAME).bin	\
	show_size

all: $(TARGETS)


GCC_TARGET_FLAGS =  -DBOARD=USER_BOARD -DWITH_USART -D__SAMS70Q21__ -march=armv7e-m

LINKER_TARGET_FLAGS = -march=armv7e-m
WITH_USART = TRUE

include ../src/bootloader.make
