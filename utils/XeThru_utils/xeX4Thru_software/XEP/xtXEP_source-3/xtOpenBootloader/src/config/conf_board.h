/**
 * \file
 *
 * \brief User board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H


#define CONF_BOARD_SRAM
#define CONF_BOARD_TWI0
#define CONF_BOARD_USB_PORT
#define BOARD_ID_USART             ID_USART0
#define BOARD_USART                USART0
#define BOARD_USART_BAUDRATE       115200
#define USART_Handler              USART0_Handler
#define USART_IRQn                 USART0_IRQn

#define BOARD_ID_TWIHS             ID_TWIHS2
#define CONF_BOARD_TWIHS           TWIHS2
#define TWIHS_Handler              TWIHS2_Handler
#define TWIHS_IRQn                 TWIHS2_IRQn


//  Board oscillator settings
#define BOARD_FREQ_SLCK_XTAL		(32768U)
#define BOARD_FREQ_SLCK_BYPASS		(32768U)
#define BOARD_FREQ_MAINCK_XTAL		(12000000U)
#define BOARD_FREQ_MAINCK_BYPASS	(12000000U)

//  Master clock frequency 
#define BOARD_MCK					CHIP_FREQ_CPU_MAX


//  Board main clock xtal statup time 
#define BOARD_OSC_STARTUP_US   15625

//  Base address of chip select 
#define SRAM_BASE_ADDRESS		(EBI_CS0_ADDR)
#define SRAM_SIZE				(0x80000)

#define XPIN_LED_RED        red_led_pin
#define XPIN_LED_GREEN      green_led_pin
#define XPIN_LED_BLUE       blue_led_pin

#define XPIN_LEVEL_LED_ACTIVE	(0)


#define XLED_On(led)        {if (led != 0xFF) ioport_set_pin_level(led, 0);}
#define XLED_Off(led)       {if (led != 0xFF) ioport_set_pin_level(led, 1);}
#define XLED_Toggle(led)    {if (led != 0xFF) ioport_toggle_pin_level(led);}

#define XPIN_IO1            (PIO_PA28_IDX)
#define XPIN_IO2            (PIO_PA25_IDX)
#define XPIN_IO3            (PIO_PA30_IDX)
#define XPIN_IO4            (PIO_PA31_IDX)
#define XPIN_IO5            (PIO_PA26_IDX)
#define XPIN_IO6            (PIO_PA27_IDX)
#define XPIN_IO7_WAKEUP     (PIO_PB5_IDX)
#define XPIN_IO8_SWCLK      (PIO_PB7_IDX)
#define XPIN_IO9_SWDIO      (PIO_PB6_IDX)
#define UI_ACTIVE_INPUT_GPIO		(XPIN_IO7_WAKEUP)
#define UI_ACTIVE_INPUT_FLAGS		(IOPORT_MODE_PULLUP)
#define UI_ACTIVE_INPUT_SENSE		(0)
#define UI_ACTIVE_WKUP_IDX			(XPIN_IO7_WAKEUP)
#define UI_ACTIVE_WKUP_FLAGS		(IOPORT_MODE_MUX_D)
#define XDEF_WAKEUP_ACTIVE			(13)

//  USART0 pins (UTXD0 and URXD0) definitions, PB0,1. 
#define PIN_USART0_RXD_IDX    (PIO_PB0_IDX)
#define PIN_USART0_RXD_FLAGS  (IOPORT_MODE_MUX_C)
#define PIN_USART0_TXD_IDX    (PIO_PB1_IDX)
#define PIN_USART0_TXD_FLAGS  (IOPORT_MODE_MUX_C)
#define VSDM_MAIN_USART	USART0 // Not pinheader, custom customer connection.

//  USART1 pins (UTXD1 and URXD1) definitions, PA21,22.
#define PIN_USART1_RXD_IDX    (PIO_PD15_IDX)
#define PIN_USART1_RXD_FLAGS  (IOPORT_MODE_MUX_B)
#define PIN_USART1_TXD_IDX    (PIO_PD16_IDX)
#define PIN_USART1_TXD_FLAGS  (IOPORT_MODE_MUX_B)


#define XDEF_COMM_MODE_MANUFACTURE	(0)
#define XDEF_COMM_MODE_SPI			(1)
#define XDEF_COMM_MODE_GPIO			(2)
#define XDEF_COMM_MODE_SERIAL		(3)

#define XPIN_USBVBUS        vbus_pin

#define RESET_REASON_GPBR_REG   4
#define RESET_COUNT_GPBR_REG    5
#define XT_SWRST_BOOTLOADER        (0x02)


#endif // CONF_BOARD_H
