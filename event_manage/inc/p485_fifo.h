#ifndef __CAN_FIFO_H
#define __CAN_FIFO_H

#include <stdint.h>
#include <string.h>
#include "can_drv.h"

#define MAX_485_CACHE_LEN			200	//UDP发送缓存
#define MAX_CACHE_NUM				  10	//最大缓存池

typedef struct 
{ 
	uint8_t byte_count;
	uint8_t  data[MAX_485_CACHE_LEN];
}_485_Msg, *_p485_Msg;


typedef struct send_queue {
	_485_Msg queue[MAX_CACHE_NUM];
	char wp;
	char rd;
	char count;
}send_queue_t, * p_send_queue_t;

void msg_485_queue_init(p_send_queue_t p_queue_buff);
unsigned int msg_485_queue_num(p_send_queue_t p_queue_buff);
void msg_485_queue_push(p_send_queue_t p_queue_buff, _485_Msg *data);
void msg_485_queue_pop(p_send_queue_t p_queue_buff, uint8_t msg_id);
/*==================================================================================
* 函 数 名： p485_frame_parse
* 参    数： None
* 功能描述:  485 接收到的数据解析
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 025540
==================================================================================*/
void clear_weigh_flag(void);

#endif
