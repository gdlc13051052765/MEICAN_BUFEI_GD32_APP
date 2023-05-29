#ifndef  __SERIAL_H
#define  __SERIAL_H
 
#include <stdint.h>
#include "serial_def_fifo.h"

//跟T113的包头包尾
#define STX 0x02
#define ETX 0x03
#define MAX_USART_REC_LEN  			150  		//定义最大接收字节数 200
#define RXBUFFERSIZE 1
extern DMA_HandleTypeDef hdma_usart1_rx;
extern uint8_t T113_USART_RX_BUF[MAX_USART_REC_LEN]; //串口1接收数据
 //主控串口句柄
extern UART_HandleTypeDef t113_huart;

typedef struct
{
	uint8_t rev_complete;		//接收完成
	void* pDev;
	
	void* p_sed_fifo;
	void* p_rev_fifo;
	_pSerial_Fifo_Func p_ffunc;	//fifo函数接口
}_Serial_Info,*_pSerial_Info;

void	T113_USART_UART_Init(int);//跟V3S通讯串口
void  W485_USART_UART_Init(int);//485串口
void	DEBUG_USART_UART_Init(int);//打印串口
/*==================================================================================
* 函 数 名： USART_Send_Data
* 参    数： None
* 功能描述:  串口发送函数
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-09-17 162343
==================================================================================*/ 
void HAL_USART_Send_Data(UART_HandleTypeDef *huart, uint8_t *USART_RX_BUF, int len);  
/*==================================================================================
* 函 数 名： USART_Send_Data
* 参    数： None
* 功能描述:  串口发送函数
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-09-17 162343
==================================================================================*/ 
void HAL_485_Send_Data( uint8_t *USART_RX_BUF, int len) ;

/*==================================================================================
* 函 数 名： HAL_T113_Uart_Send_Data
* 参    数： None
* 功能描述:  发送数据到主控
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-09-17 162343
==================================================================================*/ 
void HAL_T113_Uart_Send_Data( uint8_t *USART_RX_BUF, int len)   ;

/*==================================================================================
* 函 数 名： clear_485_buf
* 参    数： None
* 功能描述:  清空485缓存
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-09-17 162343
==================================================================================*/ 
void clear_485_buf(void)  ;

void serial_register_init(void *instance, void* pdev);
void serial_sed_buff(void *instance, uint8_t *buff, uint16_t len);
uint8_t serial_push_byte(void *instance, uint8_t value);
#endif
