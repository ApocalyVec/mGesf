/**
 * @file
 *
 * @brief Non-volatile memory layout description
 *
 */
#ifndef NVM_SECTIONS_H_
#define NVM_SECTIONS_H_

typedef struct
{
    int AppFirstPage;            ///< Index of first application page
    int AppLastPage;             ///< Index of last application page
	
    int AppStart;                ///< Address of application start
    int AppEnd;                  ///< Address of application end
    int AppStorageStart;         ///< Address of storage memory start
    int AppStorageEnd;           ///< Address of storage memory end
}MemorySections_t;



#endif /* NVM_SECTIONS_H_ */