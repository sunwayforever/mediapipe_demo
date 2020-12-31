#ifndef __HOST_UTILS_H__
#define __HOST_UTILS_H__

#if (defined _WIN32) || (defined _WIN64)
  #include <windows.h>
  #include <conio.h>
#elif defined(__linux__)
  #include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define	NU4000_A0_CHIP_VERSION  (0x40000000)
#define	NU4000_B0_CHIP_VERSION  (0x40000001)
#define	NU4000_C0_CHIP_VERSION  (0x40000002)
#define	NU3000_B0_CHIP_VERSION  (0x30000001)


#define BOOT_PATH_LENGTH_MAX			(256)

typedef enum
{
   COMPONENTS_SUCCESS             =  0,
   COMPONENTS_ERR_KERNEL_FILE     = -1,
   COMPONENTS_ERR_DTB_FILE        = -2,
   COMPONENTS_ERR_APP_FILE        = -3,
   COMPONENTS_ERR_SPL_FILE        = -4,
   COMPONENTS_ERR_INITRAMFS_FILE  = -5,
	COMPONENTS_ERR_CNN_FILE        = -6,
	COMPONENTS_ERR_CNN_LOADER_FILE = -7,
} ComponentsErrorE;

typedef struct 
{
	char linux_file_name[BOOT_PATH_LENGTH_MAX];
	char dtb_file_name[BOOT_PATH_LENGTH_MAX];
	char spl_file_name[BOOT_PATH_LENGTH_MAX];
	char app_file_name[BOOT_PATH_LENGTH_MAX];
	char initramfs_file_name[BOOT_PATH_LENGTH_MAX];
	char cnn_loader_file_name[BOOT_PATH_LENGTH_MAX];
	char cnn_file_name[BOOT_PATH_LENGTH_MAX];
	char ceva_file_name[BOOT_PATH_LENGTH_MAX];
} ComponentsParamsT;

	int host_millisleep(unsigned ms);
	int is_file_exists(char *name);
	const char *get_path_separator();
	ComponentsErrorE get_components(ComponentsParamsT *componentsParamsP, int id, const char *boot_path, int version);
#ifdef __cplusplus
}
#endif

#endif /* __HOST_UTILS_H__ */
