/**
 * @file
 *
 * @brief Implementation of GPIO functions for X4M0x boards with Atmel SAM S70 MCU.
 *
 * See @ref X4M0x_SAMS70/xtio_gpio.h and @ref xt_XEP_HAL.h for more documentation.
 */


#include "board.h"
#include "xtio_gpio.h"
#include "xt_config.h"

Pin XPIN_IO1 = {PIO_PA28, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_IO2 = {PIO_PA25, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_IO3 = {PIO_PA30, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_IO4 = {PIO_PA31, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_IO5 = {PIO_PA26, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_IO6 = {PIO_PA27, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_IO7_WAKEUP = {PIO_PB5, PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_IO8_SWCLK = {PIO_PB7, PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_IO9_SWDIO = {PIO_PB6, PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_RX = {PIO_PB0, PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_TX = {PIO_PB1, PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_NSS = {PIO_PB2, PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_SCLK = {PIO_PB13, PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_X4_IO1 = {PIO_PA4, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_X4_IO2 = {PIO_PA3, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_X4_IO3 = {PIO_PA17, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_X4_IO4 = {PIO_PD31, PIOD, ID_PIOD, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_X4_ENABLE = {PIO_PA5, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_USBVBUS = {PIO_PA2, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_SDA = {PIO_PD27, PIOD, ID_PIOD, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_SCL = {PIO_PD28, PIOD, ID_PIOD, PIO_INPUT, PIO_DEFAULT};
Pin XPIN_IRQ = {PIO_PA28, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT};


void xtio_update_pin(Pin* pin, uint8_t pin_index)
{
    if (pin_index == 0xFF)
    {
        // Non-existing pin
        pin->mask = 0;

        return;
    }

    // Find port and pin
    uint8_t port = pin_index / 32;
    uint8_t pin_val = pin_index % 32;
    
    switch (port)
    {
        case 0:
            pin->mask = 1<<pin_val;
            pin->pio = PIOA;
            pin->id = ID_PIOA;
            break;
        case 1:
            pin->mask = 1<<pin_val;
            pin->pio = PIOB;
            pin->id = ID_PIOB;
            break;
        case 2:
            pin->mask = 1<<pin_val;
            pin->pio = PIOC;
            pin->id = ID_PIOC;
            break;
        case 3:
            pin->mask = 1<<pin_val;
            pin->pio = PIOD;
            pin->id = ID_PIOD;
            break;
        case 4:
            pin->mask = 1<<pin_val;
            pin->pio = PIOE;
            pin->id = ID_PIOE;
            break;
        default:
            break;
    }
}

int xtio_decode_pin_id(xtio_pin_id_t pin_id, Pin ** pin);
int xtio_decode_pin_id(xtio_pin_id_t pin_id, Pin ** pin)
{
    // Check hardware configuration
    static bool pins_configured = false;
    if (!pins_configured)
    {
        xtio_update_pin(&XPIN_IO1, XT_CONFIG_IO1());
        xtio_update_pin(&XPIN_IO2, XT_CONFIG_IO2());
        xtio_update_pin(&XPIN_IO3, XT_CONFIG_IO3());
        xtio_update_pin(&XPIN_IO4, XT_CONFIG_IO4());
        xtio_update_pin(&XPIN_IO5, XT_CONFIG_IO5());
        xtio_update_pin(&XPIN_IO6, XT_CONFIG_IO6());
        xtio_update_pin(&XPIN_SCLK, XT_CONFIG_MODESEL1());
        xtio_update_pin(&XPIN_NSS, XT_CONFIG_MODESEL2());
        xtio_update_pin(&XPIN_USBVBUS, XT_CONFIG_VBUS());

        pins_configured = true;
    }

    switch (pin_id)
    {
        case XTIO_XETHRU_IO1:
            *pin = &XPIN_IO1;
            break;
        case XTIO_XETHRU_IO2:
            *pin = &XPIN_IO2;
            break;
        case XTIO_XETHRU_IO3:
            *pin = &XPIN_IO3;
            break;
        case XTIO_XETHRU_IO4:
            *pin = &XPIN_IO4;
            break;
        case XTIO_XETHRU_IO5:
            *pin = &XPIN_IO5;
            break;
        case XTIO_XETHRU_IO6:
            *pin = &XPIN_IO6;
            break;
        case XTIO_XETHRU_IO7:
            *pin = &XPIN_IO7_WAKEUP;
            break;
        case XTIO_XETHRU_IO8:
            *pin = &XPIN_IO8_SWCLK;
            break;
        case XTIO_XETHRU_IO9:
            *pin = &XPIN_IO9_SWDIO;
            break;
        case XTIO_X4_IO1:
            *pin = &XPIN_X4_IO1;
            break;
        case XTIO_X4_IO2:
            *pin = &XPIN_X4_IO2;
            break;
        case XTIO_X4_IO3:
            *pin = &XPIN_X4_IO3;
            break;
        case XTIO_X4_IO4:
            *pin = &XPIN_X4_IO4;
            break;
        case XTIO_X4_ENABLE:
            *pin = &XPIN_X4_ENABLE;
            break;
        case XTIO_SERIAL_RX:
            *pin = &XPIN_RX;
            break;
        case XTIO_SERIAL_TX:
            *pin = &XPIN_TX;
            break;
        case XTIO_MODE_SEL1:
        case XTIO_SERIAL_SCLK:
            *pin = &XPIN_SCLK;
            break;
        case XTIO_MODE_SEL2:
        case XTIO_SERIAL_nSS:
            *pin = &XPIN_NSS;
            break;
        case XTIO_SERIAL_SDA:
            *pin = &XPIN_SDA;
            break;
        case XTIO_SERIAL_SCL:
            *pin = &XPIN_SCL;
            break;
        case XTIO_SERIAL_IRQ:
            *pin = &XPIN_IRQ;
            break;
        case XTIO_USB_VBUS:
            *pin = &XPIN_USBVBUS;
            break;
        default:
            return XTIO_WRONG_PIN_ID;
    }

    // Non-existing pin
    if ((*pin)->mask == 0)
        return XT_ERROR;
    
    return XT_SUCCESS;
}

int xtio_set_direction(
    xtio_pin_id_t pin_id,
    int direction,
    int level
)
{
    Pin * pin = NULL;
    int status;
    status = xtio_decode_pin_id(pin_id, &pin);
    if (XT_SUCCESS == status)
    {
        if ( XTIO_INPUT == direction)
        {
            pin->type = PIO_INPUT;
        }
        else
        {
            if (XTIO_PIN_LEVEL_LOW == level)
            {
                pin->type = PIO_OUTPUT_0;
            }
            else
            {
                pin->type = PIO_OUTPUT_1;
            }
        }
        PIO_Configure(pin, 1);
    }
    return status;
}

int xtio_set_level(
    xtio_pin_id_t pin_id,
    int level
)
{
    Pin * pin = NULL;
    int status;
    status = xtio_decode_pin_id(pin_id, &pin);
    if (XT_SUCCESS == status)
    {
        if (XTIO_PIN_LEVEL_LOW == level)
            PIO_Clear(pin);
        else
            PIO_Set(pin);
    }
    return status;
}

int xtio_get_level(
    xtio_pin_id_t pin_id,
    xtio_pin_level_t * level
)
{
    Pin * pin = NULL;
    int status;
    status = xtio_decode_pin_id(pin_id, &pin);
    if (XT_SUCCESS == status)
    {
        *level = (PIO_Get(pin) != 0) ? XTIO_PIN_LEVEL_HIGH : XTIO_PIN_LEVEL_LOW;
    }
    return status;
}

int xtio_toggle_level(
    xtio_pin_id_t pin_id
)
{
    int pin_level = 0;
    Pin * pin = NULL;
    int status;
    status = xtio_decode_pin_id(pin_id, &pin);
    if (XT_SUCCESS == status)
    {
        pin_level = PIO_Get(pin);
    	if (0 == pin_level)
            PIO_Set(pin);
        else
            PIO_Clear(pin);
    }
    return status;
}

int xtio_set_pin_mode(
    xtio_pin_id_t pin_id,
    uint32_t mode
)
{
    Pin * pin = NULL;
    int status;
    status = xtio_decode_pin_id(pin_id, &pin);
    if (XT_SUCCESS == status)
    {
        pin->attribute = PIO_DEFAULT;
        if (mode & XTIO_PULL_DOWN)
        {
            pin->attribute |= PIO_DEFAULT;
        }
        if (mode & XTIO_PULL_UP)
        {
            pin->attribute |= PIO_PULLUP;
        }
        if (mode & XTIO_OPEN_DRAIN)
        {
            pin->attribute |= PIO_OPENDRAIN;
        }
        PIO_Configure(pin, 1);
    }
    return status;
}

//////////////////////////////////////////////////////////
// IO Interrupt functions

struct {
    void (*xethru_io1_callback)(void);
    void (*xethru_io2_callback)(void);
    void (*xethru_io3_callback)(void);
    void (*xethru_io4_callback)(void);
    void (*xethru_io5_callback)(void);
    void (*xethru_io6_callback)(void);
    void (*x4_io1_callback)(void);
    void (*x4_io2_callback)(void);
} irq_callbacks;


void Dummy_Handler(void);
void xtio_pio_irq_handler(const Pin* pin);
int enable_io_irq(Pin* pin, void (*p_handler) (const Pin*));
int disable_io_irq(Pin* pin);

int xtio_irq_init(void)
{
	irq_callbacks.xethru_io1_callback = Dummy_Handler;
	irq_callbacks.xethru_io2_callback = Dummy_Handler;
	irq_callbacks.xethru_io3_callback = Dummy_Handler;
	irq_callbacks.xethru_io4_callback = Dummy_Handler;
	irq_callbacks.xethru_io5_callback = Dummy_Handler;
	irq_callbacks.xethru_io6_callback = Dummy_Handler;
	irq_callbacks.x4_io1_callback = Dummy_Handler;
	irq_callbacks.x4_io2_callback = Dummy_Handler;

    PIO_InitializeInterrupts(5);

	return XT_SUCCESS;
}

int xtio_irq_register_callback(
    xtio_pin_id_t pin_id,
    void (*irq_callback)(void),
    xtio_interrupt_modes mode
)
{
    Pin * pin = NULL;
    int status;
    status = xtio_decode_pin_id(pin_id, &pin);
    if (XT_SUCCESS == status)
    {
        switch (pin_id)
        {
            case XTIO_XETHRU_IO1:
				irq_callbacks.xethru_io1_callback = irq_callback;
				break;
            case XTIO_XETHRU_IO2:
				irq_callbacks.xethru_io2_callback = irq_callback;
				break;
            case XTIO_XETHRU_IO3:
	            irq_callbacks.xethru_io3_callback = irq_callback;
				break;
            case XTIO_XETHRU_IO4:
				irq_callbacks.xethru_io4_callback = irq_callback;
				break;
            case XTIO_XETHRU_IO5:
				irq_callbacks.xethru_io5_callback = irq_callback;
				break;
            case XTIO_XETHRU_IO6:
				irq_callbacks.xethru_io6_callback = irq_callback;
				break;
            case XTIO_X4_IO1:
                irq_callbacks.x4_io1_callback = irq_callback;
                break;
            case XTIO_X4_IO2:
                irq_callbacks.x4_io2_callback = irq_callback;
                break;
			default:
				return XTIO_WRONG_PIN_ID;
        }

        pin->attribute &= ~(PIO_IT_AIME | PIO_IT_RE_OR_HL | PIO_IT_EDGE);
        if ((mode & XTIO_INTERRUPT_RISING_EDGE) && (mode & XTIO_INTERRUPT_FALLING_EDGE))
        {
			pin->attribute |= PIO_IT_EDGE;
        }
        else if (mode & XTIO_INTERRUPT_RISING_EDGE)
        {
			pin->attribute |= PIO_IT_RISE_EDGE;
        }
        else if (mode & XTIO_INTERRUPT_FALLING_EDGE)
        {
			pin->attribute |= PIO_IT_FALL_EDGE;
        }

        // Enable deglitching
        pin->attribute |= PIO_DEGLITCH;
        PIO_Configure(pin, 1);

		if (XT_SUCCESS != enable_io_irq(pin, xtio_pio_irq_handler))
		{
			return XT_ERROR;
		}
    }

    return status;
}

/**
 * Unregister interrupt handler
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_irq_unregister_callback(
    xtio_pin_id_t pin_id          ///< IO Pin ID as defined in @ref xtio_pin_id_t
)
{
    Pin * pin = NULL;
    int status;
    status = xtio_decode_pin_id(pin_id, &pin);
    if (XT_SUCCESS == status)
    {
		if (XT_SUCCESS != disable_io_irq(pin))
		{
			return XT_ERROR;
		}
        switch (pin_id)
        {
            case XTIO_XETHRU_IO1:
				irq_callbacks.xethru_io1_callback = Dummy_Handler;
				break;
            case XTIO_XETHRU_IO2:
				irq_callbacks.xethru_io2_callback = Dummy_Handler;
				break;
            case XTIO_XETHRU_IO3:
	            irq_callbacks.xethru_io3_callback = Dummy_Handler;
				break;
            case XTIO_XETHRU_IO4:
				irq_callbacks.xethru_io4_callback = Dummy_Handler;
				break;
            case XTIO_XETHRU_IO5:
				irq_callbacks.xethru_io5_callback = Dummy_Handler;
				break;
            case XTIO_XETHRU_IO6:
				irq_callbacks.xethru_io6_callback = Dummy_Handler;
				break;
            case XTIO_X4_IO1:
                irq_callbacks.x4_io1_callback = Dummy_Handler;
                break;
            case XTIO_X4_IO2:
                irq_callbacks.x4_io2_callback = Dummy_Handler;
                break;
			default:
				return XTIO_WRONG_PIN_ID;
        }
	}
	return XT_SUCCESS;
}

int enable_io_irq(Pin* pin, void (*p_handler) (const Pin*))
{
    PIO_DisableIt(pin);
    PIO_ConfigureIt(pin, p_handler);
    PIO_EnableIt(pin);
    
	return XT_SUCCESS;
}

int disable_io_irq(Pin* pin)
{
    PIO_DisableIt(pin);

	return XT_SUCCESS;
}

void xtio_pio_irq_handler(const Pin* pin)
{
	if (&XPIN_X4_IO1 == pin)
    {
        if (irq_callbacks.x4_io1_callback)
            irq_callbacks.x4_io1_callback();
    }
	else if (&XPIN_X4_IO2 == pin)
    {
        if (irq_callbacks.x4_io2_callback)
	        irq_callbacks.x4_io2_callback();
    }
	else if (&XPIN_IO1 == pin)
    {
        if (irq_callbacks.x4_io2_callback)
	        irq_callbacks.xethru_io1_callback();
    }
	else if (&XPIN_IO2 == pin)
    {
        if (irq_callbacks.xethru_io2_callback)
	        irq_callbacks.xethru_io2_callback();
    }
	else if (&XPIN_IO3 == pin)
    {
        if (irq_callbacks.xethru_io3_callback)
	        irq_callbacks.xethru_io3_callback();
    }
	else if (&XPIN_IO4 == pin)
    {
        if (irq_callbacks.xethru_io4_callback)
	        irq_callbacks.xethru_io4_callback();
    }
	else if (&XPIN_IO5 == pin)
    {
        if (irq_callbacks.xethru_io5_callback)
	        irq_callbacks.xethru_io5_callback();
    }
	else if (&XPIN_IO6 == pin)
    {
        if (irq_callbacks.xethru_io6_callback)
	        irq_callbacks.xethru_io6_callback();
    }
}