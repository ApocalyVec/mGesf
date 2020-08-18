/**
 * @file
 *
 * @brief Communication interface abstraction layer functions
 *
 */

#ifndef COMM_IFACE_H_
#define COMM_IFACE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Maximum length of fifos
#define FIFO_BUFFER_LENGTH  2048

/**
 * Direction of the interface
 */
typedef enum {
    IFACE_OUT=0,                 ///< Interface for outgoing communication
    IFACE_IN                     ///< Interface for incomming communication
} ifcdir_t;
/**
 * Types of the interfaces used to comminicate with external world
 */
typedef enum {
    IFACE_NONE=0,                ///< None interface
    IFACE_USB,                   ///< USB interface
    IFACE_USART,                 ///< USART interface
    IFACE_I2C                    ///< I2C interface
} ifctype_t;
/**
 * @brief Initializes HW interface and fifos
 */
void InitCommIface(
    ifctype_t type               ///< Type of communication interface
);
/**
 * @brief Tests if fifo is empty
 * @return true if fifo is empty, false otherwise
 */
bool CommIface_is_empty(
    ifcdir_t dir                 ///< Direction of the interface (IFACE_OUT or IFACE_IN)
);
/**
 * @brief Gets a byte from fifo
 * @return byte from fifo
 */
uint8_t CommIface_get(
    ifcdir_t dir                 ///< Direction of the interface (IFACE_OUT or IFACE_IN)
);
/**
 * @brief Adds a byte to fifo
 */
void CommIface_put(
    ifcdir_t dir,                ///< Direction of the interface (IFACE_OUT or IFACE_IN)
    uint8_t b                    ///< Byte to add to the fifo
);
/**
 * @brief Sends the fifo
 */
void CommIface_flush(
    ifcdir_t iface               ///< Type of communication interface
);
/**
 * @brief Clears the fifo
 */
void CommIface_Clear(
    ifcdir_t dir                 ///< Direction of the interface (IFACE_OUT or IFACE_IN)
);

#endif /* COMM_IFACE_H_ */