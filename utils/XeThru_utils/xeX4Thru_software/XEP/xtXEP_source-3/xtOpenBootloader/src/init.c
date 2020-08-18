/**
 * @file
 *
 * @brief HW initialization functions
 *
 */
#include "conf_board.h"
#include <asf.h>
#include "compiler.h"
#include "board.h"
#include "ioport.h"
#include "conf_usb.h"
#include "init.h"
#include <comm/comm_iface.h>
#include <xt_config/xt_config.h>
#include <nvm/nvm_sections.h>
#include <nvm/nvm.h>


/**
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

/**
 * \brief Set input mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin
 * \param sense Sense for interrupt detection
 */
#define ioport_set_pin_input_mode(pin, mode, sense) \
	do {\
		ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);\
		ioport_set_pin_mode(pin, mode);\
		ioport_set_pin_sense_mode(pin, sense);\
	} while (0)



void board_init(void)
{
    // Initialize IOPORTs
    ioport_init();
        
    ioport_enable_pin(PIN_USART0_RXD_IDX);
    ioport_set_pin_dir(PIN_USART0_RXD_IDX, IOPORT_DIR_INPUT);
        
    ioport_set_pin_mode(PIN_USART0_RXD_IDX, IOPORT_MODE_PULLUP);
    if (XPIN_USBVBUS != 0xFF)
    {
        ioport_set_pin_dir(XPIN_USBVBUS, IOPORT_DIR_INPUT);
        ioport_set_pin_mode(XPIN_USBVBUS, IOPORT_MODE_PULLDOWN);
    }

}


void board_init_ext(void)
{
	uint8_t interfaceMode =(ioport_get_pin_level(XPIN_IO5)<<0) | (ioport_get_pin_level(XPIN_IO6)<<1);
	if (interfaceMode == XDEF_COMM_MODE_MANUFACTURE)
		return;

	if (interfaceMode == XDEF_COMM_MODE_SERIAL)
	{
        // Configure UART pins
		ioport_set_pin_peripheral_mode(PIN_USART0_RXD_IDX, PIN_USART0_RXD_FLAGS);
		ioport_set_pin_peripheral_mode(PIN_USART0_TXD_IDX, PIN_USART0_TXD_FLAGS);

#if defined(CONF_BOARD_USB_PORT)
		ioport_set_pin_input_mode(USB_VBUS_PIN, IOPORT_MODE_PULLDOWN, 0);
#endif
	}

	// Enable ACTIVE pin.
	ioport_set_pin_input_mode(UI_ACTIVE_INPUT_GPIO, UI_ACTIVE_INPUT_FLAGS, UI_ACTIVE_INPUT_SENSE);

}

void application_init(void)
{
	ifctype_t type=IFACE_NONE;
	if ((communication_config[CONFIG_USB_INDEX].module != 0xFFFFFFFF) && (ioport_get_pin_level(USB_VBUS_PIN))) {
		type=IFACE_USB;
	} else if (communication_config[CONFIG_I2C_INDEX].module != 0xFFFFFFFF)
	{
		type=IFACE_I2C;
	}
	#ifdef WITH_USART
	else {
		// No VBUS means that UART 115200 8N1 will be used.
		type=IFACE_USART;
	}
	#endif
	InitCommIface(type);
}

bool stayBootloader(void){
    bool _do_bootloader = false;

    // Mode selector pins (IO5 + IO6) LOW = Manufacturing mode.
    uint8_t interfaceMode = (ioport_get_pin_level(XPIN_IO5)<<0) | (ioport_get_pin_level(XPIN_IO6)<<1);

    // If in serial mode (default), check for manual force bootloader, RX pulled LOW.
    uint8_t rx_pin_val =  ioport_get_pin_level(PIN_USART0_RXD_IDX);
    if ((interfaceMode == XDEF_COMM_MODE_SERIAL) && (!rx_pin_val)) _do_bootloader = true;


    if (!nvm_is_locked())
    {
        // Enter bootloader if last update failed
	    _do_bootloader = true;
    }

    if ((RSTC->RSTC_SR & RSTC_SR_RSTTYP_Msk) == RSTC_SR_RSTTYP_SOFT_RST)
    {
	    // Soft reset. Check reason.
	    uint32_t resetReason = gpbr_read(RESET_REASON_GPBR_REG);
	    if (resetReason == XT_SWRST_BOOTLOADER)
	    _do_bootloader = true;
    }
	return _do_bootloader;
}

void reset_watchdog(void)
{
	WDT->WDT_CR = 0xA5000001;
}

