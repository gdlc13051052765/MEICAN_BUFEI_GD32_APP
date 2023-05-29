//#include "DEVICE_CFG.h"
#include "nfc.h"
#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "FM176XX_REG.h"
#include "FM176XX.h"
#include "define.h"
#include "hwioconfig.h"
#include "delay.h"

#ifndef SPI_TIMEOUT
#define	SPI_TIMEOUT		200
#endif

SPI_HandleTypeDef fm17660_hspi;

//////////////////////////////////////////////////////////////////////////////delay


/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void FM17660_SPI_Init(void)
{
  /* SPI2 parameter configuration*/
  fm17660_hspi.Instance = FM17660_SPI;
  fm17660_hspi.Init.Mode = SPI_MODE_MASTER;
  fm17660_hspi.Init.Direction = SPI_DIRECTION_2LINES;
  fm17660_hspi.Init.DataSize = SPI_DATASIZE_8BIT;
  fm17660_hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
  fm17660_hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
  fm17660_hspi.Init.NSS = SPI_NSS_SOFT;
  fm17660_hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  fm17660_hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  fm17660_hspi.Init.TIMode = SPI_TIMODE_DISABLE;
  fm17660_hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  fm17660_hspi.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&fm17660_hspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
	__HAL_SPI_ENABLE(&fm17660_hspi);
  /* USER CODE END SPI2_Init 2 */

}

static void delay_ms(u16 nms)	//nms <= 0xffffff*8/SYSCLK; for 72M, Nms<=1864 
{    
	delay_us(1000*nms);
}

/////////////////////////////////////////////////////////////////////
void FM17660_Init(void)
{
	FM17660_SPI_Init();
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/*Configure GPIO pin : RC663 CS Pin */
  GPIO_InitStruct.Pin = FM17660_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FM17660_CS_GPIO_Port, &GPIO_InitStruct);
	
  /*Configure GPIO pin : RC663 CS Pin */
  GPIO_InitStruct.Pin = FM17660_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FM17660_RST_GPIO_Port, &GPIO_InitStruct);
	
	/*Configure GPIO pin : RC663_IRQ_Pin */
  GPIO_InitStruct.Pin = FM17660_IRQT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FM17660_IRQT_GPIO_Port, &GPIO_InitStruct);
 
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0xf, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	FM17660_PDOWN_0;	//	->RESET
	delay_ms(30);

}


//////////////////////////////////////////////
u8 FM17660_SPIWriteByte(u8 Byte)
{

	uint8_t ret = 0; 
	while((FM17660_SPI->SR & SPI_FLAG_TXE) == RESET);	//等待发送缓冲区空

	FM17660_SPI->DR = Byte; 	//发送一个字节  

	while((FM17660_SPI->SR & SPI_FLAG_RXNE) == RESET);	 //等待是否接收到一个字节  
	
	ret = FM17660_SPI->DR; 													//获得该字节

	return ret; //返回收到的字节	
}

/////////////////////////////////////////////////////////////////////

void FM17660_WriteReg(u8   Address, u8   value)
{  
	FM17660_NSS_0;
	FM17660_SPIWriteByte(Address<<1);
	FM17660_SPIWriteByte(value);
	FM17660_NSS_1;
	delay_ns(10);
}

u8 FM17660_ReadReg(u8   Address)
{
	u8  ucResult=0;
	FM17660_NSS_0;
	FM17660_SPIWriteByte((Address<<1)|0x01);
	ucResult = FM17660_SPIWriteByte(0);
	FM17660_NSS_1;
	delay_ns(10);
	return ucResult;
}

//***********************************************
//函数名称：Read_Reg(unsigned char addr,unsigned char *regdata)
//函数功能：读取寄存器值
//入口参数：addr:目标寄存器地址   regdata:读取的值
//出口参数：unsigned char  TRUE：读取成功   FALSE:失败
//***********************************************
unsigned char GetReg(unsigned char addr,unsigned char *regdata)
{	
	*regdata = FM17660_ReadReg(addr);
	return SUCCESS;
}

//***********************************************
//函数名称：Write_Reg(unsigned char addr,unsigned char* regdata)
//函数功能：写寄存器
//入口参数：addr:目标寄存器地址   regdata:要写入的值
//出口参数：unsigned char  TRUE：写成功   FALSE:写失败
//***********************************************
unsigned char SetReg(unsigned char addr,unsigned char regdata)
{	
	FM17660_WriteReg(addr,regdata);
	return SUCCESS;
}

//*******************************************************
//函数名称：ModifyReg(unsigned char addr,unsigned char* mask,FunctionalState state)
//函数功能：写寄存器
//入口参数：addr:目标寄存器地址   mask:要改变的位  
//         state:  ENABLE:标志的位置1   DISABLE：标志的位置0
//出口参数：
//********************************************************
unsigned char ModifyReg(unsigned char addr,unsigned char mask,FunctionalState state)
{
	unsigned char regdata,result;
	
	result = GetReg(addr,&regdata);
	if(result == ERROR)
		return result;
		if(state == ENABLE)
		{
			regdata |= mask;
		}
		else
		{
			regdata &= ~mask;
		}

	result = SetReg(addr,regdata);		
	return result;
}

void SPI_Write_FIFO(unsigned char reglen,unsigned char* regbuf)  //SPI接口连续写FIFO寄存器 
{
	unsigned char  i;
	unsigned char regdata;

	FM17660_NSS_0;	//NSS = 0;
	FM17660_SPIWriteByte(0x0A);
	for(i = 0;i < reglen;i++)
	{
		regdata = *(regbuf+i);	  //RegData_i
		FM17660_SPIWriteByte(regdata);
	}
	FM17660_NSS_1;
	delay_ns(10);

	return;
}

void SPI_Read_FIFO(unsigned char reglen,unsigned char* regbuf)  //SPI接口连续读FIFO
{
	unsigned char  i;
//	unsigned char timer;
	
	FM17660_NSS_0;	//NSS = 0;

	*(regbuf) = FM17660_SPIWriteByte(0x0B);

	for(i=1;i<reglen;i++)
	{
		*(regbuf+i-1) = FM17660_SPIWriteByte(0x0B);

	} 
  *(regbuf+i-1) = FM17660_SPIWriteByte(0);	 /* Read SPI1 received data */

	FM17660_NSS_1;	//NSS = 1;
	return;
}

unsigned char FM176XX_SetCommand(unsigned char command)
{
	unsigned char result;
	result = SetReg(REG_COMMAND,CMD_MASK & command);
	return result;
}

void Clear_FIFO(void)
{
	unsigned char fifolength;
		
	GetReg(REG_FIFOLENGTH,&fifolength);
	if((fifolength) != 0)			//FIFO如果不空，则FLUSH FIFO
	{
	ModifyReg(REG_FIFOCONTROL,BIT_FIFOFLUSH,ENABLE);
	}
	return ;
}

unsigned char FM176XX_HardReset(void)
{	
	unsigned char reg_data;
	FM17660_NSS_1;	//NSS = 1;;//NSS = 1
	FM17660_PDOWN_1;//RST = 1	
	delay_ns(1000);		
	FM17660_PDOWN_0;//RST = 0	
	delay_ns(2000);
	GetReg(REG_COMMAND,&reg_data);
	//debug_print("REG_COMMAND = %02X \r\n",REG_COMMAND);
	if (reg_data != 0x40)
		return ERROR;
	return SUCCESS;
}



unsigned char FM176XX_SoftReset(void)
{	
	unsigned char reg_data;
	FM176XX_SetCommand(CMD_SOFTRESET);
	delay_ns(1000);//FM176XX芯片复位需要1ms
	GetReg(REG_COMMAND,&reg_data);
	if((reg_data & 0x1F) == 0x40)
		return SUCCESS;
	else
		return ERROR;
}

unsigned char FM176XX_Standby(FunctionalState state)
{
	unsigned char result;
	result = ModifyReg(REG_COMMAND,BIT_STANDBY,state);
	if(result == ERROR)
		return ERROR;	
	return result;
}

unsigned char FM176XX_HardPowerDown(FunctionalState state)
{	
	if (state == ENABLE)
	{
		FM17660_PDOWN_0;//RST = 0	
		return 1;
	}
	else
		FM17660_PDOWN_1;//RST = 1		
	return 0 ;
}


//*********************************************************************************************************//
//** 函数名称:	ReadE2
//** 函数功能:	Read EEPROM
//** 输入参数:    *address addressH ,addressL
//** 输出参数:    无
//** 返回值:      无
//*********************************************************************************************************//
unsigned char ReadE2(unsigned char *address,unsigned char *buff)
{
	unsigned char reg_data;
	unsigned char length;	
	FM176XX_SetCommand(CMD_IDLE);					//
	Clear_FIFO();		//Clear FIFO	
	SPI_Write_FIFO(2,address);		
	length = 1;
	SPI_Write_FIFO(1,&length);				//Length=1 	
	FM176XX_SetCommand(CMD_READE2);		//发送READE2命令
	mDelay(10);	
	GetReg(REG_COMMAND,&reg_data);
	if(reg_data != CMD_IDLE)
		return ERROR;
	GetReg(REG_FIFOLENGTH,&length);
	if(length != 1)
		return ERROR;
	SPI_Read_FIFO(1,buff);
	return SUCCESS;
}

unsigned char WriteE2(unsigned char *address,unsigned char buff)
{
	unsigned char reg_data;
	FM176XX_SetCommand(CMD_IDLE);					//
	Clear_FIFO();		//Clear FIFO	
	SPI_Write_FIFO(2,address);		
	SPI_Write_FIFO(1,&buff);				//	
	FM176XX_SetCommand(CMD_WRITEE2);		//发送WRITEE2命令
	mDelay(10);	
	GetReg(REG_COMMAND,&reg_data);
	if(reg_data != CMD_IDLE)
		return ERROR;
	return SUCCESS;
}


/*********************************************************************************************************
** 函数名称:	LoadProtocol
** 函数功能:	载入E2
** 输入参数:    p_rx,p_tx,接收以及发送的协议值，可参考资料
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
unsigned char LoadProtocol(unsigned char p_rx,unsigned char p_tx)
{
	unsigned char reg_data = 0;
	FM176XX_SetCommand(CMD_IDLE);					//
	Clear_FIFO();		//Clear FIFO	
	SPI_Write_FIFO(1,&p_rx);//Rx
	SPI_Write_FIFO(1,&p_tx);//Tx

	FM176XX_SetCommand(CMD_LOADPROTOCOL);
	mDelay(1);	
	GetReg(REG_COMMAND,&reg_data);
	if(reg_data != CMD_IDLE)
		return ERROR;
	return SUCCESS;
}




/*********************************************************************************************************
** 函数名称:	ISO15693_Send_EOF
** 函数功能:	15693只发送一个EOF
** 输入参数:    无
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
void ISO15693_Send_EOF(void)
{	
	unsigned char txdn,fcon;
	GetReg(REG_TXDATANUM,&txdn);
	GetReg(REG_FRAMECON,&fcon);
	
	ModifyReg(REG_TXDATANUM,BIT_DATAEN,DISABLE);		//Start +Stop 格式发送
	ModifyReg(REG_FRAMECON,MASK_STARTSYM,DISABLE);	//无符号发送SOF
	
	FM176XX_SetCommand(CMD_TRANSCEIVE);
	
	SetReg(REG_TXDATANUM,txdn);
	SetReg(REG_FRAMECON,fcon);
}

unsigned char GetAllReg(unsigned char *reg_data)
{
	unsigned char reg_address,result;
	for(reg_address = 0;reg_address < 64;reg_address++)
	{
	result = GetReg(reg_address,(reg_data + reg_address));
	if(result == ERROR)
		return ERROR;
	}
	return result;
}




