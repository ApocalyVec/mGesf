#ifndef XTFILE_H
#define XTFILE_H

#include <stdbool.h>
#include "xep_dispatch.h"

#define XTFILE_FORMAT_KEY   0x427F00C9

typedef enum {
    XTFILE_TYPE_CRASHDUMP      = 0x04554D50,
    XTFILE_TYPE_PARFILE        = 0x037DD98C,
    XTFILE_TYPE_NOISEMAP_SLOW  = 0x05F09BCA,
    XTFILE_TYPE_NOISEMAP_FAST  = 0x05F09BCB,
    XTFILE_TYPE_USER_DEFINED   = 0x80000000,
} xtfile_types_t;

/**
 * Error codes for File System
 */
typedef enum {
    /// Executed successfully
    XTFILE_OK = 0,
    /// Invalid parameter was given
    XTFILE_INVALID_PARAMETER = 1,
    /// File is already open
    XTFILE_FILE_ALREADY_OPEN = 2,
    /// File already exists
    XTFILE_FILE_ALREADY_EXISTS = 3,
    /// File is busy
    XTFILE_FILE_BUSY = 4,
    /// File was not found
    XTFILE_FILE_NOT_FOUND = 5,
    /// File is not open
    XTFILE_FILE_NOT_OPEN = 6,
    /// Could not allocate memory
    XTFILE_OUT_OF_MEMORY = 7,
    /// File system is not initialized
    XTFILE_SYSTEM_NOT_INITIALIZED = 8,
    /// File system is full
    XTFILE_SYSTEM_FULL = 9,
    /// File system is busy
    XTFILE_SYSTEM_BUSY = 10,
    /// All storage locations for instances is occupied
    XTFILE_TOO_MANY_FILES_OPEN = 11,
    /// Unknown failure
    XTFILE_UNKNOWN_FAILURE = 12,
    /// Locking mutex timed out
    XTFILE_LOCKED = 13,
    // Not supported
    XTFILE_NOT_SUPPORTED = 14, 
}  xtfile_error_codes_t;

#define XTFILE_MAX_OPEN_FILES       10

typedef struct FileInstance_t FileInstance_t;

typedef enum 
{
    FILE_INSTANCE_FREE,
    FILE_INSTANCE_NEW,
    FILE_INSTANCE_OPEN,
    FILE_INSTANCE_COMMIT,
    FILE_INSTANCE_DELETE,
} FileInstanceState_t;

struct FileInstance_t
{
    FileInstanceState_t state;
    uint32_t type;
    uint32_t identifier;
    void* data;
    uint32_t size;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the file system
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_init(
                        XepDispatch_t* dispatch,             ///< Pointer to the dispatcher
                        uint32_t address,                   ///< Address of application storage
                        uint32_t size                       ///< Size of application storage
                    );

/**
 * Uninitialize the file system
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_uninit(void);

/**
 * Formats the file system
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_format(uint32_t format_key);

/**
 * Gets the largest free section of space in the file system. 
 * This is the largest file that can be created without deleting files or defragmenting.
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_get_largest_free_size(
                                        uint32_t* size      ///< Pointer to variable to hold size
                                     );

uint32_t xtfile_get_handle(
                                    void** handle,          ///< Pointer to handle for opened file
                                    uint32_t type,          ///< Type of file to get handle for
                                    uint32_t identifier    ///< Identifier of file to get handle for
                            );

/**
 * Finds all files and stores the type and identifier of each file in the given buffers.
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xtfile_find_all_files(
                                    uint32_t* type,         ///< Pointer to list for types
                                    uint32_t* identifier,   ///< Pointer to list for identifiers
                                    uint32_t max_count,     ///< Max number of list
                                    uint32_t* count         ///< Pointer to variable to hold count of files
                                    );

/**
 * Searches for and returns a list of identifiers for all files of the specified type.
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_search_for_files_of_type(
                        uint32_t type,                  ///< Type of file to search for
                        uint32_t* identifier,           ///< Pointer to list for identifiers
                        uint32_t max_count,             ///< Max number of identifiers for list
                        uint32_t* count                 ///< Pointer to variable to hold count of files
                      );

/**
 * Gets the size of the file with the specified type and identifier. 
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_get_size(
                            uint32_t type,              ///< Type of file
                            uint32_t identifier,        ///< Identifier of file
                            uint32_t* size              ///< Pointer to variable to hold size
                        );

/**
 * Deletes the file with the specified type and identifier. 
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_delete(
                        uint32_t type,                  ///< Type of file
                        uint32_t identifier             ///< Identifier of file
                        );
/**
 * Creates and opens a new file with given type and identifiers with an empty content of given size. 
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_create_and_open_empty(
                                void** handle,          ///< Pointer to handle for opened file
                                uint32_t type,          ///< Type of file
                                uint32_t identifier,    ///< Identifier of file
                                uint32_t size           ///< Size of the new file
                                );

/**
 * Creates and opens a new file with given type and identifiers with the contents 
 * provided in the data buffer of given size. 
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_create_and_open(
                                void** handle,          ///< Pointer to handle for opened file
                                uint32_t type,          ///< Type of file
                                uint32_t identifier,    ///< Identifier of file
                                void* data,             ///< Buffer holding the data to write to the new file
                                uint32_t size           ///< Size of the new file
                                );
                                
/**
 * Opens an existing file with given type and identifier.
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_open(
                        void** handle,                  ///< Pointer to handle for opened file
                        uint32_t type,                  ///< Type of file
                        uint32_t identifier             ///< Identifier of file
                        );

/**
 * Gets the pointer and size of the content of the file.
 * The content of the buffer is located in flash and can not be modified directly, 
 * but this can be done by using xtfile_clear and xtfile_write commands.
 * The size of the file can not be changed, and must be done by deleting and creating a new file. 
 *
 * @return  Status of execoution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_get_buffer(
                        void* handle,               ///< Handle of open file
                        void** data,                ///< Pointer to flash location
                        uint32_t* size              ///< Pointer to variable to hold size of file
                        );

/**
 * Clears the entire file contents. Must be done prior to writing. 
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_clear(
                        void* handle                ///< Handle of open file
                        );

/**
 * Writes data to file. Make sure file contents is cleared before writing. 
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */
uint32_t xtfile_write(
                        void* handle,               ///< Handle of open file
                        void* data,                 ///< Pointer to data to write
                        uint32_t offset,            ///< Offset from file start
                        uint32_t size               ///< Size to write
                        );

/**
 * Closes an open file.
 *
 * @return  Status of execution as defined in @ref xtfile_error_codes_t
 */            
uint32_t xtfile_close(
                        void* handle               ///< Handle of open file to close
                        );

#ifdef __cplusplus
}
#endif

#endif
