/**
 * @file
 *
 * @brief Implementation of USB CDC functions for X4M0x boards with Atmel SAM S70 MCU.
 *
 * See @ref X4M0x_SAMS70/xtio_usb.h and @ref xt_XEP_HAL.h for more documentation.
 *
 */


#include "board.h"
#include "pio.h"
#include "USBD.h"
#include "CDCDSerialDriver.h"
#include "xtio_usb.h"
#include "xep_hal.h"
/*----------------------------------------------------------------------------
 *      External variables
 *----------------------------------------------------------------------------*/

extern const USBDDriverDescriptors cdcdSerialDriverDescriptors;

static void _ConfigureUotghs(void);

int xtio_usb_init()
{
	/* Interrupt priority */
	NVIC_SetPriority(USBHS_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);

	/* Initialize OTG clocks */
	_ConfigureUotghs();

	/* CDC serial driver initialization */
	CDCDSerialDriver_Initialize(&cdcdSerialDriverDescriptors);

	// Start USB stack to authorize VBus monitoring
    irqflags_t flags = cpu_irq_save();
	USBD_Connect();
    cpu_irq_restore(flags);
	
	return XT_SUCCESS;
}


/**
 * Configure USBHS settings for USB device
 */
static void _ConfigureUotghs(void)
{
	/* UTMI parallel mode, High/Full/Low Speed */
	/* UOTGCK not used in this configuration (High Speed) */
	PMC->PMC_SCDR = PMC_SCDR_USBCLK;
	/* USB clock register: USB Clock Input is UTMI PLL */
	PMC->PMC_USB = PMC_USB_USBS;
	/* Enable peripheral clock for USBHS */
	PMC_EnablePeripheral(ID_USBHS);
	USBHS->USBHS_CTRL = USBHS_CTRL_UIMOD_DEVICE;
	/* Enable PLL 480 MHz */
	PMC->CKGR_UCKR = CKGR_UCKR_UPLLEN | CKGR_UCKR_UPLLCOUNT(0xF);

	/* Wait that PLL is considered locked by the PMC */
	while (!(PMC->PMC_SR & PMC_SR_LOCKU));

	/* IRQ */
	NVIC_EnableIRQ(USBHS_IRQn);
}

/*-----------------------------------------------------------------------------
 *         Callback re-implementation
 *-----------------------------------------------------------------------------*/
/**
 * Invoked when the configuration of the device changes. Parse used endpoints.
 * \param cfgnum New configuration number.
 */
void USBDDriverCallbacks_ConfigurationChanged(unsigned char cfgnum)
{
	CDCDSerialDriver_ConfigurationChangedHandler(cfgnum);
}

/**
 * Invoked when a new SETUP request is received from the host. Forwards the
 * request to the Mass Storage device driver handler function.
 * \param request  Pointer to a USBGenericRequest instance.
 */
void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
{
	CDCDSerialDriver_RequestHandler(request);
}