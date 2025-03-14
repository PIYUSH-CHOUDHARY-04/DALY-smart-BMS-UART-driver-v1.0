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
 * @brief macros for BMS specification upper limits
 */
#define MAX_BMS_STRING_COUNT 			48	//  upper limit of DALY BMS hardware, provides BMS with maximum of 48 strings
#define MAX_BMS_TEMPERATURE_SENSOR_COUNT	16	//  upper limit of DALY BMS hardware, provides BMS with maximum of 16 temperature sensors.

/**
 * @brief macro for user's BMS strings count and temperature sensors count, must be modified by user as per needs.
 */
#define STRINGS_COUNT		16	// must be modified by user, default value assumed w.r.t. BMS with 16 strings.
#define TEMP_SENSOR_COUNT	4	// must be modified by user, default value assumed w.r.t. BMS with 4 temperature sensors, minimum 4 according to Indian government law for EV(s)


/**
 * @brief macros for UART interface.
 */
#define UART_DEFAULT_BAUDRATE		9600	/**< units : bps (bits per second)	*/
#define UART_STRUCT_PTR (UART_HandleTypeDef*)(uart_handle_pointer)	// need to be set by programmer.

/**
 * @brief macros for selecting only the required memory regions corresponding to the specific data field saving memory.
 */
#define _FULL_READ_ACCESS 	0x00		/**< Need to be selected by the programmer, 0x00 means this option is disabled, 0x01 means full access is enabled */

#if _FULL_READ_ACCESS == 0x00
	/* All folloing macro can be set to either 0x00 or 0x01, if set 0x00, corresponding memory can't be accessed and thus space won't be allocated to the data for that region in RT_Battery_status structure, 	  
	   if set to 0x01, it can be accessed to read data out of it, this is benefecial for both memory and time consumption, thus fastening the read process. */
	#define _SOC_IV_ACCESS				0x00
	#define _MIN_MAX_VOLT_ACCESS			0x00
	#define	_MIN_MAX_TEMP_ACCESS			0x00
	#define _MOS_CHRG_DISCHRG_STATUS_ACCESS		0x00
	#define _STATUS_INFO1_ACCESS			0x00
	#define	_CELL_VOLT_ACCESS			0x00
	#define _CELL_TEMP_ACCESS			0x00
	#define _CELL_BALANCE_STATE_ACCESS		0x00
	#define _BATTERY_FAILURE_STATUS_ACCESS		0x00
#endif

/**
 * @brief Protocol specific macros
 */
#define CELL_VOLTS_PER_FRAME			0x03
#define CELL_TEMPS_PER_FRAME			0x07
#define MONOMER_VOLTAGE_SIZE 			0x02	// each monomer voltage/each string requires 2 bytes of the space for its data being sent.
#define SENT_TEMPERATURE_SIZE			0x01	// each temperature sensor requires 1 byte of the space for its data being sent.
#define CELL_BALANCE_STATE_PER_BYTE		0x08

/**
 * @brief MOS states macros
 */
#define MOS_STATIONARY		0x00
#define MOS_CHARGING		0x01
#define MOS_DISCHARGING		0x02

/**
 * @brief Charger and load status macros
 */
#define CHARGER_STATUS_DISCONN		0x00
#define CHARGER_STATUS_ACCESS		0x01
#define LOAD_STATUS_DISCONN		0x00
#define LOAD_STATUS_ACCESS		0x01

//================================================================================ PROTOCOL PACKET STRUCTURE ====================================================================================================

/**
 * @brief structure for creating the custom protocol packet to be sent to the BMS.
 */
typedef struct {
	uint8_t start_flag;	/**< This is a constant value indicating the start of the custom UART protocol packet/frame , start_flag=START_FLAG 								*/
	uint8_t module_addr;	/**< Address of specific chip/module inside the BMS, supported values are listed above in MACRO section										*/
	uint8_t data_id;	/**< ID of the specific data that host is querying for or want to configure BMS (may be supported in some advanced BMS(s)), supported values are listed above in MACRO section	*/
	uint8_t data_len;	/**< Length of the data in the data field, fixed for this custom protocol implementation over UART for packet parsing, it has a constant value of MAX_DATA_SIZE for all TX/RX 					     packets	*/
	uint8_t data[MAX_DATA_SIZE];		/**< Memory space holding the actual data to be sent or received												*/
	uint8_t chksum;				/**< Checksum of all the previous bytes																*/
} uart_prot_packet;

/**
 * @brief structure for holding data received from the BMS.
 */
typedef struct {

#if ((_FULL_READ_ACCESS | _SOC_IV_ACCESS) == 0x01)
	uint16_t cum_total_voltage;				// cumulative total voltage (0.1 V)
	uint16_t gath_total_voltage;				// gather total voltage (0.1 V)
	uint16_t current;					// current (30000 offset, 0.1 A)
	uint16_t soc;						// SOC (0.1%)
#endif

#if ((_FULL_READ_ACCESS | _MIN_MAX_VOLT_ACCESS) == 0x01)
	uint16_t max_cell_voltage_value;			// maximum cell voltage value (mV)
	uint8_t cell_count_with_min_voltage;			// no. of cell with maximum voltage (mV)
	uint16_t max_cell_voltage_value;			// minimum cell voltage value (mV)
	uint8_t cell_count_with_min_voltage;			// no. of cell with minimum voltage (mV)
#endif

#if ((_FULL_READ_ACCESS | _MIN_MAX_TEMP_ACCESS) == 0x01)
	uint8_t max_temp_val_40;				// maximum temperature value (40 offset, degree celsius)
	uint8_t	max_temp_cell_no;				// maximum temperature cell no.
	uint8_t min_temp_val_40;				// minimum temperature value (40 offset, degree celsius)
	uint8_t	min_temp_cell_no;				// minimum temperature cell no.
#endif

#if ((_FULL_READ_ACCESS | _MOS_CHRG_DISCHRG_STATUS_ACCESS) == 0x01) 
	uint8_t mos_state;					// mos state, stationary or charging or discharging
	uint8_t chrg_mos_state;					// charge MOS state
	uint8_t dischrg_mos_state;				// discharge MOS state
	uint8_t bms_life;					// BMS life (0-255 cycles)
	uint32_t remain_capacity;				// remain capacity (mAH)
#endif

#if ((_FULL_READ_ACCESS | _STATUS_INFO1_ACCESS) == 0x01)
	uint8_t battery_string_count;				// no. of battery strings
	uint8_t temperature_count;				// no. of temperature sensors
	uint8_t	charger_status;					// charger status
	uint8_t load_status;					// load status
	uint8_t DI_DO_state;					// DIx and DOx states
#endif

#if ((_FULL_READ_ACCESS | _CELL_VOLT_ACCESS) == 0x01)
	uint8_t cell_voltages[STRINGS_COUNT * MONOMER_VOLTAGE_SIZE];	// each string data needs 2 bytes, and each frame can carry only 6 byte of data i.e. data of 3 strings atleast, and total 16 frames can be sent at max, thus 96 bytes of voltage data can be sent which corresponds to 48 strings (same as MAX_BMS_STRING_COUNT ).
#endif

#if ((_FULL_READ_ACCESS | _CELL_TEMP_ACCESS) == 0x01)
	uint8_t cell_temperatures[TEMP_SENSOR_COUNT * SENT_TEMPERATURE_SIZE];	// each temperature sensor needs 1 byte, and each frame can carry 7 byte of temperature data atleast, and total 3 frames can be sent at max, thus 21 temperature sensors can be sent, while DALY BMS has limitation of maximum temperature sensors of 16 (same as MAX_BMS_TEMPERATURE_SENSOR_COUNT ).
#endif

#if ((_FULL_READ_ACCESS | _CELL_BALANCE_STATE_ACCESS) ==0x01)
	uint8_t cell_balance_states[(uint8_t)(STRINGS_COUNT/CELL_BALANCE_STATE_PER_BYTE)];	// per byte contains cell states for 8 cells via 8 strings, the number of useful bit will be same as number of strings and since maximum strings can be 48, thus out of 8 bytes of data, only maximum 6 bytes are used for 48 cells via 48 strings, bit value 0 means closed and bit value 1 means open cell.
#endif

#if ((_FULL_READ_ACCESS | _BATTERY_FAILURE_STATUS_ACCESS) == 0x01)
	uint8_t cell_sum_volt_level;
	uint8_t chrg_dischrg_temp_level;
	uint8_t chrg_dischrg_overI_soc_level;
	uint8_t diff_volt_temp_level;
	uint8_t chrg_dischrg_mos_info
	uint8_t all_failures;
	uint8_t all_faults;
	uint8_t fault_code;
#endif
} RT_Battery_status;


//======================================================================================== ROUTINE DECLARATION ===================================================================================================

/**
 * @brief Calculates the checksum of the packet to be sent.
 * @param uint8_t data_id passes the value of the data_id since only this field is different for each packet to be sent.
 * @retval uint8_t returns the checksum value.
 */
uint8_t get_checksum(uint8_t data_id);

/**
 * @brief Verifies the checksum of the received packet.
 * @param uart_prot_packet* recvd_packet passes the pointer to the structure holding the received packet.
 * @retval uint8_t returns 0 or 1 , 0 for incorrect checksum and 1 for correct checksum.
 */
uint8_t verify_checksum(uart_prot_packet* recvd_packet);

/**
 * @brief Send the UART command packets in order to read from the connected BMS, the data that will be read from BMS will depend on the above macro settings.
 * @param RT_Battery_status* passes the address of the battery status structure where the response of the sent command will be received.
 * @retval void
 */
void read(RT_Battery_status* stat);


#ifdef __cplusplus
}
#endif

#endif /**< BMS_UART_COMM_H  */

