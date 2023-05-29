/* Copyright (c)  .
 *
 * The information contained herein is property of Giesecke & Devrient Group ASA.
 * Terms and conditions of usage are described in detail in Giesecke & Devrient Group
 * STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
 
/** @file
 *
 * @defgroup IOPINConfig.h
 * @{
 * @ingroup  global PIN defines
 * @brief    PIN will be defined in this file for each module.
 *
 * The different applicaiton will use different PIN config, developer can 
 * update this file.
 * Each PIN have group number and pin number, 
 * for example #HW_IO_PIN_GROUP_A is group number, #HW_IO_PIN_SPI1_M_SCK_PIN is pin number.
 */
 
 
#ifndef _HW_IO_CONFIG_H__
#define _HW_IO_CONFIG_H__

#include <stdint.h>
#include "SwitchConfig.h"

/**
 * @brief This value can be provided instead of a pin number for signals, 
 * 				specify that the given signal is not used and
 *        therefore does not need to be connected to a pin.
 *				Please make group and number are 0xFF at the same time.
 */
#define HW_IO_PIN_GROUP_NOT_USED  									0xFF
#define HW_IO_PIN_NUMBER_NOT_USED  									0xFF


//check NOT use PIN
#define HW_IO_PIN_IS_NOT_USED(group, number)   			(((group == HW_IO_PIN_GROUP_NOT_USED)&&(number == HW_IO_PIN_NUMBER_NOT_USED))?true:false)

//IO group number
typedef enum {
				HW_IO_PIN_GROUP_A		=		1,
				HW_IO_PIN_GROUP_B,
				HW_IO_PIN_GROUP_C,
				HW_IO_PIN_GROUP_D,
				HW_IO_PIN_GROUP_E,
				HW_IO_PIN_GROUP_F,
				HW_IO_PIN_GROUP_G,
				HW_IO_PIN_GROUP_H,
}hw_io_pin_group;

/************************For APP_HK_MOTOR_EP START*********************/

#ifdef APP_HK_MOTOR_EP
	/*********************w5500 gpio********************************/
	#define W5500_SCS_PORT							GPIOA		
	#define W5500_SCS_PIN								GPIO_PIN_4
	#define W5500_RST_GROUP							GPIOA
	#define W5500_RST_PIN								GPIO_PIN_2
	#define W5500_INT_GROUP							GPIOA
	#define W5500_INT_PIN								GPIO_PIN_1
	#define W5500_MISO_PORT							GPIOA		
	#define W5500_MISO_PIN							GPIO_PIN_6
	#define W5500_MOSI_PORT							GPIOA
	#define W5500_MOSI_PIN							GPIO_PIN_7
	#define W5500_SCK_PORT							GPIOA
	#define W5500_SCK_PIN								GPIO_PIN_5


	#define LED_Pin 										GPIO_PIN_12
	#define LED_GPIO_Port 							GPIOB

#endif
//美餐布菲台
#ifdef APP_MEICAN_BUFEI_EP
	//布菲台灯板
	#define LED1_Pin 									GPIO_PIN_9
	#define LED1_GPIO_Port 						GPIOB
	
	#define LED2_Pin 									GPIO_PIN_5
	#define LED2_GPIO_Port 						GPIOB
	
	#define LED3_Pin 									GPIO_PIN_6
	#define LED3_GPIO_Port 						GPIOB
	
	//运行指示灯
	#define RUN_LED_Pin 							GPIO_PIN_8
	#define RUN_LED_GPIO_Port 				GPIOA
	
	//主控电源控制
	#define T113_POWER_Pin 						GPIO_PIN_4
	#define T113_POWER_GPIO_Port 			GPIOB
	
	//称重传感器电源控制
	#define WEIGH_POWER_Pin 					GPIO_PIN_8
	#define WEIGH_POWER_GPIO_Port 		GPIOB
	
	//485串口
	#define W485_DIR_Pin 							GPIO_PIN_1
	#define W485_DIR_GPIO_Port 				GPIOA
	
	#define W485_TX_Pin 							GPIO_PIN_2
	#define W485_TX_GPIO_Port 				GPIOA
	
	#define W485_RX_Pin 							GPIO_PIN_3
	#define W485_RX_GPIO_Port 				GPIOA
	
	//主控通讯串口
	#define T113_TX_Pin 							GPIO_PIN_9
	#define T113_TX_GPIO_Port 				GPIOA
	
	#define T113_RX_Pin 							GPIO_PIN_10
	#define T113_RX_GPIO_Port 				GPIOA

	//RC663 GPIO
	#define RC663_CS_Pin 							GPIO_PIN_4
	#define RC663_CS_GPIO_Port 				GPIOA
	
	#define RC663_SCK_Pin 						GPIO_PIN_5
	#define RC663_SCK_GPIO_Port 			GPIOA
	
	#define RC663_MISO_Pin 						GPIO_PIN_6
	#define RC663_MISO_GPIO_Port 			GPIOA
	
	#define RC663_MOSI_Pin 						GPIO_PIN_7
	#define RC663_MOSI_GPIO_Port 			GPIOA
	
	#define RC663_IRQT_Pin 						GPIO_PIN_0
	#define RC663_IRQT_GPIO_Port 			GPIOB
	
	#define RC663_RST_Pin 						GPIO_PIN_1
	#define RC663_RST_GPIO_Port 			GPIOB
	
	//FM17660 GPIO
	#define FM17660_CS_Pin 						GPIO_PIN_4
	#define FM17660_CS_GPIO_Port 			GPIOA
	
	#define FM17660_SCK_Pin 					GPIO_PIN_5
	#define FM17660_SCK_GPIO_Port 		GPIOA
	
	#define FM17660_MISO_Pin 					GPIO_PIN_6
	#define FM17660_MISO_GPIO_Port 		GPIOA
	
	#define FM17660_MOSI_Pin 					GPIO_PIN_7
	#define FM17660_MOSI_GPIO_Port 		GPIOA
	
	#define FM17660_IRQT_Pin 					GPIO_PIN_0
	#define FM17660_IRQT_GPIO_Port 		GPIOB
	
	#define FM17660_RST_Pin 					GPIO_PIN_1
	#define FM17660_RST_GPIO_Port 		GPIOB
	
	
	//SPI FLASH
	#define EX_FLASH_CS_Pin 					GPIO_PIN_12
	#define EX_FLASH_CS_GPIO_Port 		GPIOB
	
	#define EX_FLASH_SCK_Pin 					GPIO_PIN_13
	#define EX_FLASH_SCK_GPIO_Port 		GPIOB
	
	#define EX_FLASH_MISO_Pin 				GPIO_PIN_14
	#define EX_FLASH_MISO_GPIO_Port 	GPIOB
	
	#define EX_FLASH_MOSI_Pin 				GPIO_PIN_15
	#define EX_FLASH_MOSI_GPIO_Port 	GPIOB
	
	#define W25QXX_SPI			SPI2
	#define	W25QXX_CS_1 	 (EX_FLASH_CS_GPIO_Port->BSRR = EX_FLASH_CS_Pin)  		//片选信号使能
	#define	W25QXX_CS_0 	 (EX_FLASH_CS_GPIO_Port->BSRR = (uint32_t)(EX_FLASH_CS_Pin << 16u))		//片选信号失能
	
	//主控T113串口
	#define  T113_UART			USART1
	//485传感器串口
	#define  W485_UART			USART2
	//打印串口
	#define  DEBUG_UART			USART3
	//RC663 SPI
	#define  RC663_SPI			SPI1
	//FM17660 SPI
	#define  FM17660_SPI		SPI1

	
#endif

#endif		//_HW_IO_CONFIG_H__
