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
/*���ƣ�Mifare_Clear_Crypto																															 */
/*���ܣ�Mifare_Clear_Crypto�����֤��־																 									*/
/*���룺																																								 */
/*																																						 					*/
/*���:																																									 */
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
/*���ƣ�Mifare_Auth																		 */
/*���ܣ�Mifare_Auth��Ƭ��֤																 */
/*���룺mode����֤ģʽ��0��key A��֤��1��key B��֤����sector����֤�������ţ�0~15��		 */
/*		*mifare_key��6�ֽ���֤��Կ���飻*card_uid��4�ֽڿ�ƬUID����						 */
/*���:																					 */
/*		OK    :��֤�ɹ�																	 */
/*		ERROR :��֤ʧ��																	 */
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
		SetReg(REG_FIFODATA,RF_CMD_KEYA);//60 keyA M1��ָ֤��
		ModifyReg(REG_RXTXCON,BIT_SHMODE,DISABLE);
	}
	if(key_mode == KEY_B_M1)
	{
		SetReg(REG_FIFODATA,RF_CMD_KEYA);//61 keyB M1��ָ֤��
		ModifyReg(REG_RXTXCON,BIT_SHMODE,DISABLE);
	}
	if(key_mode == KEY_A_SH)
	{
		SetReg(REG_FIFODATA,RF_CMD_KEYA);//60 keyA SH��ָ֤��
		ModifyReg(REG_RXTXCON,BIT_SHMODE,ENABLE);
	}
	if(key_mode == KEY_B_SH)
	{
		SetReg(REG_FIFODATA,RF_CMD_KEYA);//61 keyB SH��ָ֤��
		ModifyReg(REG_RXTXCON,BIT_SHMODE,ENABLE);
	}		
	
	SetReg(REG_FIFODATA,sector * 4);//��֤�����Ŀ�0��ַ
	SPI_Write_FIFO(4,card_uid);
	FM176XX_SetCommand(CMD_AUTHENT);
	mDelay(5);
	GetReg(REG_COMMAND,&reg_data);
	if((reg_data & CMD_MASK) == CMD_IDLE)
		{
		GetReg(REG_STATUS,&reg_data);
		if(reg_data & BIT_CRYPTO1ON)//�жϼ��ܱ�־λ��ȷ����֤���
			return FM176XX_SUCCESS;			
		}	
	return FM176XX_AUTH_ERR;
}
/*****************************************************************************************/
/*���ƣ�Mifare_Blockset									 */
/*���ܣ�Mifare_Blockset��Ƭ��ֵ����							 */
/*���룺block����ţ�*buff����Ҫ���õ�4�ֽ���ֵ����					 */
/*											 */
/*���:											 */
/*		OK    :���óɹ�								 */
/*		ERROR :����ʧ��								 */
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
/*���ƣ�Mifare_Blockread																 */
/*���ܣ�Mifare_Blockread��Ƭ�������													 */
/*���룺block����ţ�0x00~0x3F����buff��16�ֽڶ�����������								 */
/*���:																					 */
/*		OK    :�ɹ�																		 */
/*		ERROR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockread(unsigned char block,unsigned char *data_buff)
{	
	uint8_t result;
	uint8_t outbuf[2],inbuf[16];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
	
  command_status.PCRC = CRC_TX_RX;
	
	command_status.pSendBuf[0] = 0x30;//30 ����ָ��
	command_status.pSendBuf[1] = block;//���ַ
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 4;
	command_status.Cycles = 1356000;//10ms
	command_status.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(&command_status);
	if ((result != FM176XX_SUCCESS )||(command_status.nBytesReceived != 4)) //���յ������ݳ���Ϊ4
		return FM176XX_COMM_ERR;
	memcpy(data_buff,command_status.pReceiveBuf,16);
	return FM176XX_SUCCESS;
}
/*****************************************************************************************/
/*���ƣ�mifare_blockwrite																 */
/*���ܣ�Mifare��Ƭд�����																 */
/*���룺block����ţ�0x00~0x3F����buff��16�ֽ�д����������								 */
/*���:																					 */
/*		OK    :�ɹ�																		 */
/*		ERROR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockwrite(unsigned char block,unsigned char *data_buff)
{	

	uint8_t outbuf[16],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;

  command_status.PCRC = CRC_TX;

	command_status.pSendBuf[0] = 0xA0;//A0 д��ָ��
	command_status.pSendBuf[1] = block;//���ַ

	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
	command_status.Cycles = 1356000;//10ms
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//���δ���յ�0x0A����ʾ��ACK
		return FM176XX_COMM_ERR;
	
	command_status.Cycles = 1356000;//10ms
	memcpy(command_status.pSendBuf,data_buff,16);
	command_status.nBytesToSend = 16;
	command_status.nBytesToReceive = 1;
	command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//���δ���յ�0x0A����ʾ��ACK
		return FM176XX_COMM_ERR;
	return FM176XX_SUCCESS;
}

/*****************************************************************************************/
/*���ƣ�																				 */
/*���ܣ�Mifare ��Ƭ��ֵ����																 */
/*���룺block����ţ�0x00~0x3F����buff��4�ֽ���ֵ��������								 */
/*���:																					 */
/*		OK    :�ɹ�																		 */
/*		ERROR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockinc(unsigned char block,unsigned char *data_buff,unsigned char *ack)
{	

	uint8_t outbuf[4],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;

  command_status.PCRC = CRC_TX;
	command_status.pSendBuf[0] = 0xC1;// C1 ��ֵָ��
	command_status.pSendBuf[1] = block;//���ַ
	command_status.Cycles = 1356000;//10ms
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if (command_status.nBitsReceived != 4)	//���δ���յ�0x0A����ʾ��ACK
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
/*���ƣ�mifare_blockdec																	 */
/*���ܣ�Mifare ��Ƭ��ֵ����																 */
/*���룺block����ţ�0x00~0x3F����buff��4�ֽڼ�ֵ��������								 */
/*���:																					 */
/*		OK    :�ɹ�																		 */
/*		ERROR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockdec(unsigned char block,unsigned char *data_buff,unsigned char *ack)
{	

	uint8_t outbuf[4],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
	
  command_status.PCRC = CRC_TX;
	command_status.pSendBuf[0] = 0xC0;// C0 ��ֵָ��
	command_status.pSendBuf[1] = block;//���ַ
	command_status.Cycles = 1356000;//10ms
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if (command_status.nBitsReceived != 4)	//���δ���յ�0x0A����ʾ��ACK
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
/*���ƣ�mifare_transfer																	 */
/*���ܣ�Mifare ��Ƭtransfer����															 */
/*���룺block����ţ�0x00~0x3F��														 */
/*���:																					 */
/*		OK    :�ɹ�																		 */
/*		ERROR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Transfer(unsigned char block,unsigned char *ack)
{	

	uint8_t outbuf[2],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;
	
  command_status.PCRC = CRC_TX;
	command_status.pSendBuf[0] = 0xB0;// B0 Transferָ��
	command_status.pSendBuf[1] = block;//���ַ
	command_status.Cycles = 1356000;//10ms	
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if (command_status.nBitsReceived != 4)	//���δ���յ�0x0A����ʾ��ACK
		return FM176XX_COMM_ERR;
	*ack = command_status.pReceiveBuf[0];
	return FM176XX_SUCCESS;
}
/*****************************************************************************************/
/*���ƣ�mifare_restore																	 */
/*���ܣ�Mifare ��Ƭrestore����															 */
/*���룺block����ţ�0x00~0x3F��														 */
/*���:																					 */
/*		OK    :�ɹ�																		 */
/*		ERROR :ʧ��																		 */
/*****************************************************************************************/

unsigned char Mifare_Restore(unsigned char block,unsigned char *ack)
{	

	uint8_t outbuf[4],inbuf[1];
	command_struct command_status;
	command_status.pSendBuf = outbuf;
	command_status.pReceiveBuf = inbuf;

  command_status.PCRC = CRC_TX;
	command_status.pSendBuf[0] = 0xC2;// C2 Restoreָ��
	command_status.pSendBuf[1] = block;//���ַ
	
	command_status.nBytesToSend = 2;
	command_status.nBitsToSend = 0;
	command_status.nBitsToReceive = 0;
	command_status.nBytesToReceive = 1;
	command_status.Cycles = 1356000;//10ms
  command_status.Cmd = CMD_TRANSCEIVE;
	Command_Execute(&command_status);
	if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//���δ���յ�0x0A����ʾ��ACK
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
