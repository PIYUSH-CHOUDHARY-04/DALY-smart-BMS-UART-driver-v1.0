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




#ifdef __cplusplus
}
#endif

#endif /**< BMS_UART_COMM_H  */

