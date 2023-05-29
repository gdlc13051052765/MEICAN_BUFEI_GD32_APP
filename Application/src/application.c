#include "application.h"
#include "includes.h"
#include "iap_protocols.h"

_App_Param mApp_Param = 
{ 
	.can_addr 		= 0x00,
	.soft_ver			= 0x00,  
	.hard_ver			= 0xFF,
};
 
_Serial_Info mSerial_Info;
 
/**
  * @brief System Clock Configuration
  * @retval None
  */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
	
	//HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTickƵ��ΪHCLK
}

/*==================================================================================
* �� �� ���� systerm_init
* ��    ���� None
* ��������:  ϵͳ��ʼ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 121318
==================================================================================*/
void systerm_init(void)
{
	//�ж��������ʼ��
	#if EN_DEBUG
		SCB->VTOR = FLASH_BASE | 0X02000; /* Vector Table Relocation in Internal FLASH. */
	#else
		
	#endif
	__disable_irq();        //�ر����ж�
	__enable_irq();         //�����ж�
	HAL_Init();
	//SystemClock_Config();
//	__enable_irq();         //�����ж�
	/*===========�弶��ʼ��==========*/
	bsp_init();
	st_crc_init(); 
	/*===========�弶��ʼ��==========*/ 
	//��ȡ���ò���
	read_config_param(&mApp_Param);

	debug_print("hard version  = %d\r\n", mApp_Param.hard_ver );
	debug_print("soft version  = %d\r\n", mApp_Param.soft_ver );
	/*============����ϵͳ�ж�========*/
	//	HAL_NVIC_EnableIRQ(EXTI0_IRQn); 
	//	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	//pIap_Func->init();//����̼�������־λ
	debug_print("bu fei systerm init is complete!\r\n");
}

/*==================================================================================
* �� �� ���� app_loop_task
* ��    ���� None
* ��������:  ����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-17 141735
==================================================================================*/ 
void app_dispatch(void)
{
	//��ʱ�������
	TaskProcess(0);  
	
	//CANЭ�����
	//can1_rev_decode();
	//can_sed_loop_check(hcan1);
}


/*==================================================================================
* �� �� ���� HAL_SysTick_Callback
* ��    ���� None
* ��������:  ϵͳ�δ�ص�
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-06 115746
==================================================================================*/
void HAL_SysTick_Callback(void)
{
	//�������
	TaskRemarks();
}

typedef struct
{
    uint16_t Head;
    uint16_t len;
    uint8_t instruction;
    uint8_t data[150];
    uint32_t crc;
}SlaveTestCmdTypeDef;

uint8_t Slave_SendTestCmd(void)
{
    SlaveTestCmdTypeDef TestCmd;
    TestCmd.Head = 0xAA55;
    TestCmd.len = sizeof(SlaveTestCmdTypeDef) - 9;
    TestCmd.instruction = 0x04;
    memset(TestCmd.data, 0xff, 150);
    TestCmd.crc = st_crc32(((uint8_t *) &TestCmd ) + 5,sizeof(SlaveTestCmdTypeDef) - 9);

//    Slave_AddTranceiverMessage((uint8_t *) &TestCmd,sizeof(SlaveTestCmdTypeDef),0);
	//si_sed_buff_to(0x0E, 0X55, (uint8_t *) &TestCmd, sizeof(SlaveTestCmdTypeDef), 1);  
	
	return 0;
}

/*==================================================================================
* �� �� ���� test_loop_task
* ��    ���� None
* ��������:  ��������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-29 170658
==================================================================================*/
void test_loop_task(void* argv)
{
	//uint8_t TxT113DataBuf[5] = {0x11,0x22,0x33,0x44,0x55};
	set_run_led_toggle();
	//HAL_T113_Uart_Send_Data(TxT113DataBuf,5);//���ڷ������ݵ�T113
	debug_print("�������� hard version  = %d; soft version  = %d\r\n", mApp_Param.hard_ver, mApp_Param.soft_ver);
//	debug_print("hard version  = %d\r\n", mApp_Param.hard_ver );
//	debug_print("soft version  = %d\r\n", mApp_Param.soft_ver );
}
