#include "command.h"
#include "includes.h"
#include "struct_type.h"
#include "pT113_fifo.h"
#include "nfc_interface.h"
#include "RC663.h"
#include "iap_protocols.h"
#include "serial.h"

const uint8_t packetHead1 = 0x55; //��һ����ͷ
const uint8_t packetHead2 = 0xAA; //��һ����ͷ
const uint8_t deviceID = 0x14; //����ID��
const uint8_t SLID = 0x00; //�Ź����п�ID
const uint8_t ackDataHeadLen = 5; //Ӧ��ָ���ͷ�����ݳ���Ϊ5���ֽ�

//���԰������
#ifdef JINGYUAN_MODE	
	//��ȡ����ָ��
	static uint8_t get_weight_cmd[] = {0x01,0x3C,0x08,0x00,0x0A,0x00,0x01,0x00,0x0C,0x00,0x02,0x00,0x5B,0xE5};
#endif

//BT559A������
#ifdef BT559A_MODE
	//��ȡ����ָ��
	static uint8_t get_weight_cmd[] = {0x05,0xa9,0x00,0x00,0x00,0xae};
	//
//	static uint8_t switch_mode_cmd[] = {0x05,0xab,0x00,0x00,0x00,0xb0};
#endif

//��ݸ��Զ������
#ifdef ZHI_YUAN_TYPE
	//��ȡ����ָ��
	static uint8_t get_weight_cmd[] = {0x01,0x03,0x01,0xC2,0x00,0x04,0xE4,0x09};
#endif


//485�շ��������
bool task_485_lock = false;
//��ǰ���������
//static int zeroWeigh = 0;
//���������־λ
//static bool zeroFlag = false;
//��ǰ������uuid�ṹ��
uuidWeigh_struct_t myUuidWeighSt;
//BT559A���ش����� �������ݳ���7
#ifdef BT559A_MODE
	struct	sDatasStruct//ͨѶ���ݸ�ʽ
	{
		uint8_t	stx;
		uint8_t	add;
		uint8_t cmd;
		uint8_t	lenL;
		uint8_t lenH;
		uint8_t	Datas[190];//���ݰ�
		uint16_t crc;
	};
	union	sDatasUnion
	{
		unsigned char	S_DatasBuffer[197];
		struct	sDatasStruct	S_DatasStruct;
	};
#endif

//��ݸ��Զ������
#ifdef ZHI_YUAN_TYPE
	struct	sDatasStruct//ͨѶ���ݸ�ʽ
	{
		uint8_t	add;
		uint8_t cmd;
		uint8_t	lenL;
		uint8_t lenH;
		uint8_t	Datas[190];//���ݰ�
		uint16_t crc;
	};
	union	sDatasUnion
	{
		unsigned char	S_DatasBuffer[196];
		struct	sDatasStruct	S_DatasStruct;
	};
#endif

static union sDatasUnion SerialUnion;//485����������

send_queue_t	p485_queue;//485���ն���
t113_send_queue_t	pt113_queue;//����T113���ڽ��ն���

/*********************************************************
// �������ƣ�crc16CcittFun
// �������ܣ�����CRC16У�麯��
// ��ڲ�����data����У������飻length����У������ݳ���
// ���ڲ���������ֵ��CRC16У����
// ע    �ͣ�
*********************************************************/
u16 crc16CcittFun(u8* data, u16 length)
{
    u8  i = 0;
    u16 num = 0;
    u16 crc = 0;   // Initial value

    if(length <= 0)
    {
        return 0;
    }

    while(length-- > 0)
    {
        crc ^= data[num++];
        
        for(i = 0; i < 8; ++i)
        {
            if((crc & 1) > 0)
            {
                crc = (u16)((crc >> 1) ^ 0x8408);
            }
            else
            {
                crc = (u16)(crc >> 1);
            }
        }
    }
    
    return crc;
}



/*********************************************************
// �������ƣ�checkReceiveDataFun
// �������ܣ�У����յ��������Ƿ���Э��һ�º���
// ��ڲ�����*checkDataBuf����У��������ͷ��ַ��checkDataLen����У������ݳ���
// ���ڲ���������ֵ��0�����ݿ��ã�1�����ݲ�����
// ע    �ͣ�
*********************************************************/
u8 checkReceiveDataFun(u8 *checkDataBuf, u16 checkDataLen)
{
    u32 crc32Data = 0;
		u8  crc32DataBuf[CRC32_DATA_LEN] = {0};
    u8  arrayIndex = 0;
    u8  ReceiveDataLen = checkDataBuf[3]; //���յ���ָ���������ݳ���
    
    if((checkDataBuf[0] == packetHead1) && (checkDataBuf[1] == packetHead2)) //�ж���ͷ�Ƿ���ȷ
    {
			if(ReceiveDataLen == (checkDataLen - CRC32_DATA_LEN))
			{
				crc32Data = CRC32DataCheckFun(&checkDataBuf[2], ReceiveDataLen - 2);
				
				crc32DataBuf[arrayIndex++] = crc32Data;
				crc32DataBuf[arrayIndex++] = crc32Data >> 8;
				crc32DataBuf[arrayIndex++] = crc32Data >> 16;
				crc32DataBuf[arrayIndex++] = crc32Data >> 24;
	
				debug_print("\r\n----------CRCУ��ֵ��----------\r\n");
				debug_print("%x ",crc32DataBuf[0]);
				debug_print("%x ",crc32DataBuf[1]);
				debug_print("%x ",crc32DataBuf[2]);
				debug_print("%x ",crc32DataBuf[3]);
				debug_print("\r\n");
				
				for(arrayIndex = 0; arrayIndex < 4; arrayIndex++) //�ж�CRCУ�����Ƿ���ȷ
				{
					if(crc32DataBuf[arrayIndex] != checkDataBuf[ReceiveDataLen + arrayIndex])
					{
						return DATA_CHECK_ERR_2;
					}
				}
				return DATA_CHECK_OK;
			}
			else
			{
				return DATA_CHECK_ERR_2;
			}
    }  
    return DATA_CHECK_ERR_1;
}
/*==================================================================================
* �� �� ���� p485_frame_parse
* ��    ���� None
* ��������:  485 ���յ������ݽ���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2022-06-08 025540
==================================================================================*/
void clear_weigh_flag(void)
{
//	zeroFlag = true;
}

/*==================================================================================
* �� �� ���� p485_frame_parse
* ��    ���� None
* ��������:  ���ش����� ���յ������ݽ���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2022-06-08 025540
==================================================================================*/
void p485_frame_parse(void* ret_msg)
{
	_p485_Msg pmsg = ret_msg;
	int ch1_weigh =0,ch2_weigh = 0;
	uint16_t crcValue = 0;

	memcpy(SerialUnion.S_DatasBuffer, pmsg->data, pmsg->byte_count);
//	debug_print("���ճ��ش����� rec data = ");
//	debug_print_hex(SerialUnion.S_DatasBuffer, pmsg->byte_count);
//	debug_print("\r\n");
//	debug_print("������ = %02X \r\n", SerialUnion.S_DatasStruct.cmd);
//	debug_print("���ݳ��� = %d \r\n", SerialUnion.S_DatasStruct.lenL);
//	
	//���԰���ش�����
	#ifdef JINGYUAN_MODE	
		switch(SerialUnion.S_DatasStruct.cmd)
		{
			case READ_DATA_CMD://��ȡ�ڴ����
				debug_print("��ȡ�������������\r\n");
				if(SerialUnion.S_DatasStruct.lenL == 0x0c)
				{
					memcpy(&pWeigh, SerialUnion.S_DatasStruct.Datas+4, 4);
					debug_print("�����볤�� = %d; ���� = %d\r\n", SerialUnion.S_DatasStruct.lenL, pWeigh);
					if(zeroFlag == true)
					{
						zeroFlag = false;
						zeroWeigh = pWeigh;
					}
							
					buffer[sendLen++] = GET_UUID_WEIGH_CMD;	
					//��ȡ��ǩ��Ϣ
					p_Tag = get_card_tag_msg();
					if(p_Tag.tag_state == NULL_STA)
						buffer[sendLen++] = 0;//�ޱ�ǩ
					else if(p_Tag.tag_state == ERR_CARD_STA)
						buffer[sendLen++] = 1;//�����Ϳ�
					else if(p_Tag.tag_state == ENTRY_BLOCK_OK_STA)
						buffer[sendLen++] = 2;//������
					else 
						buffer[sendLen++] = 0;//�ޱ�ǩ
					memcpy(buffer+sendLen, p_Tag.uid, 8);
					sendLen +=8;
					
					//��������
					pWeigh = abs(pWeigh -zeroWeigh);
					debug_print("���� = %d \r\n",pWeigh);
					memcpy(buffer+sendLen,&pWeigh,4);
					sendLen +=4;
					
					distbuffer[0] = STX;
					HexGroupToHexString(buffer,distbuffer+1,sendLen);
					distbuffer[sendLen*2+1] = 0xFF;
					
					//�������ݵ�����
					for(i=0;i<strlen(distbuffer);i++)
						debug_print("%02x ",distbuffer[i]);
					debug_print("\r\n");
					HAL_T113_Uart_Send_Data(distbuffer, strlen(distbuffer));
				}	
			break;
		}
	#endif

	//BT559A���ش����� �������ݳ���7
	#ifdef BT559A_MODE
		switch(SerialUnion.S_DatasBuffer[1])
		{
			//��ȡ����ֵ
			case 0x50:
				//�ȶ��ĸ�ֵ
			case 0x10:
				//�ȶ�����ֵ
				//memcpy(&pWeigh, SerialUnion.S_DatasStruct.Datas+2, 4);
				myUuidWeighSt.weigh = SerialUnion.S_DatasBuffer[3]*65536 + SerialUnion.S_DatasBuffer[4]*256 + SerialUnion.S_DatasBuffer[5];
				debug_print("�ȶ�������ֵ = %d\r\n", myUuidWeighSt.weigh);
			break;
			
			case 0x00:
			case 0x40:
				//memcpy(&pWeigh, SerialUnion.S_DatasStruct.Datas+2, 4);
				myUuidWeighSt.weigh = SerialUnion.S_DatasBuffer[3]*65536 + SerialUnion.S_DatasBuffer[4]*256 + SerialUnion.S_DatasBuffer[5];
				debug_print("���ȶ�������ֵ = %d\r\n", myUuidWeighSt.weigh);
				
			break;
				
			case 0x80://��������ظ�
				switch(SerialUnion.S_DatasBuffer[2])
				{
					case 0xA4://��ȡ�������汾��
						
					break;
				}		
			break;
		}
	#endif
	//��ݸ��Զ���ش����� 
	#ifdef ZHI_YUAN_TYPE
		switch(SerialUnion.S_DatasStruct.cmd)
		{
			//��ȡ����ֵ���ܴ���
			case 0x03:
				crcValue = MB_CRC16(SerialUnion.S_DatasBuffer, SerialUnion.S_DatasStruct.lenL+3);
				//debug_print("crcValue = %02X \r\n",crcValue);
				if(crcValue == SerialUnion.S_DatasBuffer[SerialUnion.S_DatasStruct.lenL+4]*256+SerialUnion.S_DatasBuffer[SerialUnion.S_DatasStruct.lenL+3])
				{
					ch1_weigh = SerialUnion.S_DatasBuffer[4]*65536 + SerialUnion.S_DatasBuffer[5]*256 + SerialUnion.S_DatasBuffer[6];
					if(SerialUnion.S_DatasStruct.lenL == 8)
						ch2_weigh = SerialUnion.S_DatasBuffer[8]*65536 + SerialUnion.S_DatasBuffer[9]*256 + SerialUnion.S_DatasBuffer[10];
					myUuidWeighSt.weigh = ch1_weigh+ch2_weigh;
					myUuidWeighSt.weigh = ch2_weigh;
					debug_print("��Զ����������ֵ = %d, ͨ��1 = %d, ͨ��2 = %d\r\n", myUuidWeighSt.weigh, ch1_weigh, ch2_weigh);
				} else {
					debug_print("Modbus RTU crc ʧ��\r\n");
				}
			break;
		}
	#endif
}

/*==================================================================================
* �� �� ���� p485_instance_init
* ��    ���� None
* ��������:  485���ն��г�ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-10-28 170617
=================================================================================*/
void p485_instance_init(void)
{
	msg_485_queue_init(&p485_queue);
}

/*==================================================================================
* �� �� ���� p485_push_data_msg
* ��    ���� None
* ��������:  485 ���յ����ݺ���ô˺����ѽ��յ�������ӵĶ�������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-09-29 170658
==================================================================================*/
void p485_push_data_msg(void* ret_msg)
{
	_p485_Msg pmsg = ret_msg;  
	
	msg_485_queue_push(&p485_queue, pmsg);
}

/*==================================================================================
* �� �� ���� p485_pop_one_frame
* ��    ���� _pRet_Msg
* ��������:  485���ݽ���
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-20 154449
==================================================================================*/ 
static uint8_t p485_pop_one_frame(void )
{
	int num;
	p_send_queue_t p_queue_buff = &p485_queue;
	
	//�����������Ƿ�������
	num = msg_485_queue_num(p_queue_buff);
	if(num)
	{
		for(int i=0; i<MAX_CACHE_NUM; i++)
		{
			if(p_queue_buff->queue[i].byte_count)
			{
				//debug_print("�����������Ϣ =%s \n",p_queue_buff->queue[i].data);
				p485_frame_parse(&(p_queue_buff->queue[i]));
				msg_485_queue_pop(p_queue_buff, 0);
				return 0;
			}	
		}
		//debug_print("�����������Ϣ���� =%d \n",num);
	} 
	//else 
	{
		return 0xff;
	}
}

/*==================================================================================
* �� �� ���� p485_send_data_task
* ��    ���� None
* ��������:  485 ������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2022-06-08 170658
==================================================================================*/
void p485_send_data_task(void* argv)
{
	HAL_485_Send_Data(get_weight_cmd,sizeof(get_weight_cmd));
}

/*==================================================================================
* �� �� ���� p485_rec_decode_task
* ��    ���� None
* ��������:  485 �������ݽ�������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2022-06-08 170658
==================================================================================*/
void p485_rec_decode_task(void* argv)
{
	p485_pop_one_frame();
}

/*==================================================================================
* �� �� ���� iap_simply_ack
* ��    ���� None
* ��������:  ������Ӧ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2021-01-07 025540
==================================================================================*/
static void iap_simply_ack(uint8_t *ackDataBuf, uint16_t ackDataLen)
{
	uint8_t  i = 0;
	uint16_t ackDataIndex = 0;
	uint32_t ackCRC32Data = 0;
	uint8_t  TxT113DataBuf[50] = {0};
	
	TxT113DataBuf[ackDataIndex++] = packetHead1;
	TxT113DataBuf[ackDataIndex++] = packetHead2;
	TxT113DataBuf[ackDataIndex++] = deviceID; //����ID��
	TxT113DataBuf[ackDataIndex++] = ackDataLen+ackDataHeadLen; //���ݳ���
	TxT113DataBuf[ackDataIndex++] = SLID; //�Ź����п�ID
	
	for(i = 0; i < ackDataLen; i++)
	{
		TxT113DataBuf[ackDataIndex++] = ackDataBuf[i];
	}

	ackCRC32Data = CRC32DataCheckFun(&TxT113DataBuf[2], ackDataIndex - 2); //����CRCУ���� 
	TxT113DataBuf[ackDataIndex++] = ackCRC32Data;
	TxT113DataBuf[ackDataIndex++] = ackCRC32Data >> 8;
	TxT113DataBuf[ackDataIndex++] = ackCRC32Data >> 16;
	TxT113DataBuf[ackDataIndex++] = ackCRC32Data >> 24;

	HAL_T113_Uart_Send_Data(TxT113DataBuf,ackDataIndex);//���ڷ������ݵ�T113
	
//	debug_print_hex(TxT113DataBuf, ackDataIndex);
//	debug_print("\r\n");
}

/*==================================================================================
* �� �� ���� pt113_frame_parse
* ��    ���� None
* ��������:  ����T113 ���յ������ݽ���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2022-06-08 025540
==================================================================================*/
static void pt113_frame_parse(void* ret_msg)
{
	_pRet_Msg pmsg = ret_msg;
	_Tag_Info p_Tag;
	uint8_t buffer[50] = {0};
//	uint8_t distbuffer[50] = {0};
//	int dalaLe=0,i;
	uint16_t ret_id = 0;
	uint8_t ret_s = 0;
	uint8_t send_len = 0;
//	uint32_t temp;
	
	if(pmsg->header !=0xAA55)
		return;

	debug_print("T113_rev ,");
	debug_print("header = %4x, ", pmsg->header);
	debug_print("id = %4x, ", pmsg->ext_id);
	debug_print("main cmd = %2x, ", pmsg->mainCmd );
	debug_print("salve cmd = %2x, ", pmsg->slaveCmd );
	debug_print("lens = %d, ", pmsg->date_len );
	debug_print("pkg_num = %d, ", pmsg->msgData[0]);
	debug_print("rev_data = ");
	debug_print_hex(pmsg->msgData, pmsg->date_len-8);
	debug_print("\r\n");
//	
	//����Э�����
	switch(pmsg->slaveCmd)
	{
		case ANDROID_CC_GET_SW_AND_HW_VER://��ȡ�汾��
			debug_print("��ȡ�汾�� 0xA1 \r\n");
			buffer[send_len++] = 0;//��ǰ״̬
			buffer[send_len++] = mApp_Param.hard_ver;//Ӳ���汾
			buffer[send_len++] = mApp_Param.soft_ver;//����汾
			iap_simply_ack(buffer, send_len);//Ӧ��
		break;
		
		case ANDROID_CC_GET_WEIGH_UID://��ȡ��ǰ����ֵ�Լ�������Ϣ
			debug_print("��ȡ�������������\r\n");	
			//��ȡ��ǩ��Ϣ
			p_Tag = get_card_tag_msg();
			if(p_Tag.tag_state == NULL_STA)
				buffer[send_len++] = 0;//�ޱ�ǩ
			else if(p_Tag.tag_state == ERR_CARD_STA)
				buffer[send_len++] = 1;//�����Ϳ�
			else if(p_Tag.tag_state == ENTRY_BLOCK_OK_STA)
				buffer[send_len++] = 2;//������
			else 
				buffer[send_len++] = 0;//�ޱ�ǩ
			memcpy(buffer+send_len, p_Tag.uid, 8);
			send_len += 8;
			//��������
			debug_print("���� = %d \r\n",myUuidWeighSt.weigh);
			memcpy(buffer+send_len,&myUuidWeighSt.weigh,4);
			send_len += 4;

			iap_simply_ack(buffer, send_len);//Ӧ��
		break;
							
	
		//�̼�����
		case ANDROID_CC_UPDATE_INFO://�·���������Ϣ 0x80
			debug_print("�·���������Ϣ 0x80 \r\n");
		  //disable ��������
			DisableTask(READ_CARD_TASK);
			DisableTask(P485_REC_TASK);
			DisableTask(TEST_TASK);
			ret_s = pIap_Func->info_opt(pmsg->msgData, &ret_id);
			if(ret_s) {
				buffer[send_len++] = 0x08;//ʧ��
			} else {
				buffer[send_len++] = 0;//�ɹ�
			}	
			iap_simply_ack(buffer, send_len);//Ӧ��
		break;
		
		case ANDROID_CC_UPDATE_DATA://�·����������� 0x81
			debug_print("�·����������� 0x81 \r\n");
			ret_s = pIap_Func->data_opt(pmsg->msgData, &ret_id);
			if(ret_s) {
				buffer[send_len++] = 0x08;//ʧ��
			} else {
				buffer[send_len++] = 0;//�ɹ�
			}	
			iap_simply_ack(buffer, send_len);//Ӧ��
		break;
			
		case ANDROID_CC_UPDATE_CHECK://ȷ������ 0x84
			debug_print("ȷ������ 0x84 \r\n");
			ret_s = pIap_Func->check_opt(&ret_id);
			if(ret_s) {
				buffer[send_len++] = 0x08;//ʧ��
			} else {
				buffer[send_len++] = 0;//�ɹ�
			}	
			iap_simply_ack(buffer, send_len);//Ӧ��
		break;
		
		case ANDROID_CC_UPDATE_LOST://�·�ȱʧ���������� 0x8A
			debug_print("�·�ȱʧ���������� 0x8A \r\n");
			ret_s = pIap_Func->patch_opt(pmsg->msgData, &ret_id);
			if(ret_s) {
				buffer[send_len++] = 0x08;//ʧ��
			} else {
				buffer[send_len++] = 0;//�ɹ�
			}	
			iap_simply_ack(buffer, send_len);//Ӧ��
		break;
		
		case ANDROID_CC_UPDATE_RESET://����ϵͳ 0x88
			debug_print("����ϵͳ 0x88 \r\n");
			buffer[send_len++] = 0;//�ɹ�
			iap_simply_ack(buffer, send_len);//Ӧ��
			HAL_NVIC_SystemReset();
		break;
	}
}

/*==================================================================================
* �� �� ���� pt113_instance_init
* ��    ���� None
* ��������:  ����T113���ڽ��ն��г�ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-10-28 170617
=================================================================================*/
void pt113_instance_init(void)
{
	msg_t113_queue_init(&pt113_queue);
}

/*==================================================================================
* �� �� ���� pt113_push_data_msg
* ��    ���� None
* ��������:  ����T1113 ���յ����ݺ���ô˺����ѽ��յ�������ӵĶ�������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-09-29 170658
==================================================================================*/
void pt113_push_data_msg(void* ret_msg)
{
	_pT113_Msg pmsg = ret_msg;  
	
	msg_t113_queue_push(&pt113_queue, pmsg);
}

/*==================================================================================
* �� �� ���� pt113_pop_one_frame
* ��    ���� _pRet_Msg
* ��������:  ����T113���ݽ���
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-20 154449
==================================================================================*/ 
static uint8_t pt113_pop_one_frame(void )
{
	int num;
	p_t113_send_queue_t p_queue_buff = &pt113_queue;
	
	//�����������Ƿ�������
	num = msg_t113_queue_num(p_queue_buff);
	if(num)
	{
		for(int i=0; i<MAX_CACHE_NUM; i++)
		{
			if(p_queue_buff->queue[i].byte_count)
			{
//				debug_print("�����������Ϣ���� = %d \r\n",p_queue_buff->queue[i].byte_count);
				pt113_frame_parse(&(p_queue_buff->queue[i].data));
				msg_t113_queue_pop(p_queue_buff, 0);
				return 0;
			}	
		}
		//debug_print("�����������Ϣ���� =%d \n",num);
	}
	return 0xff;
}

/*==================================================================================
* �� �� ���� p485_send_data_task
* ��    ���� None
* ��������:  485 ������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2022-06-08 170658
==================================================================================*/
void pt113_send_data_task(void* argv)
{
	//HAL_485_Send_Data(get_weight_cmd,sizeof(get_weight_cmd));
}
/*==================================================================================
* �� �� ���� pt113_rec_decode_task
* ��    ���� None
* ��������:  ����T113���ڷ���������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2022-06-08 170658
==================================================================================*/
extern uint8_t recv_end_flag;
extern uint8_t rx_len;
void pt113_rec_decode_task(void* argv)
{
	pt113_pop_one_frame();
//	memset(T113_USART_RX_BUF,0x66,8);
//	if(recv_end_flag)
//	{
//		recv_end_flag = 0;
//		HAL_UART_Receive_DMA(&t113_huart,T113_USART_RX_BUF,MAX_USART_REC_LEN);
//		HAL_T113_Uart_Send_Data(T113_USART_RX_BUF, 8);//���ڷ������ݵ�T113
//	}
}
