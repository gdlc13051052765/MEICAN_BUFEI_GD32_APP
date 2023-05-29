#ifndef  __COMM_CAN_H
#define  __COMM_CAN_H
#include <stdint.h>
#include <stdio.h>
#include "p485_fifo.h"

#define MAX_MUTIL_LEN   256 
#define MAX_MUTIL_ITEM_NUM	10

#define RETRY_TIME				3000 //3s没收到回复重发

#define SAME_ID					1
#define SAME_PKG_ID				2



typedef struct
{
	uint8_t is_complete;	//是否接收完成 
	uint8_t in_use;			//是否正在使用  0：空闲 1：使用中
	uint8_t recv_pkg_num;		//记录接收数据帧的数量
	uint8_t r_len;
	uint8_t r_data[MAX_MUTIL_LEN];
}_Mutil_Ring,*_pMutil_Ring;
  

typedef struct
{ 
	//uint8_t  toalLen;//总长度
	uint16_t header;//包头
	uint8_t	 ext_id;//外设ID
	uint8_t  date_len;//实际缓存中的长度
	uint8_t	 slid;
	uint8_t  mainCmd;//主指令
	uint8_t  nodeId;
	uint8_t  slaveCmd;//从指令
	uint8_t  msgData[106];//信息数据
	uint32_t crc32;
}_Ret_Msg,*_pRet_Msg;		//包含连包和单包

typedef struct
{
	uint8_t  tbale_index;	//表索引值，对应缓存
}_Mutil_Table,*_pMutil_Table;

typedef struct
{ 
	uint8_t  byte_count;		//实际缓存中的长度
	uint8_t  data[MAX_MUTIL_LEN];
}_Ack_Msg,*_pAck_Msg;		//包含连包和单包


#endif
