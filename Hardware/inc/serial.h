#ifndef  __SERIAL_H
#define  __SERIAL_H
 
#include <stdint.h>
#include "serial_def_fifo.h"

//��T113�İ�ͷ��β
#define STX 0x02
#define ETX 0x03
#define MAX_USART_REC_LEN  			150  		//�����������ֽ��� 200
#define RXBUFFERSIZE 1
extern DMA_HandleTypeDef hdma_usart1_rx;
extern uint8_t T113_USART_RX_BUF[MAX_USART_REC_LEN]; //����1��������
 //���ش��ھ��
extern UART_HandleTypeDef t113_huart;

typedef struct
{
	uint8_t rev_complete;		//�������
	void* pDev;
	
	void* p_sed_fifo;
	void* p_rev_fifo;
	_pSerial_Fifo_Func p_ffunc;	//fifo�����ӿ�
}_Serial_Info,*_pSerial_Info;

void	T113_USART_UART_Init(int);//��V3SͨѶ����
void  W485_USART_UART_Init(int);//485����
void	DEBUG_USART_UART_Init(int);//��ӡ����
/*==================================================================================
* �� �� ���� USART_Send_Data
* ��    ���� None
* ��������:  ���ڷ��ͺ���
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void HAL_USART_Send_Data(UART_HandleTypeDef *huart, uint8_t *USART_RX_BUF, int len);  
/*==================================================================================
* �� �� ���� USART_Send_Data
* ��    ���� None
* ��������:  ���ڷ��ͺ���
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void HAL_485_Send_Data( uint8_t *USART_RX_BUF, int len) ;

/*==================================================================================
* �� �� ���� HAL_T113_Uart_Send_Data
* ��    ���� None
* ��������:  �������ݵ�����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void HAL_T113_Uart_Send_Data( uint8_t *USART_RX_BUF, int len)   ;

/*==================================================================================
* �� �� ���� clear_485_buf
* ��    ���� None
* ��������:  ���485����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void clear_485_buf(void)  ;

void serial_register_init(void *instance, void* pdev);
void serial_sed_buff(void *instance, uint8_t *buff, uint16_t len);
uint8_t serial_push_byte(void *instance, uint8_t value);
#endif
