#ifndef __PT113_FIFO_H
#define __PT113_FIFO_H

#include <stdint.h>
#include <string.h>


#define MAX_T113_CACHE_LEN					200	//UDP发送缓存
#define MAX_T113_CACHE_NUM				  10	//最大缓存池

typedef struct 
{ 
	uint8_t byte_count;
	uint8_t  data[MAX_T113_CACHE_LEN];
}_t113_Msg, *_pT113_Msg;


typedef struct t113_send_queue {
	_t113_Msg queue[MAX_T113_CACHE_NUM];
	char wp;
	char rd;
	char count;
}t113_send_queue_t, * p_t113_send_queue_t;

void msg_t113_queue_init(p_t113_send_queue_t p_queue_buff);
unsigned int msg_t113_queue_num(p_t113_send_queue_t p_queue_buff);
void msg_t113_queue_push(p_t113_send_queue_t p_queue_buff, _t113_Msg *data);
void msg_t113_queue_pop(p_t113_send_queue_t p_queue_buff, uint8_t msg_id);

#endif
