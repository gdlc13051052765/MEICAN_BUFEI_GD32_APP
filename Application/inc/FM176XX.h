
#ifndef _FM175XX_H
#define _FM175XX_H
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "nfc_interface.h"
#include "hwioconfig.h"
#include "stm32f10x_type.h"

#define	 FM17660_NSS_1	 do{ HAL_GPIO_WritePin(FM17660_CS_GPIO_Port, FM17660_CS_Pin, GPIO_PIN_SET);}while(0)
#define	 FM17660_NSS_0	 do{ HAL_GPIO_WritePin(FM17660_CS_GPIO_Port, FM17660_CS_Pin, GPIO_PIN_RESET);}while(0)

#define	 FM17660_PDOWN_1	 do{ HAL_GPIO_WritePin(FM17660_RST_GPIO_Port, FM17660_RST_Pin, GPIO_PIN_SET);}while(0)
#define	 FM17660_PDOWN_0	 do{ HAL_GPIO_WritePin(FM17660_RST_GPIO_Port, FM17660_RST_Pin, GPIO_PIN_RESET);}while(0)

extern unsigned char FM176XX_HardReset(void);
extern unsigned char FM176XX_SoftReset(void);
extern void FM175XX_HPD(unsigned char mode);
extern unsigned char GetReg(unsigned char addr,unsigned char* regdata);
extern unsigned char SetReg(unsigned char addr,unsigned char regdata);
extern unsigned char ModifyReg(unsigned char addr,unsigned char mask,FunctionalState state);
extern void SPI_Write_FIFO(unsigned char reglen,unsigned char* regbuf); //SPI接口连续写FIFO 
extern void SPI_Read_FIFO(unsigned char reglen,unsigned char* regbuf);  //SPI接口连续读FIFO

extern void Clear_FIFO(void);
extern unsigned char LoadProtocol(unsigned char p_rx,unsigned char p_tx);
extern unsigned char WriteE2(unsigned char *address,unsigned char buff);
extern unsigned char ReadE2(unsigned char *address,unsigned char *buff);
extern unsigned char GetAllReg(unsigned char *reg_data);
extern unsigned char FM176XX_Standby(FunctionalState state);
extern unsigned char FM176XX_SetCommand(unsigned char command);
extern unsigned char FM176XX_HardPowerDown(FunctionalState state);

void FM17660_Init(void);
#endif





