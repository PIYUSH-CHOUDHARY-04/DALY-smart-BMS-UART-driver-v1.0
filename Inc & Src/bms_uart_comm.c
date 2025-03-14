#include "bms_uart_comm.h"
#include<string.h>

/**
 * @file bms_uart_comm.c
 * @brief Source code file for the driver routines defined in bms_uart_comm.h
 * @author Piyush
 * @date 12/01/25
 * @version 1.0 (uses polling methodology to read the data from the BMS)
 * 
 *
 * @note This Driver is designed for DALY BMS(s), Currently focused on a specific variant DALY BMS R25T-IE02 Li-ion 16S 60V 40A, other variants can also be targeted with the same if they don't differ much in underlying hardware 
 *	 architecture and specifics. 
 *	 Do not forget to include any required HAL driver header files before inclusion of this header file as it contains usage of HAL library. stdint.h is mostly included in the driver header files thus one need not to re-include
 *	 it before this file, if its not, then do include the stdint.h header file.
 *	 This driver is tested on STM32F4 microcontroller, thus uses the same name for all the required entities of the driver like HAL routines, structures, macros etc. which can be changed according to your underlying microcontroller
 *	 hardware architecture.
 */


//==================================================================================== ROUTINE DEFINITIONS ======================================================================================


/**
 * @brief Calculates the checksum of the packet to be sent.
 * @param uint8_t data_id passes the value of the data_id since only this field is different for each packet to be sent.
 * @retval uint8_t returns the checksum value.
 */
uint8_t get_checksum(uint8_t data_id){
	return (((uint16_t)START_FLAG + (uint16_t)UPPER_CMPTR_ADDR + (uint16_t)MAX_DATA_SIZE + (uint16_t data_id))&(0xFF));
}

/**
 * @brief Verifies the checksum of the received packet.
 * @param uart_prot_packet* recvd_packet passes the pointer to the structure holding the received packet.
 * @retval uint8_t returns 0 or 1 , 0 for incorrect checksum and 1 for correct checksum.
 */
uint8_t verify_checksum(uart_prot_packet* recvd_packet){
	uint16_t chksum=0x0000;
	for(uint8_t cnt=0;cnt<sizeof(uart_prot_packet)-1;cnt++){
		chksum+=(((uint8_t*)recvd_packet)[cnt]);
	}
	
	return ((chksum&(0x00FF) == ((uint8_t*)recvd_packet)[sizeof(uart_prot_packet)-1]) ? 1 : 0) ;

}

/**
 * @brief Send the UART command packets in order to read from the connected BMS, the data that will be read from BMS will depend on the above macro settings.
 * @param RT_Battery_status* passes the address of the battery status structure where the response of the sent command will be received.
 * @retval uint8_t returns error codes, 0 on success and non zero on failure.
 */
uint8_t read(RT_Battery_status* stat){

uart_prot_packet packet2send={
	.start_flag=START_FLAG,
	.module_addr=UPPER_CMPTR_ADDR,
	.data_len=MAX_DATA_SIZE
};
memset(packet2send.data,MAX_DATA_SIZE,0x00);

uart_prot_packet packet2recv;


#if ((_FULL_READ_ACCESS | _SOC_IV_ACCESS) == 0x01)
	packet2send.data_id=SOC_TOTAL_IV;
	packet2send.chksum=get_checksum(SOC_TOTAL_IV);
	if(HAL_UART_Transmit(UART_STRUCT_PTR, (uint8_t*)packet2send, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 1;
	}
	if(HAL_UART_Receive(UART_STRUCT_PTR, (uint8_t*)packet2recv, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 2;
	}
	
	if(verify_checksum(&packet2recv)!=0x01){
		return 3;
	}
	memcpy(packet2recv.data,(uint8_t*)&(stat->cum_total_voltage),MAX_DATA_SIZE);
	memset(&packet2recv,sizeof(uart_prot_packet),0x00);
#endif

#if ((_FULL_READ_ACCESS | _MIN_MAX_VOLT_ACCESS) == 0x01)
	packet2send.data_id=MAX_MIN_VOLTAGE;
	packet2send.chksum=get_checksum(MAX_MIN_VOLTAGE);
	if(HAL_UART_Transmit(UART_STRUCT_PTR, (uint8_t*)packet2send, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 4;
	}
	if(HAL_UART_Receive(UART_STRUCT_PTR, (uint8_t*)packet2recv, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 5;
	}
	
	if(verify_checksum(&packet2recv)!=0x01){
		return 6;
	}
	memcpy(packet2recv.data,(uint8_t*)&(stat->max_cell_voltage_value),MAX_DATA_SIZE-2);
	memset(&packet2recv,sizeof(uart_prot_packet),0x00);
#endif

#if ((_FULL_READ_ACCESS | _MIN_MAX_TEMP_ACCESS) == 0x01)
	packet2send.data_id=MAX_MIN_TEMPERATURE;
	packet2send.chksum=get_checksum(MAX_MIN_TEMPERATURE);
	if(HAL_UART_Transmit(UART_STRUCT_PTR, (uint8_t*)packet2send, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 7;
	}
	if(HAL_UART_Receive(UART_STRUCT_PTR, (uint8_t*)packet2recv, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 8;
	}
	
	if(verify_checksum(&packet2recv)!=0x01){
		return 9;
	}
	memcpy(packet2recv.data,(uint8_t*)&(stat->max_temp_val_40),MAX_DATA_SIZE-4);
	memset(&packet2recv,sizeof(uart_prot_packet),0x00);
#endif

#if ((_FULL_READ_ACCESS | _MOS_CHRG_DISCHRG_STATUS_ACCESS) == 0x01)
	packet2send.data_id=CHRG_DISCHRG_MOS_STATUS;
	packet2send.chksum=get_checksum(CHRG_DISCHRG_MOS_STATUS);
	if(HAL_UART_Transmit(UART_STRUCT_PTR, (uint8_t*)packet2send, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 10;
	}
	if(HAL_UART_Receive(UART_STRUCT_PTR, (uint8_t*)packet2recv, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 11;
	}
	
	if(verify_checksum(&packet2recv)!=0x01){
		return 12;
	}
	memcpy(packet2recv.data,(uint8_t*)&(stat->mos_state),MAX_DATA_SIZE);
	memset(&packet2recv,sizeof(uart_prot_packet),0x00);
#endif

#if ((_FULL_READ_ACCESS | _STATUS_INFO1_ACCESS) == 0x01)
	packet2send.data_id=STATUS_INFO_1;
	packet2send.chksum=get_checksum(STATUS_INFO_1);
	if(HAL_UART_Transmit(UART_STRUCT_PTR, (uint8_t*)packet2send, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 10;
	}
	if(HAL_UART_Receive(UART_STRUCT_PTR, (uint8_t*)packet2recv, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 11;
	}
	
	if(verify_checksum(&packet2recv)!=0x01){
		return 12;
	}
	memcpy(packet2recv.data,(uint8_t*)&(stat->battery_string_count),MAX_DATA_SIZE-3);
	memset(&packet2recv,sizeof(uart_prot_packet),0x00);
#endif

#if ((_FULL_READ_ACCESS | _CELL_VOLT_ACCESS) == 0x01)
	packet2send.data_id=CELL_VOLTAGE;
	packet2send.chksum=get_checksum(CELL_VOLTAGE);

	if(HAL_UART_Transmit(UART_STRUCT_PTR, (uint8_t*)packet2send, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 13;
	}

	for(uint8_t i=0;i<(uint8_t)(STRINGS_COUNT/ CELL_VOLTS_PER_FRAME)+1;i++){
		if(HAL_UART_Receive(UART_STRUCT_PTR, (uint8_t*)packet2recv, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
			return 14;
		}

		if(verify_checksum(&packet2recv)!=0x01){
			return 15;
		}

		if(packet2recv.data[0]!=i || packet2recv.data[0]==0xFF){
			return 16;	// incorrect frame sequence.
		}
	
		memcpy(packet2recv.data+1,(stat->cell_voltages)+(3*i),MAX_DATA_SIZE-2);	// will copy 6 bytes of data to the stat structure.
		memset(&packet2recv,sizeof(uart_prot_packet),0x00);
	}
#endif

#if ((_FULL_READ_ACCESS | _CELL_TEMP_ACCESS) == 0x01)
	packet2send.data_id=CELL_TEMPERATURE;
	packet2send.chksum=get_checksum(CELL_TEMPERATURE);	

	if(HAL_UART_Transmit(UART_STRUCT_PTR, (uint8_t*)packet2send, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 17;
	}
	
	for(uint8_t i=0;i<(uint8_t)(TEMP_SENSOR_COUNT/CELL_TEMPS_PER_FRAME)+1;i++){
		if(HAL_UART_Receive(UART_STRUCT_PTR, (uint8_t*)packet2recv, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
			return 18;
		}
		
		if(verify_checksum(&packet2recv)!=0x01){
			return 19;
		}

		if(packet2recv.data[0]!=i || packet2recv.data[0]==0xFF){
			return 20;	// incorrect frame sequence.
		}
		memcpy(packet2recv.data+1,(stat->cell_temperatures)+(7*i),MAX_DATA_SIZE-1);	// will copy 6 bytes of data to the stat structure.
		memset(&packet2recv,sizeof(uart_prot_packet),0x00);
	}
#endif

#if ((_FULL_READ_ACCESS | _CELL_BALANCE_STATE_ACCESS) ==0x01)
	packet2send.data_id=CELL_BALANCE_STATE;
	packet2send.chksum=get_checksum(CELL_BALANCE_STATE);

	if(HAL_UART_Transmit(UART_STRUCT_PTR, (uint8_t*)packet2send, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 21;
	}

	if(HAL_UART_Receive(UART_STRUCT_PTR, (uint8_t*)packet2recv, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 22;
	}
	
	if(verify_checksum(&packet2recv)!=0x01){
		return 23;
	}
	
	memcpy(packet2recv.data,(stat->cell_balance_states),(uint8_t)(STRINGS_COUNT//CELL_BALANCE_STATE_PER_BYTE));	
	memset(&packet2recv,sizeof(uart_prot_packet),0x00);
#endif

#if ((_FULL_READ_ACCESS | _BATTERY_FAILURE_STATUS_ACCESS) == 0x01)
	packet2send.data_id=BATTERY_FAILURE_STATUS;
	packet2send.chksum=get_checksum(BATTERY_FAILURE_STATUS);

	if(HAL_UART_Transmit(UART_STRUCT_PTR, (uint8_t*)packet2send, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 24;
	}

	if(HAL_UART_Receive(UART_STRUCT_PTR, (uint8_t*)packet2recv, sizeof(uart_prot_packet), HAL_MAX_DELAY)!=HAL_OK){
		return 25;
	}
	
	if(verify_checksum(&packet2recv)!=0x01){
		return 26;
	}
	
	memcpy(packet2recv.data,(uint8_t*)&(stat->cell_sum_volt_level),MAX_DATA_SIZE);	
	memset(&packet2recv,sizeof(uart_prot_packet),0x00);
#endif
}

