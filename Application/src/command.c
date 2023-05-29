#include "command.h"
#include "includes.h"
#include "struct_type.h"
#include "pT113_fifo.h"
#include "nfc_interface.h"
#include "RC663.h"
#include "iap_protocols.h"
#include "serial.h"

const uint8_t packetHead1 = 0x55; //第一个字头
const uint8_t packetHead2 = 0xAA; //第一个字头
const uint8_t deviceID = 0x14; //外设ID号
const uint8_t SLID = 0x00; //九宫格中控ID
const uint8_t ackDataHeadLen = 5; //应答指令的头部数据长度为5个字节

//天津景园传感器
#ifdef JINGYUAN_MODE	
	//获取重量指令
	static uint8_t get_weight_cmd[] = {0x01,0x3C,0x08,0x00,0x0A,0x00,0x01,0x00,0x0C,0x00,0x02,0x00,0x5B,0xE5};
#endif

//BT559A传感器
#ifdef BT559A_MODE
	//获取重量指令
	static uint8_t get_weight_cmd[] = {0x05,0xa9,0x00,0x00,0x00,0xae};
	//
//	static uint8_t switch_mode_cmd[] = {0x05,0xab,0x00,0x00,0x00,0xb0};
#endif

//东莞智远传感器
#ifdef ZHI_YUAN_TYPE
	//获取重量指令
	static uint8_t get_weight_cmd[] = {0x01,0x03,0x01,0xC2,0x00,0x04,0xE4,0x09};
#endif


//485收发任务枷锁
bool task_485_lock = false;
//当前归零的重量
//static int zeroWeigh = 0;
//重量归零标志位
//static bool zeroFlag = false;
//当前重量，uuid结构体
uuidWeigh_struct_t myUuidWeighSt;
//BT559A称重传感器 返回数据长度7
#ifdef BT559A_MODE
	struct	sDatasStruct//通讯数据格式
	{
		uint8_t	stx;
		uint8_t	add;
		uint8_t cmd;
		uint8_t	lenL;
		uint8_t lenH;
		uint8_t	Datas[190];//数据包
		uint16_t crc;
	};
	union	sDatasUnion
	{
		unsigned char	S_DatasBuffer[197];
		struct	sDatasStruct	S_DatasStruct;
	};
#endif

//东莞智远传感器
#ifdef ZHI_YUAN_TYPE
	struct	sDatasStruct//通讯数据格式
	{
		uint8_t	add;
		uint8_t cmd;
		uint8_t	lenL;
		uint8_t lenH;
		uint8_t	Datas[190];//数据包
		uint16_t crc;
	};
	union	sDatasUnion
	{
		unsigned char	S_DatasBuffer[196];
		struct	sDatasStruct	S_DatasStruct;
	};
#endif

static union sDatasUnion SerialUnion;//485数据联合体

send_queue_t	p485_queue;//485接收队列
t113_send_queue_t	pt113_queue;//主控T113串口接收队列

/*********************************************************
// 函数名称：crc16CcittFun
// 函数功能：数据CRC16校验函数
// 入口参数：data：待校验的数组；length：待校验的数据长度
// 出口参数：返回值：CRC16校验码
// 注    释：
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
// 函数名称：checkReceiveDataFun
// 函数功能：校验接收到的数据是否与协议一致函数
// 入口参数：*checkDataBuf：待校验的数组的头地址；checkDataLen：待校验的数据长度
// 出口参数：返回值：0：数据可用；1：数据不可用
// 注    释：
*********************************************************/
u8 checkReceiveDataFun(u8 *checkDataBuf, u16 checkDataLen)
{
    u32 crc32Data = 0;
		u8  crc32DataBuf[CRC32_DATA_LEN] = {0};
    u8  arrayIndex = 0;
    u8  ReceiveDataLen = checkDataBuf[3]; //接收到的指令包里的数据长度
    
    if((checkDataBuf[0] == packetHead1) && (checkDataBuf[1] == packetHead2)) //判断字头是否正确
    {
			if(ReceiveDataLen == (checkDataLen - CRC32_DATA_LEN))
			{
				crc32Data = CRC32DataCheckFun(&checkDataBuf[2], ReceiveDataLen - 2);
				
				crc32DataBuf[arrayIndex++] = crc32Data;
				crc32DataBuf[arrayIndex++] = crc32Data >> 8;
				crc32DataBuf[arrayIndex++] = crc32Data >> 16;
				crc32DataBuf[arrayIndex++] = crc32Data >> 24;
	
				debug_print("\r\n----------CRC校验值：----------\r\n");
				debug_print("%x ",crc32DataBuf[0]);
				debug_print("%x ",crc32DataBuf[1]);
				debug_print("%x ",crc32DataBuf[2]);
				debug_print("%x ",crc32DataBuf[3]);
				debug_print("\r\n");
				
				for(arrayIndex = 0; arrayIndex < 4; arrayIndex++) //判断CRC校验码是否正确
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
* 函 数 名： p485_frame_parse
* 参    数： None
* 功能描述:  485 接收到的数据解析
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 025540
==================================================================================*/
void clear_weigh_flag(void)
{
//	zeroFlag = true;
}

/*==================================================================================
* 函 数 名： p485_frame_parse
* 参    数： None
* 功能描述:  称重传感器 接收到的数据解析
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 025540
==================================================================================*/
void p485_frame_parse(void* ret_msg)
{
	_p485_Msg pmsg = ret_msg;
	int ch1_weigh =0,ch2_weigh = 0;
	uint16_t crcValue = 0;

	memcpy(SerialUnion.S_DatasBuffer, pmsg->data, pmsg->byte_count);
//	debug_print("接收称重传感器 rec data = ");
//	debug_print_hex(SerialUnion.S_DatasBuffer, pmsg->byte_count);
//	debug_print("\r\n");
//	debug_print("命令字 = %02X \r\n", SerialUnion.S_DatasStruct.cmd);
//	debug_print("数据长度 = %d \r\n", SerialUnion.S_DatasStruct.lenL);
//	
	//天津景园称重传感器
	#ifdef JINGYUAN_MODE	
		switch(SerialUnion.S_DatasStruct.cmd)
		{
			case READ_DATA_CMD://读取内存变量
				debug_print("读取重量命令。。。。\r\n");
				if(SerialUnion.S_DatasStruct.lenL == 0x0c)
				{
					memcpy(&pWeigh, SerialUnion.S_DatasStruct.Datas+4, 4);
					debug_print("数据与长度 = %d; 净重 = %d\r\n", SerialUnion.S_DatasStruct.lenL, pWeigh);
					if(zeroFlag == true)
					{
						zeroFlag = false;
						zeroWeigh = pWeigh;
					}
							
					buffer[sendLen++] = GET_UUID_WEIGH_CMD;	
					//获取标签信息
					p_Tag = get_card_tag_msg();
					if(p_Tag.tag_state == NULL_STA)
						buffer[sendLen++] = 0;//无标签
					else if(p_Tag.tag_state == ERR_CARD_STA)
						buffer[sendLen++] = 1;//非美餐卡
					else if(p_Tag.tag_state == ENTRY_BLOCK_OK_STA)
						buffer[sendLen++] = 2;//正常卡
					else 
						buffer[sendLen++] = 0;//无标签
					memcpy(buffer+sendLen, p_Tag.uid, 8);
					sendLen +=8;
					
					//绝对重量
					pWeigh = abs(pWeigh -zeroWeigh);
					debug_print("净重 = %d \r\n",pWeigh);
					memcpy(buffer+sendLen,&pWeigh,4);
					sendLen +=4;
					
					distbuffer[0] = STX;
					HexGroupToHexString(buffer,distbuffer+1,sendLen);
					distbuffer[sendLen*2+1] = 0xFF;
					
					//发送数据到主控
					for(i=0;i<strlen(distbuffer);i++)
						debug_print("%02x ",distbuffer[i]);
					debug_print("\r\n");
					HAL_T113_Uart_Send_Data(distbuffer, strlen(distbuffer));
				}	
			break;
		}
	#endif

	//BT559A称重传感器 返回数据长度7
	#ifdef BT559A_MODE
		switch(SerialUnion.S_DatasBuffer[1])
		{
			//获取重量值
			case 0x50:
				//稳定的负值
			case 0x10:
				//稳定的正值
				//memcpy(&pWeigh, SerialUnion.S_DatasStruct.Datas+2, 4);
				myUuidWeighSt.weigh = SerialUnion.S_DatasBuffer[3]*65536 + SerialUnion.S_DatasBuffer[4]*256 + SerialUnion.S_DatasBuffer[5];
				debug_print("稳定的重量值 = %d\r\n", myUuidWeighSt.weigh);
			break;
			
			case 0x00:
			case 0x40:
				//memcpy(&pWeigh, SerialUnion.S_DatasStruct.Datas+2, 4);
				myUuidWeighSt.weigh = SerialUnion.S_DatasBuffer[3]*65536 + SerialUnion.S_DatasBuffer[4]*256 + SerialUnion.S_DatasBuffer[5];
				debug_print("不稳定的重量值 = %d\r\n", myUuidWeighSt.weigh);
				
			break;
				
			case 0x80://其他命令回复
				switch(SerialUnion.S_DatasBuffer[2])
				{
					case 0xA4://读取传感器版本号
						
					break;
				}		
			break;
		}
	#endif
	//东莞智远称重传感器 
	#ifdef ZHI_YUAN_TYPE
		switch(SerialUnion.S_DatasStruct.cmd)
		{
			//获取重量值功能代码
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
					debug_print("智远传感器重量值 = %d, 通道1 = %d, 通道2 = %d\r\n", myUuidWeighSt.weigh, ch1_weigh, ch2_weigh);
				} else {
					debug_print("Modbus RTU crc 失败\r\n");
				}
			break;
		}
	#endif
}

/*==================================================================================
* 函 数 名： p485_instance_init
* 参    数： None
* 功能描述:  485接收队列初始化
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-10-28 170617
=================================================================================*/
void p485_instance_init(void)
{
	msg_485_queue_init(&p485_queue);
}

/*==================================================================================
* 函 数 名： p485_push_data_msg
* 参    数： None
* 功能描述:  485 接收到数据后调用此函数把接收的数据添加的队列里面
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void p485_push_data_msg(void* ret_msg)
{
	_p485_Msg pmsg = ret_msg;  
	
	msg_485_queue_push(&p485_queue, pmsg);
}

/*==================================================================================
* 函 数 名： p485_pop_one_frame
* 参    数： _pRet_Msg
* 功能描述:  485数据接收
* 返 回 值： None
* 备    注： None
* 作    者： lc
* 创建时间： 2021-06-20 154449
==================================================================================*/ 
static uint8_t p485_pop_one_frame(void )
{
	int num;
	p_send_queue_t p_queue_buff = &p485_queue;
	
	//检查队列里面是否有数据
	num = msg_485_queue_num(p_queue_buff);
	if(num)
	{
		for(int i=0; i<MAX_CACHE_NUM; i++)
		{
			if(p_queue_buff->queue[i].byte_count)
			{
				//debug_print("队列里面的消息 =%s \n",p_queue_buff->queue[i].data);
				p485_frame_parse(&(p_queue_buff->queue[i]));
				msg_485_queue_pop(p_queue_buff, 0);
				return 0;
			}	
		}
		//debug_print("队列里面的消息个数 =%d \n",num);
	} 
	//else 
	{
		return 0xff;
	}
}

/*==================================================================================
* 函 数 名： p485_send_data_task
* 参    数： None
* 功能描述:  485 发送数据任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 170658
==================================================================================*/
void p485_send_data_task(void* argv)
{
	HAL_485_Send_Data(get_weight_cmd,sizeof(get_weight_cmd));
}

/*==================================================================================
* 函 数 名： p485_rec_decode_task
* 参    数： None
* 功能描述:  485 接收数据解析任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 170658
==================================================================================*/
void p485_rec_decode_task(void* argv)
{
	p485_pop_one_frame();
}

/*==================================================================================
* 函 数 名： iap_simply_ack
* 参    数： None
* 功能描述:  升级响应
* 返 回 值： None
* 备    注： 
* 作    者：  
* 创建时间： 2021-01-07 025540
==================================================================================*/
static void iap_simply_ack(uint8_t *ackDataBuf, uint16_t ackDataLen)
{
	uint8_t  i = 0;
	uint16_t ackDataIndex = 0;
	uint32_t ackCRC32Data = 0;
	uint8_t  TxT113DataBuf[50] = {0};
	
	TxT113DataBuf[ackDataIndex++] = packetHead1;
	TxT113DataBuf[ackDataIndex++] = packetHead2;
	TxT113DataBuf[ackDataIndex++] = deviceID; //外设ID号
	TxT113DataBuf[ackDataIndex++] = ackDataLen+ackDataHeadLen; //数据长度
	TxT113DataBuf[ackDataIndex++] = SLID; //九宫格中控ID
	
	for(i = 0; i < ackDataLen; i++)
	{
		TxT113DataBuf[ackDataIndex++] = ackDataBuf[i];
	}

	ackCRC32Data = CRC32DataCheckFun(&TxT113DataBuf[2], ackDataIndex - 2); //生成CRC校验码 
	TxT113DataBuf[ackDataIndex++] = ackCRC32Data;
	TxT113DataBuf[ackDataIndex++] = ackCRC32Data >> 8;
	TxT113DataBuf[ackDataIndex++] = ackCRC32Data >> 16;
	TxT113DataBuf[ackDataIndex++] = ackCRC32Data >> 24;

	HAL_T113_Uart_Send_Data(TxT113DataBuf,ackDataIndex);//串口发送数据到T113
	
//	debug_print_hex(TxT113DataBuf, ackDataIndex);
//	debug_print("\r\n");
}

/*==================================================================================
* 函 数 名： pt113_frame_parse
* 参    数： None
* 功能描述:  主控T113 接收到的数据解析
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 025540
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
	//进行协议解析
	switch(pmsg->slaveCmd)
	{
		case ANDROID_CC_GET_SW_AND_HW_VER://获取版本号
			debug_print("获取版本号 0xA1 \r\n");
			buffer[send_len++] = 0;//当前状态
			buffer[send_len++] = mApp_Param.hard_ver;//硬件版本
			buffer[send_len++] = mApp_Param.soft_ver;//软件版本
			iap_simply_ack(buffer, send_len);//应答
		break;
		
		case ANDROID_CC_GET_WEIGH_UID://获取当前重量值以及卡号信息
			debug_print("读取重量命令。。。。\r\n");	
			//获取标签信息
			p_Tag = get_card_tag_msg();
			if(p_Tag.tag_state == NULL_STA)
				buffer[send_len++] = 0;//无标签
			else if(p_Tag.tag_state == ERR_CARD_STA)
				buffer[send_len++] = 1;//非美餐卡
			else if(p_Tag.tag_state == ENTRY_BLOCK_OK_STA)
				buffer[send_len++] = 2;//正常卡
			else 
				buffer[send_len++] = 0;//无标签
			memcpy(buffer+send_len, p_Tag.uid, 8);
			send_len += 8;
			//绝对重量
			debug_print("净重 = %d \r\n",myUuidWeighSt.weigh);
			memcpy(buffer+send_len,&myUuidWeighSt.weigh,4);
			send_len += 4;

			iap_simply_ack(buffer, send_len);//应答
		break;
							
	
		//固件升级
		case ANDROID_CC_UPDATE_INFO://下发升级包信息 0x80
			debug_print("下发升级包信息 0x80 \r\n");
		  //disable 其他任务
			DisableTask(READ_CARD_TASK);
			DisableTask(P485_REC_TASK);
			DisableTask(TEST_TASK);
			ret_s = pIap_Func->info_opt(pmsg->msgData, &ret_id);
			if(ret_s) {
				buffer[send_len++] = 0x08;//失败
			} else {
				buffer[send_len++] = 0;//成功
			}	
			iap_simply_ack(buffer, send_len);//应答
		break;
		
		case ANDROID_CC_UPDATE_DATA://下发升级包数据 0x81
			debug_print("下发升级包数据 0x81 \r\n");
			ret_s = pIap_Func->data_opt(pmsg->msgData, &ret_id);
			if(ret_s) {
				buffer[send_len++] = 0x08;//失败
			} else {
				buffer[send_len++] = 0;//成功
			}	
			iap_simply_ack(buffer, send_len);//应答
		break;
			
		case ANDROID_CC_UPDATE_CHECK://确认升级 0x84
			debug_print("确认升级 0x84 \r\n");
			ret_s = pIap_Func->check_opt(&ret_id);
			if(ret_s) {
				buffer[send_len++] = 0x08;//失败
			} else {
				buffer[send_len++] = 0;//成功
			}	
			iap_simply_ack(buffer, send_len);//应答
		break;
		
		case ANDROID_CC_UPDATE_LOST://下发缺失升级包数据 0x8A
			debug_print("下发缺失升级包数据 0x8A \r\n");
			ret_s = pIap_Func->patch_opt(pmsg->msgData, &ret_id);
			if(ret_s) {
				buffer[send_len++] = 0x08;//失败
			} else {
				buffer[send_len++] = 0;//成功
			}	
			iap_simply_ack(buffer, send_len);//应答
		break;
		
		case ANDROID_CC_UPDATE_RESET://重启系统 0x88
			debug_print("重启系统 0x88 \r\n");
			buffer[send_len++] = 0;//成功
			iap_simply_ack(buffer, send_len);//应答
			HAL_NVIC_SystemReset();
		break;
	}
}

/*==================================================================================
* 函 数 名： pt113_instance_init
* 参    数： None
* 功能描述:  主控T113串口接收队列初始化
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-10-28 170617
=================================================================================*/
void pt113_instance_init(void)
{
	msg_t113_queue_init(&pt113_queue);
}

/*==================================================================================
* 函 数 名： pt113_push_data_msg
* 参    数： None
* 功能描述:  主控T1113 接收到数据后调用此函数把接收的数据添加的队列里面
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void pt113_push_data_msg(void* ret_msg)
{
	_pT113_Msg pmsg = ret_msg;  
	
	msg_t113_queue_push(&pt113_queue, pmsg);
}

/*==================================================================================
* 函 数 名： pt113_pop_one_frame
* 参    数： _pRet_Msg
* 功能描述:  主控T113数据接收
* 返 回 值： None
* 备    注： None
* 作    者： lc
* 创建时间： 2021-06-20 154449
==================================================================================*/ 
static uint8_t pt113_pop_one_frame(void )
{
	int num;
	p_t113_send_queue_t p_queue_buff = &pt113_queue;
	
	//检查队列里面是否有数据
	num = msg_t113_queue_num(p_queue_buff);
	if(num)
	{
		for(int i=0; i<MAX_CACHE_NUM; i++)
		{
			if(p_queue_buff->queue[i].byte_count)
			{
//				debug_print("队列里面的消息长度 = %d \r\n",p_queue_buff->queue[i].byte_count);
				pt113_frame_parse(&(p_queue_buff->queue[i].data));
				msg_t113_queue_pop(p_queue_buff, 0);
				return 0;
			}	
		}
		//debug_print("队列里面的消息个数 =%d \n",num);
	}
	return 0xff;
}

/*==================================================================================
* 函 数 名： p485_send_data_task
* 参    数： None
* 功能描述:  485 发送数据任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 170658
==================================================================================*/
void pt113_send_data_task(void* argv)
{
	//HAL_485_Send_Data(get_weight_cmd,sizeof(get_weight_cmd));
}
/*==================================================================================
* 函 数 名： pt113_rec_decode_task
* 参    数： None
* 功能描述:  解析T113串口发过来的命令任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 170658
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
//		HAL_T113_Uart_Send_Data(T113_USART_RX_BUF, 8);//串口发送数据到T113
//	}
}
