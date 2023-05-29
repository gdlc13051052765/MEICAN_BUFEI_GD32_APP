#include "serial.h"
#include "main.h"
#include "stm32f1xx_hal_uart.h"
#include "includes.h"
#include "hwioconfig.h"
#include <p485_fifo.h>
#include "pT113_fifo.h"

//���԰������
#ifdef JINGYUAN_MODE	
	#define STX_485  0x55//485���ݰ�ͷ
#endif

//BT559A������
#ifdef BT559A_MODE
	#define STX_485  0x05//232���ݰ�ͷ
#endif


static uint8_t uart1_rev_buff[RXBUFFERSIZE] = {0};		//���ջ���
static uint8_t uart1_sed_buff[RXBUFFERSIZE] = {0};		//���ͻ���
//static uint8_t uart2_rev_buff[RXBUFFERSIZE] = {0};		//���ջ���
//static uint8_t uart2_sed_buff[RXBUFFERSIZE] = {0};		//���ͻ���
//static uint16_t USARTq_RX_STA=0;       //����״̬���	 
static uint8_t SerialReCase = 0;//���ڽ�������״̬��
//static uint8_t data485Len  =0;//485�����򳤶�

static uint8_t V3sReCase = 0;//���ڽ�������״̬��
//static uint8_t dataV3SLen  =0;//485�����򳤶�


uint8_t t113_usart_rec_len = 0;//����1�������ݳ���
uint8_t w485_usart_rec_len = 0;//����1�������ݳ���
uint8_t T113_USART_RX_BUF[MAX_USART_REC_LEN]; //����1��������
uint8_t W485_USART_RX_BUF[MAX_USART_REC_LEN]; //����2��������

//���ش��ھ��
UART_HandleTypeDef t113_huart;
//485���������ھ��
UART_HandleTypeDef w485_huart;
//��ӡ���ھ��
UART_HandleTypeDef debug_huart;
//UART DMA
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;

/*==================================================================================
* �� �� ���� serial_init
* ��    ���� None
* ��������:  ���ڳ�ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void serial_register_init(void *instance, void* pdev)
{ 
	_pSerial_Info pthis = (_pSerial_Info)instance;
	
	pthis->p_ffunc = pSerial_Fifo_Func;
	pthis->pDev = pdev;
	pthis->p_rev_fifo = pthis->p_ffunc->init_m(uart1_rev_buff, sizeof(uart1_rev_buff));
	pthis->p_sed_fifo = pthis->p_ffunc->init_m(uart1_sed_buff, sizeof(uart1_sed_buff)); 
}

/*==================================================================================
* �� �� ���� serial_sed_buff
* ��    ���� None
* ��������:  ���ڷ�������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void serial_sed_byte(void *instance, uint8_t value)
{
	_pSerial_Info pthis = (_pSerial_Info)instance;
	USART_TypeDef * serial = 	(USART_TypeDef *) pthis->pDev;
	 
	serial->DR = value;
	while((serial->SR & UART_IT_TXE) == RESET);
//		USART1->DR = value;
//	while((USART1->SR & UART_IT_TXE) == RESET);
}

/*==================================================================================
* �� �� ���� serial_sed_buff
* ��    ���� None
* ��������:  ���ڷ�������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void serial_sed_buff(void *instance, uint8_t *buff, uint16_t len)
{
	for(int i=0; i<len; i++)
	{
		serial_sed_byte(instance, buff[i]);
	}
}

/*==================================================================================
* �� �� ���� serial_sed_buff
* ��    ���� None
* ��������:  ���ڷ�������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
uint8_t serial_push_byte(void *instance, uint8_t value)
{
	_pSerial_Info pthis = (_pSerial_Info)instance;
	
	return pthis->p_ffunc->push(pthis->p_rev_fifo, &value);
}

/*==================================================================================
* �� �� ���� T113_USART_UART_Init
* ��    ���� None
* ��������:  T113���ش��ڳ�ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void T113_USART_UART_Init(int baud)
{
  t113_huart.Instance = T113_UART;
  t113_huart.Init.BaudRate = baud;
  t113_huart.Init.WordLength = UART_WORDLENGTH_8B;
  t113_huart.Init.StopBits = UART_STOPBITS_1;
  t113_huart.Init.Parity = UART_PARITY_NONE;
  t113_huart.Init.Mode = UART_MODE_TX_RX;
  t113_huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  t113_huart.Init.OverSampling = UART_OVERSAMPLING_16;
  if(HAL_UART_Init(&t113_huart) != HAL_OK)
  {
    Error_Handler();
  }
	//__HAL_UART_ENABLE_IT(&t113_huart, UART_IT_RXNE); //ʹ��IDLE�ж�
	__HAL_UART_ENABLE_IT(&t113_huart, UART_IT_IDLE); //ʹ��IDLE�ж�
	//DMA���պ������˾�һ��Ҫ�ӣ����ӽ��ղ�����һ�δ�������ʵ���ݣ��ǿյģ��Ҵ�ʱ���յ������ݳ���Ϊ�����������ݳ���
	HAL_UART_Receive_DMA(&t113_huart,T113_USART_RX_BUF,MAX_USART_REC_LEN);
}

/*==================================================================================
* �� �� ���� W485_USART_UART_Init
* ��    ���� None
* ��������:  485���������ڳ�ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void W485_USART_UART_Init(int baud)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	//485�������GPIO��ʼ��
	GPIO_InitStruct.Pin = W485_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(W485_DIR_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE END USART2_Init 1 */
  w485_huart.Instance = W485_UART;
  w485_huart.Init.BaudRate = baud;
  w485_huart.Init.WordLength = UART_WORDLENGTH_8B;
  w485_huart.Init.StopBits = UART_STOPBITS_1;
  w485_huart.Init.Parity = UART_PARITY_NONE;
  w485_huart.Init.Mode = UART_MODE_TX_RX;
  w485_huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  //huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&w485_huart) != HAL_OK)
  {
    Error_Handler();
  }
//  /* USER CODE BEGIN USART2_Init 2 */
//	__HAL_UART_ENABLE(&w485_huart);
//  /* USER CODE END USART2_Init 2 */
//	HAL_UART_Receive_IT(&w485_huart, (uint8_t *)uart2_rev_buff, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
//	
	__HAL_UART_ENABLE_IT(&w485_huart, UART_IT_IDLE); //ʹ��IDLE�ж�
	//DMA���պ������˾�һ��Ҫ�ӣ����ӽ��ղ�����һ�δ�������ʵ���ݣ��ǿյģ��Ҵ�ʱ���յ������ݳ���Ϊ�����������ݳ���
	HAL_UART_Receive_DMA(&w485_huart,W485_USART_RX_BUF,MAX_USART_REC_LEN);
}

/*==================================================================================
* �� �� ���� DEBUG_USART_UART_Init
* ��    ���� None
* ��������:  debug���ڳ�ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void DEBUG_USART_UART_Init(int baud)
{
  debug_huart.Instance = DEBUG_UART;
  debug_huart.Init.BaudRate = baud;
  debug_huart.Init.WordLength = UART_WORDLENGTH_8B;
  debug_huart.Init.StopBits = UART_STOPBITS_1;
  debug_huart.Init.Parity = UART_PARITY_NONE;
  debug_huart.Init.Mode = UART_MODE_TX_RX;
  debug_huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  debug_huart.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&debug_huart) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
	__HAL_UART_ENABLE(&debug_huart);
  /* USER CODE END USART1_Init 2 */
}

/*==================================================================================
* �� �� ���� USART_Send_Data
* ��    ���� None
* ��������:  ���ڷ��ͺ���
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void HAL_USART_Send_Data(UART_HandleTypeDef *huart, uint8_t *USART_RX_BUF, int len)    
{
	if(huart->Instance==T113_UART)//����Ǵ���1
	{
		HAL_UART_Transmit(&t113_huart,USART_RX_BUF,len,100);	//���ͽ��յ�������
		while(__HAL_UART_GET_FLAG(&t113_huart,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���
	}
	if(huart->Instance==W485_UART)//����Ǵ���2
	{
		HAL_UART_Transmit(&w485_huart,USART_RX_BUF,len,100);	//���ͽ��յ�������
		while(__HAL_UART_GET_FLAG(&w485_huart,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���
	}
	if(huart->Instance==DEBUG_UART)//����Ǵ���3
	{
		HAL_UART_Transmit(&debug_huart,USART_RX_BUF,len,100);	//���ͽ��յ�������
		while(__HAL_UART_GET_FLAG(&debug_huart,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���
	}
}

/*==================================================================================
* �� �� ���� USART_Send_Data
* ��    ���� None
* ��������:  ���ڷ��ͺ���
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void HAL_485_Send_Data( uint8_t *USART_RX_BUF, int len)   
{
	HAL_GPIO_WritePin(W485_DIR_GPIO_Port,W485_DIR_Pin,GPIO_PIN_SET); 
	
	HAL_USART_Send_Data(&w485_huart,USART_RX_BUF,len);
	
	HAL_GPIO_WritePin(W485_DIR_GPIO_Port,W485_DIR_Pin,GPIO_PIN_RESET);
}

/*==================================================================================
* �� �� ���� HAL_T113_Uart_Send_Data
* ��    ���� None
* ��������:  �������ݵ�����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void HAL_T113_Uart_Send_Data( uint8_t *USART_RX_BUF, int len)   
{
	HAL_USART_Send_Data(&t113_huart,USART_RX_BUF,len);
}

/*==================================================================================
* �� �� ���� HAL_UART_RxCpltCallback
* ��    ���� None
* ��������:  ���ڽ��ջص�����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//_485_Msg pmsg;
	_t113_Msg pT113msg;
	
	if(huart->Instance==T113_UART)//����Ǵ���1//����ͨѶ����
	{
		if(!V3sReCase)
		{
			if(uart1_rev_buff[0]==STX)
			{
				t113_usart_rec_len = 0;
				//USART1_RX_BUF[usart1_rec_len++]  = uart1_rev_buff[0] ;
				V3sReCase=1;	
			}
		}
		else if(V3sReCase == 1)
		{
			T113_USART_RX_BUF[t113_usart_rec_len++]  = uart1_rev_buff[0] ;
			if(uart1_rev_buff[0] == ETX)
			{
				pT113msg.byte_count = t113_usart_rec_len-1;
				memcpy(pT113msg.data, T113_USART_RX_BUF, t113_usart_rec_len-1);
				pt113_push_data_msg(&pT113msg);
				t113_usart_rec_len = 0;
				memset(T113_USART_RX_BUF,0,2);
				V3sReCase = 0;
			}
		}
	}
	
	if(huart->Instance==W485_UART)//����Ǵ���2 ���ش���������
	{
		//���԰���ش�����
		#ifdef JINGYUAN_MODE	
			if (!SerialReCase)
			{
				if (uart2_rev_buff[0]==STX_485)
				{
					w485_usart_rec_len = 0;
					W485_USART_RX_BUF[w485_usart_rec_len++]  = uart2_rev_buff[0] ;
					SerialReCase=1;	
				}
			}
			else if(SerialReCase == 1)
			{
				W485_USART_RX_BUF[w485_usart_rec_len++]  = uart2_rev_buff[0] ;
				if(w485_usart_rec_len == 5)//�����������򳤶�
				{
					data485Len = (uint16_t)W485_USART_RX_BUF[4]*256 + W485_USART_RX_BUF[3];
					SerialReCase = 2;
				}
			}
			else if(SerialReCase == 2)
			{
				W485_USART_RX_BUF[w485_usart_rec_len++]  = uart2_rev_buff[0] ;
				if(w485_usart_rec_len == data485Len+7)
				{
					SerialReCase = 0;
					pmsg.byte_count = w485_usart_rec_len;
					memcpy(pmsg.data, W485_USART_RX_BUF, w485_usart_rec_len);
					//HAL_USART_Send_Data(&huart1,USART1_RX_BUF,usart1_rec_len);
					p485_push_data_msg(&pmsg);
				}
			}
		#endif

		//BT559A���ش����� �������ݳ���7
		#ifdef BT559A_MODE
			if(!SerialReCase)
			{
				if (uart2_rev_buff[0]==STX_485)
				{
					w485_usart_rec_len = 0;
					W485_USART_RX_BUF[w485_usart_rec_len++] = uart2_rev_buff[0] ;
					SerialReCase=1;	
				}
			}
			else if(SerialReCase == 1)
			{
				W485_USART_RX_BUF[w485_usart_rec_len++] = uart2_rev_buff[0] ;
				if(w485_usart_rec_len == 7)//�����������򳤶�
				{
					SerialReCase = 0;
					pmsg.byte_count = w485_usart_rec_len;
					memcpy(pmsg.data, W485_USART_RX_BUF, w485_usart_rec_len);	
					p485_push_data_msg(&pmsg);
				}
			}
		#endif
	}
}
/*==================================================================================
* �� �� ���� clear_485_buf
* ��    ���� None
* ��������:  ���485����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void clear_485_buf(void)   
{
	SerialReCase = 0;
}
/*==================================================================================
* �� �� ���� USART1_IRQHandler
* ��    ���� None
* ��������:  ����1�ж�
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
	uint32_t temp =0;
	uint8_t rx_len = 0,recv_end_flag = 0;
	
void USART1_IRQHandler(void)                	
{ 
	_t113_Msg pT113msg;
	
	HAL_UART_IRQHandler(&t113_huart);
	if(__HAL_UART_GET_FLAG(&t113_huart,UART_FLAG_IDLE)!= RESET)//������ܵ���һ֡����
	{ 
		__HAL_UART_CLEAR_IDLEFLAG(&t113_huart);//�����־λ
		//temp = huart1.Instance->SR;  //���״̬�Ĵ���SR,��ȡSR�Ĵ�������ʵ�����SR�Ĵ����Ĺ���
		//temp = huart1.Instance->DR; //��ȡ���ݼĴ����е�����
		HAL_UART_DMAStop(&t113_huart); //
		temp =  __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);// ��ȡDMA��δ��������ݸ���   
		//temp = hdma_usart1_rx.Instance->CNDTR ;//��ȡNDTR�Ĵ��� ��ȡDMA��δ��������ݸ�����
		//���������Ǿ��Ч
		rx_len = MAX_USART_REC_LEN - temp; //�ܼ�����ȥδ��������ݸ������õ��Ѿ����յ����ݸ���

		recv_end_flag = 1;	// ������ɱ�־λ��1	
		HAL_UART_Receive_DMA(&t113_huart,T113_USART_RX_BUF,MAX_USART_REC_LEN);
		pT113msg.byte_count = rx_len;
		memcpy(pT113msg.data, T113_USART_RX_BUF, rx_len);
		pt113_push_data_msg(&pT113msg);
	}
} 

/*==================================================================================
* �� �� ���� USART2_IRQHandler
* ��    ���� None
* ��������:  ����2�ж�
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void USART2_IRQHandler(void)                	
{ 
	_485_Msg pmsg;

	HAL_UART_IRQHandler(&w485_huart);	//����HAL���жϴ����ú���
	
	if(__HAL_UART_GET_FLAG(&w485_huart,UART_FLAG_IDLE)!= RESET)//������ܵ���һ֡����
	{ 
		__HAL_UART_CLEAR_IDLEFLAG(&w485_huart);//�����־λ
		HAL_UART_DMAStop(&w485_huart); //
		temp =  __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);// ��ȡDMA��δ��������ݸ���   
		//���������Ǿ��Ч
		rx_len = MAX_USART_REC_LEN - temp; //�ܼ�����ȥδ��������ݸ������õ��Ѿ����յ����ݸ���

		HAL_UART_Receive_DMA(&w485_huart,W485_USART_RX_BUF,MAX_USART_REC_LEN);
		pmsg.byte_count = rx_len;
		memcpy(pmsg.data, W485_USART_RX_BUF, rx_len);	
		p485_push_data_msg(&pmsg);
	}
} 


//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  

/* USER CODE BEGIN 1 */
int fputc(int ch, FILE *f)
{
 
  unsigned char temp;
	temp=(unsigned char)ch;
  //USART_SendData(USART1, (unsigned char) ch);
	HAL_UART_Transmit(&debug_huart,&temp,1,1000);
  //while (!(USART1->SR & USART_FLAG_TXE));
 
  return (ch);
}
/* USER CODE END 1 */

/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
//int fgetc(FILE * f)
//{
//  uint8_t ch = 0;
//  ch = debug_read();
//  return ch;
//}

void print_hex(uint8_t *buff, uint16_t len)
{
	for(int i=0; i<len; i++)
	{
		debug_print("%02x ", buff[i]);
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


