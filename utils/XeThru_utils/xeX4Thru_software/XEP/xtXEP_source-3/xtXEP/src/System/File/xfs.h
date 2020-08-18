#ifndef XFS_H
#define XFS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define XFS_MAGIC_KEY   0x24584653

/**
 * Error codes for File System
 */
typedef enum {
    /// Executed successfully
    XFS_OK                  = 0,
    /// File system is not initialized
    XFS_NOT_INITIALIZED     = 1,
    /// Invalid parameter was given
    XFS_INVALID_PARAMETER   = 2,
    /// Could not allocate memory
    XFS_OUT_OF_MEMORY       = 3,
    /// File system is corrupted
    XFS_CORRUPTED           = 4,
    /// File was not found
    XFS_FILE_NOT_FOUND      = 5,
    /// File already exists
    XFS_FILE_ALREADY_EXISTS = 6,
    /// File system is full
    XFS_NO_FREE_SPACE       = 7,
    /// Function not implemented
    XFS_NOT_IMPLEMENTED     = 8,
    /// All available locations in the allocation table is taken
    XFS_FILE_ENTRY_FULL     = 9,
    /// Error occured while storing to flash
    XFS_STORAGE_ERROR       = 10,
    /// Lock mutex timed out
    XFS_LOCKED              = 11,
} xfs_error_codes_t;

#define XFS_MAX_FILES   20

typedef struct XFSHeader_t XFSHeader_t;
typedef struct XFSAllocationTableV1_t XFSAllocationTableV1_t;
typedef struct XFSFileEntryV1_t XFSFileEntryV1_t;

struct XFSHeader_t
{
    uint32_t magic_key;
    uint32_t version;
};

struct XFSFileEntryV1_t
{
    uint32_t offset; // Offset from start of data section
    uint32_t allocated_size;
    uint32_t used_size;
    uint32_t type;
    uint32_t identifier;
}; 

struct XFSAllocationTableV1_t
{
    uint32_t crc;
    uint32_t page_size;
    uint32_t block_size;
    uint32_t file_entry_count;
    struct XFSFileEntryV1_t file_entries[XFS_MAX_FILES];
};

/**
 * Initialize the file system
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_init(
                    uint32_t address,                   ///< Address of application storage
                    uint32_t size                       ///< Size of application storage
                 );

/**
 * Function to ignore locking mechanism
 *
 * @return Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_ignore_lock(bool ignore);

/**
 * Formats the file system to an empty state
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_format(void);

/**
 * Defragments the file system to optimize the free space
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_defragment(void);

/**
 * Gets the size of the largest free section. 
 * This is the size of the largest file that can be stored without deleting files or defragmenting.
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_get_largest_free_size(
                                    uint32_t* size          ///< Pointer to variable that will hold free size
                                    );

/**
 * Finds all files and stores the type and identifier of each file in the given buffers.
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_find_all_files(
                                    uint32_t* type,         ///< Pointer to list for types
                                    uint32_t* identifier,   ///< Pointer to list for identifiers
							        uint32_t max_count,     ///< Max number of list
                                    uint32_t* count         ///< Pointer to variable to hold count of files
                                    );

/**
 * Searches for files of a specific type and stores the identifier of each file in the given buffer.
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_search_for_files_of_type(
                                    uint32_t type,          ///< Type of file
                                    uint32_t* identifier,   ///< Pointer to list for identifiers
							        uint32_t max_count,     ///< Max number of identifiers for list
                                    uint32_t* count         ///< Pointer to variable to hold count of files
                                    );
                                    
/**
 * Checks if a file with given type and identifier exists.
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_file_exists(
                            uint32_t type,                  ///< Type of file
                            uint32_t identifier             ///< Identifier of file
                            );

/**
 * Gets the size of file with given type and identifier.
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_get_size(
                            uint32_t type,                  ///< Type of file
                            uint32_t identifier,            ///< Identifier of file
                            uint32_t* size                  ///< Pointer to variable to hold size of file
                            );

/**
 * Creates a new file. This only creates an entry in the buffered allocation table.
 * The file will not be stored to flash before commit is called. 
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_create_file(
                            uint32_t type,                  ///< Type of file
                            uint32_t identifier,            ///< Identifier of file
                            uint32_t size                   ///< Size of file
                            );

/**
 * Clears the flash area used to store file data.
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_clear_file(
                            uint32_t type,                  ///< Type of file
                            uint32_t identifier             ///< Identifier of file
                            );

/**
 * Writes the file data to flash at given offset. 
 * Make sure to clear the file before writing data. 
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_write_file(
                            uint32_t type,                  ///< Type of file
                            uint32_t identifier,            ///< Identifier of file
                            void* data,                     ///< Pointer to file contents to write
                            uint32_t data_offset,           ///< Offset into file to write contents
                            uint32_t size                   ///< Size of contents
                            );

/**
 * Gets a pointer to the file data in flash. 
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_get_file_pointer(
                            uint32_t type,                  ///< Type of file
                            uint32_t identifier,            ///< Identifier of file
                            void** data,                    ///< Pointer to file contents
                            uint32_t* size                  ///< Size of contents
                            );

/**
 * Commits the file allocation and file contents to flash.
 * Note that the file allocation will be committed for all pending files, 
 * no matter what type and identifier is given.
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_commit_file(
                            uint32_t type,                  ///< Type of file
                            uint32_t identifier,            ///< Identifier of file
                            void* data,                     ///< Pointer to file contents to commit
                            uint32_t size                   ///< Size of file, must match with the allocated size and can't be changed
                            );

/**
 * Deletes the file with the given type and identifier. 
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_delete_file(
                            uint32_t type,                  ///< Type of file
                            uint32_t identifier             ///< Identifier of file
                            );

/**
 * Copies the data from the file into the given buffer.
 *
 * @return  Status of execution as defined in @ref xfs_error_codes_t
 */
uint32_t xfs_get_file_data(
                            uint32_t type,                  ///< Type of file
                            uint32_t identifier,            ///< Identifier of file
                            void* data,                     ///< Pointer to buffer to store data in
                            uint32_t data_offset,           ///< Offset into file to start read, must be 4-byte aligned
                            uint32_t max_size,              ///< Maximum size to read
                            uint32_t* size                  ///< Pointer to variable to store size read
                            );

#endif