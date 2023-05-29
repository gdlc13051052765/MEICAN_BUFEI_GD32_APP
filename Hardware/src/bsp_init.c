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
* 函 数 名： get_mcu_id
* 参    数： 
* 功能描述:  获取mcu四个字节ID
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-02-05 044124
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
* 函 数 名： bsp_init
* 参    数： None
* 功能描述:  板级初始化
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-05-29 121426
==================================================================================*/
void bsp_init()
{
	HAL_Delay(1000);
	MX_GPIO_Init();
	led_power_gpio_init();
	MX_DMA_Init();
	T113_USART_UART_Init(115200);//跟主控T113通讯串口
	W485_USART_UART_Init(9600);//485串口称重传感器
	DEBUG_USART_UART_Init(115200);//打印串口
	nfc_init();
	w25qxx_spi_init();
}
