/****************************************************************************
 *
 *   FileName: inu_storage.h
 *
 *   Author:  Konstantin Sinyuk
 *
 *   Date: 15/03/2016
 *
 *   Description: Inuitive storage access API. 
 *				  This layer works via USB link in bare metal mode.
 *   
 ****************************************************************************/
#ifndef INU_STORAGE_H
#define INU_STORAGE_H

#ifdef INUSTORAGE_EXPORTS
    #ifdef __GNUC__
              #ifdef __linux__
                     #define INUSTORAGE_API __attribute__((visibility("default")))
              #else
                     #define INUSTORAGE_API __attribute__ ((visibility ("default")))
              #endif
    #else
        #define INUSTORAGE_API __declspec(dllexport)
    #endif
    #define INUSTORAGE_API_TEMPLATE
#else
    #ifdef __GNUC__
              #ifdef __linux__
                     #define INUSTORAGE_API
              #else
                     #define INUSTORAGE_API __attribute__ ((visibility ("default")))
              #endif
    #else
        #define INUSTORAGE_API __declspec(dllimport)
    #endif
    #define INUSTORAGE_API_TEMPLATE extern
#endif

#define	NU3000_B0_CHIP_VERSION  (0x30000001)
#define	NU3000_A0_CHIP_VERSION  (0)

#define PRODUCTION_STRING_SIZE			(16)
#define VERSION_STRING_SIZE				(32)

#define BOOT_PATH_LENGTH_MAX			(256)


typedef enum
{
   INU_STORAGE_ERR_SUCCESS         = 0,
   INU_STORAGE_ERR_INIT_FAILED     = 1,
   INU_STORAGE_ERR_IO_FAILED       = 2,
   INU_STORAGE_ERR_NO_MEMORY       = 3,
   INU_STORAGE_ERR_NO_SECOND_FLASH = 4,
   INU_STORAGE_ERR_NOT_SUPPORTED   = 5,
   INU_STORAGE_ERR_DRYRUN          = 6,
   INU_STORAGE_ERR_BURN_ATTINY     = 7,
} InuStorageErrorE;


typedef enum
{
	INU_STORAGE_SECTION_FORMAT_RAW	= 0,
	INU_STORAGE_SECTION_FORMAT_ZIP	= 1,	      
} InuSectionFormatE;

typedef enum
{
	INU_STORAGE_BOOT_FROM_USB	= 0,
	INU_STORAGE_BOOT_FROM_FLASH	= 1	
} InuStrapOverrideE;

typedef enum
 {
   INU_STORAGE_SECTION_BOOTFIX          = 0,
   INU_STORAGE_SECTION_BOOTFIX_METADATA = 1,
   INU_STORAGE_SECTION_PRODUCTION_DATA  = 2,
   INU_STORAGE_SECTION_CALIBRATION_DATA = 3,
   INU_STORAGE_SECTION_BOOTSPL          = 4,
   INU_STORAGE_SECTION_DTB              = 5,
   INU_STORAGE_SECTION_KERNEL           = 6,
   INU_STORAGE_SECTION_ROOTFS           = 7,
   INU_STORAGE_SECTION_CEVA             = 8,
   INU_STORAGE_SECTION_CNN_LD           = 9,
   INU_STORAGE_SECTION_CNN              = 10,
   INU_STORAGE_SECTION_APP              = 11
} InuStorageSectionTypeE;

typedef enum
{
	INU_STORAGE_SPI_FLASH = 0,
	INU_STORAGE_EEPROM = 1,
	INU_STORAGE_EFUSE = 2,
	INU_STORAGE_NAND = 3,
	INU_STORAGE_SPI = 4,
} InuStorageTypeE;

typedef struct 
{   
	// Section data format
	InuSectionFormatE sectionDataFormat;	
	unsigned int sectionDataSize;
	unsigned char *sectionDataBuffer;
	// Current date and time
	unsigned long long timestamp;	
	// Version string for the current section (if supported)
	char versionString[VERSION_STRING_SIZE];
} InuSectionDataT;

typedef struct
{
    unsigned int spi_freq;
	unsigned int spi_num;
	unsigned int spi_cs;
 }SpiParamsT;

typedef struct
{
	unsigned int dataSize;
	unsigned char *dataBuffer;
	unsigned int dataOffset;
} InuFlashDataT;

typedef struct
{
	unsigned int dataSize;
	unsigned char *dataBuffer;
	unsigned int dataOffset;
} InuNandDataT;

typedef struct 
{   
	// Unique ID of each sensor
    char serialNumber[PRODUCTION_STRING_SIZE];
    // General operational data
    char modelNumber[PRODUCTION_STRING_SIZE];
    char partNumber[PRODUCTION_STRING_SIZE];
    char sensorRevision[PRODUCTION_STRING_SIZE];
	// boot folder id - obtained from Boot_Model_Cross.csv production table
	unsigned int bootId;
	unsigned int chipHardwareVersion;
} InuProductionDataT;

typedef struct 
{   
	char linux_file_name[BOOT_PATH_LENGTH_MAX];
	char dtb_file_name[BOOT_PATH_LENGTH_MAX];
	char spl_file_name[BOOT_PATH_LENGTH_MAX];
	char app_file_name[BOOT_PATH_LENGTH_MAX];
	char initramfs_file_name[BOOT_PATH_LENGTH_MAX];
} InuComponentDataT;


#ifdef __cplusplus
extern "C" {
#endif

	/****************************************************************************
	*
	*  Function Name: inu_storage_open
	*
	*  Description: Wait for USB insertion event and opens USB link
	*
	*  Inputs: SpiParamsT spiParams
	*          second_flash -    use secondary flash google X
	*          outputDir         to copy partitions from flash
	*          flashSizeInMega
	*          currChipId  -     ex. 0x40000001 for b0
	*  Returns: InuStorageErrorE error code
	*
	****************************************************************************/
INUSTORAGE_API 
InuStorageErrorE inu_storage_open(SpiParamsT spiParams,
	                              unsigned int secondFlash,
	                              char *outputDir,
	                              unsigned int flashSizeInMega,
	                              unsigned int currChipid);
/* Close USB interface */ 
INUSTORAGE_API
	InuStorageErrorE inu_storage_close();

/* 
	Write section data, supports all supported section types
	Example:
		InuProductionDataT prodData;
		InuSectionDataT sectionData;
		memset(sectionData, 0, sizeof(InuSectionDataT));
		sectionData.sectionDataBuffer = &prodData;
		sectionData.sectionDataSize = sizeof(InuProductionDataT)
		sectionData.timestamp =  gettime(); // POSIX epoch format
		sectionDataFormat.INU_STORAGE_SECTION_FORMAT_RAW;
		inu_storage_write_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, &sectionData);
		
*/ 
INUSTORAGE_API 
	InuStorageErrorE inu_storage_write_section_data(InuStorageSectionTypeE sectionType, InuSectionDataT *sectionData);

/* 
	Read section data and meta data, supports all supported section types
	Example:
		InuSectionDataT sectionData;
		InuProductionDataT *prodData;
		inu_storage_read_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, &sectionData);
		prodData = (InuProductionDataT *)sectionData.sectionDataBuffer;				
*/

INUSTORAGE_API 
	InuStorageErrorE inu_storage_read_section_data(InuStorageSectionTypeE sectionType, InuSectionDataT *sectionData);


/* 
	Erase section from the flash. 
	SoftErase just invalidates the section (cleans magic number)
*/

INUSTORAGE_API 
	InuStorageErrorE inu_storage_erase_section(InuStorageSectionTypeE sectionType, int softErase);

/* 
	Resize section on the flash. 
	Typical use case: resize calibration data section
	Warning: This is dangerous procedure as it will relocate other sections.	
*/

INUSTORAGE_API 
	InuStorageErrorE inu_storage_resize_section(InuStorageSectionTypeE sectionType, unsigned int size);

INUSTORAGE_API
	InuStorageErrorE inu_storage_read_bootdetect(InuProductionDataT *productionData, unsigned int *transfered_size);

// program EFUSE (Warning: can be performed only once)
INUSTORAGE_API
InuStorageErrorE inu_storage_program_efuse(unsigned int *efuseBuff);

// write directly to flash
INUSTORAGE_API
InuStorageErrorE inu_storage_write_data(InuFlashDataT *inuFlashData);

// read directly to flash
INUSTORAGE_API
InuStorageErrorE inu_storage_read_data(InuFlashDataT *inuFlashData);

INUSTORAGE_API
InuStorageErrorE inu_storage_burn_avr_file(char *avrFile);

INUSTORAGE_API
InuStorageErrorE inu_storage_rw_spi(InuStorageTypeE storage_type, unsigned char *txBuffer, unsigned char *rxBuffer, unsigned int txRxSize, unsigned int numOfMsgs);

// write directly to NAND flash
INUSTORAGE_API
InuStorageErrorE inu_storage_write_nand_data(InuNandDataT *inuNandData);

// read directly from NAND flash
INUSTORAGE_API
InuStorageErrorE inu_storage_read_nand_data(InuNandDataT *inuNandData);

// erase NAND flash
INUSTORAGE_API
InuStorageErrorE inu_storage_erase_nand_data(InuNandDataT *inuNandData);

// update partition/section including header update
INUSTORAGE_API
InuStorageErrorE inu_storage_update_nand_image(unsigned int partition, unsigned int section, InuNandDataT *inuNandData);
/*
	General notes: 

	There are 2 structs the define data layout of two section: BOOTFIX with InuBootfixMetaDataT and PRODUCTION with InuProductionDataT 

	This API assumes that section data are written in the following order
		- bootfix,
		- production,
		- calibration,
		- kernel,
		- dtb,
		- app,
		- extended,
		- bootfix_metadata
	Such order allows optimal flash allocation policy and most suitable for typical usage scenarios.
	Although it is possible to write sections out of order. 
	In such case the heavy use of resize function is expected.

*/

#ifdef _DEBUG

INUSTORAGE_API
InuStorageErrorE inu_storage_write_raw(InuStorageTypeE storage_type, unsigned int offset, unsigned char *buffer, unsigned int size);
INUSTORAGE_API
InuStorageErrorE inu_storage_read_raw(InuStorageTypeE storage_type, unsigned int offset, unsigned char *buffer, unsigned int size, unsigned int *transfered_size);
INUSTORAGE_API
InuStorageErrorE inu_storage_erase(InuStorageTypeE storage_type, unsigned int offset, unsigned int size);
INUSTORAGE_API
InuStorageErrorE inu_storage_set_version(unsigned int flashChipVersion);
#endif

INUSTORAGE_API
InuStorageErrorE inu_storage_get_components(InuComponentDataT *componentsP, const char *boot_path);


#ifdef __cplusplus
}
#endif

#endif // INU_STORAGE_H 
