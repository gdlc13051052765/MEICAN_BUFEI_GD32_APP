#include "led_beep.h"
#include "hwioconfig.h"

/*==================================================================================
* �� �� ���� led_power_gpio_init
* ��    ���� 
* ��������:  �ư壬��Դ����gpio��ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-10-18 160210
==================================================================================*/
void led_power_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	//led�ư�GPIO��ʼ��
	GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);
	
	//����ָʾ��GPIO��ʼ��
	GPIO_InitStruct.Pin = RUN_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RUN_LED_GPIO_Port, &GPIO_InitStruct);
	
	//���ذ��ԴGPIO��ʼ��
	GPIO_InitStruct.Pin = T113_POWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(T113_POWER_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(T113_POWER_GPIO_Port,T113_POWER_Pin,GPIO_PIN_SET);  //Ĭ�ϴ�T113���ع���
	
	//���ش���������GPIO��ʼ��
	GPIO_InitStruct.Pin = WEIGH_POWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WEIGH_POWER_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(WEIGH_POWER_GPIO_Port,WEIGH_POWER_Pin,GPIO_PIN_SET);  //Ĭ�ϴ򿪳��ش�������Դ
}
/*==================================================================================
* �� �� ���� open_t113_power
* ��    ���� type == 1�򿪵�Դ;type == 0�رյ�Դ
* ��������:  ������T113��Դ
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-10-18 160210
==================================================================================*/
void open_t113_power(uint8_t type)
{
	if(type)
		HAL_GPIO_WritePin(T113_POWER_GPIO_Port,T113_POWER_Pin,GPIO_PIN_SET);  
	else	
		HAL_GPIO_WritePin(T113_POWER_GPIO_Port,T113_POWER_Pin,GPIO_PIN_RESET);  
}

/*==================================================================================
* �� �� ���� control_led_board_power
* ��    ���� value
* ��������:  ����led�ư��Դ
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-10-18 160222
==================================================================================*/
void control_led_board_power(uint8_t value)
{
	
}

/*==================================================================================
* �� �� ���� open_weigh_power
* ��    ���� value =1 �� value =0�ر�
* ��������:  �򿪳��ش��������Ƶ�Դ
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-10-18 160222
==================================================================================*/
void open_weigh_power(uint8_t value)
{
	if(value)
		HAL_GPIO_WritePin(WEIGH_POWER_GPIO_Port,WEIGH_POWER_Pin,GPIO_PIN_SET);  
	else	
		HAL_GPIO_WritePin(WEIGH_POWER_GPIO_Port,WEIGH_POWER_Pin,GPIO_PIN_RESET);  
}

/*==================================================================================
* �� �� ���� set_run_led_state
* ��    ���� None
* ��������:  ��������led��״̬
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-18 120135
==================================================================================*/
void set_run_led_state(uint8_t type)
{
	if(type)
		HAL_GPIO_WritePin(RUN_LED_GPIO_Port,RUN_LED_Pin,GPIO_PIN_SET);  
	else	
		HAL_GPIO_WritePin(RUN_LED_GPIO_Port,RUN_LED_Pin,GPIO_PIN_RESET);  
}

/*==================================================================================
* �� �� ���� set_run_led_toggle
* ��    ���� None
* ��������:  ��������led�ƽ�����˸
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-18 120135
==================================================================================*/
void set_run_led_toggle(void)
{
	HAL_GPIO_TogglePin(RUN_LED_GPIO_Port, RUN_LED_Pin);
	//���Եư�
//	HAL_GPIO_TogglePin(WEIGH_POWER_GPIO_Port,WEIGH_POWER_Pin);
//	HAL_GPIO_TogglePin(T113_POWER_GPIO_Port,T113_POWER_Pin);
//	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
//	HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
//	HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
}
