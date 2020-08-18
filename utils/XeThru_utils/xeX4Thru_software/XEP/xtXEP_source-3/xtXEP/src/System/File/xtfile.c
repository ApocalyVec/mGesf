#include "xtfile.h"
#include "xfs.h"
#include "xep_hal.h"
#include "xtmemory.h"
#include "xtsemaphore.h"
#include "task_monitor.h"
#include <FreeRTOS.h>
#include <task.h>
#include <string.h>

//#define DISABLE_XTFILE_ON_NOEXTRAM

void* xtfile_lock_object = NULL;

FileInstance_t file_instance[XTFILE_MAX_OPEN_FILES];

static bool xtfile_lock(void);
static void xtfile_unlock(void);

static uint32_t xtfile_create_instance(void** handle, uint32_t type, uint32_t identifier);
static uint32_t xtfile_free_instance(void* handle);
static bool xtfile_is_open(uint32_t type, uint32_t identifier);
static uint32_t xtfile_close_all_files(void);

uint32_t xtfile_init(XepDispatch_t* dispatch, uint32_t address, uint32_t size)
{
#ifdef DISABLE_XTFILE_ON_NOEXTRAM
    if (!xt_external_memory_enabled())
        return XTFILE_OUT_OF_MEMORY;
#endif

    if (xtfile_lock_object == NULL)
        xtfile_lock_object = xtlock_create();

    // Initialize file system
    xfs_init(address, size);

    // Initialize file instance table
    for (int i = 0; i < XTFILE_MAX_OPEN_FILES; i++)
        file_instance[i].state = FILE_INSTANCE_FREE;

    return XTFILE_OK;
}

uint32_t xtfile_uninit(void)
{
    // TODO - If any files are in the process of being written to flash, wait until done. 
    //        Also make sure that no new files can be opened or created. 
	
	return XTFILE_UNKNOWN_FAILURE;
}

uint32_t xtfile_format(uint32_t format_key)
{
    if (format_key != XTFILE_FORMAT_KEY)
        return XTFILE_INVALID_PARAMETER;

    uint32_t status = xtfile_close_all_files();

    if (status == XTFILE_OK)
        xfs_format();

    return status;
}

static bool xtfile_lock(void)
{
    return xtlock_lock(xtfile_lock_object, 250/portTICK_PERIOD_MS) == XTLOCK_OK;
}

static void xtfile_unlock(void)
{
	xtlock_unlock(xtfile_lock_object);
}

static uint32_t xtfile_close_all_files(void)
{
    if (!xtfile_lock())
        return XTFILE_LOCKED;

    for (int i = 0; i < XTFILE_MAX_OPEN_FILES; i++)
    {
        if (file_instance[i].state != FILE_INSTANCE_FREE)
            file_instance[i].state = FILE_INSTANCE_FREE;
    }

    xtfile_unlock();

    return XTFILE_OK;
}

static uint32_t xtfile_create_instance(void** handle, uint32_t type, uint32_t identifier)
{
    *handle = NULL;

    if (xtfile_is_open(type, identifier))
    {
        return XTFILE_FILE_ALREADY_OPEN;
    }

    for (int i = 0; i < XTFILE_MAX_OPEN_FILES; i++)
    {
        if (file_instance[i].state == FILE_INSTANCE_FREE)
        {
            file_instance[i].state = FILE_INSTANCE_NEW;
            file_instance[i].type = type;
            file_instance[i].identifier = identifier;
            file_instance[i].data = NULL;
            file_instance[i].size = 0;
                
            *handle = &file_instance[i];

            return XTFILE_OK;
        }
    }
        
    return XTFILE_TOO_MANY_FILES_OPEN;
}

static uint32_t xtfile_free_instance(void* handle)
{
    ((FileInstance_t*)handle)->state = FILE_INSTANCE_FREE;

    return XTFILE_OK;
}
static bool xtfile_is_open(uint32_t type, uint32_t identifier)
{
    for (int i = 0; i < XTFILE_MAX_OPEN_FILES; i++)
    {
        if ((file_instance[i].type == type) && 
            (file_instance[i].identifier == identifier))
        {
            if (file_instance[i].state != FILE_INSTANCE_FREE)
            {
                return true;
            }
        }
    }

    return false;
}

uint32_t xtfile_get_largest_free_size(uint32_t* size)
{
    switch (xfs_get_largest_free_size(size))
    {
        case XFS_OK:
            return XTFILE_OK;
        case XFS_NOT_INITIALIZED:
            return XTFILE_SYSTEM_NOT_INITIALIZED;
        case XFS_LOCKED:
            return XTFILE_SYSTEM_BUSY;
        default:
            break;
    }

    return XTFILE_UNKNOWN_FAILURE;
}

uint32_t xtfile_get_size(uint32_t type, uint32_t identifier, uint32_t* size)
{
    switch (xfs_get_size(type, identifier, size))
    {
        case XFS_OK:
            return XTFILE_OK;
        case XFS_NOT_INITIALIZED:
            return XTFILE_SYSTEM_NOT_INITIALIZED;
        case XFS_LOCKED:
            return XTFILE_SYSTEM_BUSY;
        case XFS_FILE_NOT_FOUND:
            return XTFILE_FILE_NOT_FOUND;
        default:
            break;
    }

    return XTFILE_UNKNOWN_FAILURE;
}

uint32_t xtfile_find_all_files(uint32_t* type, uint32_t* identifier, uint32_t max_count, uint32_t* count)
{
    switch (xfs_find_all_files(type, identifier, max_count, count))
    {
        case XFS_OK:
            return XTFILE_OK;
        case XFS_NOT_INITIALIZED:
            return XTFILE_SYSTEM_NOT_INITIALIZED;
        case XFS_INVALID_PARAMETER:
            return XTFILE_INVALID_PARAMETER;
        case XFS_LOCKED:
            return XTFILE_SYSTEM_BUSY;
        default:
            break;
    }

    return XTFILE_UNKNOWN_FAILURE;
}

uint32_t xtfile_search_for_files_of_type(uint32_t type, uint32_t* identifier, uint32_t max_count, uint32_t* count)
{
    switch (xfs_search_for_files_of_type(type, identifier, max_count, count))
    {
        case XFS_OK:
            return XTFILE_OK;
        case XFS_NOT_INITIALIZED:
            return XTFILE_SYSTEM_NOT_INITIALIZED;
        case XFS_INVALID_PARAMETER:
            return XTFILE_INVALID_PARAMETER;
        case XFS_LOCKED:
            return XTFILE_SYSTEM_BUSY;
        default:
            break;
    }

    return XTFILE_UNKNOWN_FAILURE;
}

uint32_t xtfile_create_and_open_empty(void** handle, uint32_t type, uint32_t identifier, uint32_t size)
{
    if (!xtfile_lock())
        return XTFILE_LOCKED;

    // Create entry for file instance
    *handle = NULL;
    uint32_t status = xtfile_create_instance(handle, type, identifier);
    if (status != XTFILE_OK)
    {
        xtfile_unlock();
        return status;
    }

    // Create initial file entry
    switch (xfs_create_file(type, identifier, size))
    {
        case XFS_NOT_INITIALIZED:
            status = XTFILE_SYSTEM_NOT_INITIALIZED;
			break;
        case XFS_LOCKED:
            status = XTFILE_SYSTEM_BUSY;
            break;
        case XFS_FILE_ALREADY_EXISTS:
            status = XTFILE_FILE_ALREADY_EXISTS;
            break;
        case XFS_FILE_ENTRY_FULL:
        case XFS_NO_FREE_SPACE:
            status = XTFILE_SYSTEM_FULL;
            break;
        case XFS_OK:
            status = XTFILE_OK;
            break;
        default:
            status = XTFILE_UNKNOWN_FAILURE;
            break;
    }
    if (status != XTFILE_OK)
    {
        xtfile_free_instance(*handle);
        xtfile_unlock();
        return status;
    }

    // Get file pointers
    void* data = NULL;
    uint32_t data_size = 0;
    xfs_get_file_pointer(type, identifier, &data, &data_size);

    ((FileInstance_t*)*handle)->state = FILE_INSTANCE_OPEN;
    ((FileInstance_t*)*handle)->data = data;
    ((FileInstance_t*)*handle)->size = size;

    // Erase flash area
    xfs_clear_file(type, identifier);

    xtfile_unlock();

    return XTFILE_OK;
}

uint32_t xtfile_create_and_open(void** handle, uint32_t type, uint32_t identifier, void* data, uint32_t size)
{
    // Create and open an empty file
    uint32_t status = xtfile_create_and_open_empty(handle, type, identifier, size);
    if (status != XFS_OK)
        return status;

    // Write data to flash
    if (!xtfile_lock())
        return XTFILE_LOCKED;

    xfs_write_file(type, identifier, data, 0, size);

    xtfile_unlock();

    return XTFILE_OK;
}

uint32_t xtfile_delete(uint32_t type, uint32_t identifier)
{
    if (!xtfile_lock())
        return XTFILE_LOCKED;

    if (xtfile_is_open(type, identifier))
    {
        xtfile_unlock();
        return XTFILE_FILE_ALREADY_OPEN;
    }

    if (xfs_file_exists(type, identifier) != XFS_OK)
    {
        xtfile_unlock();
        return XTFILE_FILE_NOT_FOUND;
    }

    void* handle = NULL;
    uint32_t status = xtfile_create_instance(&handle, type, identifier);
    if (status != XTFILE_OK)
    {
        xtfile_unlock();
        return status;
    }
    FileInstance_t instance = *((FileInstance_t*)handle);
    ((FileInstance_t*)handle)->state = FILE_INSTANCE_FREE;

    xfs_delete_file(instance.type, instance.identifier);

    xtfile_unlock();

    return XTFILE_OK;
}

uint32_t xtfile_get_handle(void** handle, uint32_t type, uint32_t identifier)
{
    *handle = NULL;

    if (!xtfile_lock())
        return XTFILE_LOCKED;
    
    for (int i = 0; i < XTFILE_MAX_OPEN_FILES; i++)
    {
        if ((file_instance[i].type == type) && 
            (file_instance[i].identifier == identifier))
        {
            if (file_instance[i].state != FILE_INSTANCE_FREE)
            {
                *handle = &file_instance[i];
            }
        }
    }

    xtfile_unlock();

    if (*handle == NULL)
        return XTFILE_FILE_NOT_OPEN;
        
    return XTFILE_OK;
}

uint32_t xtfile_open(void** handle, uint32_t type, uint32_t identifier)
{
    *handle = NULL;

    if (!xtfile_lock())
        return XTFILE_LOCKED;

    if (xtfile_is_open(type, identifier))
    {
        xtfile_unlock();
        return XTFILE_FILE_ALREADY_OPEN;
    }

    uint32_t size = 0;
    switch (xfs_get_size(type, identifier, &size))
    {
		case XFS_OK:
			break;
        case XFS_FILE_NOT_FOUND:
            xtfile_unlock();
            return XTFILE_FILE_NOT_FOUND;
        case XFS_NOT_INITIALIZED:
        default:
            xtfile_unlock();
            return XTFILE_UNKNOWN_FAILURE;
    }

    uint32_t status = xtfile_create_instance(handle, type, identifier);
    if (status != XTFILE_OK)
    {
        xtfile_unlock();
        return status;
    }
    
    // Get file pointers
    void* data = NULL;
    uint32_t data_size = 0;
    xfs_get_file_pointer(type, identifier, &data, &data_size);

    ((FileInstance_t*)*handle)->state = FILE_INSTANCE_OPEN;
    ((FileInstance_t*)*handle)->data = data;
    ((FileInstance_t*)*handle)->size = data_size;

    xtfile_unlock();

    return XTFILE_OK;
}

uint32_t xtfile_get_buffer(void* handle, void** data, uint32_t* size)
{
    if (!xtfile_lock())
        return XTFILE_LOCKED;

    if (((FileInstance_t*)handle)->state != FILE_INSTANCE_OPEN)
    {
        xtfile_unlock();
        return XTFILE_FILE_NOT_OPEN;
    }

    *data = ((FileInstance_t*)handle)->data;
    *size = ((FileInstance_t*)handle)->size;

    xtfile_unlock();

    return XTFILE_OK;
}

uint32_t xtfile_clear(void* handle)
{
    if (!xtfile_lock())
        return XTFILE_LOCKED;

    if (((FileInstance_t*)handle)->state != FILE_INSTANCE_OPEN)
    {
        xtfile_unlock();
        return XTFILE_FILE_NOT_OPEN;
    }

    // Erase flash area
    xfs_clear_file(((FileInstance_t*)handle)->type, ((FileInstance_t*)handle)->identifier);
    
    xtfile_unlock();
    
    return XTFILE_OK;
}

uint32_t xtfile_write(void* handle, void* data,
                        uint32_t offset, uint32_t size)
{
    if (!xtfile_lock())
        return XTFILE_LOCKED;

    if (((FileInstance_t*)handle)->state != FILE_INSTANCE_OPEN)
    {
        xtfile_unlock();
        return XTFILE_FILE_NOT_OPEN;
    }

    // Write to flash file
    xfs_write_file(((FileInstance_t*)handle)->type, ((FileInstance_t*)handle)->identifier,
                            data, offset, size);
                            
    xtfile_unlock();
    
    return XTFILE_OK;
}

uint32_t xtfile_close(void* handle)
{
    if (!xtfile_lock())
        return XTFILE_LOCKED;

    if (((FileInstance_t*)handle)->state != FILE_INSTANCE_OPEN)
    {
        xtfile_unlock();
        return XTFILE_FILE_NOT_OPEN;
    }

    ((FileInstance_t*)handle)->state = FILE_INSTANCE_FREE;

    xtfile_unlock();

    return XTFILE_OK;
}