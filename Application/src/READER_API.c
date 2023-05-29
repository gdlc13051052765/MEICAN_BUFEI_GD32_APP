#include "string.h"
//#include "DEVICE_CFG.h"
#include "FM176XX_REG.h"
#include "READER_API.h"
#include "FM176XX.h"
#include "delay.h"
#include "define.h"
#include "CPU_CARD.h"
#include "application.h"
//#include "p485_fifo.h"

//定义美餐标识
#define MEICAN_CODE  215

struct picc_a_struct PICC_A; 
struct picc_b_struct PICC_B;
struct picc_v_struct PICC_V;

//卡信息结构体//卡信息结构体

unsigned char SetRf(FunctionalState state)
{
	unsigned char result;
		if(state == ENABLE)
		{
	  result = ModifyReg(REG_COMMAND,BIT_MODEMOFF,DISABLE);
		result = ModifyReg(REG_TXMODE,BIT_TXEN,ENABLE);
		}
		else
		{
	  result = ModifyReg(REG_COMMAND,BIT_MODEMOFF,ENABLE);
		result = ModifyReg(REG_TXMODE,BIT_TXEN,DISABLE);			
		}
		mDelay(5);
		return result;
}


/*********************************************************************************************************
** 函数名称:	SetTAPPS
** 函数功能:	设置TypeA卡pps
** 输入参数:    pps1
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
void SetTAPPS(unsigned char pps1)
{
	unsigned char p_rx=0,p_tx=0;	

	//DSI 接收bps
	switch(pps1&0x0C)
	{
	case 0x00:		//106
		p_rx = 0x00;//Rx-iso14443a-106k-manchester
		break;

	case 0x04:		//212
		p_rx = 0x01;//Rx-iso14443a-212k-bpsk
		break;

	case 0x08:		//424
		p_rx = 0x02;//Rx-iso14443a-424k-bpsk
		break;

	case 0x0C:		//848
		p_rx = 0x03;//Rx-iso14443a-848k-bpsk
		break;

	default:
		break;
	}
	
    //DRI发送pps
	switch(pps1&0x03)
	{
	case 0x00:		//106
        p_tx = 0x00;//Tx-iso14443a-106k-miller
		break;

	case 0x01:		//212
		p_tx = 0x01;//Tx-iso14443a-212k-miller
		break;

	case 0x02:		//424
		p_tx = 0x02;//Tx-iso14443a-424k-miller
		break;

	case 0x03:		//848
		p_tx = 0x03;//Tx-iso14443a-848k-miller
		break;

	default:
		break;
	}
	LoadProtocol(p_rx,p_tx);	
	SetReg(REG_THNSET,0xD0);
	SetReg(REG_THNMIN,0xC0);
	ModifyReg(REG_TXAMP,0xC0,DISABLE);
	ModifyReg(REG_TXAMP,0x40,ENABLE);
}


/*********************************************************************************************************
** 函数名称:	SetTBPPS
** 函数功能:	设置TypeB卡pps
** 输入参数:    pps1
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
void SetTBPPS(unsigned char pps1)
{
	unsigned char p_rx=0,p_tx=0;

	//DSI 接收bps
	switch(pps1&0x0C)
	{
	case 0x00:		//106
		p_rx = 0x04;	//Rx-iso14443b-106k-bpsk
		break;

	case 0x04:		//212
		p_rx = 0x05;	//Rx-iso14443b-212k-bpsk
		break;

	case 0x08:		//424
		p_rx = 0x06;	//Rx-iso14443b-424k-bpsk
		break;

	case 0x0C:		//848
		p_rx = 0x07;	//Rx-iso14443b-848k-bpsk
		break;

	default:
		break;
	}
	
    //DRI发送pps
	switch(pps1&0x03)
	{
	case 0x00:		//106
		p_tx = 0x04;	//Tx-iso14443b-106k-nrz
		break;

	case 0x01:		//212
		p_tx = 0x05;	//Tx-iso14443b-212k-nrz
		break;

	case 0x02:		//424
		p_tx = 0x06;	//Tx-iso14443b-424k-nrz
		break;

	case 0x03:		//848
		p_tx = 0x07;	//Tx-iso14443b-848k-nrz
		break;

	default:
		break;
	}
	LoadProtocol(p_rx,p_tx);	
	SetReg(REG_TXAMP,0x4E);		//调制深度11.7%
}

void SetCRC(crc_type type)		// 0 CRC_NONE ; 1 CRC_TX; 2 CRC_RX ;3 CRC_TX_RX;
{
    if(type == CRC_TX)
    {
//        SetReg(REG_TXCRCCON, 0x18);		//0x6363,CRC16
//        SetReg(REG_RXCRCCON, 0x18);

        ModifyReg(REG_TXCRCCON, BIT_CRCEN,ENABLE);
        ModifyReg(REG_RXCRCCON, BIT_CRCEN,DISABLE);
    }
    else if(type == CRC_RX)		//
    {
//        SetReg(REG_TXCRCCON, 0x7B);		//0xFFFF,crc16,TxCRCInvert
//        SetReg(REG_RXCRCCON, 0x7B);

        ModifyReg(REG_TXCRCCON, BIT_CRCEN,DISABLE);
        ModifyReg(REG_RXCRCCON, BIT_CRCEN,ENABLE);
    }
	else if(type == CRC_TX_RX)
	{
//        SetReg(REG_TXCRCCON, 0x18);		//0x6363,CRC16
//        SetReg(REG_RXCRCCON, 0x18);
		
				ModifyReg(REG_TXCRCCON, BIT_CRCEN,ENABLE);
				ModifyReg(REG_RXCRCCON, BIT_CRCEN,ENABLE);
				
//				ModifyReg(REG_RXCRCCON, BIT_RXFORCECRCWRITE|BIT_CRCEN,DISABLE);	
//				ModifyReg(REG_RXCTRL,BIT_RXALLOWBITS,ENABLE);
	}
    else//CRC_NONE
    {
        ModifyReg(REG_TXCRCCON, BIT_CRCEN,DISABLE);
        ModifyReg(REG_RXCRCCON, BIT_CRCEN,DISABLE);
    }
	return;
}


void SetParity(FunctionalState state)
{
	ModifyReg(REG_FRAMECON,BIT_TXPARITYEN|BIT_RXPARITYEN,state);
}


void SetTimer(unsigned long cycles)	//
{
    unsigned long prescale = 1;
    unsigned long t,fc;
		fc = cycles;
    t = fc;

    while(fc > 65535)
    {
        prescale*=2;
        fc = t/prescale;
        if(fc*prescale != t)
            fc++;
    }
	
	if(prescale>1)
	{
		SetReg(REG_T0CONTROL, BIT_TSTOP_RX | BIT_TSTART_TX | BIT_TAUTORESTARTED | VALUE_TCLK_1356_MHZ );
		SetReg(REG_T0RELOADHI,(uint8_t)(prescale>>8));
		SetReg(REG_T0RELOADLO,(uint8_t)prescale);	

		SetReg(REG_T1CONTROL, BIT_TSTOP_RX | BIT_TSTART_TX | VALUE_TCLK_T0 );
		SetReg(REG_T1RELOADHI,(uint8_t)(fc>>8));
		SetReg(REG_T1RELOADLO,(uint8_t)fc);	
	}
	else
	{		
		SetReg(REG_T1CONTROL, BIT_TSTOP_RX | BIT_TSTART_TX | VALUE_TCLK_1356_MHZ );
		SetReg(REG_T1RELOADHI,(uint8_t)(fc>>8));
		SetReg(REG_T1RELOADLO,(uint8_t)fc);	
	}
	
}
/****************************************************************/
/*名称: Set_Baud_Rate																						*/
/*功能: 调整通信波特率																					*/
/*输入:	baudrate BIT0~BIT1 = TX, BIT2~BIT3 = RX									*/
/*	0:106Kbps;1:212Kpbs;2:424Kpbs;3:848Kpbs			*/														
/* 		FM175XX_SUCCESS: 应答正确											*/
/****************************************************************/
//unsigned char SetBaudRate(unsigned char baudrate) //baudrate = 0:106Kbps;1:212Kpbs;2:424Kpbs;3:848Kpbs
//{
//	ModifyReg(JREG_TXMODE,BIT4|BIT5|BIT6,DISABLE);
//	ModifyReg(JREG_RXMODE,BIT4|BIT5|BIT6,DISABLE);

//  switch (baudrate & (BIT0 | BIT1))//TX
//  {
//  case 0:	  //106kbps 	
//    SetReg(JREG_MODWIDTH,0x26);		
//		break;
//  case BIT0:	  //212kbps
//		ModifyReg(JREG_TXMODE,BIT4,ENABLE);
//	  SetReg(JREG_MODWIDTH,0x13);	 
//		break;
//  case BIT1:	  //424kbps
//		ModifyReg(JREG_TXMODE,BIT5,ENABLE);		
//    SetReg(JREG_MODWIDTH,0x09);	 
//		break;
//  case (BIT0|BIT1):	  //848kbps
//		ModifyReg(JREG_TXMODE,BIT4|BIT5,ENABLE);
//		SetReg(JREG_MODWIDTH,0x04);	 
//		break;
//  }
//	 switch (baudrate & (BIT2 | BIT3))//RX
//  {
//  case 0:	  //106kbps  
//		SetReg(JREG_MANUALRCV,0x00);//HPFC	
//		break;
//  case BIT2:	  //212kbps
//		ModifyReg(JREG_RXMODE,BIT4,ENABLE);   
//	  SetReg(JREG_MANUALRCV,0x01);//HPFC	
//		break;
//  case BIT3:	  //424kbps		
//		ModifyReg(JREG_RXMODE,BIT5,ENABLE);  
//	  SetReg(JREG_MANUALRCV,0x02);//HPFC	
//		break;
//  case (BIT2|BIT3):	  //848kbps	
//		ModifyReg(JREG_RXMODE,BIT4|BIT5,ENABLE);   
//	  SetReg(JREG_MANUALRCV,0x03);//HPFC	
//		break;
//  }	
//	
//return FM175XX_SUCCESS;
//}


//unsigned char SetTimeOut(unsigned int microseconds)
//{
//	unsigned long  timereload;
//	unsigned int  prescaler;

//	prescaler = 0;
//	timereload = 0;
//	while(prescaler < 0xfff)
//	{
//		timereload = ((microseconds*(long)13560)-1)/(prescaler*2+1);
//		
//		if( timereload<0xffff)
//			break;
//		prescaler++;
//	}
//		timereload = timereload & 0xFFFF;
//		SetReg(JREG_TMODE,(prescaler >> 8) | BIT7);
//		SetReg(JREG_TPRESCALER,prescaler & 0xFF);					
//		SetReg(JREG_TRELOADHI,timereload >> 8);
//		SetReg(JREG_TRELOADLO,timereload & 0xFF);
//    return FM175XX_SUCCESS;
//}

//******************************************************************************/
//函数名：Command_Execute
//入口参数：
//			unsigned char* sendbuf:发送数据缓冲区	unsigned char sendlen:发送数据长度
//			unsigned char* recvbuf:接收数据缓冲区	unsigned char* recvlen:接收到的数据长度
//出口参数
/******************************************************************************/
unsigned char Command_Execute(command_struct *comm_status)
{
	unsigned char reg_data;
	unsigned char irq0,irq1;
	unsigned char result;

	comm_status->nBitsReceived = 0;
	comm_status->nBytesReceived = 0;
	comm_status->CollPos = 0;
	comm_status->Error = 0;

	FM176XX_SetCommand(CMD_IDLE);	
	Clear_FIFO();	
	
	ModifyReg(REG_TXDATANUM,BIT2|BIT1|BIT0,DISABLE);		
	ModifyReg(REG_TXDATANUM,comm_status->nBitsToSend,ENABLE);

	ModifyReg(REG_RXBITCTRL,BIT6|BIT5|BIT4,DISABLE);		
	ModifyReg(REG_RXBITCTRL,comm_status->nBitsToReceive,ENABLE);				
	SetCRC(comm_status->PCRC);	
	SetTimer(comm_status->Cycles);//
	SetReg(REG_IRQ0,0x7F);
	SetReg(REG_IRQ1, BIT_TIMER0IRQ | BIT_TIMER1IRQ);	//清T0,T1的irq			
	
	SPI_Write_FIFO(comm_status->nBytesToSend,comm_status->pSendBuf);		
	FM176XX_SetCommand(comm_status->Cmd);		
	
	while(1)
		{			
				GetReg(REG_IRQ0,&irq0);
				GetReg(REG_IRQ1,&irq1);
				if(irq1 & BIT_TIMER1IRQ)//Timer1IRq = 1
				{
					result = FM176XX_TIMEROUT_ERR;		
					break;
				}
				
				if(irq0 & BIT_ERRIRQ)//ERRIRq = 1
				{		
				
//					do
//					{
//						GetReg(REG_STATUS,&reg_data);	
//					}						
//					while((reg_data & 0x07) == 0x07);//如果正在接收数据，等待数据接收完成
					
					GetReg(REG_ERROR,&reg_data);	
					comm_status->Error = reg_data;

					GetReg(REG_RXBITCTRL,&reg_data);
					comm_status->nBitsReceived = reg_data & 0x07;//接收位长度				
					GetReg(REG_FIFOLENGTH,&reg_data);
					comm_status->nBytesReceived = reg_data & 0x7F;//接收字节长度														
					SPI_Read_FIFO(comm_status->nBytesReceived,comm_status->pReceiveBuf);
					
					if(comm_status->Error & BIT7)
					{
						result = FM176XX_EEPROM_ERR;
						break;	
					}
					
					if(comm_status->Error & BIT6)
					{
						result = FM176XX_FIFO_ERR;
						break;	
					}
					
					if(comm_status->Error & BIT2)
					{
						GetReg(REG_RXCOLL,&reg_data);	
						if((reg_data & BIT7)!=BIT7)
							comm_status->CollPos = reg_data & 0x1F;//读取冲突位置
						else
							comm_status->CollPos = 0;
						result = FM176XX_COLL_ERR;
						break;	
					}
										
					if(comm_status->Error & BIT1)
					{
						result = FM176XX_PROTOCOL_ERR;
						break;	
					}
					
					if(comm_status->Error & BIT0)
					{
						result = FM176XX_CRC_PARITY_ERR;
						break;	
					}			

				}							
				
				if(irq0 & BIT_IDLEIRQ)//IdleTRq = 1
				{
					//SetReg(JREG_COMMIRQ,BIT4);			
					result = FM176XX_SUCCESS;
					break;	
				}				
				
				if(irq0 & BIT_RXIRQ)//RxIRq = 1
				{				
					//SetReg(JREG_COMMIRQ,BIT5);			
					result = FM176XX_SUCCESS;					
					break;						
				}
				
				if(irq0 & BIT_TXIRQ)//TxIRq = 1
				{
					if(comm_status->Cmd == CMD_TRANSMIT)
						{
						result = FM176XX_SUCCESS;
						break;					
						}	
			
				}
				
		}
		
		if((comm_status->Cmd == CMD_TRANSCEIVE)||(comm_status->Cmd == CMD_RECEIVE))
		{
					GetReg(REG_RXBITCTRL,&reg_data);
					comm_status->nBitsReceived = reg_data & 0x07;//接收位长度				
					GetReg(REG_FIFOLENGTH,&reg_data);
					comm_status->nBytesReceived = reg_data & 0x7F;//接收字节长度														
					SPI_Read_FIFO(comm_status->nBytesReceived,comm_status->pReceiveBuf);					
					if((comm_status->nBytesToReceive != comm_status->nBytesReceived)&&(comm_status->nBytesToReceive != 0))
					{
					result = FM176XX_COMM_ERR;//接收到的数据长度错误
					}			
		}							

	FM176XX_SetCommand(CMD_IDLE);
	return result;
}

unsigned char FM176XX_Initial_ReaderA(void)
{
	LoadProtocol(RX_TYPEA_106,TX_TYPEA_106);
	//SetReg(REG_THNSET,0xD0);
	//SetReg(REG_THNMIN,0xC0);
	//ModifyReg(REG_TXCON,BIT_CWMAX,DISABLE);		//修改tvdd
	//ModifyReg(REG_TXAMP,0xC0,DISABLE);
	SetReg(REG_TXAMP,AMPLITUDE_A<<6);
	ModifyReg(REG_RXTXCON,BIT_SHMODE,ENABLE);
	ModifyReg(REG_RXANA,BIT3|BIT2|BIT1|BIT0,DISABLE);
	ModifyReg(REG_RXANA,(HPCF_A<<2)|GAIN_A,ENABLE);//39h
	SetParity(ENABLE);
	//SetReg(REG_TXCRCCON, 0x18);		//0x6363,CRC16
  //SetReg(REG_RXCRCCON, 0x18);
	return FM176XX_SUCCESS;
}


unsigned char FM176XX_Initial_ReaderB(void)
{
	LoadProtocol(RX_TYPEB_106,TX_TYPEB_106);

	ModifyReg(REG_RXANA,BIT3|BIT2|BIT1|BIT0,DISABLE);
	ModifyReg(REG_RXANA,(HPCF_B<<2)|GAIN_B,ENABLE);//39h
	SetParity(DISABLE);
	SetReg(REG_TXAMP,(AMPLITUDE_B<<6)|MODULATION);				//调制深度11.7%
	return FM176XX_SUCCESS;
}

unsigned char FM176XX_Initial_ReaderV(void)
{
	//unsigned char reg_data;
	LoadProtocol(RX_TYPEV_26,RX_TYPEV_26);

	ModifyReg(REG_RXANA,BIT3|BIT2|BIT1|BIT0,DISABLE);
	ModifyReg(REG_RXANA,(HPCF_V<<2)|GAIN_V,ENABLE);//39h
	SetParity(DISABLE);
	SetReg(REG_TXAMP,AMPLITUDE_V<<5);	
	SetReg(REG_TXCON,0x01);
	//SetReg(REG_TXI,0x06);
  SetReg(REG_THNADJ,0x08);
	SetReg(REG_THNSET,0xFF);
	SetReg(REG_THNMIN,0x80);
//  GetReg(REG_THNMIN,&reg_data);
//	Uart_Send_Msg(">REG_RXCFG = ");Uart_Send_Hex(&reg_data,1);Uart_Send_Msg("\r\n");
  SetReg(REG_RXBITCTRL,0x08);
//  GetReg(REG_RXBITCTRL,&reg_data);
//	Uart_Send_Msg(">REG_RXBITCTRL = ");Uart_Send_Hex(&reg_data,1);Uart_Send_Msg("\r\n");
	return FM176XX_SUCCESS;
}


unsigned char ReaderV_Inventory(struct picc_v_struct *picc_v)
{
	unsigned char result;
	unsigned char outbuf[3],inbuf[10];
	
	memset(inbuf,0x55,10);
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
  command_status.PCRC = CRC_TX_RX;
	command_status.pSendBuf[0] = 0x26;   //
	command_status.pSendBuf[1] = 0x01; 
	command_status.pSendBuf[2] = 0x00; 
	command_status.nBytesToSend = 3;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 10;
	command_status.Cycles = 13560;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if(result == FM176XX_SUCCESS)
	{
		memcpy(picc_v->UID, command_status.pReceiveBuf + 2,8);	
//		for(i=0;i<8;i++)
//		debug_print(" %02X",command_status.pReceiveBuf[i+2]);
//		debug_print("\n");
	}			
	return result;
}

unsigned char ReaderV_Select(struct picc_v_struct *picc_v,unsigned char *response)
{
	unsigned char result;
	unsigned char outbuf[10],inbuf[2];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
  command_status.PCRC = CRC_TX_RX;
	command_status.pSendBuf[0] = 0x22;   //
	command_status.pSendBuf[1] = 0x25; 
	memcpy(command_status.pSendBuf + 2,picc_v->UID,8); 
//command_status.pSendBuf[2] = 0x25;
	command_status.nBytesToSend = 10;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 0;
	command_status.Cycles = 135600;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	//if(result == FM176XX_SUCCESS)
	{
		memcpy(response, command_status.pReceiveBuf ,command_status.nBytesReceived);		
	}	
	//Uart_Send_Msg("response sel = ");Uart_Send_Hex(command_status.pReceiveBuf,command_status.nBytesReceived);	Uart_Send_Msg("\r\n");
	return result;
}


unsigned char ReaderV_ReadSingleBlock(unsigned char block_num,unsigned char *response)
{
	unsigned char result;
	unsigned char outbuf[10],inbuf[2];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
  command_status.PCRC = CRC_TX_RX;
	command_status.pSendBuf[0] = 0x12;   //
	command_status.pSendBuf[1] = 0x20; 
	command_status.pSendBuf[2] = block_num; 	
	command_status.nBytesToSend = 3;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 5;
	command_status.Cycles = 13560;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if(result == FM176XX_SUCCESS)
	{
		memcpy(response, command_status.pReceiveBuf ,5);		
	}	
	return result;
}


unsigned char ReaderV_SendSlot(void)
{
	unsigned char outbuf[1],inbuf[1];

	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
	 command_status.PCRC = CRC_NONE;

	command_status.nBytesToSend = 0;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 0;
	command_status.Cycles = 13560;//1ms
	command_status.Cmd = CMD_TRANSMIT;
	
	ModifyReg(REG_TXDATANUM,BIT_DATAEN,DISABLE);
	ModifyReg(REG_FRAMECON,MASK_STARTSYM,DISABLE);
	Command_Execute(&command_status);	
	
	ModifyReg(REG_TXDATANUM,BIT_DATAEN,ENABLE);
	ModifyReg(REG_FRAMECON,MASK_STARTSYM,ENABLE);
	return FM176XX_SUCCESS;
}


unsigned char ReaderV_WriteSingleBlock(unsigned char block_num,unsigned char *block_data,unsigned char *response)
{
	unsigned char result;
	unsigned char outbuf[10],inbuf[2];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
  command_status.PCRC = CRC_TX_RX;
	command_status.pSendBuf[0] = 0x02;   //
	command_status.pSendBuf[1] = 0x21; 	
	command_status.pSendBuf[2] = block_num; 	
	command_status.pSendBuf[3] = block_data[0];
	command_status.pSendBuf[4] = block_data[1];
	command_status.pSendBuf[5] = block_data[2];
	command_status.pSendBuf[6] = block_data[3];
	command_status.nBytesToSend = 7;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 2;
	command_status.Cycles = 135600;//10ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if(result == FM176XX_SUCCESS)
	{
		memcpy(response, command_status.pReceiveBuf ,2);		
	}	
	return result;
}

//unsigned char ReaderB_Setting(unsigned char *value)
//{
//	SetReg(JREG_TYPEB,*value);
//	SetReg(JREG_GSN,*(value + 1));
//	SetReg(JREG_CWGSP,*(value + 2));
//	SetReg(JREG_MODGSP,*(value + 3));	
//	return FM175XX_SUCCESS;
//}

unsigned char ReaderA_Halt(void)
{
	unsigned char result;
	unsigned char outbuf[2],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;	
  command_status.PCRC = CRC_TX;
	command_status.pSendBuf[0] = 0x50;
  command_status.pSendBuf[1] = 0x00;
  command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 0;
	command_status.Cycles = 135600;//
	command_status.Cmd = CMD_TRANSMIT;	
  result = Command_Execute(&command_status);
  return result;
}

unsigned char ReaderA_Wakeup(struct picc_a_struct *picc_a)
{
	unsigned char result;
	unsigned char outbuf[1],inbuf[2];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;	
  command_status.PCRC = CRC_NONE;
	command_status.pSendBuf[0] = RF_CMD_WUPA;   //
	command_status.nBytesToSend = 1;//
	command_status.nBitsToSend = 7;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 2;
	command_status.Cycles = 135600;//
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if(command_status.nBytesReceived == 2)
	{
		memcpy(picc_a->ATQA,command_status.pReceiveBuf,2);		
	}
	else
		result =  FM176XX_COMM_ERR;
	return result;
}

unsigned char ReaderA_Request(struct picc_a_struct *picc_a)
{
	unsigned char result;
	unsigned char outbuf[1],inbuf[2];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
  command_status.PCRC = CRC_NONE;
	command_status.pSendBuf[0] = RF_CMD_REQA;   //
	command_status.nBytesToSend = 1;//
	command_status.nBitsToSend = 7;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 2;
	command_status.Cycles = 135600;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if(command_status.nBytesReceived == 2)
	{
		memcpy(picc_a->ATQA, command_status.pReceiveBuf,2);		
	}
	else
		result =  FM176XX_COMM_ERR;
	return result;
}

//*************************************
//函数  名：ReaderA_AntiColl
//入口参数：size:防冲突等级，包括0、1、2
//出口参数：unsigned char:0:OK  others：错误
//*************************************
unsigned char ReaderA_AntiColl(struct picc_a_struct *picc_a)
{
	unsigned char result;
	unsigned char outbuf[2],inbuf[5];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;	
  command_status.PCRC = CRC_NONE;
	if(picc_a->CASCADE_LEVEL > 2)
		return FM176XX_PARAM_ERR;
	command_status.pSendBuf[0] = RF_CMD_ANTICOL[picc_a->CASCADE_LEVEL];   //防冲突等级
	command_status.pSendBuf[1] = 0x20;
	command_status.nBytesToSend = 2;						//发送长度：2
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 5;
	command_status.Cycles = 135600;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);

	if((result == FM176XX_SUCCESS)&&(command_status.nBytesReceived == 5))
	{
		memcpy(picc_a->UID +(picc_a->CASCADE_LEVEL*4),command_status.pReceiveBuf,4);
		memcpy(picc_a->BCC + picc_a->CASCADE_LEVEL,command_status.pReceiveBuf + 4,1);
		if((picc_a->UID[picc_a->CASCADE_LEVEL*4] ^  picc_a->UID[picc_a->CASCADE_LEVEL*4 + 1] ^ picc_a->UID[picc_a->CASCADE_LEVEL*4 + 2]^ picc_a->UID[picc_a->CASCADE_LEVEL*4 + 3] ^ picc_a->BCC[picc_a->CASCADE_LEVEL]) !=0)
				result = FM176XX_COMM_ERR;
	}
	return result;
}

//*************************************
//函数  名：ReaderA_Select
//入口参数：size:防冲突等级，包括0、1、2
//出口参数：unsigned char:0:OK  others：错误
//*************************************
unsigned char ReaderA_Select(struct picc_a_struct *picc_a)
{
	unsigned char result;
	unsigned char outbuf[7],inbuf[2];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;	
  command_status.PCRC = CRC_TX;
	if(picc_a->CASCADE_LEVEL > 2)							 //最多三重防冲突
		return FM176XX_PARAM_ERR;
	*(command_status.pSendBuf+0) = RF_CMD_ANTICOL[picc_a->CASCADE_LEVEL];   //防冲突等级
	*(command_status.pSendBuf+1) = 0x70;   					//
	*(command_status.pSendBuf+2) = picc_a->UID[4*picc_a->CASCADE_LEVEL];
	*(command_status.pSendBuf+3) = picc_a->UID[4*picc_a->CASCADE_LEVEL+1];
	*(command_status.pSendBuf+4) = picc_a->UID[4*picc_a->CASCADE_LEVEL+2];
	*(command_status.pSendBuf+5) = picc_a->UID[4*picc_a->CASCADE_LEVEL+3];
	*(command_status.pSendBuf+6) = picc_a->BCC[picc_a->CASCADE_LEVEL];
	command_status.nBytesToSend = 7;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
	command_status.Cycles = 135600;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if((result == FM176XX_SUCCESS)&&(command_status.nBytesReceived == 2))
		picc_a->SAK [picc_a->CASCADE_LEVEL] = *(command_status.pReceiveBuf);
	else
		result = FM176XX_COMM_ERR;
	return result;
}




unsigned char ReaderA_CardActivate(struct picc_a_struct *picc_a)
{
unsigned char  result,cascade_level;	

		result = ReaderA_Request(picc_a);//
		if (result != FM176XX_SUCCESS)
			return FM176XX_COMM_ERR;
				
			if 	((picc_a->ATQA[0]&0xC0)==0x00)	//1重UID
			{
				cascade_level = 1;
				picc_a->UID_Length = 4;			
			}
			if 	((picc_a->ATQA[0]&0xC0)==0x40)	//2重UID		
			{
				cascade_level = 2;
				picc_a->UID_Length = 8;				
			}
			if 	((picc_a->ATQA[0]&0xC0)==0x80)	//3重UID
			{
				cascade_level = 3;	
				picc_a->UID_Length = 12;
			}
			for (picc_a->CASCADE_LEVEL = 0; picc_a->CASCADE_LEVEL < cascade_level; picc_a->CASCADE_LEVEL++)
				{
					result = ReaderA_AntiColl(picc_a);//
					if (result != FM176XX_SUCCESS)
						return FM176XX_COMM_ERR;
					
					result = ReaderA_Select(picc_a);//
					if (result != FM176XX_SUCCESS)
						return FM176XX_COMM_ERR;
				}						
			picc_a->CASCADE_LEVEL--;
		return result;
}

unsigned char ReaderB_Wakeup(struct picc_b_struct *picc_b)
{
	unsigned char result;
	unsigned char outbuf[3],inbuf[12];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;	
  command_status.PCRC = CRC_TX_RX;
	command_status.pSendBuf[0] = 0x05;  //APf
	command_status.pSendBuf[1] = 0x00;   //AFI (00:for all cards)
	command_status.pSendBuf[2] = 0x08;   //PARAM(REQB,Number of slots =0)
	command_status.nBytesToSend = 3;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 12;
	command_status.Cycles = 135600;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if((result == FM176XX_SUCCESS)&&(command_status.nBytesReceived == 12))
	{
		memcpy(picc_b->ATQB, command_status.pReceiveBuf,12);	
		memcpy(picc_b->PUPI,picc_b->ATQB + 1,4);
		memcpy(picc_b->APPLICATION_DATA,picc_b->ATQB + 6,4);
		memcpy(picc_b->PROTOCOL_INF,picc_b->ATQB + 10,3);
	}
	else
		result = FM176XX_COMM_ERR;
	return result;
}

unsigned char ReaderB_Request(struct picc_b_struct *picc_b)
{
	unsigned char result;
	unsigned char outbuf[3],inbuf[12];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
  command_status.PCRC = CRC_TX_RX;
	command_status.pSendBuf[0] = 0x05;  //APf
	command_status.pSendBuf[1] = 0x00;   //AFI (00:for all cards)
	command_status.pSendBuf[2] = 0x00;   //PARAM(REQB,Number of slots =0)
 
	command_status.nBytesToSend = 3;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 12;
	command_status.Cycles = 135600;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if((result == FM176XX_SUCCESS)&&(command_status.nBytesReceived == 12))
	{
		memcpy(picc_b->ATQB, command_status.pReceiveBuf,12);	
		memcpy(picc_b->PUPI,picc_b->ATQB + 1,4);
		memcpy(picc_b->APPLICATION_DATA,picc_b->ATQB + 6,4);
		memcpy(picc_b->PROTOCOL_INF,picc_b->ATQB + 10,3);
	}
	else
		result = FM176XX_COMM_ERR;
	return result;
}

unsigned char ReaderB_Attrib(unsigned char *param,struct picc_b_struct *picc_b)
{
	unsigned char result;
	unsigned char outbuf[9],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
  command_status.PCRC = CRC_TX_RX;
	command_status.pSendBuf[0] = 0x1D;  //
	command_status.pSendBuf[1] = picc_b->PUPI[0];   //
	command_status.pSendBuf[2] = picc_b->PUPI[1];   //
	command_status.pSendBuf[3] = picc_b->PUPI[2];   //
	command_status.pSendBuf[4] = picc_b->PUPI[3];   //	
	command_status.pSendBuf[5] = 0x00;  //Param1
	command_status.pSendBuf[6] = *param;  //Param2  BIT0~BIT3 Frame Size 0 = 16, 1 = 24, 2 = 32, 3 = 40, 4 = 48, 5 = 64, 6 = 96, 7 = 128, 8 = 256
	//Param2 BIT4~BIT5 TX BaudRate BIT6~BIT7 RX BaudRate,00 = 106Kbps, 01 = 212Kbps, 10 = 424Kbps, 11 = 848Kbps
	command_status.pSendBuf[7] = 0x01;  //COMPATIBLE WITH 14443-4
	command_status.pSendBuf[8] = *(param + 1);  //CID:01 
	command_status.nBytesToSend = 9;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 0;
	command_status.Cycles = 1356000;//10ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if(result == FM176XX_SUCCESS)
	{
		*(picc_b->CID) = command_status.pReceiveBuf[0] & 0x0F;
		//CPU_CARD.CID = *(picc_b->CID);
	}
	else
		result = FM176XX_COMM_ERR;
	return result;
}

unsigned char ReaderB_Halt(struct picc_b_struct *picc_b)
{
	unsigned char result;
	unsigned char outbuf[5],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
  command_status.PCRC = CRC_TX_RX;
	command_status.pSendBuf[0] = 0x50;  //
	command_status.pSendBuf[1] = picc_b->PUPI[0];   //
	command_status.pSendBuf[2] = picc_b->PUPI[1];   //
	command_status.pSendBuf[3] = picc_b->PUPI[2];   //
	command_status.pSendBuf[4] = picc_b->PUPI[3];   //	
	
	command_status.nBytesToSend = 5;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
	command_status.Cycles = 135600;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if(result == FM176XX_SUCCESS)
	{	
		memcpy(picc_b->Answer_to_HALT, command_status.pReceiveBuf,1);		
	}
	else
		result = FM176XX_COMM_ERR;
	return result;
}

unsigned char ReaderB_Get_SN(struct picc_b_struct *picc_b)
{
	unsigned char result;
	unsigned char outbuf[5],inbuf[10];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
  command_status.PCRC = CRC_TX_RX;
	command_status.pSendBuf[0] = 0x00;  //
	command_status.pSendBuf[1] = 0x36;   //
	command_status.pSendBuf[2] = 0x00;   //
	command_status.pSendBuf[3] = 0x00;   //
	command_status.pSendBuf[4] = 0x08;   //	
	
	command_status.nBytesToSend = 5;//
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 10;
	command_status.Cycles = 135600;//1ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if(result == FM176XX_SUCCESS)
	{
		//picc_b->LEN_SN = command_status.nBytesReceived;
		memcpy(picc_b->SN,command_status.pReceiveBuf,8);		
	}
	else
		result = FM176XX_COMM_ERR;
	return result;
}

//unsigned char FM175XX_Polling(unsigned char *polling_card,struct picc_a_struct *picc_a,struct picc_b_struct *picc_b)
//{
//unsigned char result;
//	*polling_card = 0;
//	FM175XX_Initial_ReaderA();
//	result = ReaderA_Wakeup(picc_a);//
//		if (result == FM175XX_SUCCESS)	
//			*polling_card |= BIT0;
//	FM175XX_Initial_ReaderB();
//	result = ReaderB_Wakeup(picc_b);//
//		if (result == FM175XX_SUCCESS)
//			*polling_card |= BIT1;
//if (*polling_card !=0)		
//	return SUCCESS;	
//else
//	return ERROR;
//}

unsigned char FM176XX_TPDU(unsigned char *send_param,unsigned char *receive_param)
{	
	unsigned char result;	
	command_struct command_status;
	command_status.pSendBuf = send_param + 3;
	command_status.pReceiveBuf = receive_param + 2;
	command_status.PCRC = CRC_NONE;
	if(*send_param & BIT4)
		command_status.PCRC |= CRC_TX;//param[0] BIT0 TX CRC
	if(*send_param & BIT5)
		command_status.PCRC |= CRC_RX;//param[0] BIT1 RX CRC
	if(*send_param & BIT6)
			SetParity(ENABLE);	
	else
			SetParity(DISABLE);	

	command_status.nBitsToReceive = ((*(send_param + 1)) & 0x70) >> 4;//param[1] BIT4~BIT6 
	command_status.nBitsToSend = (*(send_param + 1)) & 0x07;//param[1] BIT0~BIT2 
	command_status.nBytesToReceive = 0;//接收长度未知，设置为0
	command_status.nBytesToSend = *(send_param + 2);
	command_status.Cmd = CMD_TRANSCEIVE;
	command_status.Cycles = 1356000;//100ms
	result = Command_Execute(&command_status);
	*receive_param = command_status.nBitsReceived;
	*(receive_param + 1) = command_status.nBytesReceived;
	return result;
}

/*==================================================================================
* 函 数 名： fm17660_read_card_block
* 参    数： None
* 功能描述:  FM17660获取卡片信息
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-08-04 170658
==================================================================================*/
 _Tag_Info fm17660_read_card_block(void)
{
	int i = 0;
	unsigned char result,response[2];
	unsigned char BLOCK_DATA[16];
	_Tag_Info pTag;
	
	//result =-1;
	if(FM176XX_HardReset()== SUCCESS)
	{
		//debug_print("复位成功");
	} else {
		debug_print("复位失败");
	}
	FM176XX_Initial_ReaderV();
	SetRf(ENABLE); 
	result = ReaderV_Inventory(&PICC_V);
	if(result != FM176XX_SUCCESS)
	{
		pTag.tag_state = NULL_STA;//无标签
		memcpy(pTag.uid, "\x00\x00\x00\x00\x00\x00\x00\x00", TAG_UID_LENS);
		return pTag;
	}
	if(pTag.tag_state != EXIST_STA)
	{
		//上报读卡信息到V3S
	//	clear_weigh_flag();//重量清零
		debug_print("检测到卡片入场\r\n");
	}
	
	debug_print("寻到卡 uid = ");
	pTag.tag_state = EXIST_STA;//有标签存在
	memcpy(pTag.uid, PICC_V.UID, TAG_UID_LENS);
	for(i=0;i<8;i++)
		debug_print(" %02X",pTag.uid[i]);
	debug_print("\n");
	
	result = ReaderV_Select(&PICC_V, response);
	if(result != FM176XX_SUCCESS)
	{
		pTag.tag_state = NULL_STA;//无标签
		memcpy(pTag.uid, "\x00\x00\x00\x00\x00\x00\x00\x00", TAG_UID_LENS);
		return pTag;
	}
	
	result = ReaderV_ReadSingleBlock(2, BLOCK_DATA);
	if(result != FM176XX_SUCCESS)
		return pTag;
	else {
		debug_print("black数据 = ");
		for(i=0;i<8;i++)
			debug_print(" %02X",BLOCK_DATA[i]);
		debug_print("\n");
		if(BLOCK_DATA[1] != MEICAN_CODE)
		{
			pTag.tag_state = ERR_CARD_STA;//非美餐标识卡
			return pTag;
		}
		//memcpy(pTag.block,dat,len);//复制扇区数据
		pTag.tag_state = ENTRY_BLOCK_OK_STA;//block 数据读取成功
		//上报读卡信息到V3S
//		clear_weigh_flag();//重量清零
		return pTag;
	}
}
