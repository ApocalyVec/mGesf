/**
 * @file
 * @mainpage 
 * How to use the xtOpenBootloader and XEP:
 * 
 * *run 'build.sh' to build the bootloader \n
 * *flash the X4 module with the bootloader\n
 * *build application FW (XEP) for the bootloader (xep_x4m0x_s70_4bl.hex)\n
 * *run 'add_checksum.py -i xep_x4m0x_s70_4bl.hex -o xep_x4m0x_s70_4bl_csum.hex' to calculate checksum and combine it with the FW application\n
 * *use script/SW to update the FW (e.g. upgrade_firmware_x4m300.py)\n
 * 
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <asf.h>
#include "protocol.h"
#include <xt_config/xt_config.h>
#include <nvm/nvm_sections.h>
#include "compiler.h"
#include "led_ctrl.h"
#include <nvm/nvm.h>
#include <conf_nvm.h>
#include <comm/comm_iface.h>
#include <comm/protocol_handlers.h>
#include "init.h"

// PLL source configuration
int config_pll0_source = SYSCLK_SRC_MAINCK_XTAL; 

// Variables needed for message parser
static uint8_t destination[2048]  __attribute__ ((aligned (4)));
static xtProtocol * protocolInstance = 0;
/**
 * @brief main() function of the xtOpenBootloader
 */
int main (void)
{
    // Read flash based configuration
    xt_config_init();
    
    if (XT_CONFIG_SYSTEM_INT_ONLY_OSC())
        config_pll0_source = SYSCLK_SRC_MAINCK_12M_RC;
    else if (XT_CONFIG_SYSTEM_BYPASS_OSC())
        config_pll0_source = SYSCLK_SRC_MAINCK_BYPASS;

    // Configure LED IOs
    led_ctrl_init();
    // Initialize the SAM system.
    irq_initialize_vectors();
    sleepmgr_init();
    sysclk_init();
    board_init();
    nvm_init();  

	led_ctrl(OFF);
    // Check if we need to stay in bootloader
    bool _do_bootloader = stayBootloader();
    // Do board initialization specific to interface mode.
    board_init_ext(); 
    if (!_do_bootloader) {
        // indicate that we enter application 
        delay_ms(5);
        reset_watchdog();
        nvm_exec_app((void *)APP_START_ADDRESS);
        // Does not return
    }
    // Prepare message parser
    unsigned char instanceMemory[getInstanceSize()];
    protocolInstance = createApplicationProtocol(
        &handleProtocolPacket,
        &handleProtocolError,
        0,
        instanceMemory,
        destination,
        sizeof(destination));

    // Application specific setup
    application_init();
    // Event driven from here on
    cpu_irq_enable();
    // Visual cue that we are in bootloader
    led_ctrl(IN_BOOT);

    while (1) {
        reset_watchdog();
        if (!CommIface_is_empty(IFACE_IN)) {
            uint8_t b = CommIface_get(IFACE_IN);
            int result = parseData(protocolInstance, &b, 1);
            if (result < 0) {
                led_ctrl(ERROR);
            }
        }
    }
}
