#include "bsp_init.h"
#include "can_drv.h"
#include "application.h"
#include "led_beep.h"
#include "main.h"
#include "command.h"
#include "myiic.h"
#include "RC663.h"
#include "gd25q_driver.h"
#include "ex_flash.h"
#include "dma.h"
#include "nfc_interface.h"
/*==================================================================================
* �� �� ���� get_mcu_id
* ��    ���� 
* ��������:  ��ȡmcu�ĸ��ֽ�ID
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-02-05 044124
==================================================================================*/
uint32_t get_mcu_id(void)
{
	return *(__IO uint32_t*)(0X1FFFF7F0); 
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
}

/*==================================================================================
* �� �� ���� bsp_init
* ��    ���� None
* ��������:  �弶��ʼ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-05-29 121426
==================================================================================*/
void bsp_init()
{
	HAL_Delay(1000);
	MX_GPIO_Init();
	led_power_gpio_init();
	MX_DMA_Init();
	T113_USART_UART_Init(115200);//������T113ͨѶ����
	W485_USART_UART_Init(9600);//485���ڳ��ش�����
	DEBUG_USART_UART_Init(115200);//��ӡ����
	nfc_init();
	w25qxx_spi_init();
}
