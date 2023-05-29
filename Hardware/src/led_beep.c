#include "led_beep.h"
#include "hwioconfig.h"

/*==================================================================================
* 函 数 名： led_power_gpio_init
* 参    数： 
* 功能描述:  灯板，电源控制gpio初始化
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-10-18 160210
==================================================================================*/
void led_power_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	//led灯板GPIO初始化
	GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);
	
	//运行指示灯GPIO初始化
	GPIO_InitStruct.Pin = RUN_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RUN_LED_GPIO_Port, &GPIO_InitStruct);
	
	//主控板电源GPIO初始化
	GPIO_InitStruct.Pin = T113_POWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(T113_POWER_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(T113_POWER_GPIO_Port,T113_POWER_Pin,GPIO_PIN_SET);  //默认打开T113主控供电
	
	//称重传感器控制GPIO初始化
	GPIO_InitStruct.Pin = WEIGH_POWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WEIGH_POWER_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(WEIGH_POWER_GPIO_Port,WEIGH_POWER_Pin,GPIO_PIN_SET);  //默认打开称重传感器电源
}
/*==================================================================================
* 函 数 名： open_t113_power
* 参    数： type == 1打开电源;type == 0关闭电源
* 功能描述:  打开主控T113电源
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-10-18 160210
==================================================================================*/
void open_t113_power(uint8_t type)
{
	if(type)
		HAL_GPIO_WritePin(T113_POWER_GPIO_Port,T113_POWER_Pin,GPIO_PIN_SET);  
	else	
		HAL_GPIO_WritePin(T113_POWER_GPIO_Port,T113_POWER_Pin,GPIO_PIN_RESET);  
}

/*==================================================================================
* 函 数 名： control_led_board_power
* 参    数： value
* 功能描述:  控制led灯板电源
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-10-18 160222
==================================================================================*/
void control_led_board_power(uint8_t value)
{
	
}

/*==================================================================================
* 函 数 名： open_weigh_power
* 参    数： value =1 打开 value =0关闭
* 功能描述:  打开称重传感器控制电源
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-10-18 160222
==================================================================================*/
void open_weigh_power(uint8_t value)
{
	if(value)
		HAL_GPIO_WritePin(WEIGH_POWER_GPIO_Port,WEIGH_POWER_Pin,GPIO_PIN_SET);  
	else	
		HAL_GPIO_WritePin(WEIGH_POWER_GPIO_Port,WEIGH_POWER_Pin,GPIO_PIN_RESET);  
}

/*==================================================================================
* 函 数 名： set_run_led_state
* 参    数： None
* 功能描述:  设置运行led灯状态
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-09-18 120135
==================================================================================*/
void set_run_led_state(uint8_t type)
{
	if(type)
		HAL_GPIO_WritePin(RUN_LED_GPIO_Port,RUN_LED_Pin,GPIO_PIN_SET);  
	else	
		HAL_GPIO_WritePin(RUN_LED_GPIO_Port,RUN_LED_Pin,GPIO_PIN_RESET);  
}

/*==================================================================================
* 函 数 名： set_run_led_toggle
* 参    数： None
* 功能描述:  设置运行led灯交替闪烁
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-09-18 120135
==================================================================================*/
void set_run_led_toggle(void)
{
	HAL_GPIO_TogglePin(RUN_LED_GPIO_Port, RUN_LED_Pin);
	//测试灯板
//	HAL_GPIO_TogglePin(WEIGH_POWER_GPIO_Port,WEIGH_POWER_Pin);
//	HAL_GPIO_TogglePin(T113_POWER_GPIO_Port,T113_POWER_Pin);
//	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
//	HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
//	HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
}
