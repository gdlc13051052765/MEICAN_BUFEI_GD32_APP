#include "RC663.h"
#include "nfc.h"
#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "delay.h"

//定义美餐标识
#define MEICAN_CODE  215
SPI_HandleTypeDef rc663_hspi;
extern u8 Status_INT;


/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void RC663_SPI_Init(void)
{
  /* SPI2 parameter configuration*/
  rc663_hspi.Instance = RC663_SPI;
  rc663_hspi.Init.Mode = SPI_MODE_MASTER;
  rc663_hspi.Init.Direction = SPI_DIRECTION_2LINES;
  rc663_hspi.Init.DataSize = SPI_DATASIZE_8BIT;
  rc663_hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
  rc663_hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
  rc663_hspi.Init.NSS = SPI_NSS_SOFT;
  rc663_hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  rc663_hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  rc663_hspi.Init.TIMode = SPI_TIMODE_DISABLE;
  rc663_hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  rc663_hspi.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&rc663_hspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
	__HAL_SPI_ENABLE(&rc663_hspi);
  /* USER CODE END SPI2_Init 2 */

}

static void delay_ms(u16 nms)	//nms <= 0xffffff*8/SYSCLK; for 72M, Nms<=1864 
{    
	delay_us(1000*nms);
}

/////////////////////////////////////////////////////////////////////
void RC663_Init(void)
{
	uint8_t temp;
	RC663_SPI_Init();
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/*Configure GPIO pin : RC663 CS Pin */
  GPIO_InitStruct.Pin = RC663_NSS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RC663_NSS_GROUP, &GPIO_InitStruct);
	
  /*Configure GPIO pin : RC663 CS Pin */
  GPIO_InitStruct.Pin = RC663_RST_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RC663_RST_GROUP, &GPIO_InitStruct);
	
	/*Configure GPIO pin : RC663_IRQ_Pin */
  GPIO_InitStruct.Pin = RC663_IRQ_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RC663_IRQ_GROUP, &GPIO_InitStruct);
 
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0xf, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	PDOWN_0;	//	->RESET
	delay_ms(30);
	temp = RC663_ReadReg(rRegVersion);
	#ifdef UART_PRINT
	debug_print("RC663_ReadReg version: %02x \r\n",temp);
	#endif
	
//	pTag.tag_state = NULL_STA;
//	RC663_PcdConfigISOType('V');
//	delay_ms(5);	
}

//////////////////////////////////////////////
u8 RC663_SPIWriteByte(u8 Byte)
{
	uint8_t ret = 0; 
	while((RC663_SPI->SR & SPI_FLAG_TXE) == RESET);	//等待发送缓冲区空

	RC663_SPI->DR = Byte; 	//发送一个字节  

	while((RC663_SPI->SR & SPI_FLAG_RXNE) == RESET);	 //等待是否接收到一个字节  
	
	ret = RC663_SPI->DR; 													//获得该字节

	return ret; //返回收到的字节	
}

/////////////////////////////////////////////////////////////////////

void RC663_WriteReg(u8   Address, u8   value)
{  
	RC663_NSS_0;
	RC663_SPIWriteByte(Address<<1);
	RC663_SPIWriteByte(value);
	RC663_NSS_1;
	delay_ns(10);
}

u8 RC663_ReadReg(u8   Address)
{
	u8  ucResult=0;
	RC663_NSS_0;
	RC663_SPIWriteByte((Address<<1)|0x01);
	ucResult = RC663_SPIWriteByte(0);
	RC663_NSS_1;
	delay_ns(10);
	return ucResult;
}

void RC663_SetBitMask(u8   reg,u8   mask)  
{
    u8  tmp = RC663_ReadReg(reg);
    RC663_WriteReg(reg,tmp | mask);  
}

void RC663_ClearBitMask(u8   reg,u8   mask)  
{
    u8  tmp = RC663_ReadReg(reg);
    RC663_WriteReg(reg, tmp & ~mask);  
}

void RC663_SetRawRC(u8   reg,u8 mask,u8 set)
{
	u8 temp = RC663_ReadReg(reg);
	temp = (temp&mask)|set;
	RC663_WriteReg(reg,temp);
}
///////////////////////////////////////////////////////////
void RC663_FlushFifo()
{
	RC663_SetBitMask(rRegFIFOControl,0x10);
}

void RC663_FieldOn()
{
	RC663_SetBitMask(rRegDrvMod,0x08);
}

void RC663_FieldOff()
{
	RC663_ClearBitMask(rRegDrvMod,0x08);
}

void RC663_FieldReset()
{
	RC663_FieldOff();
	delay_ms(20);
	RC663_FieldOn();
	delay_ms(20);
}

extern u8 Status_INT;
u8 mode;
s8 RC663_Command_Int(struct TranSciveBuffer *pi)
{
	u16 i;
	u8 j,n;
	RC663_WriteReg(rRegCommand,RC663_Idle);
	RC663_SetBitMask(rRegFIFOControl,0x10);		//FlushFifo
	RC663_WriteReg(rRegIRQ0,0x7F);		
	RC663_WriteReg(rRegIRQ1,0x7F);
	
	for(n=0;n<pi->Length;n++)
		RC663_WriteReg(rRegFIFOData, pi->Data[n]);
	if(pi->Command&0x80)
	{
		RC663_WriteReg(rRegIRQ0En,0x90);		
		if(mode)
			RC663_WriteReg(rRegIRQ1En,0xE0);		
		else
			RC663_WriteReg(rRegIRQ1En,0xE8);		
		
		Status_INT=0;
		RC663_WriteReg(rRegCommand, pi->Command);
		while(Status_INT==0);				//wait for IRQ
		//delay_ms(10);
		Status_INT=0;
		
		RC663_WriteReg(rRegIRQ0En,0x10);		
		if(mode)
			RC663_WriteReg(rRegIRQ1En,0x20);		
		else
			RC663_WriteReg(rRegIRQ1En,0x28);		
	}
	else
		RC663_WriteReg(rRegCommand, pi->Command);
	
	for(i=2000;i>0;i--)
	{
		n = RC663_ReadReg(rRegIRQ0);	
		if(n&0x10) break;		//IDLEIRQ
	}
	if(i==0)
		return MI_ERR;
	n = RC663_ReadReg(rRegFIFOLength);
	for(j=0;j<n;j++)
		pi->Data[j]= RC663_ReadReg(rRegFIFOData);
	return MI_OK;
}
	
s8 RC663_CMD_LoadProtocol(u8 rx,u8 tx)
{
	struct TranSciveBuffer ComData;
	
	ComData.Command = RC663_LoadProtocol;
	ComData.Length = 2;
	ComData.Data[0] = rx;
	ComData.Data[1] = tx;

	return RC663_Command_Int(&ComData);
}

s8 RC663_CMD_LoadKey(u8* pkey)
{
	struct TranSciveBuffer ComData;
	
	ComData.Command = RC663_LoadKey;
	ComData.Length = 6;
	memcpy(ComData.Data,pkey,6);

	return RC663_Command_Int(&ComData);
}

s8 RC663_CMD_MfcAuthenticate(u8 auth_mode,u8 block,u8 *pSnr)
{
	s8 status;
	u8 reg;
	struct TranSciveBuffer ComData;
	
	ComData.Command = RC663_MFAuthent;
	ComData.Length = 6;
	ComData.Data[0] = auth_mode;
	ComData.Data[1] = block;
	memcpy(&ComData.Data[2],pSnr,4);

	status= RC663_Command_Int(&ComData);
	if(status==MI_OK)
	{
		reg = RC663_ReadReg(rRegStatus);
		if(!(reg&0x20))
			status=MI_AUTHERR;
	}
	return status;
}

s8 RC663_PcdConfigISOType(u8 type)
{
//	u8 temp;
	RC663_WriteReg(rRegT0Control,0x98); //Starts at the end of Tx. Stops after Rx of first data. Auto-reloaded. 13.56 MHz input clock.
	RC663_WriteReg(rRegT1Control,0x92); //Starts at the end of Tx. Stops after Rx of first data. Input clock - cascaded with Timer-0.
	RC663_WriteReg(rRegT2Control,0x20); //Timer used for LFO trimming
	RC663_WriteReg(rRegT2ReloadHi,0x03);	//
	RC663_WriteReg(rRegT2ReloadLo,0xFF);	//
	RC663_WriteReg(rRegT3Control,0x00);	//Not started automatically. Not reloaded. Input clock 13.56 MHz	
	if(type=='A')
	{
		RC663_WriteReg(rRegWaterLevel,0x10);	//Set WaterLevel =(FIFO length -1)
										 
		RC663_WriteReg(rRegRxBitCtrl,0x80);	//Received bit after collision are replaced with 1.
		RC663_WriteReg(rRegDrvMod,0x80);	//Tx2Inv=1
		RC663_WriteReg(rRegTxAmp,0xC0);	// 0x00
		RC663_WriteReg(rRegDrvCon,0x09);	//01
		RC663_WriteReg(rRegTxl,0x05);	//
		RC663_WriteReg(rRegRxSofD,0x00);	//
		
		RC663_CMD_LoadProtocol(0,0);

		// Disable Irq 0,1 sources
		RC663_WriteReg(rRegIRQ0En,0);
		RC663_WriteReg(rRegIRQ1En,0);
		
		RC663_WriteReg(rRegFIFOControl,0xB0);

		RC663_WriteReg(rRegTxModWidth,0x20); // Length of the pulse modulation in carrier clks+1  
		RC663_WriteReg(rRegTxSym10BurstLen,0); // Symbol 1 and 0 burst lengths = 8 bits.
		RC663_WriteReg(rRegFrameCon,0xCF); // Start symbol=Symbol2, Stop symbol=Symbol3
		
		RC663_WriteReg(rRegRxCtrl,0x04); // Set Rx Baudrate 106 kBaud 
		RC663_WriteReg(rRegRxThreshold,0x55); // Set min-levels for Rx and phase shift  //32 
		RC663_WriteReg(rRegRcv,0x12);	//
		RC663_WriteReg(rRegRxAna,0x0A); //0
		
		RC663_WriteReg(rRegDrvMod,0x81);
		//> MIFARE Crypto1 state is further disabled.
		RC663_WriteReg(rRegStatus,0);
		//>  FieldOn
		RC663_WriteReg(rRegDrvMod,0x89);
	}
	else if(type=='B')
	{
		RC663_WriteReg(rRegWaterLevel,0x10);	//Set WaterLevel =(FIFO length -1)
										 
		RC663_WriteReg(rRegRxBitCtrl,0x80);	//Received bit after collision are replaced with 1.
		RC663_WriteReg(rRegDrvMod,0x8F);	//Tx2Inv=1
		RC663_WriteReg(rRegTxAmp,0x0C);	// 0xCC	
		RC663_WriteReg(rRegDrvCon,0x01);	
		RC663_WriteReg(rRegTxl,0x05);	
		RC663_WriteReg(rRegRxSofD,0x00);	
		
		RC663_CMD_LoadProtocol(4,4);

		// Disable Irq 0,1 sources
		RC663_WriteReg(rRegIRQ0En,0);
		RC663_WriteReg(rRegIRQ1En,0);
		
		RC663_WriteReg(rRegFIFOControl,0xB0);

		RC663_WriteReg(rRegTxModWidth,0x0A); // Length of the pulse modulation in carrier clks+1  
		RC663_WriteReg(rRegTxSym10BurstLen,0); // Symbol 1 and 0 burst lengths = 8 bits.
		RC663_WriteReg(rRegTXWaitCtrl,1);	
		RC663_WriteReg(rRegFrameCon,0x05); 
		RC663_WriteReg(rRegRxSofD,0xB2);
		
		RC663_WriteReg(rRegRxCtrl,0x34); // Set Rx Baudrate 106 kBaud 
		RC663_WriteReg(rRegRxThreshold,0x9F); // Set min-levels for Rx and phase shift  0x7F 
		RC663_WriteReg(rRegRcv,0x12);
		RC663_WriteReg(rRegRxAna,0x0a); //0x0a 0X0e
		
		RC663_WriteReg(rRegDrvMod,0x87);
		RC663_WriteReg(rRegStatus,0);
		//>  FieldOn
		RC663_WriteReg(rRegDrvMod,0x8F);
	}
	else if(type=='F')
	{
		RC663_WriteReg(rRegWaterLevel,0x10);	//Set WaterLevel =(FIFO length -1)
										 
		RC663_WriteReg(rRegRxBitCtrl,0x80);	//Received bit after collision are replaced with 1.
		RC663_WriteReg(rRegDrvMod,0x88);	//Tx2Inv=1
		RC663_WriteReg(rRegTxAmp,0x04);	//
		RC663_WriteReg(rRegDrvCon,0x01);	//
		RC663_WriteReg(rRegTxl,0x05);	//
		RC663_WriteReg(rRegRxSofD,0x00);	//
		
		RC663_CMD_LoadProtocol(8,8);

		// Disable Irq 0,1 sources
		RC663_WriteReg(rRegIRQ0En,0);
		RC663_WriteReg(rRegIRQ1En,0);
		
		RC663_WriteReg(rRegFIFOControl,0xB0);
		
		RC663_WriteReg(rRegTxModWidth,0x00); // Length of the pulse modulation in carrier clks+1  
		RC663_WriteReg(rRegTxSym10BurstLen,0x03); // Symbol 1 and 0 burst lengths = 8 bits.
		//RC663_WriteReg(rRegTXWaitCtrl,0xC0);	
		//RC663_WriteReg(rRegTxWaitLo,0);
		RC663_WriteReg(rRegFrameCon,0x01);
		
		//RC663_WriteReg(rRegRxSofD,0xB2);
		
		RC663_WriteReg(rRegRxCtrl,0x05); // Set Rx Baudrate 212 kBaud 
		RC663_WriteReg(rRegRxThreshold,0x5C); // Set min-levels for Rx and phase shift  0x3C 
		RC663_WriteReg(rRegRcv,0x12);
		RC663_WriteReg(rRegRxAna,0x02); //0xa  initial value 0x02
		
		RC663_WriteReg(rRegRxWait,0x86);
		
		RC663_WriteReg(rRegDrvMod,0x87);
		RC663_WriteReg(rRegStatus,0);
		//>  FieldOn
		RC663_WriteReg(rRegDrvMod,0x8F);
	}
	else if(type=='V')
	{
		RC663_WriteReg(rRegWaterLevel,0x10);	//Set WaterLevel =(FIFO length -1)
										 
		RC663_WriteReg(rRegRxBitCtrl,0x80);	//Received bit after collision are replaced with 1.
		RC663_WriteReg(rRegDrvMod,0x89);	//Tx2Inv=1 0x80
		RC663_WriteReg(rRegTxAmp,0x10);	//0	//0x04
		RC663_WriteReg(rRegDrvCon,0x09);	//0x01
		RC663_WriteReg(rRegTxl,0x0A);	//0x05
		RC663_WriteReg(rRegRxSofD,0x00);	//
		
		RC663_CMD_LoadProtocol(0x0A,0x0A);

		// Disable Irq 0,1 sources
		RC663_WriteReg(rRegIRQ0En,0);
		RC663_WriteReg(rRegIRQ1En,0);
		
		RC663_WriteReg(rRegFIFOControl,0xB0);
		
		RC663_WriteReg(rRegTxModWidth,0x00); // Length of the pulse modulation in carrier clks+1  
		RC663_WriteReg(rRegTxSym10BurstLen,0); // Symbol 1 and 0 burst lengths = 8 bits.
		//RC663_WriteReg(rRegTXWaitCtrl,0xC0);	//0x88
		//RC663_WriteReg(rRegTxWaitLo,0);	//0xa9
		RC663_WriteReg(rRegFrameCon,0x0F);
		
		//RC663_WriteReg(rRegRxSofD,0xB2);
		
		RC663_WriteReg(rRegRxCtrl,0x02); // Set Rx Baudrate 26 kBaud 
		RC663_WriteReg(rRegRxThreshold,0x74); // Set min-levels for Rx and phase shift  
		RC663_WriteReg(rRegRcv,0x12);
		RC663_WriteReg(rRegRxAna,0x07); 
		
		RC663_WriteReg(rRegRxWait,0x9C);	
		
		RC663_WriteReg(rRegDrvMod,0x81);
		RC663_WriteReg(rRegStatus,0);
		//>  FieldOn
		RC663_WriteReg(rRegDrvMod,0x89);
	}
	return MI_OK;
}

s8 RC663_PcdComTransceive(struct TranSciveBuffer *pi)
{
	s8 status= MI_ERR;
	u16 i;
	u8 reg1,temp,lastBits;	//reg0,
	u8 errReg;
	// Terminate any running command. 
	RC663_WriteReg(rRegCommand,RC663_Idle);             // 0x00 // Starts and stops command execution
	RC663_SetBitMask(rRegFIFOControl,0x10);		//Flush_FiFo     0x02 // Control register of the FIFO
	// Clear all IRQ 0,1 flags
	RC663_WriteReg(rRegIRQ0,0x7F);		
	RC663_WriteReg(rRegIRQ1,0x7F);

	for(i=0;i<pi->Length;i++)
		RC663_WriteReg(rRegFIFOData,pi->Data[i]);           //  0x05 // Data In/Out exchange register of FIFO buffer
	// Idle interrupt(Command terminated), RC663_BIT_IDLEIRQ=0x10
	RC663_WriteReg(rRegIRQ0En,0x18);	//IdleIRQEn,TxIRQEn
	RC663_WriteReg(rRegIRQ1En,0x42);	//Global IRQ,Timer1IRQEn
	//>  Start RC663 command "Transcieve"=0x07. Activate Rx after Tx finishes.
	RC663_WriteReg(rRegCommand,RC663_Transceive);
	
	do	
  {
      reg1 = RC663_ReadReg(rRegIRQ1);	  //07h //wait for TxIRQ
  }while((reg1&0x40)==0); //GlobalIRQ

	RC663_WriteReg(rRegIRQ0En,0x54);	//HiAlertIRQEN,IdleIRQEn,RxIRQEn
	RC663_WriteReg(rRegIRQ1En,0x42);	//Global IRQ,Timer1IRQEn

	for(i=8000;i>0;i--)
	{
		reg1 = RC663_ReadReg(rRegIRQ1);	  //07h  //wait for RxIRQ
		if(reg1&0x40) break;	//GlobalIRQ
	}

	RC663_WriteReg(rRegIRQ0En,0);		
	RC663_WriteReg(rRegIRQ1En,0);

	errReg = RC663_ReadReg(rRegError);
	if(i==0)
		status = MI_QUIT;
	else if(reg1&0x02)		//Timer1IRQ
		status = MI_NOTAGERR;
	else if( errReg)	  //0Bh
	{
		if(errReg&0x04)
			status = MI_COLLERR;
		else if(errReg&0x01)
			status = MI_FRAMINGERR;
		else
			status = MI_ERR;
	}
	else
	{
		status = MI_OK;
		if (pi->Command == RC663_Transceive)
		{
				temp = RC663_ReadReg(rRegFIFOLength);		 //04h
				lastBits = RC663_ReadReg(rRegRxBitCtrl) & 0x07;	 //0ch
				if (lastBits)
					pi->Length = (temp-1)*8 + lastBits;
				else
					pi->Length = temp*8;
				if (temp == 0)	temp = 1;
				if (temp > 250) temp = 250; //maxlen ...
				for (i=0; i<temp; i++)
					pi->Data[i] = RC663_ReadReg(rRegFIFOData);  //05h
		}
	}
	return status;
}
s8 RC663_PcdHaltA(void)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;
	
	ComData.Command = RC663_Transceive;
	ComData.Length  = 2;
	ComData.Data[0] = PICC_HALT;
	ComData.Data[1] = 0;

	status = RC663_PcdComTransceive(pi);
	if(status == MI_NOTAGERR)		//halt command has no response
		status = MI_OK;
	else
		status = MI_ERR;
	return status;
}

s8 RC663_PcdRequestA(u8   req_code,u8 *pTagType)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;

	RC663_WriteReg(rRegTxCrcPreset,0x18);  //0x2C Transmitter CRC control register, preset value
	RC663_WriteReg(rRegRxCrcPreset,0x18);
	RC663_WriteReg(rRegStatus,0);           //  0x0B  Contains status of the communication
	
	RC663_WriteReg(rRegTXWaitCtrl,0xC0);	//0x31  TxWaitStart at the end of Rx data
	RC663_WriteReg(rRegTxWaitLo,0x0B);	// 0x32   Set min.time between Rx and Tx or between two Tx   
	// Set timeout for this command cmd. Init reload values for timers-0,1 
	RC663_WriteReg(rRegT0ReloadHi,0x08); 	//2196/fc        0x10 // High register of the reload value of Timer0
	RC663_WriteReg(rRegT0ReloadLo,0x94);            //0x11 // Low register of the reload value of Timer0
	RC663_WriteReg(rRegT1ReloadHi,0);                     //0x15 // High register of the reload value of Timer1
	RC663_WriteReg(rRegT1ReloadLo,0x40);	//timerout ~= 10ms      	0x16 // Low register of the reload value of Timer1
	
	RC663_WriteReg(rRegIRQ0,0x08);                          //	0x06 // Interrupt register 0
	RC663_WriteReg(rRegRxWait,0x90);                            //0x36 // Receiver wait register
	RC663_WriteReg(rRegTxDataNum,0x0F);  //7bit                      0x2E // Transmitter data number register
	
	//> Send the ReqA command
	ComData.Command = RC663_Transceive;           //0x07 //- transmits data from the FIFO buffer and automatically activates the receiver after transmission finished
	ComData.Length  = 1;
	ComData.Data[0] = req_code;
	//FLAG_1;
	status = RC663_PcdComTransceive(pi);
	//FLAG_0;
	if (status == MI_OK)
	{
		if(ComData.Length == 0x10)
		{
			*pTagType     = ComData.Data[0];
			*(pTagType+1) = ComData.Data[1];
		}
		else
			status = MI_VALERR;
	}
	//RC663_WriteReg(rRegTxDataNum,0x08);
	return status;
}

s8 RC663_PcdAnticoll(u8 *pSnr)
{
    s8 status ;
    u8 i;
    u8 ucBits,ucBytes;
    u8 snr_check = 0;
    u8 ucCollPosition = 0;
    u8 ucTemp;
    u8 ucSNR[5] = {0, 0, 0, 0 ,0};
    struct TranSciveBuffer ComData,*pi = &ComData;
    
		RC663_WriteReg(rRegTxDataNum,0x08);
    do
    {
			ucBits = (ucCollPosition) % 8;
			if (ucBits != 0)
			{
			    ucBytes = ucCollPosition / 8 + 1;
					RC663_SetRawRC(rRegRxBitCtrl, 0x8f,ucBits<<4);
					RC663_SetRawRC(rRegTxDataNum, 0xf8,ucBits);
			}
			else
			     ucBytes = ucCollPosition / 8;

			ComData.Command = RC663_Transceive;
			ComData.Data[0] = PICC_ANTICOLL1;
			ComData.Data[1] = 0x20 + ((ucCollPosition / 8) << 4) + (ucBits & 0x0F);
			for (i=0; i<ucBytes; i++)
			  ComData.Data[i + 2] = ucSNR[i];
			ComData.Length = ucBytes + 2;
	
	    status = RC663_PcdComTransceive(pi);
	
	    ucTemp = ucSNR[(ucCollPosition / 8)];
			if (status == MI_COLLERR)
			{
			  for (i=0; i < 5 - (ucCollPosition / 8); i++)
			     ucSNR[i + (ucCollPosition / 8)] = ComData.Data[i+1];
			  ucSNR[(ucCollPosition / 8)] |= ucTemp;
			  ucCollPosition = ComData.Data[0];
			}
			else if (status == MI_OK)
			{
		    for (i=0; i < (ComData.Length / 8); i++)
		       ucSNR[4 - i] = ComData.Data[ComData.Length/8 - i - 1];
		    ucSNR[(ucCollPosition / 8)] |= ucTemp;
			}
    } while (status == MI_COLLERR);
			
    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
       {   
         *(pSnr+i)  = ucSNR[i];
         snr_check ^= ucSNR[i];
       }
       if (snr_check != ucSNR[i])
       	status = MI_COM_ERR;
    }
    return status;
}

s8 RC663_PcdSelect(u8 *pSnr,u8 *pSize)
{
	s8   status;
	u8   i,snr_check = 0;
	struct TranSciveBuffer ComData,*pi = &ComData;

	RC663_SetRawRC(rRegTxCrcPreset,0xfe,0x01);	//On
	RC663_SetRawRC(rRegRxCrcPreset,0xfe,0x01);	//On
    
	ComData.Command = RC663_Transceive;
	ComData.Length  = 7;
	ComData.Data[0] = PICC_ANTICOLL1;
	ComData.Data[1] = 0x70;
	for (i=0; i<4; i++)
	{
		snr_check ^= *(pSnr+i);
		ComData.Data[i+2] = *(pSnr+i);
	}
	ComData.Data[6] = snr_check;
	
	status = RC663_PcdComTransceive(pi);
	
	if (status == MI_OK)
	{
    if (ComData.Length != 0x8)
    	status = MI_BITCOUNTERR;
    else
    	*pSize = ComData.Data[0];
	}
	return status;
}

s8 RC663_PcdRead(u8   addr,u8 *pReaddata)
{
	s8 status;
	struct TranSciveBuffer ComData,*pi = &ComData;
	
	RC663_SetRawRC(rRegTxCrcPreset,0xfe,0x01);	//on
	RC663_SetRawRC(rRegRxCrcPreset,0xfe,0x00);	//off
	
  ComData.Command = RC663_Transceive;
	ComData.Length  = 2;
	ComData.Data[0] = PICC_READ;
	ComData.Data[1] = addr;
	
	status = RC663_PcdComTransceive(pi);
  if (status == MI_OK)
	{
		if (ComData.Length != 0x90)
			status = MI_BITCOUNTERR;
		else
			memcpy(pReaddata, &ComData.Data[0], 16);
	}
	return status;
}

s8 RC663_PcdWrite(u8   addr,u8 *pWritedata)
{
	s8 status;
	struct TranSciveBuffer ComData,*pi = &ComData;
	
	ComData.Command = RC663_Transceive;
	ComData.Length  = 2;
	ComData.Data[0] = PICC_WRITE;
	ComData.Data[1] = addr;

	status = RC663_PcdComTransceive(pi);
	if (status != MI_NOTAGERR)
	{
			if(ComData.Length != 4)
				status=MI_BITCOUNTERR;
			else
			{
				 ComData.Data[0] &= 0x0F;
				 switch (ComData.Data[0])
				 {
						case 0x00:
							 status = MI_NOTAUTHERR;
							 break;
						case 0x0A:
							 status = MI_OK;
							 break;
						default:
							 status = MI_CODEERR;
							 break;
				 }
			}
	}    
	if (status == MI_OK)
	{
			ComData.Command = RC663_Transceive;
			ComData.Length  = 16;
			memcpy(&ComData.Data[0], pWritedata, 16);
			
			status = RC663_PcdComTransceive(pi);
			if (status != MI_NOTAGERR)
			{
					ComData.Data[0] &= 0x0F;
					switch(ComData.Data[0])
					{
						 case 0x00:
								status = MI_WRITEERR;
								break;
						 case 0x0A:
								status = MI_OK;
								break;
						 default:
								status = MI_CODEERR;
								break;
				 }
			}
	}
	return status;
}

void RC663_MifareClassic(void)
{
	s8 status;
	u8 i;
	u8 M1_Data[7],RD_Data[16];	//CT:2 SN:4 SAK:1
	static u8 KEY[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	RC663_PcdConfigISOType('A');
	//delay_ms(10);
	///////////////////////////////////mifare S50	
	status = RC663_PcdRequestA(PICC_REQALL,M1_Data);
#ifdef UART_PRINT
	debug_print("ATQA: %d_",status);
	if(status==MI_OK)
		debug_print(" %02X %02X",M1_Data[1],M1_Data[0]);
	debug_print("\n");
#endif
	if(status!=MI_OK) return;
	
	status = RC663_PcdAnticoll(&M1_Data[2]);
#ifdef UART_PRINT
	debug_print("UID: %d_",status);
	if(status==MI_OK)
		debug_print("%02X %02X %02X %02X",M1_Data[2],M1_Data[3],M1_Data[4],M1_Data[5]);
	debug_print("\n");
#endif
	if(status!=MI_OK) return;
	
	status = RC663_PcdSelect(&M1_Data[2],&M1_Data[6]);
#ifdef UART_PRINT
	debug_print("SELECT: %d_",status);
	if(status==MI_OK)
		debug_print("%02X",M1_Data[6]);
	debug_print("\n");
#endif
	if(status!=MI_OK) return;
	
	status = RC663_CMD_LoadKey(KEY);
#ifdef UART_PRINT
	debug_print("LoadKey: %d\n",status);
#endif
	if(status!=MI_OK) return;
	status = RC663_CMD_MfcAuthenticate(0x60,0,&M1_Data[2]);
#ifdef UART_PRINT
	debug_print("Auth: %d\n",status);
#endif
	if(status!=MI_OK) return;
	
	status=RC663_PcdRead(1,RD_Data);
#ifdef UART_PRINT
	debug_print("READ:%d_",status);
	for(i=0;i<16;i++)
		debug_print(" %02X",RD_Data[i]);
	debug_print("\n");
#endif
	if(status!=MI_OK) return;
	/*RD_Data[15]++;
	status=RC663_PcdWrite(1,RD_Data);
#ifdef UART_PRINT
	debug_print("Write:%d  --\n",status);
#endif
	if(status!=MI_OK) return;
	status=RC663_PcdRead(1,RD_Data);
#ifdef UART_PRINT
	debug_print("READ:%d  --",status);
	for(i=0;i<16;i++)
		debug_print(" %2X ",RD_Data[i]);
	debug_print("\n");
	//debug_print("Please remove card!\n");
#endif*/
	//if(status!=MI_OK) return;
	do
	{
		RC663_FieldOff();
		delay_ms(10);
		RC663_FieldOn();
		//status = RC663_PcdHaltA();
		LED_1;
		delay_ms(10);
		status = RC663_PcdRequestA(PICC_REQALL,M1_Data);
		if(status==MI_OK)
		{
			LED_0;
		}
		else 
			break;
	}while(status==MI_OK);
}
//////////////////////////////////////////////////////ID2
s8 RC663_PcdRequestB(u8 *pTagType,u8 *pLen)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;
	
	//RC663_WriteReg(rRegStatus,0);
	RC663_WriteReg(rRegTxCrcPreset,0x7B); 
	RC663_WriteReg(rRegRxCrcPreset,0x7B); 
	RC663_WriteReg(rRegTxDataNum,0x08); 
	
	RC663_WriteReg(rRegTXWaitCtrl,0xC1);	//  TxWaitStart at the end of Rx data
	RC663_WriteReg(rRegTxWaitLo,0x0B);	// Set min.time between Rx and Tx or between two Tx   
	// Set timeout for this command cmd. Init reload values for timers-0,1 
	RC663_WriteReg(rRegT0ReloadHi,0x08); 	//2196/fc
	RC663_WriteReg(rRegT0ReloadLo,0x94); 
	RC663_WriteReg(rRegT1ReloadHi,0); 
	RC663_WriteReg(rRegT1ReloadLo,0x20);	//timerout ~= 10ms
	
	RC663_WriteReg(rRegIRQ0,0x08);
	RC663_WriteReg(rRegRxWait,0x90);
	//> Send the ReqB command
	ComData.Command = RC663_Transceive;
	ComData.Length  = 3;
	ComData.Data[0] = PICC_ANTI;
	ComData.Data[1] = 0;
	ComData.Data[2] = 0;
	//FLAG_0;
	status = RC663_PcdComTransceive(pi);
	//FLAG_1;
	if (status == MI_OK)
	{
		if((ComData.Length == 0x60) || (ComData.Length == 0x10))
		{
			*pLen = ComData.Length/8;
			memcpy(pTagType, &ComData.Data[0], *pLen);
		}
		else
			status = MI_VALERR;
	}
	return status;
}

s8 RC663_PcdAttribB(u8 *pCid)
{
	s8   status;
	struct TranSciveBuffer ComData,*pi = &ComData;

	ComData.Command = RC663_Transceive;
	ComData.Length  = 9;
	ComData.Data[0] = PICC_ATTRIB;
	ComData.Data[1] = 0;	//pupi
	ComData.Data[2] = 0;
	ComData.Data[3] = 0;
	ComData.Data[4] = 0;
	ComData.Data[5] = 0;	//
	ComData.Data[6] = 8;
	ComData.Data[7] = 1;
	ComData.Data[8] = 0;	//0 8

	status = RC663_PcdComTransceive(pi);
	
	if (status == MI_OK)
	{
    if (ComData.Length != 0x8)
    	status = MI_BITCOUNTERR;
    else
    	*pCid = ComData.Data[0];
	}
	return status;
}

s8 RC663_PcdGetUidB(u8 *pUID)
{
	s8   status;
	struct TranSciveBuffer ComData,*pi = &ComData;

	ComData.Command = RC663_Transceive;
	ComData.Length  = 5;
	ComData.Data[0] = 0;
	ComData.Data[1] = 0x36;
	ComData.Data[2] = 0;
	ComData.Data[3] = 0;
	ComData.Data[4] = 8;
	status = RC663_PcdComTransceive(pi);
	
	if ((status == MI_OK) && (ComData.Length == 0x50))
    memcpy(pUID, &ComData.Data[0], 10);
	else
		status = MI_ERR;
	return status;
}

void RC663_ID2(void)
{
	s8 status;
	u8 i;//,temp;
	u8 RD_Data[12],Len;	
	RC663_PcdConfigISOType('B');
	delay_ms(10);
	///////////////////////////////////2nd ID

	status = RC663_PcdRequestB(RD_Data,&Len);
	//FLAG_0;
#ifdef UART_PRINT
	debug_print("ATQB: %d_",status);
	if(status==MI_OK)
	{
		for(i=0;i<Len;i++)
			debug_print(" %02X",RD_Data[i]);
	}
	debug_print("\n");
#endif
	if(status!=MI_OK) return;
	
	if(Len==12)
	{
		status = RC663_PcdAttribB(RD_Data);
#ifdef UART_PRINT
		debug_print("SELECT:%d_",status);
		if(status==MI_OK)
			debug_print(" %02X",RD_Data[0]);
		debug_print("\n");
#endif
		if(status!=MI_OK) return;
	}
	
	status = RC663_PcdGetUidB(RD_Data);
#ifdef UART_PRINT
	debug_print("UID: %d_",status);
	if(status==MI_OK)
	{
		for(i=0;i<10;i++)
			debug_print(" %02X",RD_Data[i]);
	}
	debug_print("\n");
#endif

	while(1)
	{
		RC663_FieldOff();
		delay_ms(10);
		RC663_FieldOn();
		LED_1;
		delay_ms(10);
		status = RC663_PcdRequestB(RD_Data,&Len);
		if(status==MI_OK)
		{
			LED_0;
		}
		else 
			break;
	}
}
//////////////////////////////////////////////////////Felica
s8 RC663_PcdRequestF(u8 *pIDm,u8 *pPMm)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;
	
	//RC663_WriteReg(rRegStatus,0);
	RC663_WriteReg(rRegTxCrcPreset,0x09); 
	RC663_WriteReg(rRegRxCrcPreset,0x09); 
	RC663_WriteReg(rRegTxDataNum,0x08); 
	
	RC663_WriteReg(rRegTXWaitCtrl,0xC0);	//  TxWaitStart at the end of Rx data
	RC663_WriteReg(rRegTxWaitLo,0x00);	// Set min.time between Rx and Tx or between two Tx   
	// Set timeout for this command cmd. Init reload values for timers-0,1 
	RC663_WriteReg(rRegT0ReloadHi,0x00); 	//2196/fc 0x08
	RC663_WriteReg(rRegT0ReloadLo,0x80); 	//0x94
	RC663_WriteReg(rRegT1ReloadHi,0x03); 
	RC663_WriteReg(rRegT1ReloadLo,0x22);	//timerout ~= 10ms
	
	//RC663_WriteReg(rRegIRQ0,0x08);
	//RC663_WriteReg(rRegRxWait,0x90);	
	//> Send the ReqF command
	ComData.Command = RC663_Transceive;
	ComData.Length  = 6;
	ComData.Data[0] = 0x06;
	ComData.Data[1] = 0;
	ComData.Data[2] = 0xFF;
	ComData.Data[3] = 0xFF;
	ComData.Data[4] = 0;
	ComData.Data[5] = 0;
	status = RC663_PcdComTransceive(pi);
	if (status == MI_OK)
	{
		if(ComData.Length == 0x90)
		{
			//*pLen = ComData.Length/8;
			memcpy(pIDm, &ComData.Data[2], 8);
			memcpy(pPMm, &ComData.Data[10], 8);
		}
		else
			status = MI_VALERR;
	}
	return status;
}

s8 RC663_PcdReadF(u8 *pIDm,u8 *pDat)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;
	
	RC663_WriteReg(rRegT0ReloadHi,0x41); 	//2196/fc 0x08
	RC663_WriteReg(rRegT0ReloadLo,0x0B); 	//0x94
	
	ComData.Command = RC663_Transceive;
	ComData.Length  = 16;
	ComData.Data[0] = 0x10;		//len
	ComData.Data[1] = 0x06;
	memcpy(&ComData.Data[2],pIDm, 8);
	ComData.Data[10] = 1;			//bNumServices
	ComData.Data[11] = 0x0B;	//pServiceList..
	ComData.Data[12] = 0;
	ComData.Data[13] = 1;			//bTxNumBlocks
	ComData.Data[14] = 0x80;	//pBlockList..
	ComData.Data[15] = 0x01;
	status = RC663_PcdComTransceive(pi);
	if (status == MI_OK)
	{
		if(ComData.Length == 0xE8)		//
		{
			//*pLen = ComData.Length/8;
			//0x1d,0x07,IDm(8),0,0,1,data(16)
			//memcpy(pIDm, &ComData.Data[2], 8);
			memcpy(pDat, &ComData.Data[13], 16);
		}
		else
			status = MI_VALERR;
	}
	return status;
}
s8 RC663_PcdWriteF(u8 *pIDm,u8 *pDat)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;
	
	RC663_WriteReg(rRegT0ReloadHi,0x00); 	//2196/fc 0x08
	RC663_WriteReg(rRegT0ReloadLo,0x80); 	//0x94
	RC663_WriteReg(rRegT1ReloadHi,0x02); 	//2196/fc 0x08
	RC663_WriteReg(rRegT1ReloadLo,0xA1); 	//0x94
	
	ComData.Command = RC663_Transceive;
	ComData.Length  = 32;
	ComData.Data[0] = 0x20;		//len
	ComData.Data[1] = 0x08;
	memcpy(&ComData.Data[2],pIDm, 8);
	ComData.Data[10] = 1;			//bNumServices
	ComData.Data[11] = 0x09;	//pServiceList..
	ComData.Data[12] = 0;
	ComData.Data[13] = 1;			//bTxNumBlocks
	ComData.Data[14] = 0x80;	//pBlockList..
	ComData.Data[15] = 0x01;
	memcpy(&ComData.Data[16],pDat, 16);
	status = RC663_PcdComTransceive(pi);
	if(status == MI_OK)
	{
		if(ComData.Length != 0x60)		//12
		{
			//*pLen = ComData.Length/8;
			//0x0C,0x09,IDm(8),0,0
			status = MI_VALERR;
		}
	}
	return status;
}
void RC663_Felica(void)
{
	s8 status;
	u8 i;//,temp;
	u8 IDm[8],PMm[8],buf[16];	
	RC663_PcdConfigISOType('F');
	delay_ms(10);		
	///////////////////////////////////
	status = RC663_PcdRequestF(IDm,PMm);
#ifdef UART_PRINT
	debug_print("ATQF: %d_",status);
	if(status==MI_OK)
	{
		for(i=0;i<8;i++)
			debug_print(" %02X ",IDm[i]);
		debug_print("_");
		for(i=0;i<8;i++)
			debug_print(" %02X",PMm[i]);
	}
	debug_print("\n");
#endif
	if(status!=MI_OK) return;
	
	status = RC663_PcdReadF(IDm,buf);
#ifdef UART_PRINT
	debug_print("Read: %d_",status);
	if(status==MI_OK)
	{
		for(i=0;i<16;i++)
			debug_print(" %02X",buf[i]);
	}
	debug_print("\n");
#endif
	if(status!=MI_OK) return;
	
	/*delay_ms(100);
	buf[15]++;
	status = RC663_PcdWriteF(IDm,buf);
#ifdef UART_PRINT
	debug_print("Write: %d\n",status);
#endif
	if(status!=MI_OK) return;
	*/
	do
	{
		RC663_FieldOff();
		delay_ms(10);
		RC663_FieldOn();
		LED_1;
		delay_ms(10);
		status = RC663_PcdRequestF(IDm,PMm);
		if(status==MI_OK)
		{
			LED_0;
		}
		else 
			break;
		
	}while(status==MI_OK);
}
//////////////////////////////////////////////////////ISO15693
s8 RC663_PcdInventoryV(u8 *pUID)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;
	
	//RC663_WriteReg(rRegStatus,0);
	RC663_WriteReg(rRegTxCrcPreset,0x7B); 
	RC663_WriteReg(rRegRxCrcPreset,0x7B);
	RC663_WriteReg(rRegTxDataNum,0x08); 
	
	RC663_WriteReg(rRegTXWaitCtrl,0x88);	//  TxWaitStart at the end of Rx data 0xC0
	RC663_WriteReg(rRegTxWaitLo,0xA9);	// Set min.time between Rx and Tx or between two Tx   0
	// Set timeout for this command cmd. Init reload values for timers-0,1 
	RC663_WriteReg(rRegT0ReloadHi,0x00); 	//2196/fc 0x0080
	RC663_WriteReg(rRegT0ReloadLo,0x80); 	//0x94
	RC663_WriteReg(rRegT1ReloadHi,0x03); 	//0x0232
	RC663_WriteReg(rRegT1ReloadLo,0x22);	//timerout ~= 10ms
	
	//RC663_WriteReg(rRegIRQ0,0x08);
	//RC663_WriteReg(rRegRxWait,0x90);
	//> Send the ReqF command
	ComData.Command = RC663_Transceive;
	ComData.Length  = 3;
	ComData.Data[0] = 0x26;	//flags
	ComData.Data[1] = 0x01;	//invetory
	//ComData.Data[2] = 0;	//afi
	ComData.Data[2] = 0;	//masklen
	status = RC663_PcdComTransceive(pi);
	if (status == MI_OK)
	{
		if(ComData.Length == 0x50)
		{
			//*pLen = ComData.Length/8;
			memcpy(pUID, &ComData.Data[2], 8);
		}
		else
			status = MI_VALERR;
	}
	return status;
}
s8 RC663_SelectV(u8 *pUid)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;
	
	RC663_WriteReg(rRegT0ReloadHi,0x24); 	
	RC663_WriteReg(rRegT0ReloadLo,0xEB); 	
	RC663_WriteReg(rRegT1ReloadHi,0x00); 	
	RC663_WriteReg(rRegT1ReloadLo,0x00);	
	
	ComData.Command = RC663_Transceive;
	ComData.Length  = 10;
	ComData.Data[0] = 0x22;	//bflag
	ComData.Data[1] = 0x25;	//PHPAL_SLI15693_SW_CMD_SELECT
	memcpy(&ComData.Data[2],pUid, 8);
	status = RC663_PcdComTransceive(pi);
	if (status == MI_OK)
	{
		if(ComData.Length ==0 || ComData.Data[0]!=0)
			status = MI_VALERR;
	}
	return status;
}

s8 RC663_ReadSingleBlockV(u8 block,u8 *pDat,u8 *pLen)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;
	
	RC663_WriteReg(rRegT0ReloadHi,0x24); 	//2196/fc 0x0080
	RC663_WriteReg(rRegT0ReloadLo,0xEB); 	//0x94
	RC663_WriteReg(rRegT1ReloadHi,0x00); 	//0x0232
	RC663_WriteReg(rRegT1ReloadLo,0x00);	//timerout ~= 10ms
	
	ComData.Command = RC663_Transceive;
	ComData.Length  = 3;
	ComData.Data[0] = 0x12;	//bflag
	ComData.Data[1] = 0x20;	//PHAL_ICODE_CMD_READ_SINGLE_BLOCK
	ComData.Data[2] = block;
	status = RC663_PcdComTransceive(pi);
	if (status == MI_OK)
	{
		if(ComData.Length !=0)//== 0x50)
		{
			*pLen = ComData.Length/8-1;
			memcpy(pDat, &ComData.Data[1], *pLen);
		}
		else
			status = MI_VALERR;
	}
	return status;
}

s8 RC663_WriteSingleBlockV(u8 block,u8 *pDat,u8 Len)
{
	s8 status; 
	struct TranSciveBuffer ComData,*pi= &ComData;
	
	RC663_WriteReg(rRegT0ReloadHi,0x00); 	
	RC663_WriteReg(rRegT0ReloadLo,0x80); 	
	RC663_WriteReg(rRegT1ReloadHi,0x08); 	
	RC663_WriteReg(rRegT1ReloadLo,0x6E);	//timerout ~= 20ms
	
	ComData.Command = RC663_Transceive;
	ComData.Length  = Len+3;
	ComData.Data[0] = 0x12;	//bflag
	ComData.Data[1] = 0x21;	//PHAL_ICODE_CMD_WRITE_SINGLE_BLOCK
	ComData.Data[2] = block;
	memcpy(&ComData.Data[3],pDat,Len);
	
	status = RC663_PcdComTransceive(pi);
	if (status == MI_OK)
	{
		if(ComData.Length ==0 || ComData.Data[0]!=0)
			status = MI_VALERR;
	}
	return status;
}

void RC663_ISO15693(void)
{
	s8 status;
	u8 i;//,temp;
	u8 uid[8];
	u8 dat[16],len;	
	RC663_PcdConfigISOType('V');
	delay_ms(5);		
	///////////////////////////////////Type V/ISO15693
	status = RC663_PcdInventoryV(uid);
#ifdef UART_PRINT
	//debug_print("Inventory: %d_",status);
	if(status==MI_OK)
	{
		
		debug_print("寻到卡 uid = ");
		for(i=0;i<8;i++)
			debug_print(" %02X",uid[i]);
		debug_print("\n");
	}
	
#endif
	if(status!=MI_OK) return;
	
	status = RC663_SelectV(uid);
#ifdef UART_PRINT
	debug_print("Select: %d\n",status);
#endif
	if(status!=MI_OK) return;
	
	status = RC663_ReadSingleBlockV(3,dat,&len);
#ifdef UART_PRINT
	debug_print("ReadBlock: %d_",status);
	if(status==MI_OK)
	{
		for(i=0;i<len;i++)
			debug_print(" %02X",dat[i]);
	}
	debug_print("\n");
#endif
	if(status!=MI_OK) return;

	/*dat[2]++;
	status = RC663_WriteSingleBlockV(3,dat,len);
#ifdef UART_PRINT
	debug_print("WriteBlock: %d\n",status);
#endif
	*/
//	do
//	{
//		RC663_FieldOff();
//		delay_ms(1);
//		RC663_FieldOn();
//		LED_1;
//		delay_ms(1);
//		status = RC663_PcdInventoryV(uid);
//		if(status==MI_OK)
//		{
//			LED_0;
//		}
//		else 
//		{
//			break;
//		}
//			
//		//delay_ms(50);
//		//status = RC663_PcdInventoryV(uid);
//	}while(status==MI_OK);
}
//////////////////////////////////////////////////////
//mode 0:calibration; 1:detection
s8 RC663_CMD_Lpcd()
{
	struct TranSciveBuffer ComData;
	s8 status = MI_OK;
	u8 backAna,temp;

	RC663_WriteReg(rRegT3ReloadHi,0x01);		//0x1f	//powerdown 20us
	RC663_WriteReg(rRegT3ReloadLo,0x0F);		//0x20
	RC663_WriteReg(rRegT4ReloadHi,0x14);		//0x24: 0x14 0	//poweron 300ms
	RC663_WriteReg(rRegT3ReloadLo,0xA0);		//0x25: 0xA0 5
	
	if(mode)
		RC663_WriteReg(rRegT4Control,0xDC);			//0x23	//start T4 ,0x1C dc
	else
		RC663_WriteReg(rRegT4Control,0xF4);			//0x23	//start T4 ,0x34 f4
	RC663_WriteReg(rRegLPCD_Q_Result,0);		//0x43	//clear LPCD result

	RC663_SetBitMask(rRegRcv,0x40);					//0x38	//set Mix2Adc bit
	backAna = RC663_ReadReg(rRegRxAna);		//0x39	//Backup current RxAna setting
	RC663_WriteReg(rRegRxAna,0x03);

	while(1)
	{
		temp = RC663_ReadReg(rRegT4Control);		//0x23 //wait until T4 is started
		if(temp&0x80) break;
		else debug_print("RC663_ReadReg test\n ");
	}

	ComData.Command = 0x81;		//bStandby
	ComData.Length = 0;
	status = RC663_Command_Int(&ComData);

	RC663_WriteReg(rRegRxAna,backAna);	//0x39	//restore RxAna setting
	RC663_ClearBitMask(rRegRcv,0x40);		//0x38	//clear Mix2Adc bit
	RC663_SetRawRC(rRegT4Control,0x7f,0x40);			//0x23	//stop T4
	
	RC663_WriteReg(rRegCommand,RC663_Idle);
	RC663_WriteReg(rRegFIFOControl,0xB0);	//flush fifo
	if(mode)
	{
		temp = RC663_ReadReg(rRegIRQ1);
		if(temp&0x20)	//PHHAL_HW_RC663_BIT_LPCDIRQ
		{
			RC663_WriteReg(rRegLPCD_Q_Result,0x40);		//0x43	//clear LPCD interrupt
		}
		else
			status = MI_ERR;
	}
	return status;
}

s8 RC663_Lpcd_Calib(u8 *pI,u8 *pQ)
{
	s8 status;
	RC663_WriteReg(rRegLPCD_QMin,0xc0);
	RC663_WriteReg(rRegLPCD_QMax,0xff);
	RC663_WriteReg(rRegLPCD_IMin,0xc0);
	mode =0;
	status = RC663_CMD_Lpcd();
	*pI = RC663_ReadReg(rRegLPCD_I_Result);
	*pQ = RC663_ReadReg(rRegLPCD_Q_Result);
	return status;
}

s8 RC663_Lpcd_Det(u8 ValueI,u8 ValueQ)
{
	s8 status;
	u8 bQMin,bQMax,bIMin,bIMax;
	u8 reg;
	bQMin= bQMax = ValueQ;
	bIMin= bIMax = ValueI;
	if(bQMax < 0x3FU) bQMax++;
  if(bIMax < 0x3FU) bIMax++;
  if(bQMin > 0U) bQMin--;
  if(bIMin > 0U) bIMin--;
	reg = bQMin | (uint8_t)((bIMax & 0x30U) << 2U);
	RC663_WriteReg(rRegLPCD_QMin,reg);
	reg = bQMax | (uint8_t)((bIMax & 0x0CU) << 4U);
	RC663_WriteReg(rRegLPCD_QMax,reg);
	reg = bIMin | (uint8_t)((bIMax & 0x03U) << 6U);
	RC663_WriteReg(rRegLPCD_IMin,reg);
	mode = 1;
	status = RC663_CMD_Lpcd();
	return status;
}

int read_card_block( uint8_t* uid, uint8_t* block_buff, uint16_t* read_block_len)
{
	s8 status;
	u8 i;//,temp;
	u8 dat[16],len;	
//	int err;
	
	RC663_PcdConfigISOType('V');
	delay_ms(5);		
	///////////////////////////////////Type V/ISO15693
	status = RC663_PcdInventoryV(uid);
	#ifdef UART_PRINT
	//debug_print("Inventory: %d_",status);
	if(status==MI_OK)
	{
		debug_print("寻到卡 uid = ");
		for(i=0;i<8;i++)
			debug_print(" %02X",uid[i]);
		debug_print("\n");
	}

	#endif
	if(status!=MI_OK) 
		return MI_NOTAGERR;

	status = RC663_SelectV(uid);
	#ifdef UART_PRINT
	debug_print("Select: %d\n",status);
	#endif
	if(status!=MI_OK) return MI_NOTAGERR;

	status = RC663_ReadSingleBlockV(3,dat,&len);
	#ifdef UART_PRINT
	debug_print("ReadBlock: %d_",status);
	if(status==MI_OK)
	{
		for(i=0;i<len;i++)
			debug_print(" %02X",dat[i]);
	}
	debug_print("\n");
	#endif
	if(status!=MI_OK) return MI_NOTAGERR;

	return MI_OK;
}

/*==================================================================================
* 函 数 名： rc663_read_card_block
* 参    数： None
* 功能描述:  RC663获取卡片信息
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-08-04 170658
==================================================================================*/
_Tag_Info pTag;//卡信息结构体
_Tag_Info rc663_read_card_block(void)
{
	s8 status;
	u8 i;
	u8 dat[16],len;	
	uint8_t uid[TAG_UID_LENS];
	//_Tag_Info pTag;//卡信息结构体
	
	RC663_PcdConfigISOType('V');
	delay_ms(5);		
	//////////////Type V/ISO15693
	status = RC663_PcdInventoryV(uid);
	if(status==MI_OK)
	{
//		debug_print("寻到卡 uid = ");
//		for(i=0;i<8;i++)
//			debug_print(" %02X",uid[i]);
//		debug_print("\n");
		
		memcpy(pTag.uid, uid, TAG_UID_LENS);
		if(pTag.tag_state == NULL_STA)
		{
			LED_1;//打开指示灯
			pTag.tag_state = EXIST_STA;//有标签存在
			debug_print("寻到卡 uid = ");
			for(i=0;i<8;i++)
				debug_print(" %02X",uid[i]);
			debug_print("\n");
		}
	}
	if(status!=MI_OK) 
	{
		if(pTag.tag_state)//前面已经寻到卡，卡片移走时多次判断
		{
			for(i=0; i<3; i++)
			{
				RC663_PcdConfigISOType('V');
				delay_ms(5);		
				status = RC663_PcdInventoryV(uid);
				if(status==MI_OK)
				{
					break;
				}
			}
			if(status!=MI_OK )//多次寻卡仍未寻到卡，上报卡片移开
			{
				LED_0;//关闭指示灯
				pTag.tag_state = NULL_STA;//无标签
				memcpy(pTag.uid, "\x00\x00\x00\x00\x00\x00\x00\x00", TAG_UID_LENS);
				debug_print("卡片移开 \r\n");
			}
		} else {
			pTag.tag_state = NULL_STA;//无标签
		}
		return pTag;
	}
	
	status = RC663_SelectV(uid);
	//debug_print("Select: %d\n",status);
	if(status!=MI_OK)
	{
		debug_print("RC663_SelectV: %d\n",status);
		//pTag.tag_state = NULL_STA;//无标签
		return pTag;
	}		
	//读取美餐标识
	status = RC663_ReadSingleBlockV(2,dat,&len);
	//debug_print("ReadBlock: %d_",status);
	if(status==MI_OK)
	{
//		for(i=0;i<len;i++)
//			debug_print(" %02X",dat[i]);
//		debug_print("\n");
//		memcpy(pTag.block,dat,len);//复制扇区数据
		
		if(pTag.tag_state != ENTRY_BLOCK_OK_STA)
		{
			debug_print("读取block数据 = ");
			for(i=0;i<len;i++)
				debug_print(" %02X",dat[i]);
			debug_print("\n");
			if(dat[0] != MEICAN_CODE)
			{
				if(pTag.tag_state == EXIST_STA)
				{
					//上报读卡信息到V3S
					clear_weigh_flag();//重量清零
				}
				pTag.tag_state = ERR_CARD_STA;//非美餐标识卡
			
				return pTag;
			}
			//memcpy(pTag.block,dat,len);//复制扇区数据
			pTag.tag_state = ENTRY_BLOCK_OK_STA;//block 数据读取成功
			//上报读卡信息到V3S
			clear_weigh_flag();//重量清零
		}
	}
	if(status!=MI_OK)
	{
		debug_print("RC663_ReadSingleBlockV: %d\n",status);
		//pTag.tag_state = NULL_STA;//无标签
		return pTag;
	}		
	return pTag;
}
