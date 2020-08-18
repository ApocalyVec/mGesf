/**
 * @file
 *
 * @brief HW initialization interface
 *
 */
#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

/**
 * @brief Initializes board HW
 */
void board_init(void);
/**
 * @brief Set-ups external interface IOs
 */
void board_init_ext(void);
/**
 * @brief Initializes communication interface
 */
void application_init(void);
/**
 * @brief Tests if it should stay in the bootloader
 * Due to missing or corrupt application image or bootloader mode enforced by IO pin
 * @return true if bootloader mode enforced, false otherwise
 */
bool stayBootloader(void);
/**
 * @brief Reset watchdog timer
 */
void reset_watchdog(void);

#endif
