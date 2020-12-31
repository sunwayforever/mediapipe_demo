/****************************************************************************
*
*   FileName: inu_storage_layout.h
*
*   Author:  Konstantin Sinyuk
*
*   Date: 15/03/2016
*
*   Description: Defines layout of Inuitive flash storage.
*				  Includes definition for Bootrom override region, production info,
calibration data and boot data
*
****************************************************************************/
#ifndef INU_STORAGE_LAYOUT_H
#define INU_STORAGE_LAYOUT_H

#include <time.h>
#include <inu_storage.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
	This file defines the layout of data on flash. 
	Currently each section is flash block aligned for data write safety.
	Each section will start from the header (InuSectionHeaderT), sectionMetaData(if exists) and then section binary data.
	The only exception is BOOTFIX section that will have BootROM header, than binary data, then section header and then section MetaData  (at separate block)

	NOTE:
		The BOOTFIX section version is the version number for the whole flash layout
*/

// Legacy defines
#define BOOTROM_STORAGE_NU3000_MAGIC_NUMBER     (0xDA)
#define LEGACY_PRODUCTION_HEADER_MAGIC_NUMBER	(0x87654321)

#define SECTION_START_BLOCK_DYNAMIC				(0xfffffffa)
#define SECTION_START_BLOCK_LAST				(0xfffffffb)
#define SECTION_START_BLOCK_BEFORE_LAST			(0xfffffffc)
#define SECTION_SIZE_DYNAMIC					(0xfffffffd)

#define BOOTFIX_DATA_SIZE						(127*1024)
// INU_STORAGE_SECTION_BOOTFIX
#define SECTION_BOOTFIX_MAGIC_NUMBER			(0x87654321)
#define SECTION_BOOTFIX_VERSION_NUMBER			(0x100)
#define SECTION_BOOTFIX_START_BLOCK				(0)
#define SECTION_BOOTFIX_SIZE_IN_BLOCKS			(2)
#define SECTION_BOOTFIX_AUTO_SIZE_INCREASE		(0)

// INU_STORAGE_SECTION_BOOTFIX_META
#define SECTION_BOOTFIX_META_MAGIC_NUMBER		(0x87654322)
#define SECTION_BOOTFIX_META_VERSION_NUMBER		(0x100)
#define SECTION_BOOTFIX_META_START_BLOCK		(3)
#define SECTION_BOOTFIX_META_SIZE_IN_BLOCKS		(1)
#define SECTION_BOOTFIX_META_AUTO_SIZE_INCREASE	(0)

//INU_STORAGE_SECTION_PRODUCTION_DATA
#define SECTION_PRODUCTION_MAGIC_NUMBER			(0x87654323)
#define SECTION_PRODUCTION_VERSION_NUMBER		(0x100)
#define SECTION_PRODUCTION_START_BLOCK			(2)
#define SECTION_PRODUCTION_SIZE_IN_BLOCKS		(1)
#define SECTION_PRODUCTION_AUTO_SIZE_INCREASE	(0)

//INU_STORAGE_SECTION_CALIBRATION_DATA
#define SECTION_CALIBRATION_MAGIC_NUMBER     (0x87654324)
#define SECTION_CALIBRATION_VERSION_NUMBER   (0x100)
#define SECTION_CALIBRATION_START_BLOCK      (SECTION_START_BLOCK_LAST)
#define SECTION_CALIBRATION_SIZE_IN_BLOCKS   (SECTION_SIZE_DYNAMIC)
#define SECTION_CALIBRATION_AUTO_SIZE_INCREASE  (30)

//INU_STORAGE_SECTION_BOOTSPL_DATA
#define SECTION_BOOTSPL_MAGIC_NUMBER         (0x87654325)
#define SECTION_BOOTSPL_VERSION_NUMBER       (0x100)
#define SECTION_BOOTSPL_START_BLOCK          (4)
#define SECTION_BOOTSPL_SIZE_IN_BLOCKS       (SECTION_SIZE_DYNAMIC)
#define SECTION_BOOTSPL_AUTO_SIZE_INCREASE   (30)

//INU_STORAGE_SECTION_DTB
#define SECTION_DTB_MAGIC_NUMBER             (0x87654326)
#define SECTION_DTB_VERSION_NUMBER           (0x100)
#define SECTION_DTB_START_BLOCK              (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_DTB_SIZE_IN_BLOCKS           (1)
#define SECTION_DTB_AUTO_SIZE_INCREASE       (0)

//INU_STORAGE_SECTION_KERNEL
#define SECTION_KERNEL_MAGIC_NUMBER          (0x87654327)
#define SECTION_KERNEL_VERSION_NUMBER        (0x100)
#define SECTION_KERNEL_START_BLOCK           (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_KERNEL_SIZE_IN_BLOCKS        (SECTION_SIZE_DYNAMIC)
#define SECTION_KERNEL_AUTO_SIZE_INCREASE    (10)

//INU_STORAGE_SECTION_ROOTFS
#define SECTION_ROOTFS_MAGIC_NUMBER          (0x87654328)
#define SECTION_ROOTFS_VERSION_NUMBER        (0x100)
#define SECTION_ROOTFS_START_BLOCK           (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_ROOTFS_SIZE_IN_BLOCKS        (SECTION_SIZE_DYNAMIC)
#define SECTION_ROOTFS_AUTO_SIZE_INCREASE    (10)

//INU_STORAGE_SECTION_CEVA
#define SECTION_CEVA_MAGIC_NUMBER            (0x87654329)
#define SECTION_CEVA_VERSION_NUMBER          (0x100)
#define SECTION_CEVA_START_BLOCK             (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_CEVA_SIZE_IN_BLOCKS          (SECTION_SIZE_DYNAMIC)
#define SECTION_CEVA_AUTO_SIZE_INCREASE      (10)

//INU_STORAGE_SECTION_CNN_LOADER
#define SECTION_CNN_LD_MAGIC_NUMBER          (0x8765432A)
#define SECTION_CNN_LD_VERSION_NUMBER        (0x100)
#define SECTION_CNN_LD_START_BLOCK           (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_CNN_LD_SIZE_IN_BLOCKS        (SECTION_SIZE_DYNAMIC)
#define SECTION_CNN_LD_AUTO_SIZE_INCREASE    (10)

//INU_STORAGE_SECTION_CNN
#define SECTION_CNN_MAGIC_NUMBER             (0x8765432B)
#define SECTION_CNN_VERSION_NUMBER           (0x100)
#define SECTION_CNN_START_BLOCK              (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_CNN_SIZE_IN_BLOCKS           (SECTION_SIZE_DYNAMIC)
#define SECTION_CNN_AUTO_SIZE_INCREASE       (10)

   //INU_STORAGE_SECTION_APP
#define SECTION_APP_MAGIC_NUMBER             (0x8765432C)
#define SECTION_APP_VERSION_NUMBER           (0x100)
#define SECTION_APP_START_BLOCK              (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_APP_SIZE_IN_BLOCKS           (SECTION_SIZE_DYNAMIC)
#define SECTION_APP_AUTO_SIZE_INCREASE       (10)

#define OLD_CALIB_BLOCK_OFFSET               (3)




typedef struct
{
	unsigned int magicNumber;
	// version number of Header
	unsigned int versionNumber;	
	// real size of data in section
	unsigned int sectionDataSize;
	// full section size
	unsigned int sectionSize;
	// section data format
	InuSectionFormatE sectionFormat;
	// Current date and time
	unsigned long long timestamp;	
	// Version string for the current section (if supported)
	char versionString[VERSION_STRING_SIZE];	
} InuSectionHeaderT;

typedef struct
{		
	// when true, the bootspl will proceed to boot kernel and nu3000.zip from flash
	// should be overridable by push button from MANO board
	unsigned int isBootfromFlash;
	// allows to change chip default strap value
	unsigned int strapOverrideMask;
	unsigned int strapOverrideValue;	
	unsigned int isQuadFlash;
	unsigned int unsedNorFlags[8];
	// NAND Flags
	unsigned int loadSplFromNor;
	unsigned int bootFromFactoryDefault;	
	unsigned int nandNewImages;
	unsigned int nandBootSuccess;		
	unsigned int bootCounter;		
} InuBootfixHeaderT;

typedef struct
{
	// Unique ID of each sensor
	char serialNumber[PRODUCTION_STRING_SIZE];
	// General operational data
	char modelNumber[PRODUCTION_STRING_SIZE];
	char partNumber[PRODUCTION_STRING_SIZE];
	char sensorRevision[PRODUCTION_STRING_SIZE];	
	// boot folder id - obtained from BootModelCross.csv production table
	unsigned int  bootId;	
} InuProductionHeaderT;

#define PRODUCTION_STRING_SIZE_LEGACY		(16)
typedef struct
{
	// Arbitrary ID that is used in order to differentiate sensors with old data
	unsigned int ID;
	// Version number of Header
	unsigned int VersionNumber;
	// Unique ID of each sensor
	char SerialNumber[PRODUCTION_STRING_SIZE_LEGACY];
	// Size of calibration data
	unsigned int CalibrationDataSize;
	// !!! Must keep all the above members in this order for backward compatibility !!!
	// Size of this header
	unsigned int HeaderSize;
	// General operational data
	char ModelNumber[PRODUCTION_STRING_SIZE_LEGACY];
	char PartNumber[PRODUCTION_STRING_SIZE_LEGACY];
	char SensorRevision[PRODUCTION_STRING_SIZE_LEGACY];
	// Burned revision - a counter that is increased when data is burned on flash.
	unsigned int SWRevision;
}InuStorageLegacyLayoutT;


#define NAND_FLASH_RAW_STORAGE_LAYOUT


#ifdef NAND_FLASH_RAW_STORAGE_LAYOUT

// Current nand raw partition. Total 380 logical(400 phys blocks)//
//---------------------------//
// Section               |    Blocks      //
//---------------------------//
// HEADER              |    1  (1/4M) //
//---------------------------//
// SPL                    |    2  (1/2M) // (1)
//---------------------------//
// KERNEL              |    24 (6M)    // (16)
//---------------------------//
// DTB                   |    1  (1/4M) //
//---------------------------//
// CEVA                 |    4  (1M)     //
//---------------------------//
// EV62_BOOT        |    1  (1/4M) //
//---------------------------//
// EV62_APP          |    16  (4M)    //
//---------------------------//
// INITRAMFS         |    120 (30M) //
//---------------------------//
// APP                   |    211 (52.75M)//
//---------------------------//


#define NAND_TOTAL_BLOCKS_NUMBER			(4096)		//2 NOTE: Flash NAND device specific
#define NAND_BLOCK_SIZE						(256*1024)	//2 NOTE: Flash NAND device specific

#define NAND_EMPTY_UINT32 					(0xFFFFFFFF)
#define NAND_EMPTY_UINT8 					(0xFF)



//Raw partition mapping
#define NAND_PARTITION_RAW_LOGICAL_SIZE_BLOCKS		(380)		// 95 MiB
#define NAND_PARTITION_RAW_PHYSICAL_SIZE_BLOCKS		(400)		// 100 MiB

#define NAND_PARTITION_FACTORY_START_BLOCK 			(0)
#define NAND_PARTITION_FW_UPDATE_START_BLOCK		(NAND_PARTITION_RAW_PHYSICAL_SIZE_BLOCKS)

// NAND flash all partitions
////////////////////////
#define NAND_PARTITION_RAW_FACTORY_OFFSET_BLOCKS	(0)
#define NAND_PARTITION_RAW_FACTORY_TOTAL_BLOCKS		(NAND_PARTITION_RAW_PHYSICAL_SIZE_BLOCKS)		// factory	400 blocks
#define NAND_PARTITION_RAW_UPGRADE_OFFSET_BLOCKS	(NAND_PARTITION_RAW_FACTORY_OFFSET_BLOCKS + NAND_PARTITION_RAW_FACTORY_TOTAL_BLOCKS)
#define NAND_PARTITION_RAW_UPGRADE_TOTAL_BLOCKS		(NAND_PARTITION_RAW_PHYSICAL_SIZE_BLOCKS)		// fw update	400 blocks
#define NAND_PARTITION_FS_UPGRADE_OFFSET_BLOCKS		(NAND_PARTITION_RAW_UPGRADE_OFFSET_BLOCKS + NAND_PARTITION_RAW_UPGRADE_TOTAL_BLOCKS)
#define NAND_PARTITION_FS_UPGRADE_TOTAL_BLOCKS		(800)																					// 800 blocks
#define NAND_PARTITION_APPLICATION_OFFSET_BLOCKS	(NAND_PARTITION_FS_UPGRADE_OFFSET_BLOCKS + NAND_PARTITION_FS_UPGRADE_TOTAL_BLOCKS)
#define NAND_PARTITION_APPLICATION_TOTAL_BLOCKS		(NAND_TOTAL_BLOCKS_NUMBER - NAND_PARTITION_APPLICATION_OFFSET_BLOCKS)					// The rest of the flash
/////////////////////////

#define NAND_SECTION_HEADER_START_BLOCK		(0)
#define NAND_SECTION_HEADER_SIZE_BLOCKS		(1)

#define NAND_SECTION_HEADER_HEADER_OFFSET	(0x00)
#define NAND_SECTION_HEADER_BB_MAP_OFFSET	(0x400)
#define NAND_SECTION_HEADER_L2P_MAP_OFFSET	(0x800)


#define NAND_SECTION_BOOTSPL_START_BLOCK	(NAND_SECTION_HEADER_START_BLOCK    + NAND_SECTION_HEADER_SIZE_BLOCKS)
#define NAND_SECTION_BOOTSPL_SIZE_BLOCKS	(2)

#define NAND_SECTION_KERNEL_START_BLOCK		(NAND_SECTION_BOOTSPL_START_BLOCK   + NAND_SECTION_BOOTSPL_SIZE_BLOCKS)
#define NAND_SECTION_KERNEL_SIZE_BLOCKS		(24)

#define NAND_SECTION_DTB_START_BLOCK		(NAND_SECTION_KERNEL_START_BLOCK    + NAND_SECTION_KERNEL_SIZE_BLOCKS)
#define NAND_SECTION_DTB_SIZE_BLOCKS		(1)

#define NAND_SECTION_CEVA_START_BLOCK		(NAND_SECTION_DTB_START_BLOCK       + NAND_SECTION_DTB_SIZE_BLOCKS)
#define NAND_SECTION_CEVA_SIZE_BLOCKS		(4)

#define NAND_SECTION_EV62_BOOT_START_BLOCK	(NAND_SECTION_CEVA_START_BLOCK      + NAND_SECTION_CEVA_SIZE_BLOCKS)
#define NAND_SECTION_EV62_BOOT_SIZE_BLOCKS	(1)

#define NAND_SECTION_EV62_APP_START_BLOCK	(NAND_SECTION_EV62_BOOT_START_BLOCK + NAND_SECTION_EV62_BOOT_SIZE_BLOCKS)
#define NAND_SECTION_EV62_APP_SIZE_BLOCKS	(16)

#define NAND_SECTION_INITRAMFS_START_BLOCK	(NAND_SECTION_EV62_APP_START_BLOCK  + NAND_SECTION_EV62_APP_SIZE_BLOCKS)
#define NAND_SECTION_INITRAMFS_SIZE_BLOCKS	(120)

#define NAND_SECTION_APP_START_BLOCK		(NAND_SECTION_INITRAMFS_START_BLOCK + NAND_SECTION_INITRAMFS_SIZE_BLOCKS)
#define NAND_SECTION_APP_SIZE_BLOCKS		(NAND_PARTITION_RAW_LOGICAL_SIZE_BLOCKS - NAND_SECTION_APP_START_BLOCK)			// The rest of the section is for application

#define NAND_SECTION_MAX_SECTIONS			(0x10)


#define NAND_HEADER_MAGIC_NUMBER			(0xBABEFACE)
#define NAND_HEADER_HEADER_VERSION			(0xFFFFFF01)	//2 Note: Update once header is changed
#define NAND_HEADER_BB_MAP_MAGIC_NUMBER		(0xFFBADBAD)


//Raw partition section types
typedef enum
{
	NAND_SECTION_TYPE_HEADER	= 0,		//2 Note: Header always the first section
	NAND_SECTION_TYPE_BOOTSPL	,	
	NAND_SECTION_TYPE_KERNEL	,
	NAND_SECTION_TYPE_DTB		,
	NAND_SECTION_TYPE_CEVA		,
	NAND_SECTION_TYPE_EV62_BOOT	,
	NAND_SECTION_TYPE_EV62_APP	,
	NAND_SECTION_TYPE_INITRAMFS	,
	NAND_SECTION_TYPE_APP		,			//2 Note: App always the last section
	NAND_SECTION_TYPE_LAST		= NAND_SECTION_TYPE_APP,
	NAND_SECTION_TYPE_NONE		= NAND_EMPTY_UINT32,
}InuStorageNandSectionTypE;

typedef struct
{
	unsigned int 				major;
	unsigned int 				minor;	
	unsigned int 				build;	
}InuStorageNandImgVerT;

typedef struct
{
	InuStorageNandSectionTypE 	type;
	unsigned int 				startAddress;
	unsigned int 				sizeBytes;
	unsigned int 				imgStartAddress;
	unsigned int 				imgSizeBytes;
	InuStorageNandImgVerT		imgVersion;

}InuStorageNandLayoutSecT;



typedef struct
{
	unsigned int				magicNumber;
	unsigned int				headerVersion;
	unsigned int				reserved0;
	unsigned int				reserved1;
	unsigned int				reserved2;
	unsigned int				reserved3;
	InuStorageNandLayoutSecT	section[NAND_SECTION_MAX_SECTIONS];
}InuStorageNandHeaderT;

typedef struct
{
	unsigned int				magicNumber;
	char						block[NAND_PARTITION_RAW_PHYSICAL_SIZE_BLOCKS];
}InuStorageNandBbMapT;

typedef struct
{
	unsigned int				block[NAND_PARTITION_RAW_LOGICAL_SIZE_BLOCKS];
}InuStorageNandL2PMapT;

#endif	//NAND_FLASH_RAW_STORAGE_LAYOUT


#if (defined _WIN32) || (defined _WIN64)
#pragma pack(push,1)
#endif
typedef struct {
	unsigned char   magic;
	unsigned char   storage_size_in_kb;
	unsigned short  storage_page_size;
	unsigned char   storage_mode;
	unsigned char   storage_type;
	unsigned int  bootcode_load_address;
	unsigned int  bootcode_size;
#if (defined _WIN32) ||(defined _WIN64)
}BootromStorageHeaderT;
#pragma pack(pop)
#else
} __attribute__((__packed__)) BootromStorageHeaderT;
#endif

#if (defined _WIN32) || (defined _WIN64)
#pragma pack(push,1)
#endif
typedef struct {
	BootromStorageHeaderT bootromHdr;
	unsigned char bootfixData[BOOTFIX_DATA_SIZE - sizeof(BootromStorageHeaderT)];
	InuSectionHeaderT bootfixSectionHeader;
#if (defined _WIN32) ||(defined _WIN64)
}InuStorageLayoutT;
#pragma pack(pop)
#else
} __attribute__((__packed__)) InuStorageLayoutT;
#endif


// defines a union for legacy and new layout for the first block of flash storage
typedef union
{
	InuStorageLayoutT storagelayout;
	InuStorageLegacyLayoutT storageLegacyLayout;
} InuStorageU;

#ifdef __cplusplus
}
#endif

#endif // INU_STORAGE_LAYOUT_H 

