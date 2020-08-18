#include "xfs.h"
#include "crc.h"
#include "xep_hal.h"
#include "xtmemory.h"
#include "xtsemaphore.h"
#include <FreeRTOS.h>
#include <string.h>

void* xfs_lock_object = NULL;
bool xfs_lock_ignored = false;

bool xfs_initialized = false;
void* xfs_start = NULL;
int xfs_size = -1;
int xfs_page_size = -1;
int xfs_block_size = -1;
const int xfs_allocation_table_size = 512;

void* xfs_allocation_table_buffer = NULL;

static bool xfs_lock(void);
static void xfs_unlock(void);

static uint32_t xfs_delete_file_entry_from_allocation_table(uint32_t type, uint32_t identifier);
static uint32_t xfs_store_file_entry_in_allocation_table(struct XFSFileEntryV1_t* file_entry);
static uint32_t xfs_update_allocation_table_crc(void);
static uint32_t xfs_commit_allocation_table(void* buffer);

static int xfs_file_count(void);
static uint32_t xfs_get_file_entry(uint32_t type, uint32_t identifier, XFSFileEntryV1_t* file_entry);

static void* xfs_file_offset_to_pointer(uint32_t offset);
static uint32_t xfs_find_free_contiguous_section_of_size(uint32_t requested_size, uint32_t* offset);

static uint32_t xfs_check_integrity(void);

uint32_t xfs_init(uint32_t address, uint32_t size)
{
    if (xfs_lock_object == NULL)
        xfs_lock_object = xtlock_create();

    // Initialize CRC calculation
    crcInit();
    
    xfs_start = address;
    xfs_size = size;
    xfs_page_size = xtio_get_block_size();
    xfs_block_size = xtio_get_block_size();

    // Size must be divisible in blocks
    xfs_size = xfs_size - (xfs_size % xfs_block_size); 

    // Size must be at least two pages
    if (xfs_size < (xfs_page_size*2))
        return XFS_NO_FREE_SPACE;

    // Initialize allocation table buffer
    xfs_allocation_table_buffer = xtmemory_malloc_slow(xfs_allocation_table_size);
    if (xfs_allocation_table_buffer == NULL)
        return XFS_OUT_OF_MEMORY;
    memcpy(xfs_allocation_table_buffer, xfs_start, xfs_allocation_table_size);

	xfs_initialized = true;

    // Verify that the integrity of the allocation table is good
    if (xfs_check_integrity() != XFS_OK)
        xfs_format();
	
	return XFS_OK;
}

uint32_t xfs_ignore_lock(bool ignore)
{
    xfs_lock_ignored = ignore;

    return XFS_OK;
}

uint32_t xfs_defragment(void)
{
    return XFS_NOT_IMPLEMENTED;
}

static bool xfs_lock(void)
{
    if (xfs_lock_ignored)
        return true;

    return xtlock_lock(xfs_lock_object, 250/portTICK_PERIOD_MS) == XTLOCK_OK;
}

static void xfs_unlock(void)
{
	if (xfs_lock_ignored)
		return;

	xtlock_unlock(xfs_lock_object);
}

static void* xfs_file_offset_to_pointer(uint32_t offset)
{
    return (void*)((uint32_t)xfs_start + xfs_page_size + offset);
}

static int xfs_file_count(void)
{
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
    
    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_start + sizeof(XFSHeader_t));

    return xfs_table->file_entry_count;
}

uint32_t xfs_file_exists(uint32_t type, uint32_t identifier)
{
    if (!xfs_lock())
        return XFS_LOCKED;

	int file_count = xfs_file_count();
	XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_start + sizeof(XFSHeader_t));

	for (int i = 0; i < file_count; i++)
	{
		if ((type == xfs_table->file_entries[i].type) &&
		(identifier == xfs_table->file_entries[i].identifier))
		{
            xfs_unlock();
			return XFS_OK;
		}
	}
    
    xfs_unlock();
	
    return XFS_FILE_NOT_FOUND;
}

uint32_t xfs_find_all_files(uint32_t* type, uint32_t* identifier, uint32_t max_count, uint32_t* count)
{
    *count = 0;

    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
    
    if (!xfs_lock())
        return XFS_LOCKED;
    
    int file_count = xfs_file_count();
    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_start + sizeof(XFSHeader_t));

    for (int i = 0; (i < file_count) && (*count < max_count); i++)
    {
        type[*count] = xfs_table->file_entries[i].type;
        identifier[*count] = xfs_table->file_entries[i].identifier;
        (*count)++;
    }

    xfs_unlock();

    return XFS_OK;
}

uint32_t xfs_search_for_files_of_type(uint32_t type, uint32_t* identifier, 
							uint32_t max_count, uint32_t* count)
{
    *count = 0;

    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
    
    if (!xfs_lock())
        return XFS_LOCKED;
    
    int file_count = xfs_file_count();
    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_start + sizeof(XFSHeader_t));

    for (int i = 0; (i < file_count) && (*count < max_count); i++)
    {
        if (type == xfs_table->file_entries[i].type)
        {
            identifier[*count] = xfs_table->file_entries[i].identifier;
            (*count)++;
        }
    }

    xfs_unlock();

    return XFS_OK;
}

static uint32_t xfs_find_free_contiguous_section_of_size(uint32_t requested_size, uint32_t* offset)
{
    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_start + sizeof(XFSHeader_t));

    // Look for free space from start to finish, take first free section found
    if (xfs_table->file_entry_count == 0)
    {
		*offset = 0;
		return XFS_OK;
	} else 
	{
        if (xfs_table->file_entries[0].offset >= requested_size)
        {
            *offset = 0;
            return XFS_OK;
        }
    }
    for (int i = 1; i < (int)xfs_table->file_entry_count; i++)
    {
        uint32_t free_space_between_files = xfs_table->file_entries[i].offset - 
            (xfs_table->file_entries[i-1].offset + xfs_table->file_entries[i-1].allocated_size);
        
        if (free_space_between_files >= requested_size)
        {
            *offset = xfs_table->file_entries[i-1].offset + xfs_table->file_entries[i-1].allocated_size;
            return XFS_OK;
        }
    }
    if (xfs_table->file_entry_count > 0)
    {
        uint32_t free_space_at_end = xfs_size - xfs_page_size - 
            (xfs_table->file_entries[xfs_table->file_entry_count - 1].offset +
            xfs_table->file_entries[xfs_table->file_entry_count - 1].allocated_size);
        
        if (free_space_at_end >= requested_size)
        {
            *offset = (xfs_table->file_entries[xfs_table->file_entry_count - 1].offset +
                        xfs_table->file_entries[xfs_table->file_entry_count - 1].allocated_size);
            return XFS_OK;
        }
    }

    return XFS_NO_FREE_SPACE;
}

static uint32_t xfs_get_file_entry(uint32_t type, uint32_t identifier, XFSFileEntryV1_t* file_entry)
{
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;

    if (!xfs_lock())
        return XFS_LOCKED;

    int file_count = xfs_file_count();
    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_start + sizeof(XFSHeader_t));

    for (int i = 0; i < file_count; i++)
    {
        if ((type == xfs_table->file_entries[i].type) && 
            (identifier == xfs_table->file_entries[i].identifier))
        {
            memcpy(file_entry, &xfs_table->file_entries[i], sizeof(XFSFileEntryV1_t));

			xfs_unlock();

            return XFS_OK;
        }
    }
	
    xfs_unlock();

    return XFS_FILE_NOT_FOUND;
}

static uint32_t xfs_update_allocation_table_crc(void)
{
    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_allocation_table_buffer + sizeof(XFSHeader_t));

    xfs_table->crc = 0xFFFFFFFF;
    xfs_table->crc = crcFastWithPad(xfs_allocation_table_buffer, xfs_allocation_table_size, (const unsigned char)0xFF, xfs_page_size-xfs_allocation_table_size);

    return XFS_OK;
}

static uint32_t xfs_store_file_entry_in_allocation_table(struct XFSFileEntryV1_t* file_entry)
{
    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_allocation_table_buffer + sizeof(XFSHeader_t));

    // Find index of where this entry should be stored
    int index = xfs_table->file_entry_count;
    for (int i = 0; i < (int)xfs_table->file_entry_count; i++)
    {
        if (xfs_table->file_entries[i].offset > file_entry->offset)
        {
            index = i;

            break;
        }
    }

    // Move entries to make space
    for (int i = xfs_table->file_entry_count; i > index; i--)
        xfs_table->file_entries[i] = xfs_table->file_entries[i-1];
    
    // Put entry into table
    xfs_table->file_entries[index] = *file_entry;
	xfs_table->file_entry_count++;
	
	xfs_update_allocation_table_crc();

    return XFS_OK;
}

static uint32_t xfs_delete_file_entry_from_allocation_table(uint32_t type, uint32_t identifier)
{
    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_allocation_table_buffer + sizeof(XFSHeader_t));

    // Find index of entry
    int index = -1;
    for (int i = 0; i < (int)xfs_table->file_entry_count; i++)
    {
        if ((xfs_table->file_entries[i].type == type) && 
            (xfs_table->file_entries[i].identifier == identifier))
        {
            index = i;

            break;
        }
    }

    if (index == -1)
        return XFS_FILE_NOT_FOUND;
    
    // Move all entries below upwards to fill gap
    for (int i = index; i < xfs_table->file_entry_count-1; i++)
        xfs_table->file_entries[i] = xfs_table->file_entries[i+1];
    
    xfs_table->file_entry_count--;

    xfs_update_allocation_table_crc();

    return XFS_OK;
}

static uint32_t xfs_commit_allocation_table(void* buffer)
{
    // Store updated table
    if (xtio_flash_erase((uint32_t)xfs_start, xfs_page_size))
    {
        return XFS_STORAGE_ERROR;
    }
    if (xtio_flash_write((uint32_t)xfs_start, (uint8_t*)buffer, xfs_allocation_table_size))
    {
        return XFS_STORAGE_ERROR;
    }

    return XFS_OK;
}

uint32_t xfs_get_largest_free_size(uint32_t* size)
{
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;

    if (!xfs_lock())
        return XFS_LOCKED;

    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_allocation_table_buffer + sizeof(XFSHeader_t));

    // If there are no files, we have all the space, except the allocation table
    if (xfs_table->file_entry_count == 0)
    {
		*size = xfs_size - xfs_page_size;
        xfs_unlock();
		return XFS_OK;
	}

    // Look for free space from start to finish
    uint32_t largest_free_section = 0;
    for (int i = 0; i < (int)xfs_table->file_entry_count-1; i++)
    {
        uint32_t free_space_between_files = xfs_table->file_entries[i+1].offset - 
            (xfs_table->file_entries[i].offset + xfs_table->file_entries[i].allocated_size);
        
        if (free_space_between_files > largest_free_section)
        {
            largest_free_section = free_space_between_files;
        }
    }
    if (xfs_table->file_entry_count > 0)
    {
        uint32_t free_space_at_end = xfs_size - xfs_page_size - 
            (xfs_table->file_entries[xfs_table->file_entry_count - 1].offset +
            xfs_table->file_entries[xfs_table->file_entry_count - 1].allocated_size);
        
        if (free_space_at_end > largest_free_section)
        {
            largest_free_section = free_space_at_end;
        }
    }

    xfs_unlock();

	*size = largest_free_section;

    return XFS_OK;
}

uint32_t xfs_get_size(uint32_t type, uint32_t identifier, uint32_t* size)
{
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
        
    XFSFileEntryV1_t xfs_file_entry;
    if (xfs_get_file_entry(type, identifier, &xfs_file_entry) == XFS_OK)
    {
        *size = xfs_file_entry.used_size;

        return XFS_OK;
    }

    return XFS_FILE_NOT_FOUND;
}

uint32_t xfs_create_file(uint32_t type, uint32_t identifier, uint32_t size)
{
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
    
    if (size < 1)
        return XFS_INVALID_PARAMETER;
	
    // Check for already existing file
    if (xfs_file_exists(type, identifier) == XFS_OK)
    {
        return XFS_FILE_ALREADY_EXISTS;
    }

    if (!xfs_lock())
        return XFS_LOCKED;
    
    // Check that the allocation table has room for a new entry
    if (xfs_file_count() >= XFS_MAX_FILES)
    {
        xfs_unlock();
        return XFS_FILE_ENTRY_FULL;
    }

    // Find a big enough contiguous section of free space
    uint32_t offset = 0;
    uint32_t allocated_size = size;
    if ((size % xfs_page_size) != 0)
        allocated_size += xfs_page_size - (size % xfs_page_size);
    if (xfs_find_free_contiguous_section_of_size(allocated_size, &offset) != XFS_OK)
    {
        xfs_unlock();
        return XFS_NO_FREE_SPACE; // Consider doing a defragmentation
    }

    // Put file entry into table, maintain order
    XFSFileEntryV1_t file_entry = {
        .offset = offset,
        .allocated_size= allocated_size,
        .used_size = size,
        .type = type,
        .identifier = identifier
    };
    uint32_t status = xfs_store_file_entry_in_allocation_table(&file_entry);
    if (status != XFS_OK)
    {
        xfs_unlock();
        return status;
    }
    
    // Commit allocation table changes
    status = xfs_commit_allocation_table(xfs_allocation_table_buffer);
    xfs_unlock();
    if (status != XFS_OK)
    {
        return status;
    }

	return XFS_OK;
}

uint32_t xfs_clear_file(uint32_t type, uint32_t identifier)
{
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
        
    // Verify file entry exists
    XFSFileEntryV1_t file_entry;
    if (xfs_get_file_entry(type, identifier, &file_entry) != XFS_OK)
    {
        return XFS_FILE_NOT_FOUND;
    }

    // Store file contents
    void* file_pointer = xfs_file_offset_to_pointer(file_entry.offset);
    uint32_t size = file_entry.allocated_size;
    if (xtio_flash_erase((uint32_t) file_pointer, (size % xfs_page_size) ? (size + xfs_page_size) & ~(xfs_page_size - 1) : size))
    {
        return XFS_STORAGE_ERROR;
    }
    
    return XFS_OK;
}

uint32_t xfs_write_file(uint32_t type, uint32_t identifier,
                        void* data, uint32_t data_offset, uint32_t size)
{
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
        
    // Verify file entry exists
    XFSFileEntryV1_t file_entry;
    if (xfs_get_file_entry(type, identifier, &file_entry) != XFS_OK)
    {
        return XFS_FILE_NOT_FOUND;
    }

    // Store file contents
    void* file_pointer = xfs_file_offset_to_pointer(file_entry.offset);

    if (xtio_flash_write((uint32_t) file_pointer + data_offset, (uint8_t*)data, size))
    {
        return XFS_STORAGE_ERROR;
    }
    
    return XFS_OK;
}

uint32_t xfs_get_file_pointer(uint32_t type, uint32_t identifier, 
                                void** data, uint32_t* size)
{
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
        
    // Verify file entry exists
    XFSFileEntryV1_t file_entry;
    if (xfs_get_file_entry(type, identifier, &file_entry) != XFS_OK)
    {
        return XFS_FILE_NOT_FOUND;
    }

    // Store file contents
    *data = xfs_file_offset_to_pointer(file_entry.offset);
    *size = file_entry.used_size;

    return XFS_OK;
}

uint32_t xfs_commit_file(uint32_t type, uint32_t identifier, void* data, uint32_t size)
{    
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
	    
    // Verify file entry exists
    XFSFileEntryV1_t file_entry;
    if (xfs_get_file_entry(type, identifier, &file_entry) != XFS_OK)
    {
        return XFS_FILE_NOT_FOUND;
    }

    if (file_entry.used_size != size)
    {
        return XFS_INVALID_PARAMETER;
    }

    // Store file contents
    if (xfs_clear_file(type, identifier) != XFS_OK)
        return XFS_STORAGE_ERROR;
    if (xfs_write_file(type, identifier, data, 0, size) != XFS_OK)
        return XFS_STORAGE_ERROR;
        
    return XFS_OK;
}

uint32_t xfs_delete_file(uint32_t type, uint32_t identifier)
{
	if (!xfs_lock())
		return XFS_LOCKED;
    
    uint32_t status = xfs_delete_file_entry_from_allocation_table(type, identifier);
    if (status != XFS_OK)
    {
        xfs_unlock();
        return status;
    }

    status = xfs_commit_allocation_table(xfs_allocation_table_buffer);

    xfs_unlock();
    
    return status;
}

uint32_t xfs_get_file_data(uint32_t type, uint32_t identifier, 
					    void* data, uint32_t data_offset, 
                        uint32_t max_size, uint32_t* size)
{   
    *size = 0;

    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;
    
    if ((data_offset % 4) != 0)
        return XFS_INVALID_PARAMETER;

    XFSFileEntryV1_t xfs_file_entry;
    if (xfs_get_file_entry(type, identifier, &xfs_file_entry) == XFS_OK)
    {
        void* file_data = xfs_file_offset_to_pointer(xfs_file_entry.offset + data_offset);
            
        int data_size = xfs_file_entry.used_size - data_offset;
        if (data_size > (int)max_size)
            data_size = max_size;
        if (data_size < 0)
            data_size = 0;
		
        *size = data_size;
		
		// Optimizing for aligned accesses
		// Leaving this to the compiler seems to crash sometimes
		int remainder = data_size % 4;
		memcpy(data, file_data, data_size - remainder);
		for (int i = data_size - remainder; i < data_size; i++)
			((uint8_t*)data)[i] = ((uint8_t*)file_data)[i];
           
        return XFS_OK;
    }

    return XFS_FILE_NOT_FOUND;
}

uint32_t xfs_format(void)
{        
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;

	// Clear all data in allocation table
	memset(xfs_allocation_table_buffer, 0xFF, xfs_allocation_table_size);

    // Build file system header
    XFSHeader_t* xfs_header = (struct XFSHeader_t*) xfs_allocation_table_buffer;
    xfs_header->magic_key = XFS_MAGIC_KEY;
    xfs_header->version = 1;

    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_header + sizeof(XFSHeader_t));
    xfs_table->page_size = xfs_page_size;
    xfs_table->block_size = xfs_block_size;
    xfs_table->file_entry_count = 0;

    // Calculate CRC after all fields have been filled
	xfs_update_allocation_table_crc();

    // Store file system header
    if (xtio_flash_erase((uint32_t)xfs_start, xfs_page_size))
    {
        return XFS_STORAGE_ERROR;
    }
    if (xtio_flash_write((uint32_t)xfs_start, (uint8_t*)xfs_allocation_table_buffer, xfs_allocation_table_size))
    {
        return XFS_STORAGE_ERROR;
    }

    return XFS_OK;
}

static uint32_t xfs_check_integrity(void)
{
    if (!xfs_initialized)
        return XFS_NOT_INITIALIZED;

    if (xfs_size == -1)
        return XFS_CORRUPTED;
    
    XFSHeader_t* xfs_header = (XFSHeader_t*) xfs_allocation_table_buffer;

    if (xfs_header->magic_key != XFS_MAGIC_KEY)
        return XFS_CORRUPTED;
    
    if (xfs_header->version != 1)
        return XFS_CORRUPTED;
    
    XFSAllocationTableV1_t* xfs_table = (XFSAllocationTableV1_t*) ((int)xfs_allocation_table_buffer + sizeof(XFSHeader_t));
    uint32_t crc_val = xfs_table->crc;
    xfs_table->crc = 0xFFFFFFFF;

    if (crcFastWithPad(xfs_allocation_table_buffer, xfs_allocation_table_size, (const unsigned char)0xFF, xfs_page_size-xfs_allocation_table_size) != crc_val)
	{
        return XFS_CORRUPTED;
	}
	
	xfs_table->crc = crc_val;
	
    if (((int)xfs_table->page_size != xfs_page_size) ||
        ((int)xfs_table->block_size != xfs_block_size))
        return XFS_CORRUPTED;

    return XFS_OK;
}
