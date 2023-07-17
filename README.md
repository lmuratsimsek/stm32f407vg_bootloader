This README file contains information about the STM32F407VG_User_APP_BLINK_At_Sector_2 project. In this project, an LED that blinks at specific intervals is defined. The application is loaded into the SECTOR2 region of the FLASH memory.

Memory Allocation The memory allocation of the project was done with the following steps:

The STM32F407VGTX_FLASH.ld file was opened, and the following memory definitions were found:

MEMORY { CCMRAM (xrw): ORIGIN = 0x10000000, LENGTH = 64K RAM (xrw): ORIGIN = 0x20000000, LENGTH = 128K FLASH (rx): ORIGIN = 0x8000000, LENGTH = 1024K }

The expression ORIGIN = 0x8000000 was changed to ORIGIN = 0x8008000.

Vector Table Address The USER_VECT_TAB_ADDRESS is defined in the system_stm32f4xx.c file of the project. The following change was made:

#define VECT_TAB_OFFSET 0x00000000U #define VECT_TAB_OFFSET 0x8000U

.hex Files The .hex files generated with the STM32CubeIDE application are as follows:

all_blink_app.hex blue_blink_app.hex red_blink_app.hex These files have been copied to the Boot_Loader_HOST folder.

Loading .hex Files to the Bootloader To load the .hex files onto the bootloader, follow these steps:

Go to the Boot_Loader_HOST folder. Run the host_boot_loader.py file. Download the serial library. Put the STM32F407VG device into boot mode: Press the blue button and then press the reset button once while holding the blue button. Observe the green LED turning on. Open the host_boot_loader.py file and replace the file name "all_blink_app.hex" in the code line parsed_data = parse_hex_file("all_blink_app.hex") with the name of your desired .hex file. Adjust the file path as needed since the code was written in Visual Studio.

These pieces of information provide a general overview of the STM32F407VG_User_APP_BLINK_At_Sector_2 project. For detailed information about the project, refer to the relevant files and project documentation.

The VIDEO_BootLoader_TEST file contains a test video of the project in action.
