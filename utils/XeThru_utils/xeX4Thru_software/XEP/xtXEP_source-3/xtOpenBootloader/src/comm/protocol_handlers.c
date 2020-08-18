/**
 * @file
 *
 * @brief Communication protocol handlers
 *
 */
#include <asf.h>
#include <comm/comm_iface.h>
#include <comm/cdc.h>
#include <comm/protocol_handlers.h>
#include <nvm/nvm_sections.h>
#include <init.h>
#include "protocol.h"
#include <led_ctrl.h>
#include <conf_nvm.h>
#include <nvm/nvm.h>
#include "../../../src/version.h"



static bool signature_ok = true;                    ///< Variable to hold checksum test result
static uint16_t first_page=0xFFFF;                  ///< Index of the first flash memory page
static uint16_t last_page=0;                        ///< Index of the last flash memory page
static uint32_t checksum=0;                         ///< Checksum 

extern const MemorySections_t memory_sections;      ///< Memory sections definitions

static void respond(const unsigned char byte)
{
	send(&byte, sizeof(byte));
}

void handleProtocolPacket( void * userData, const unsigned char * data, unsigned int length)
{
	 const unsigned char bootloader_command = 0xc0;

	 if (data[0] != bootloader_command || length <= 1)
	 return;
	 handle_packet((unsigned char*)&data[1], length-1);
}

void handleProtocolError(void * userData,unsigned int error)
{
	led_ctrl(ERROR);

	unsigned char error_code = 0xff & error;
	unsigned char response_data[] = {PROTO_RSP_ERROR, error_code};
	send(response_data, sizeof(response_data));
}

void handle_packet(uint8_t *pkt, uint16_t length)
{

    if ((pkt == NULL) || (length == 0))
        return;

    switch (pkt[0]) {
        case PROTO_DEVICE_INFO:
            send_device_info();
            break;
        case PROTO_PAGE_SIZE:
            send_page_size();
            break;
        case PROTO_PAGE_CNT:
            send_page_count();
            break;
        case PROTO_WRITE_PAGE:
            handle_proto_write_page(pkt);
            break;
        case PROTO_START_APP:
            handle_start_app();
            break;
        case PROTO_ECHO:
            send(pkt, length);
            break;
        default:
            //Add some assert in the default case
            signature_ok = false;
            respond(PROTO_RSP_ERROR + 1);
            break;
    }
}

static void capture_data(unsigned char byte, void * user_data)
{
	CommIface_put(IFACE_OUT, byte);
}

 void send(const unsigned char * data, unsigned int length)
{
	CommIface_Clear(IFACE_OUT);
    createCommand(
        data,
        length,
        capture_data,
        0);
    CommIface_flush(IFACE_OUT);
}


void send_device_info(void)
{
    char system_core_id[40];
    uint32_t max_size = sizeof(system_core_id)-1; // Room for '\0'
    uint32_t sz = 0;
    nvm_get_unique_id(system_core_id, max_size, &sz);
    system_core_id[sz] = '\0';

    const char * name = "XETHRU OPENBOOTLOADER";
    const unsigned int size = 200;
    char buffer[size];
    unsigned int len = 0;
    buffer[len++] = PROTO_RSP_SUCCESS;
    len += snprintf(&buffer[len], size - len, "%s ", name);
    len += snprintf(&buffer[len], size - len, "V:%d.%d.%d ", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
    len += snprintf(&buffer[len], size - len, "ID:%s ", system_core_id);
    send((unsigned char *)buffer, len);
}


void send_page_size(void)
{
    const unsigned char buffer[] =
        {PROTO_RSP_SUCCESS,
         (0xFF & (IFLASH_PAGE_SIZE >> 8*0)),
         (0xFF & (IFLASH_PAGE_SIZE >> 8*1))};

    send(buffer, sizeof(buffer));
}

void send_page_count(void)
{
    unsigned char buffer[] =
        {PROTO_RSP_SUCCESS,
         0xff & (APP_PAGE_CNT >> 8*0),
         0xff & (APP_PAGE_CNT >> 8*1)};
    send(buffer, sizeof(buffer));
}

static void unlock_flash()
{
	nvm_unlock();
}

void handle_proto_write_page(uint8_t *pkt)
{
    uint16_t page;
    //Extract page address
    page = pkt[1];
    page <<= 8;
    page |= pkt[2];

    //Check that we are not over-writing pages that are protected
    if ((page >= memory_sections.AppFirstPage) && (page <= memory_sections.AppLastPage) && signature_ok) {
        led_ctrl(PROG);
        write_single_page(page, pkt);
        const unsigned char response[] = {PROTO_RSP_SUCCESS, 0x33};
        send(response, sizeof(response));
        led_ctrl(OFF);
    }
    else if (page == 0) {
        led_ctrl(PROG);
		checksum=*((uint32_t*)(&pkt[4]));
        if (signature_ok) {
            unlock_flash();
            respond(PROTO_RSP_SUCCESS);
        } else {
            led_ctrl(CERT);
            respond(PROTO_RSP_ERROR + 2);
        }
    }
    else if(page > memory_sections.AppLastPage)
    {
        respond(PROTO_RSP_ERROR + 4);//page out of bounds...
        led_ctrl(ERROR);
    }
    else {
        led_ctrl(ERROR);
        respond(PROTO_RSP_SUCCESS);//Is this a success?
    }
    reset_watchdog();
}

static void page_tracking(uint16_t page)
{
    if (page < first_page) {
        first_page = page;
    }

    if (page > last_page) {
        last_page = page;
    }
}

void write_single_page(uint16_t page, const uint8_t * pkt)
{
    page_tracking(page);
    uint32_t status = 0;
    if ((page % MIN_NUMBER_OF_PAGES_TO_ERASE ) == 0) {
        status = (uint32_t)nvm_erase_pages(page);
    }
    
    if((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY)
    {
        uint32_t status;
        do {
            status = EFC->EEFC_FSR;
        } while ((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);
    }
    status = nvm_write_page(page, &pkt[4]);
    if((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY)
    {
        uint32_t status;
        do {
            status = EFC->EEFC_FSR;
        } while ((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);
    }
        
}

static uint32_t calculate_crc(uint8_t *start_address, uint32_t length){
	uint32_t crc=0;
	uint32_t i=0;
	irqflags_t flags = cpu_irq_save();
	for(i=0;i<length;i++){
		crc +=*((uint8_t*)(start_address+i));
		reset_watchdog();
	}
	cpu_irq_restore(flags);
	return crc;
}

void handle_start_app()
{
    uint32_t temp;
    if (first_page <= last_page) {
        temp = (last_page - first_page) + 1;
        temp *= IFLASH_PAGE_SIZE;
		signature_ok= true;
        led_ctrl(CERT);
		uint32_t crc_calculated=calculate_crc((uint8_t *)APP_START_ADDRESS, temp);
        if(crc_calculated==checksum){
			signature_ok= true;
		}else{
			signature_ok= false;
		}

        //Check the hash we have
        if (signature_ok) {
            //We are committed to run application - disable all interruptions.
            signature_ok = false;
            respond(PROTO_RSP_SUCCESS);

            led_ctrl(DONE);

            nvm_lock_pages(last_page);

            delay_ms(100);

            /* We need to disconnect from USB
             * It has to pass some time between deattaching and starting potentially a new connection
             * by the application.
             */
            if (ioport_get_pin_level(USB_VBUS_PIN))
            {
                cdc_deinit();
            }

            nvm_exec_app((void *)APP_START_ADDRESS);
        }
        else
        {
            respond(PROTO_RSP_ERROR + 3);
        }
    }
    signature_ok = false;

    //Getting here means that something went wrong...try again.
    led_ctrl(IN_BOOT);

    //Reset page tracking
    first_page = 0xFFFF;
    last_page = 0;
}

