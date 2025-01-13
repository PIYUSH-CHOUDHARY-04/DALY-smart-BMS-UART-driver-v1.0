#ifndef BMS_UART_COMM_H
#define BMS_UART_COMM_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file bms_uart_comm.h
 * @brief Header file for the driver routines defined in bms_uart_comm.h
 * 	  This file includes all the necessary declarations and definitions of C programming language entities like routines, structures, macros, other datatypes etc.
 * @author Piyush
 * @date 12/01/25
 * @version 1.0
 * 
 *
 * @note This Driver is designed for DALY BMS(s), Currently focused on a specific variant DALY BMS R25T-IE02 Li-ion 16S 60V 40A, other variants can also be targeted with the same if they don't differ much in underlying hardware 
 *	 architecture and specifics. 
 *	 Do not forget to include any required HAL driver header files before inclusion of this header file as it contains usage of HAL library. stdint.h is mostly included in the driver header files thus one need not to re-include
 *	 it before this file, if its not, then do include the stdint.h header file.
 *	 This driver is tested on STM32F4 microcontroller, thus uses the same name for all the required entities of the driver like HAL routines, structures, macros etc. which can be changed according to your underlying microcontroller
 *	 hardware architecture.
 */ 


// ====================================================================================================== MACROS ==========================================================================================================================

/**
 * @brief macros for hardware address of the different modules available on BMS.
 */
#define BMS_MASTER_ADDR		0x01	/**< Address for the microcontroller chip inside BMS		*/
#define GPRS_ADDR		0x20	/**< Address for the GPRS chip inside BMS			*/
#define UPPER_CMPTR_ADDR	0x40	/**< Address for the host computer/microcontroller inside BMS	*/
#define BLUETOOTH_APP_ADDR	0x80	/**< Address for the bluetooth chip inside BMS

/**
 *@brief macros for custom protocol packet structure for UART since UART is a serial protocol, thus its responsibility of hardware designed to choose a specific protocol packet format.
 */
#define MAX_DATA_SIZE 	0x08	/**< In bytes	*/
#define START_FLAG	0xA5	/**< First byte of custom protocol packet indicating packet transmission initialization	*/

/**
 * @brief macros for "Data ID" field values.
 */
#define BMS_RESET			0x00
#define SOC_TOTAL_IV			0x90
#define MAX_MIN_VOLTAGE			0x91
#define MAX_MIN_TEMPERATURE		0x92
#define CHRG_DISCHRG_MOS_STATUS		0x93
#define STATUS_INFO_1			0x94
#define CELL_VOLTAGE			0x95
#define CELL_TEMPERATURE		0x96
#define CELL_BALANCE_STATE		0x97
#define	BATTERY_FAILURE_STATUS		0x98
#define DISCHRG_FET			0xD9
#define CHRG_FET			0xDA

/**
 * @brief macro for default baudrate of UART interface.
 */
#define UART_DEFAULT_BAUDRATE		9600	/**< units : bps (bits per second)	*/

/**
 * @brief macros for selecting only the required memory regions corresponding to the specific data field saving memory.
 */
#define _FULL_READ_ACCESS 	0x00		/**< Need to be selected by the programmer, 0x00 means this option is disabled, 0x01 means full access is enabled */

#if _FULL_READ_ACCESS == 0x00
	/* All folloing macro can be set to either 0x00 or 0x01, if set 0x00, corresponding memory can't be accessed and thus space won't be allocated to the data for that region in RT_Battery_status structure, if set to 0x01, it can be 	   accessed to read data out of it, this is benefecial for both memory and time consumption, thus fastening the read process. */
	#define _SOC_IV_ACCESS				0x00
	#define _MIN_MAX_VOLT_ACCESS			0x00
	#define	_MIN_MAX_TEMP_ACCESS			0x00
	#define _MOS_CHRG_DISCHRG_STATUS_ACCESS		0x00
	#define _STATUS_INFO1_ACCESS			0x00
	#define	_CELL_VOLT_ACCESS			0x00
	#define _CELL_TEMP_ACCESS			0x00
	#define _CELL_BALANCE_STATE_ACCESS		0X00
	#define _BATTERY_FAILURE_STATUS_ACCESS		0X00
#endif

//================================================================================================ PROTOCOL PACKET STRUCTURE ===============================================================================================================

/**
 * @brief structure for creating the custom protocol packet to be sent to the BMS.
 */
typedef struct {
	uint8_t start_flag;			/**< This is a constant value indicating the start of the custom UART protocol packet/frame , start_flag=START_FLAG 									*/
	uint8_t module_addr;			/**< Address of specific chip/module inside the BMS, supported values are listed above in MACRO section											*/
	uint8_t data_id;			/**< ID of the specific data that host is querying for or want to configure BMS (may be supported in some advanced BMS(s)), supported values are listed above in MACRO section		*/
	uint8_t data_len;			/**< Length of the data in the data field, fixed for this custom protocol implementation over UART for packet parsing, it has a constant value of MAX_DATA_SIZE for all TX/RX packets	*/
	uint8_t data[MAX_DATA_SIZE];		/**< Memory space holding the actual data to be sent or received															*/
	uint8_t chksum;				/**< Checksum of all the previous bytes																			*/
} uart_prot_packet;

/**
 * @brief structure for holding data received from the BMS.
 */
typedef struct {

#if ((_FULL_READ_ACCESS | _SOC_IV_ACCESS) == 0x01)
	uint16_t cum_total_voltage;
	uint16_t gath_total_voltage;
	uint16_t current;
	uint16_t soc;
#endif

#if ((_FULL_READ_ACCESS | _MIN_MAX_VOLT_ACCESS) == 0x01)
	uint16_t max_cell_voltage_value;
	uint8_t cell_count_with_min_voltage;
	uint16_t max_cell_voltage_value;
	uint8_t cell_count_with_min_voltage;
#endif

#if

#endif

} RT_Battery_status;


//================================================================================================ ROUTINE DECLARATION =====================================================================================================================







/**
 *
 *

#ifdef __cplusplus
}
#endif

#endif /**< BMS_UART_COMM_H  */

