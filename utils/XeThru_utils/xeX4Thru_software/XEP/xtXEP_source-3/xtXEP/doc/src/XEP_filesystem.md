XEP file system {#xep_filesystem}
============================

XEP File System (XFS) allows for storing data in the embedded flash of the MCU.

It only supports the most basic functionality to be very light-weight. Multiple 
files of different sizes can be created and be stored in flash. The contents can
be changed at a later time, but the size of the file can not be changed. However,
a new file with the new size can be created and the old one deleted.

Each file is identified with a type that is defined in @ref xtfile_types_t, 
and an identifier. The type/identifier combination must be unique. To avoid 
collisions with XEP defined types, all user defined functions are defined from
0x80000000 and up. Functions to search for files are available, both for all files 
and by specified type. 

The size of the file system is defined by overriding the function
@ref xep_app_storage_section. By default, no memory is reserved. Flash page size determines
the minimum allocation unit, and is automatically detected through the HAL layer.

@ref xtfile.h file contains the interface that should be used to access the file 
system.

Example of using the file system:

    #include "xtfile.h"

    static void task_application(void *pvParameters)
    {
        // Your init code

        // Create file
        uint32_t status;
        void* handle;

		char text_data[] = "This is some data.";
		status = xtfile_create_and_open(&handle, XTFILE_TYPE_USER_DEFINED | XTFILE_TYPE_LOG, 0, text_data, sizeof(text_data));
        if (status != XTFILE_OK)
        {
            // Error handling...
        }
		status = xtfile_close(handle); 

        // Search for files of given type
        uint32_t identifiers[10];
        uint32_t size = 0;
        status = xtfile_search(XTFILE_TYPE_USER_DEFINED | XTFILE_TYPE_LOG, identifiers,
                                10, &size);
        
        if (size > 0)
        {
            status = xtfile_open(&handle, XTFILE_TYPE_USER_DEFINED | XTFILE_TYPE_LOG, identifiers[0]);
            void* data = 0;
            uint32_t data_size = 0;
            status = xtfile_get_buffer(handle, &data, &data_size);

            // Do something with the data

            status = xtfile_close(handle); 
            
            status = xtfile_delete(XTFILE_TYPE_USER_DEFINED | XTFILE_TYPE_LOG, identifiers[0]);
        }

        // The task loop
        for(;;)
        {
            // Your task code
        }
    }

Note that each write to flash will impose a wear on the embedded flash. This 
means that the firmware should not write too often, as this will eventually 
wear out the embedded flash. There is no wear-leveling implemented.