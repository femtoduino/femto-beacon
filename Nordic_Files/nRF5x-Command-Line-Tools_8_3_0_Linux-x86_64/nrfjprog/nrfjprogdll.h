#ifndef NRFJPROGDLL_FUNC_H
#define NRFJPROGDLL_FUNC_H


#include <stdint.h>
#include "DllCommonDefinitions.h"


#if defined(__cplusplus)
extern "C" {
#endif

/* Expected log function prototype for logging operations. */
typedef void msg_callback(const char * msg_str);


/**
 * @brief	Returns the JLinkARM.dll version.
 *
 * @param	major								Pointer for storing of dll major version.
 * @param	minor								Pointer for storing of dll minor version.
 * @param	revision							Pointer for storing of dll revision.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 * @retval	INVALID_PARAMETER					The major parameter is NULL.
 *												The minor parameter is NULL.
 *												The revision parameter is NULL.
 */
nrfjprogdll_err_t NRFJPROG_dll_version(uint32_t * major, uint32_t * minor, char * revision);


/**
 * @brief	Opens the JLinkARM DLL and sets the log callback. Prepares the dll for work with a specific nRF device family.
 *
 * @details	This function searches for the appropriate family sub dll and load its function
 *			pointers to redirect further commands. It will call the family sub dll open()
 *			function with the received JLinkARM.DLL path. This JLinkARM.dll path should include
 *          the name of the DLL	itself (i.e. "JLinkARM.dll"). Only JLinkARM.dlls whose versions
 *          are greater than a minimum version will be accepted. The minimum version for the 
 *          JLinkARM.dll is defined in MIN_JLINK_MAJOR_VERSION and MIN_JLINK_MINOR_VERSION defines.
 *          The log callback may be NULL. In that case no logging mechanism is provided. The 
 *          msg_callback typedef is defined elsewhere in this file.
 *
 * @param	jlink_path								Path to the JLinkARM DLL.
 * @param	cb										Callback for reporting informational and error messages.
 * @param	family									Defines the device family the next commands are going to be called to.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION						The open_dll() function has already been called.
 * @retval	INVALID_PARAMETER						The jlink_path is NULL.
 *													The provided device family is not supported by this DLL.
 * @retval  JLINKARM_DLL_TOO_OLD                    The version of JLinkARM is lower than the minimum version required.                                                  
 * @retval	JLINKARM_DLL_NOT_FOUND					The jlink_path did not yield a usable DLL.
 * @retval	JLINKARM_DLL_COULD_NOT_BE_OPENED		An error occurred while opening the JLinkARM DLL.
 * 													A required function could not be loaded from the DLL.
 * @retval  NRFJPROG_SUB_DLL_NOT_FOUND				The nrfjprog_sub_dll path did not yield a usable DLL.
 * @retval  NRFJPROG_SUB_DLL_COULD_NOT_BE_OPENED	An error occurred while opening the NRFJPROG SUB DLL.
 *													A required function could not be loaded from the DLL.
 */
nrfjprogdll_err_t NRFJPROG_open_dll(const char * jlink_path, msg_callback * cb, device_family_t family);


/**
 * @brief	Closes and frees the JLinkARM DLL.
 *
 * @details	This function needs to be called before exiting if open_dll has been called.
 */
void NRFJPROG_close_dll(void);


/**
 * @brief	Enumerates the serial numbers of connected USB J-Link emulators.
 *
 * @details	This function asks the JLinkARM DLL how many USB J-Link emulators are connected to
 *			the PC, and writes that value into the num_available parameter. It also copies
 *			up to serial_numbers_len serial numbers into the serial_numbers array parameter.
 *			The function can be called with serial_numbers set to NULL and serial_numbers_len
 *			set to zero to obtain the number of connected emulators.
 *
 * @param	serial_numbers						Array in which to store the enumerated serial numbers.
 * @param	serial_numbers_len					Number of uint32_t values that can be stored in the
 *												serial_numbers array (may be zero).
 * @param	num_available						The number of serial numbers that were enumerated.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	INVALID_PARAMETER					The serial_numbers parameter is NULL but serial_numbers_len is > 0.
 *												The num_available parameter is NULL.
 * @retval	OUT_OF_MEMORY						Could not allocate buffer for reading serial numbers.
 */
nrfjprogdll_err_t NRFJPROG_enum_emu_snr(uint32_t serial_numbers[], uint32_t serial_numbers_len, uint32_t * num_available);


/**
 * @brief	Checks if the emulator has an established connection with Segger emulator/debugger.
 *
 * @param	is_pc_connected_to_emu				Pointer of the location to store the result.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 * @retval	INVALID_PARAMETER					The is_connected_to_emu pointer is NULL.
 */
nrfjprogdll_err_t NRFJPROG_is_connected_to_emu(bool * is_pc_connected_to_emu);


/**
 * @brief	Connects to a given emulator/debugger.
 *
 * @details	This function connects to serial_number emulator and sets the SWD communication speed at
 *          clock_speed_in_khz.
 *
 * @param	serial_number						Serial number of the emulator to connect to.
 * @param	clock_speed_in_khz					Speed for the SWD communication. It must be between JLINKARM_SWD_MIN_SPEED_KHZ
 *                                              and JLINKARM_SWD_MAX_SPEED_KHZ defined in DllCommonDefinitions.h. If the emulator
 *                                              does not support the input clock_speed, the emulators maximum supported speed 
 *                                              will be used.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() has already been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	LOW_VOLTAGE							Low voltage was detected at the target device.
 * @retval	INVALID_PARAMETER					The clock_speed_in_khz parameter is not within limits.
 * @retval	EMULATOR_NOT_CONNECTED				The serial_number emulator is not connected to the PC. 
 */
nrfjprogdll_err_t NRFJPROG_connect_to_emu_with_snr(uint32_t serial_number, uint32_t clock_speed_in_khz);


/**
 * @brief	Connects to an emulator/debugger.
 *
 * @details	This function connects to an available emulator and sets the SWD communication speed at
 *          clock_speed_in_khz. If more than one emulator is available, a pop-up window will 
 *			appear to make a selection.
 *
 * @param	clock_speed_in_khz					Speed for the SWD communication. It must be between JLINKARM_SWD_MIN_SPEED_KHZ
 *                                              and JLINKARM_SWD_MAX_SPEED_KHZ defined in DllCommonDefinitions.h. If the emulator
 *                                              does not support the input clock_speed, the emulators maximum supported speed 
 *                                              will be used.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() has already been called.
 * @retval	NO_EMULATOR_CONNECTED				There is no emulator connected to the PC.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	LOW_VOLTAGE							Low voltage was detected at the target device.										
 * @retval	INVALID_PARAMETER					The clock_speed_in_khz parameter is not within limits.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_emu_without_snr(uint32_t clock_speed_in_khz);


/**
 * @brief	Disconnects from an emulator.
 *
 * @details	This function disconnects from a connected emulator. This also disconnects from a connected device if connected. Will
 *			not fail if we have never connected to an emulator.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. [nRF52 only]
 */
nrfjprogdll_err_t NRFJPROG_disconnect_from_emu(void);


/**
 * @brief	Recovers the device.
 *
 * @details	This operation attempts to recover the device and leave it as it was when it left Nordic factory. It will attempt to 
 *          connect, erase all user available flash, halt and eliminate any protection. After recovery, all RAM will be turned on.
 *          Note that this operation may take up to 30 s for nRF51 devices.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval  NVMC_ERROR							Flash operation failed.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_recover(void);


/**
 * @brief	Checks if the emulator has an established connection with an nRF device.
 *
 * @param	is_emu_connected_to_device			Pointer of the location to store the result.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The is_emu_connected_to_device pointer is NULL.
 */
nrfjprogdll_err_t NRFJPROG_is_connected_to_device(bool * is_emu_connected_to_device);


/**
 * @brief	Connects to the nRF device and halts it.
 *
 * @details	This function connects to the nRF device connected to the connected emulator/debugger and halts it.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 *												The connect_to_device() function has already been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_device(void);


/**
 * @brief	Protects the device against read or debug.
 *
 * @details	Protects the device against read or debug by writing into UICR.RBPCONF and resetting. The function 
 *          will attempt to connect to the device if disconnected. The function will disconnect with a pin reset
 *          after execution. 
 *
 * @param	desired_protection					Desired protection level of readback protection.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The desired_protection is REGION_0 or BOTH. [nRF52 only]
 *      										The desired_protection is NONE.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 * @retval  NVMC_ERROR							Flash operation failed.
 */
nrfjprogdll_err_t NRFJPROG_readback_protect(readback_protection_status_t desired_protection);


/**
 * @brief	Returns the status of the readback protection.
 *
 * @details	Returns the status of the readback protection. For nRF51, if emu is not connected
 *          to the device, connect_to_device() will be called. For nRF51, if the device is not halted, 
 *          halt() will be called.
 *
 * @param	status								Pointer for storing of readback status.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The status pointer is NULL.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open(). [nRF51 only]
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device. [nRF51 only]
 */
nrfjprogdll_err_t NRFJPROG_readback_status(readback_protection_status_t * status);


/**
 * @brief	Returns the region 0 size and source of protection if any.
 *
 * @details	Returns the region 0 size and source of protection if any. For nRF51, if emu is not connected
 *          to the device, connect_to_device() will be called. For nRF51, if the device is not halted, 
 *          halt() will be called. For nRF52, returns 0 size and NO_REGION_0 source since region 0 cannot
 *			be configured in this family.
 *
 * @param	size								Pointer for storing of region 0 protection size.
 * @param	source								Pointer for storing of region 0 protection source.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called. [nRF51 only]
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called. [nRF51 only]
 * @retval	INVALID_PARAMETER					The size pointer is NULL.
 *      	                 					The source pointer is NULL.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open(). [nRF51 only]
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. [nRF51 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device. [nRF51 only]
 */
nrfjprogdll_err_t NRFJPROG_read_region_0_size_and_source(uint32_t * size, region_0_source_t * source);


/**
 * @brief	Executes a reset using the CTRL-AP.
 *
 * @details	Executes a reset by the use of the CTRL-AP RESET register. Not available for NRF51.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	INVALID_DEVICE_FOR_OPERATION        This operation cannot be performed in your device.
 */
nrfjprogdll_err_t NRFJPROG_debug_reset(void);


/**
 * @brief	Executes a system reset request.
 *
 * @details	Halts the core and executes a cortex M standard sys_reset_request. Note that after this operation the core will be halted.
 *          If emu is not connected to the device, connect_to_device() will be called. If nRF CPU is not halted, halt() will be called.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_sys_reset(void);


/**
 * @brief	Executes a pin reset.
 *
 * @details	For nRF52, executes a pin reset by lowering to GND the nReset pin in the SWD connector for 20 ms. For nRF51,
 *          executes a pin reset by toggling SWIO and SWCLK lines as explained in the nRF51 user manual. For nRF51, to avoid
 *          problems with emulated SystemOff, a system_reset() is executed prior to the pin_reset(). This will not be possible
 *          if the device is an XLR or XLR2 and is readback protected by PALL. Note that after this operation the core
 *          will be running.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open(). [nRF51 only]
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device. [nRF51 only]
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The device is one of the early versions and is readback protected by PALL. [nRF51 only]
 */
nrfjprogdll_err_t NRFJPROG_pin_reset(void);


/**
 * @brief	Disables BPROT.
 *
 * @details	Disables BPROT by writing into register BPROT.DISABLEINDEBUG and if nRF51 a sys_reset(). If emu is not 
 *          connected to the device, connect_to_device() will be called. If nRF CPU is not halted, halt() will be called.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_disable_bprot(void);


/**
 * @brief	Erases all flash.
 *
 * @details	Erases all code and UICR flash regions. In the case of nRF51 it will only work if the device is not programmed at the Nordic
 *          factory with a SoftDevice. If emu is not connected to the device, connect_to_device() will be called. If nRF CPU
 *          is not halted, halt() will be called. Note that erase_all() will not disable BPROT if enabled while debug.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval  NVMC_ERROR							Flash operation failed.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_erase_all(void);


/**
 * @brief	Erases a page of code flash.
 *
 * @details	Erases a page of code flash beginning at the addr. In the case of nRF51 it will only work if the page is not protected in region 0,
 *          either at Nordic factory or by the user. If emu is not connected to the device, connect_to_device() will be called.
 *          If nRF CPU is not halted, halt() will be called. Note that erase_page() will not disable BPROT if enabled while debug.
 *
 * @param	addr								Address of the code flash page to erase.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval  NVMC_ERROR							Flash operation failed.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_erase_page(uint32_t addr);


/**
 * @brief	Erases UICR.
 *
 * @details	Erases UICR info page. In the case of nRF51, it will only work if the device is programmed at the Nordic factory with a 
 *          SoftDevice. If emu is not connected to the device, connect_to_device() will be called. If nRF CPU is not halted,
 *          halt() will be called.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval  NVMC_ERROR							Flash operation failed.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_erase_uicr(void);


/**
 * @brief	Writes one uint32_t data at the given address.
 *
 * @details	Writes one uint32_t data to the given addr without verifying that the address is accessible or even exists.
 *          If nvmc_control is true, it will control the NVMC in order to write into flash. Writes need to be 32-bit aligned.
 *          Note that if the target address is in RAM that is unpowered, the operation will fail. If emu is not connected
 *          to the device, connect_to_device() will be called. If nRF CPU is not halted, halt() will be called.
 *
 * @param	addr								Address to write to.
 * @param	data								Value to write.
 * @param	nvmc_control						If the target address needs NVMC control.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The addr parameter is not 32-bit aligned.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 *                                              The address to write is in unpowered RAM.
 * @retval  NVMC_ERROR							Flash operation failed.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_write_u32(uint32_t addr, uint32_t data, bool nvmc_control);


/**
 * @brief	Reads one uint32_t address.
 *
 * @details	Reads one uint32_t data from the given addr without verifying that the address is accessible or even exists.
 *          Reads need to be 32-bit aligned. Note that if the target address is in RAM that is unpowered, the operation will fail.
 *          If emu is not connected to the device, connect_to_device() will be called. If nRF CPU is not halted, halt() will be called.
 *
 * @param	addr								Address to read from.
 * @param	data								Pointer of the location to store the value.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The addr parameter is not 32-bit aligned.
 *												The data parameter is null.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 *                                              The address to read is in unpowered RAM.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_read_u32(uint32_t addr, uint32_t * data);


/**
 * @brief	Writes data from the array starting at the given address.
 *
 * @details	Writes data_len bytes from the data array to the given address without verifying that the address is accessible
 *          or even exists. If nvmc_control is true, it will control the NVMC in order to write into flash. Writes need
 *          to be 32-bit aligned. Note that if the target address is in RAM that is unpowered, the operation will fail.
 *          If emu is not connected to the device, connect_to_device() will be called. If nRF CPU is not halted, halt() will be called.
 *
 * @param	addr								Start address of the region to write to.
 * @param	data								Pointer to an array with the data to write.
 * @param	data_len							Length of the data array.
 * @param	nvmc_control						If the target address needs NVMC control.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The addr parameter is not 32-bit aligned.
 *												The data_len parameter is 0.
 *												The data parameter is NULL.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 *                                              The address to write is in unpowered RAM.
 * @retval  NVMC_ERROR							Flash operation failed.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_write(uint32_t addr, const uint8_t * data, uint32_t data_len, bool nvmc_control);


/**
 * @brief	Reads data_len bytes starting at address addr.
 *
 * @details	Reads data_len bytes starting at the given addr without verifying that the addresses are accessible or even 
 *          exist. Note that if the target address is in RAM that is unpowered, the operation will fail. If emu is not
 *          connected to the device, connect_to_device() will be called. If nRF CPU is not halted, halt() will be called.
 *
 * @param	addr								Address to read from.
 * @param	data								Pointer of the location to store the value.
 * @param	data_len							Number of bytes to read.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The data parameter is null.
 *												The data_len parameter is 0.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 *                                              The address to write is in unpowered RAM.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_read(uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief	Checks if the nRF CPU is halted.
 *
 * @details Checks if device is halted.
 *          If emu is not connected to the device, connect_to_device() will be called.
 *
 * @param	is_device_halted					Pointer of the location to store the result.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	INVALID_PARAMETER					The is_device_halted pointer is NULL.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_is_halted(bool * is_device_halted);


/**
 * @brief	Halts the nRF CPU.
 *
 * @details Halts the nRF CPU.
 *          If emu is not connected to the device, connect_to_device() will be called.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_halt(void);


/**
 * @brief	Starts the nRF CPU with the given pc and sp.
 *
 * @details Starts the nRF CPU with the given pc and sp.
 *          If emu is not connected to the device, connect_to_device() will be called.
 *          If nRF CPU is not halted, halt() will be called.
 *
 * @param	pc									Program Counter to start running from.
 * @param	sp									Stack Pointer to use when running.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_run(uint32_t pc, uint32_t sp);


/**
 * @brief	Starts the nRF CPU.
 *
 * @details Starts the nRF CPU.
 *          If emu is not connected to the device, connect_to_device() will be called.
 *          If nRF CPU is not halted, halt() will be called.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_go(void);


/**
 * @brief	Steps the nRF CPU one instruction.
 *
 * @details Steps the CPU. If emu is not connected to the device, connect_to_device() will be called.
 *          If the CPU is not halted, the function will fail.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 *												The device is not halted.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_step(void);


/**
 * @brief	Reads the RAM power status.
 *
 * @details Reads the RAM power status. Will read the status of the ram sections and return if they are either on or off.
 *          Parameter ram_sections_number is used by the dll to indicate how many of the ram_sections_power_status are 
 *          used to store results. Parameter ram_sections_size is used to report the size of those RAM sections. As a special
 *          use case, if ram_sections_power_status_array_size is 0, the dll will report the size and number of ram power
 *          sections, but will not populate ram_sections_power_status array. If emu is not connected to the device, connect_to_device()
 *          will be called. If nRF CPU is not halted, halt() will be called. For nRF52, the operation will fail if the device
 *          is protected by PALL. For nRF52, if the device is an NRF52832 ENGA, please note that the ram sections power status 
 *          mapping is wrong, see Errata-65	from the errata document for nRF52 devices.
 *
 * @param	ram_sections_power_status				Array to store the results.
 * @param   ram_sections_power_status_array_size	Size of ram_sections_power_status array.
 * @param   ram_sections_number						Pointer of the location to store the number of RAM sections in the device.
 * @param   ram_sections_size						Pointer of the location to store the size of RAM sections in the device.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The ram_sections_power_status pointer is NULL and ram_sections_power_status_array_size is different than 0.
 *												The ram_sections_size pointer is NULL.
 *												The ram_sections_number pointer is NULL.
 *                                              The ram_sections_power_status_array_size is not 0 but is less than the number of ram power sections in the device, 4 for nRF51 XLR3, 2 for all other nRF51 devices and 16 for nRF52 FP1.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_is_ram_powered(ram_section_power_status_t * ram_sections_power_status, uint32_t ram_sections_power_status_array_size, uint32_t * ram_sections_number, uint32_t * ram_sections_size);


/**
 * @brief	Powers up all RAM sections of the device.
 *
 * @details Powers up all the RAM of the device in ON state. Will not affect the RAM retention in SystemOff. If emu is not connected to the device,
 *			connect_to_device() will be called. If nRF CPU is not halted, halt() will be called. The operation will fail is the device is readback
 *          protected by PALL.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_power_ram_all(void);


/**
 * @brief	Powers down a RAM section of the device.
 *
 * @details Powers down a RAM section of the device in ON state. Will not affect the RAM retention in SystemOff. If emu is not connected to the device,
 *			connect_to_device() will be called. If nRF CPU is not halted, halt() will be called. Will fail if the device is protected by PALL. For nRF52,
 *			if the device is an NRF52832 ENGA, please note that the ram sections power status mapping is wrong, see Errata-65 from the errata document for
 *			nRF52 devices. The use of this function is therefore discouraged.
 *
 * @param	section_index			            Section of ram to power down.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The section section_index does not exist in the device.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_unpower_ram_section(uint32_t section_index);


/**
 * @brief	Reads a CPU register.
 *
 * @details	Reads a CPU register. Valid registers are R0-R15, XPSR, MSP and PSP. If emu is not connected to the device, connect_to_device() 
 *          will be called. If nRF CPU is not halted, halt() will be called.
 *
 * @param	register_name						Register name to read. See cpu_registers_t definition for valid values.
 * @param	register_value						Pointer of the location to store the read register.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The register_data parameter is null.
												The register_name parameter is not a valid register.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_read_cpu_register(cpu_registers_t register_name, uint32_t * register_value);


/**
 * @brief	Writes a CPU register.
 *
 * @details	Writes a CPU register. Valid registers are R0-R15, XPSR, MSP and PSP.
 *          If emu is not connected to the device, connect_to_device() will be called.
 *          If nRF CPU is not halted, halt() will be called.
 *
 * @param	register_name						CPU register to write. See cpu_registers_t definition for valid values.
 * @param	register_value						Value to write into the CPU register.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The register_name parameter is not a valid register.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_write_cpu_register(cpu_registers_t register_name, uint32_t register_value);


/**
 * @brief	Reads the device version connected to the device.
 *
 * @details	Reads the device version of the nRF device connected. If emu is not connected to the device, 
 *          connect_to_device() will be called. If nRF CPU is not halted, halt() will be called.
 *
 * @param	version								Pointer of the location to store the device type.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The version parameter is null.
 * @retval	WRONG_FAMILY_FOR_DEVICE				The device connected is not from the same family as the family parameter used in dll_open().
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error.
 * @retval	NOT_AVAILABLE_BECAUSE_PROTECTION	The operation is not available due to readback protection. [nRF52 only]
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_read_device_version(device_version_t * version);


/**
 * @brief	Reads a debugger debug port register.
 *
 * @details	Reads into data pointer a debugger debug port register. 
 *
 * @param	reg_addr							Register address to read, either in debug port or access port.
 * @param	data								Pointer of the location to store the value read.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_read_debug_port_register(uint8_t reg_addr, uint32_t * data);


/**
 * @brief	Writes a debugger debug port register.
 *
 * @details	Writes data parameter into a debugger debug port register.
 *
 * @param	reg_addr							Register address to write, either in debug port or access port.
 * @param	data								Data to write into the register.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_write_debug_port_register(uint8_t reg_addr, uint32_t data);


/**
 * @brief	Reads a debugger access port register.
 *
 * @details	Reads into data pointer a debugger access port register.
 *
 * @param	ap_index							Access port index for read if ap_access.
 * @param	reg_addr							Register address to read, either in debug port or access port.
 * @param	data								Pointer of the location to store the value read.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_read_access_port_register(uint8_t ap_index, uint8_t reg_addr, uint32_t * data);


/**
 * @brief	Writes a debugger access port register.
 *
 * @details	Writes data parameter into a debugger access port register.
 *
 * @param	ap_index							Access port index for write if ap_access.
 * @param	reg_addr							Register address to write, either in debug port or access port.
 * @param	data								Data to write into the register.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval	INVALID_PARAMETER					The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_write_access_port_register(uint8_t ap_index, uint8_t reg_addr, uint32_t data);


/**
 * @brief	Indicates to the dll the location of the RTT control block in the device memory.
 *
 * @details	Indicates to the dll the location of the RTT control block in the device memory, to accelerate the discovery of the RTT control block.
 *          Cannot be called after rtt_start() has been called.
 *
 * @param	address								RTT control block address in device memory.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The rtt_start() function has been called.
 */
nrfjprogdll_err_t NRFJPROG_rtt_set_control_block_address(uint32_t address);


/**
 * @brief	Starts RTT.
 *
 * @details	Starts RTT, locating RTT control block and preparing the DLL for RTT operations. If rtt_set_control_block_address has been called, 
 *          the address provided will be used to locate the control block. If emu is not connected to the device, connect_to_device() will be called.
 *          After rtt_start() has been executed, please ensure that rtt_stop() will be executed.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 *												The rtt_start() function has already been called.
 * @retval	CANNOT_CONNECT						It is impossible to connect to any nRF device.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_rtt_start(void);


/**
 * @brief	Checks if RTT control block has been found.
 *
 * @details	Checks if RTT control block has been found. Can be used to make sure RTT is ready to be used before calling RTT read/write.
 *
 * @param	is_control_block_found				Pointer of the location to store the result.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 *												There is no connection between the emulator and the device.
 *												The rtt_start() function has not been called.
 * @retval	INVALID_PARAMETER					The is_found parameter is null.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_rtt_is_control_block_found(bool * is_control_block_found);


/**
 * @brief   Stops RTT.
 *
 * @details Stops RTT and invalidates the RTT Control Buffer address. Might delay the time it takes to find the RTT Control Block if rtt_start() is
 *          called again before closing the DLL.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The open_dll() function has not been called.
 *                                              The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_rtt_stop(void);


/**
 * @brief	Reads from an RTT channel.
 *
 * @details	Reads up to data_len characters from the up_channel_index channel.
 *
 * @param	up_channel_index					RTT channel index to read from.
 * @param	data								Pointer of the location to store the value.
 * @param	data_len							Number of bytes to read.
 * @param	data_read							Pointer to the location to store the actual number of read characters.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 *												There is no connection between the emulator and the device.
 *												The rtt_start() function has not been called.
 * @retval	INVALID_PARAMETER					The data parameter is null.
 *                                              The data_read parameter is null.
 *												There is no channel in the device with the given up_channel_index index.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_rtt_read(uint32_t up_channel_index, char * data, uint32_t data_len, uint32_t * data_read);
	

/**
 * @brief	Writes to an RTT channel.
 *
 * @details	Writes the given data_len length data string into the down_channel_index channel.
 *
 * @param	down_channel_index					RTT channel index to write to.
 * @param	data								Pointer to a string with the string to write.
 * @param	data_len							Length of the string.
 * @param	data_written						Pointer to the location to store the actual number of written characters.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 *												There is no connection between the emulator and the device.
 *												The rtt_start() function has not been called.
 * @retval	INVALID_PARAMETER					The data parameter is null.
 *                                              The data_read parameter is null.
 *												There is no channel in the device with the given down_channel_index index.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_rtt_write(uint32_t down_channel_index, const char * data, uint32_t data_len, uint32_t * data_written);


/**
 * @brief	Gets the number of RTT channels.
 *
 * @details	Gets the number of down and up channels in the device.
 *
 * @param	down_channel_number					Pointer to store the number of down channels.
 * @param	up_channel_number					Pointer to store the number of up channels.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 *												There is no connection between the emulator and the device.
 *												The rtt_start() function has not been called.
 * @retval	INVALID_PARAMETER					The down_channel_number parameter is null.
 *                                              The up_channel_number parameter is null.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_count(uint32_t * down_channel_number, uint32_t * up_channel_number);


/**
 * @brief	Reads the info from one RTT channel.
 *
 * @details	Reads the info from one RTT channel of index channel_index of direction dir. Reads the rtt channel size into channel_size and name into channel_name pointers.
 *
 * @param	channel_index						Channel index to read the info from.
 * @param	dir									Channel direction of channel to read the info from.
 * @param	channel_name						Pointer to store the channel name, must be able to store at least 32 characters.
 * @param	channel_name_len					Size of channel_name storage size.
 * @param	channel_size						Pointer to store the channel size.
 *
 * @retval	SUCCESS
 * @retval	INVALID_OPERATION					The open_dll() function has not been called.
 *												The jlinkarm_connect_with_snr() or jlinkarm_connect_without_snr() function has not been called.
 *												There is no connection between the emulator and the device.
 *												The rtt_start() function has not been called.
 * @retval	INVALID_PARAMETER					The channel_name parameter is null.
 *                                              The channel_size parameter is null.
 *                                              The channel_size_len parameter is less than 32.
 *												The dir parameter is neither UP_DIRECTION or DOWN_DIRECTION defined in rtt_direction_t enum in DllCommonDefinitions.h
 *												The channel with index channel_index does not exist in the device.
 * @retval	JLINKARM_DLL_ERROR					The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_info(uint32_t channel_index, rtt_direction_t dir, char * channel_name, uint32_t channel_name_len, uint32_t * channel_size);


#if defined(__cplusplus)
}
#endif

#endif  /* NRFJPROGDLL_FUNC_H */
