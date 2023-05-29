#include <pT113_fifo.h>


void msg_t113_queue_init(p_t113_send_queue_t p_queue_buff)
{
  memset(p_queue_buff, 0, sizeof(t113_send_queue_t));
}

unsigned int msg_t113_queue_num(p_t113_send_queue_t p_queue_buff)
{
	return p_queue_buff->count;
}

static volatile unsigned char queue_mutex=0;
void msg_t113_queue_push(p_t113_send_queue_t p_queue_buff, _t113_Msg *data)
{
	if(queue_mutex == 0)
	{
		queue_mutex = 1;
		if (p_queue_buff->count >= MAX_T113_CACHE_NUM)
		{
			memset(&p_queue_buff->queue[p_queue_buff->rd], 0, sizeof(_t113_Msg));
			p_queue_buff->rd = (p_queue_buff->rd + 1) % MAX_T113_CACHE_NUM;
			//log_printf("Delete the oldest member \r\n");
		} else {
			p_queue_buff->count++;
			if(p_queue_buff->count>MAX_T113_CACHE_NUM)
			{
				
			}
		}
		memcpy(&p_queue_buff->queue[p_queue_buff->wp], data, sizeof(_t113_Msg));

		p_queue_buff->wp++;
		if(p_queue_buff->wp >= MAX_T113_CACHE_NUM)
			p_queue_buff->wp = 0;
		queue_mutex = 0;
	}
	else
	{
	//log_printf("queue is locked by others.\r\n");
	}
}

void msg_t113_queue_pop(p_t113_send_queue_t p_queue_buff, uint8_t msg_id)
{
	memset(&p_queue_buff->queue[p_queue_buff->rd], 0, sizeof(_t113_Msg));
	p_queue_buff->count--;
	p_queue_buff->rd++;
	if(p_queue_buff->rd >= MAX_T113_CACHE_NUM)
		p_queue_buff->rd = 0;
}
