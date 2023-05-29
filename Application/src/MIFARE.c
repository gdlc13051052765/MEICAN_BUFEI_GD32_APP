#include "MIFARE.h"
#include "FM176XX.h"
#include "Reader_API.h"
#include "string.h"
#include "FM176XX_REG.h"
#include "define.h"
#include "delay.h"
unsigned char SECTOR,BLOCK,BLOCK_NUM;
unsigned char BLOCK_DATA[16];
unsigned char KEY_A[16][6]=
							{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//0
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//2
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//3
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//4
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//5
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//6
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//7
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//10
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//11
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//12
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//13
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//14
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}};//15
							 
unsigned char KEY_B[16][6]=
							{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//0
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//2
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//3
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//4
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//5
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//6
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//7
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//10
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//11
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//12
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//13
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//14
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}};//15
							
/*****************************************************************************************/
/*名称：Mifare_Clear_Crypto																															 */
/*功能：Mifare_Clear_Crypto清除认证标志																 									*/
/*输入：																																								 */
/*																																						 					*/
/*输出:																																									 */
/*																																											 */
/*																																											 */
/*****************************************************************************************/							 
//void Mifare_Clear_Crypto(void)
//{
//	ModifyReg(JREG_STATUS2,BIT3,RESET);
//	return;
//}	
							 
							 
 unsigned char Mifare_LoadKey(unsigned char *mifare_key)
 {
  unsigned char reg_data;
		FM176XX_SetCommand(CMD_IDLE);	
		Clear_FIFO();
		SPI_Write_FIFO(6,mifare_key);
		FM176XX_SetCommand(CMD_LOADKEY);
		mDelay(1);
	  GetReg(REG_COMMAND,&reg_data);
		if((reg_data & CMD_MASK) == CMD_IDLE)
			return FM176XX_SUCCESS;
		else
			return FM176XX_LOADKEY_ERR;
 }	 
							 
/*****************************************************************************************/
/*名称：Mifare_Auth																		 */
/*功能：Mifare_Auth卡片认证																 */
/*输入：mode，认证模式（0：key A认证，1：key B认证）；sector，认证的扇区号（0~15）		 */
/*		*mifare_key，6字节认证密钥数组；*card_uid，4字节卡片UID数组						 */
/*输出:																					 */
/*		OK    :认证成功																	 */
/*		ERROR :认证失败																	 */
/*****************************************************************************************/
 unsigned char Mifare_Auth(unsigned char key_mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid)
{
	unsigned char result,reg_data;
	result = Mifare_LoadKey(mifare_key);
	if (result != FM176XX_SUCCESS)
		return result;
	FM176XX_SetCommand(CMD_IDLE);	
	Clear_FIFO();
	if(key_mode == KEY_A_M1)
	{
		SetReg(REG_FIFODATA,RF_CMD_KEYA);//60 keyA M1认证指令
		ModifyReg(REG_RXTXCON,BIT_SHMODE,DISABLE);
	}
	if(key_mode == KEY_B_M1)
	{
		SetReg(REG_FIFODATA,RF_CMD_KEYA);//61 keyB M1认证指令
		ModifyReg(REG_RXTXCON,BIT_SHMODE,DISABLE);
	}
	if(key_mode == KEY_A_SH)
	{
		SetReg(REG_FIFODATA,RF_CMD_KEYA);//60 keyA SH认证指令
		ModifyReg(REG_RXTXCON,BIT_SHMODE,ENABLE);
	}
	if(key_mode == KEY_B_SH)
	{
		SetReg(REG_FIFODATA,RF_CMD_KEYA);//61 keyB SH认证指令
		ModifyReg(REG_RXTXCON,BIT_SHMODE,ENABLE);
	}		
	
	SetReg(REG_FIFODATA,sector * 4);//认证扇区的块0地址
	SPI_Write_FIFO(4,card_uid);
	FM176XX_SetCommand(CMD_AUTHENT);
	mDelay(5);
	GetReg(REG_COMMAND,&reg_data);
	if((reg_data & CMD_MASK) == CMD_IDLE)
		{
		GetReg(REG_STATUS,&reg_data);
		if(reg_data & BIT_CRYPTO1ON)//判断加密标志位，确认认证结果
			return FM176XX_SUCCESS;			
		}	
	return FM176XX_AUTH_ERR;
}
/*****************************************************************************************/
/*名称：Mifare_Blockset									 */
/*功能：Mifare_Blockset卡片数值设置							 */
/*输入：block，块号；*buff，需要设置的4字节数值数组					 */
/*											 */
/*输出:											 */
/*		OK    :设置成功								 */
/*		ERROR :设置失败								 */
/*****************************************************************************************/
 unsigned char Mifare_Blockset(unsigned char block,unsigned char *data_buff)
 {
  unsigned char block_data[16],result;
	block_data[0] = data_buff[3];
	block_data[1] = data_buff[2];
	block_data[2] = data_buff[1];
	block_data[3] = data_buff[0];
	block_data[4] = ~data_buff[3];
	block_data[5] = ~data_buff[2];
	block_data[6] = ~data_buff[1];
	block_data[7] = ~data_buff[0];
  block_data[8] = data_buff[3];
	block_data[9] = data_buff[2];
	block_data[10] = data_buff[1];
	block_data[11] = data_buff[0];
	block_data[12] = block;
	block_data[13] = ~block;
	block_data[14] = block;
	block_data[15] = ~block;
  result = Mifare_Blockwrite(block,block_data);
  return result;
 }
/*****************************************************************************************/
/*名称：Mifare_Blockread																 */
/*功能：Mifare_Blockread卡片读块操作													 */
/*输入：block，块号（0x00~0x3F）；buff，16字节读块数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockread(unsigned char block,unsigned char *data_buff)
{	
	uint8_t result;
	uint8_t outbuf[2],inbuf[16];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
	
  command_status.PCRC = CRC_TX_RX;
	
	command_status.pSendBuf[0] = 0x30;//30 读块指令
	command_status.pSendBuf[1] = block;//块地址
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 4;
	command_status.Cycles = 1356000;//10ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if ((result != FM176XX_SUCCESS )||(command_status.nBytesReceived != 4)) //接收到的数据长度为4
		return FM176XX_COMM_ERR;
	memcpy(data_buff,command_status.pReceiveBuf,16);
	return FM176XX_SUCCESS;
}
/*****************************************************************************************/
/*名称：mifare_blockwrite																 */
/*功能：Mifare卡片写块操作																 */
/*输入：block，块号（0x00~0x3F）；buff，16字节写块数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockwrite(unsigned char block,unsigned char *data_buff)
{	

	uint8_t outbuf[16],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;

  command_status.PCRC = CRC_TX;

	command_status.pSendBuf[0] = 0xA0;//A0 写块指令
	command_status.pSendBuf[1] = block;//块地址

	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
	command_status.Cycles = 1356000;//10ms
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
		return FM176XX_COMM_ERR;
	
	command_status.Cycles = 1356000;//10ms
	memcpy(command_status.pSendBuf,data_buff,16);
	command_status.nBytesToSend = 16;
	command_status.nBytesToReceive = 1;
	command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
		return FM176XX_COMM_ERR;
	return FM176XX_SUCCESS;
}

/*****************************************************************************************/
/*名称：																				 */
/*功能：Mifare 卡片增值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节增值数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockinc(unsigned char block,unsigned char *data_buff,unsigned char *ack)
{	

	uint8_t outbuf[4],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;

  command_status.PCRC = CRC_TX;
	command_status.pSendBuf[0] = 0xC1;// C1 增值指令
	command_status.pSendBuf[1] = block;//块地址
	command_status.Cycles = 1356000;//10ms
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if (command_status.nBitsReceived != 4)	//如果未接收到0x0A，表示无ACK
		return FM176XX_COMM_ERR;
	*ack = command_status.pReceiveBuf[0];
	command_status.Cycles = 1356000;//10ms
	memcpy(command_status.pSendBuf,data_buff,4);
	command_status.nBytesToSend = 4;
	command_status.nBytesToReceive = 0;
	command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);

	return FM176XX_SUCCESS;
}
/*****************************************************************************************/
/*名称：mifare_blockdec																	 */
/*功能：Mifare 卡片减值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节减值数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockdec(unsigned char block,unsigned char *data_buff,unsigned char *ack)
{	

	uint8_t outbuf[4],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
	
  command_status.PCRC = CRC_TX;
	command_status.pSendBuf[0] = 0xC0;// C0 减值指令
	command_status.pSendBuf[1] = block;//块地址
	command_status.Cycles = 1356000;//10ms
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if (command_status.nBitsReceived != 4)	//如果未接收到0x0A，表示无ACK
		return FM176XX_COMM_ERR;
	*ack = command_status.pReceiveBuf[0];
	command_status.Cycles = 1356000;//10ms
	memcpy(command_status.pSendBuf,data_buff,4);
	command_status.nBytesToSend = 4;
	command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);

	return FM176XX_SUCCESS;
}
/*****************************************************************************************/
/*名称：mifare_transfer																	 */
/*功能：Mifare 卡片transfer操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Transfer(unsigned char block,unsigned char *ack)
{	

	uint8_t outbuf[2],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
	
  command_status.PCRC = CRC_TX;
	command_status.pSendBuf[0] = 0xB0;// B0 Transfer指令
	command_status.pSendBuf[1] = block;//块地址
	command_status.Cycles = 1356000;//10ms	
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if (command_status.nBitsReceived != 4)	//如果未接收到0x0A，表示无ACK
		return FM176XX_COMM_ERR;
	*ack = command_status.pReceiveBuf[0];
	return FM176XX_SUCCESS;
}
/*****************************************************************************************/
/*名称：mifare_restore																	 */
/*功能：Mifare 卡片restore操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/

unsigned char Mifare_Restore(unsigned char block,unsigned char *ack)
{	

	uint8_t outbuf[4],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;

  command_status.PCRC = CRC_TX;
	command_status.pSendBuf[0] = 0xC2;// C2 Restore指令
	command_status.pSendBuf[1] = block;//块地址
	
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
	command_status.Cycles = 1356000;//10ms
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
		return FM176XX_COMM_ERR;
	*ack = command_status.pReceiveBuf[0];
	command_status.Cycles = 1356000;//10ms
	memcpy(command_status.pSendBuf,"\x00\x00\x00\x00",4);
	command_status.nBytesToSend = 4;
	command_status.nBytesToReceive = 1;
	command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);

	return FM176XX_SUCCESS;
}
