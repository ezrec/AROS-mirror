
/****************************************************************
   This file was created automatically by `FlexCat 2.4'
   from "DiskSpeed.cd".

   Do NOT edit by hand!
****************************************************************/
#include <proto/locale.h>
#include <aros/symbolsets.h>

static struct Catalog *catalog; 

const char *MSG_CANT_OPEN_LIB_STR;
const char *MSG_CANT_CREATE_APP_STR;
const char *MSG_MAIN_WINTITLE_STR;
const char *MSG_ABOUT_WINTITLE_STR;
const char *MSG_CONFIG_WINTITLE_STR;
const char *MSG_Welcome_STR;
const char *MSG_W_DEVICE_STR;
const char *MSG_W_FILESYS_STR;
const char *WORD_MENU_Project_STR;
const char *WORD_MENU_About_STR;
const char *WORD_MENU_Quit_STR;
const char *MSG_OK_STR;
const char *MSG_GAD_SAVE_STR;
const char *MSG_GAD_USE_STR;
const char *MSG_GAD_CANCEL_STR;
const char *MSG_FILE_CREATE_STR;
const char *MSG_FILE_OPEN_STR;
const char *MSG_FILE_SCAN_STR;
const char *MSG_FILE_DELETE_STR;
const char *MSG_SEEK_READ_STR;
const char *MSG_BYTES_CREATE_STR;
const char *MSG_BYTES_WRITE_STR;
const char *MSG_BYTES_READ_N_STR;
const char *MSG_BYTES_READ_S_STR;
const char *MSG_SEEK_UNITS_STR;
const char *MSG_FILE_UNITS_STR;
const char *MSG_BYTE_UNITS_STR;
const char *MSG_START_TEST_STR;
const char *MSG_SAVE_RESULTS_STR;
const char *MSG_STOP_TEST_STR;
const char *MSG_BUF1TXT_STR;
const char *MSG_BUF2TXT_STR;
const char *MSG_BUF3TXT_STR;
const char *MSG_BUF4TXT_STR;
const char *MSG_RWBUFT_STR;
const char *MSG_RWBUFS_STR;
const char *MSG_CMMNTS_STR;
const char *MSG_DEVICE_STR;
const char *MSG_BLDAMOS_STR;
const char *MSG_BLDAROS_STR;
const char *MSG_PROCESSOR_STR;
const char *MSG_DMAHIGH_STR;
const char *MSG_DMANORM_STR;
const char *MSG_VIDDMA_STR;
const char *MSG_TSTMANP_STR;
const char *MSG_TSTBUFF_STR;
const char *MSG_CPUSPD_STR;
const char *MSG_AVGCPU_STR;
const char *MSG_NOCPU_STR;
const char *MSG_CPUAVAIL_STR;
const char *MSG_SKIPMEM_STR;
const char *MSG_ERR_CALIB1_STR;
const char *MSG_ERR_CALIB2_STR;
const char *MSG_ERR_FAIL_STR;
const char *MSG_ERR_NOLOCK_STR;
const char *MSG_ERR_NODIR_STR;
const char *MSG_ERR_BADDIR_STR;
const char *MSG_ERR_NOCONF_STR;
const char *MSG_ERR_BADSPEC_STR;
const char *MSG_ERR_NODEV_STR;
const char *MSG_ERR_OFILE_STR;
const char *MSG_ERR_RFILE_STR;
const char *MSG_ERR_WFILE_STR;
const char *MSG_ERR_CFILE_STR;
const char *MSG_ERR_DFILE_STR;
const char *MSG_ERR_SEEK_STR;
const char *MSG_ERR_DIRSCAN_STR;
const char *MSG_ERR_RDEV_STR;
const char *MSG_ERR_DATA_STR;
const char *MSG_USERINT_STR;

static const struct
{
    ULONG msg_ID;
    const char  *msg_DEFSTR;
    const char **msg_STR;
} msg_ARRAY[] = 
{
    { 0, "Can't open %s V%ld!\n", &MSG_CANT_OPEN_LIB_STR },
    { 1, "Can't create application object!", &MSG_CANT_CREATE_APP_STR },
    { 2, "%s 4.9 Benchmark Window", &MSG_MAIN_WINTITLE_STR },
    { 3, "About AROS DiskSpeed..", &MSG_ABOUT_WINTITLE_STR },
    { 4, "Benchmark Configuration..", &MSG_CONFIG_WINTITLE_STR },
    { 5, "Welcome to the AROS %s Benchmarking Tool.", &MSG_Welcome_STR },
    { 6, "Device", &MSG_W_DEVICE_STR },
    { 7, "Filesystem", &MSG_W_FILESYS_STR },
    { 8, "Project", &WORD_MENU_Project_STR },
    { 9, "About", &WORD_MENU_About_STR },
    { 10, "Quit", &WORD_MENU_Quit_STR },
    { 11, "Ok", &MSG_OK_STR },
    { 12, "Save", &MSG_GAD_SAVE_STR },
    { 13, "Use", &MSG_GAD_USE_STR },
    { 14, "Cancel", &MSG_GAD_CANCEL_STR },
    { 15, "File Create:   ", &MSG_FILE_CREATE_STR },
    { 16, "File Open:     ", &MSG_FILE_OPEN_STR },
    { 17, "Directory Scan:", &MSG_FILE_SCAN_STR },
    { 18, "File Delete:   ", &MSG_FILE_DELETE_STR },
    { 19, "Seek/Read:     ", &MSG_SEEK_READ_STR },
    { 20, "Create file:   ", &MSG_BYTES_CREATE_STR },
    { 21, "Write to file: ", &MSG_BYTES_WRITE_STR },
    { 22, "Read from file:", &MSG_BYTES_READ_N_STR },
    { 23, "Read from SCSI:", &MSG_BYTES_READ_S_STR },
    { 24, "seeks/sec", &MSG_SEEK_UNITS_STR },
    { 25, "files/sec", &MSG_FILE_UNITS_STR },
    { 26, "bytes/sec", &MSG_BYTE_UNITS_STR },
    { 27, "Begin Test", &MSG_START_TEST_STR },
    { 28, "Save Results", &MSG_SAVE_RESULTS_STR },
    { 29, "End Test", &MSG_STOP_TEST_STR },
    { 30, "Test Buffer 1", &MSG_BUF1TXT_STR },
    { 31, "Test Buffer 2", &MSG_BUF2TXT_STR },
    { 32, "Test Buffer 3", &MSG_BUF3TXT_STR },
    { 33, "Test Buffer 4", &MSG_BUF4TXT_STR },
    { 34, "Read/Write Buffer Types", &MSG_RWBUFT_STR },
    { 35, "Read/Write Buffer Sizes", &MSG_RWBUFS_STR },
    { 36, "Comments:", &MSG_CMMNTS_STR },
    { 37, "Device:", &MSG_DEVICE_STR },
    { 38, "  AmigaOS Build: ", &MSG_BLDAMOS_STR },
    { 39, "  AROS Build: ", &MSG_BLDAROS_STR },
    { 40, "Processor: ", &MSG_PROCESSOR_STR },
    { 41, "  High", &MSG_DMAHIGH_STR },
    { 42, "  Normal", &MSG_DMANORM_STR },
    { 43, " Video DMA", &MSG_VIDDMA_STR },
    { 44, "Testing directory manipulation speed.", &MSG_TSTMANP_STR },
    { 45, "Testing with a %ld byte, MEMF_%s, %s-aligned buffer.", &MSG_TSTBUFF_STR },
    { 46, "CPU Speed Rating: %ld", &MSG_CPUSPD_STR },
    { 47, "Average CPU Available: %ld%%  |  CPU Availability index: %ld", &MSG_AVGCPU_STR },
    { 48, "No CPU Speed Rating -- CPU % not available.", &MSG_NOCPU_STR },
    { 49, "  |  CPU Available: %ld%%", &MSG_CPUAVAIL_STR },
    { 50, "Skipping %ld byte MEMF_%s test due to lack of memory.", &MSG_SKIPMEM_STR },
    { 51, "CPU Calibration shows that CPU availability tests", &MSG_ERR_CALIB1_STR },
    { 52, "would be inaccurate in the current system state.", &MSG_ERR_CALIB2_STR },
    { 53, "Error:  %s test failed.", &MSG_ERR_FAIL_STR },
    { 54, "Error:  Could not get a lock on test device.", &MSG_ERR_NOLOCK_STR },
    { 55, "Error:  Could not create test directory.", &MSG_ERR_NODIR_STR },
    { 56, "Error:  Could not access test directory.", &MSG_ERR_BADDIR_STR },
    { 57, "Could not open configuration requester.", &MSG_ERR_NOCONF_STR },
    { 58, "Error:  Bad device specification.", &MSG_ERR_BADSPEC_STR },
    { 59, "Error:  Could not open device specified", &MSG_ERR_NODEV_STR },
    { 60, "File Open", &MSG_ERR_OFILE_STR },
    { 61, "Read File", &MSG_ERR_RFILE_STR },
    { 62, "Write File", &MSG_ERR_WFILE_STR },
    { 63, "Create File", &MSG_ERR_CFILE_STR },
    { 64, "File Delete", &MSG_ERR_DFILE_STR },
    { 65, "Seek/Read", &MSG_ERR_SEEK_STR },
    { 66, "Directory Scan", &MSG_ERR_DIRSCAN_STR },
    { 67, "Device Reported Error on Read", &MSG_ERR_RDEV_STR },
    { 68, "*** Data Error ***  Buffer did not read correctly.", &MSG_ERR_DATA_STR },
    { 69, "*** Interrupted by user ***", &MSG_USERINT_STR },
};

static int OpenCatalog_DiskSpeed(void)        
{
    ULONG count;

    if (LocaleBase != NULL)
    {
        catalog = OpenCatalog
	(
	    NULL, "DiskSpeed.catalog",
	    OC_Version, 0,
	    TAG_DONE
	);
    }

    for (count = 0; count < sizeof(msg_ARRAY)/sizeof(msg_ARRAY[0]); count++)
    {
	*msg_ARRAY[count].msg_STR = LocaleBase != NULL
	    ? GetCatalogStr
	      (
	          catalog, msg_ARRAY[count].msg_ID, msg_ARRAY[count].msg_DEFSTR
	      )
	    : (CONST_STRPTR)msg_ARRAY[count].msg_DEFSTR;
    }

    return 1;
}

static void CloseCatalog_DiskSpeed(void)
{
    if (LocaleBase != NULL)
        CloseCatalog(catalog);
}

ADD2INIT(OpenCatalog_DiskSpeed, 0);
ADD2EXIT(CloseCatalog_DiskSpeed, 0);
