#ifndef  __COMM_CAN_H
#define  __COMM_CAN_H
#include <stdint.h>
#include <stdio.h>
#include "p485_fifo.h"

#define MAX_MUTIL_LEN   256 
#define MAX_MUTIL_ITEM_NUM	10

#define RETRY_TIME				3000 //3sû�յ��ظ��ط�

#define SAME_ID					1
#define SAME_PKG_ID				2



typedef struct
{
	uint8_t is_complete;	//�Ƿ������� 
	uint8_t in_use;			//�Ƿ�����ʹ��  0������ 1��ʹ����
	uint8_t recv_pkg_num;		//��¼��������֡������
	uint8_t r_len;
	uint8_t r_data[MAX_MUTIL_LEN];
}_Mutil_Ring,*_pMutil_Ring;
  

typedef struct
{ 
	//uint8_t  toalLen;//�ܳ���
	uint16_t header;//��ͷ
	uint8_t	 ext_id;//����ID
	uint8_t  date_len;//ʵ�ʻ����еĳ���
	uint8_t	 slid;
	uint8_t  mainCmd;//��ָ��
	uint8_t  nodeId;
	uint8_t  slaveCmd;//��ָ��
	uint8_t  msgData[106];//��Ϣ����
	uint32_t crc32;
}_Ret_Msg,*_pRet_Msg;		//���������͵���

typedef struct
{
	uint8_t  tbale_index;	//������ֵ����Ӧ����
}_Mutil_Table,*_pMutil_Table;

typedef struct
{ 
	uint8_t  byte_count;		//ʵ�ʻ����еĳ���
	uint8_t  data[MAX_MUTIL_LEN];
}_Ack_Msg,*_pAck_Msg;		//���������͵���


#endif
