#include "mini_dispatch.h"
#include "command.h"
#include "RC663.h"
//#include "stm32f1xx_hal_uart.h"

extern void test_loop_task(void* argv);
extern void get_temperature_task(void* argv);

/*==================================================================================
* �� �� ���� TaskComps
* ��    ���� None
* ��������:  �����
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2017-7-28 15:12:42
==================================================================================*/
TASK_COMPONENTS TaskComps[] = 
{
	{0, TASK_ENABLE, 0, 50, 	50, 	read_card_task},    			//���ȼ� 1 ��ʱѰ������
	{0, TASK_ENABLE, 0, 20, 	20, 	pt113_rec_decode_task},   //���ȼ� 2 �������ط������Ĵ�������
	
	//��ݸ��Զ���ش����� 
	#ifdef ZHI_YUAN_TYPE
		{0, TASK_ENABLE, 0, 20, 	20, 	p485_send_data_task},    //���ȼ� 4 ��ʱ���ͻ�ȡ����
	#endif
		
	{0, TASK_ENABLE, 0, 20, 	20, 	p485_rec_decode_task},    //���ȼ� 3 ��ʱ�������ش��������� 
	{0, TASK_ENABLE, 0, 1000, 1000, test_loop_task},          //���ȼ� 4 ��������
};

/*==================================================================================
* �� �� ���� EnableTask
* ��    ���� None
* ��������:  ʹ������
* �� �� ֵ�� None
* ��    ע�� 0:û��ʹ��  1��ʹ��   0xff:δ֪ͨ��
* ��    �ߣ�  
* ����ʱ�䣺 2017-7-28 15:12:42
==================================================================================*/
uint8_t IsTaskEnable(TASK_LIST Task)
{
	if(Task > TASKS_MAX)
		return 0xff;
	
	if(TaskComps[Task].RunState == TASK_ENABLE)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

/*==================================================================================
* �� �� ���� TaskRefresh
* ��    ���� None
* ��������:  ˢ�¸���ʱ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2017-7-28 15:12:42
==================================================================================*/
void TaskRefresh(TASK_LIST Task)
{
 	if(Task > TASKS_MAX)
		return;
	
	TaskComps[Task].Timer = TaskComps[Task].ItvTime; 
}

/*==================================================================================
* �� �� ���� EnableTask
* ��    ���� None
* ��������:  ʹ������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2017-7-28 15:12:42
==================================================================================*/
void EnableTask(TASK_LIST Task)
{
	if(Task > TASKS_MAX)
		return;
	
//	TaskComps[Task].Timer = TaskComps[Task].ItvTime;
	TaskComps[Task].RunState = TASK_ENABLE;
}

/*==================================================================================
* �� �� ���� DisableTask
* ��    ���� None
* ��������:  ʧ������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2017-7-28 15:12:42
==================================================================================*/
void DisableTask(TASK_LIST Task)
{
	if(Task > TASKS_MAX)
		return;
	
	TaskComps[Task].Timer = TaskComps[Task].ItvTime;
	TaskComps[Task].RunState = TASK_DISABLE;
}

/*==================================================================================
* �� �� ���� TaskSetTimes
* ��    ���� None
* ��������:  �����־��Ǻ���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2017-7-28 15:12:42
==================================================================================*/
void TaskSetTimes(TASK_LIST Task , uint32_t Times)
{
	if(Task > TASKS_MAX)
		return;
	
	TaskComps[Task].Timer = Times;
	TaskComps[Task].RunState = TASK_ENABLE;
}

/*==================================================================================
* �� �� ���� TaskRemarks
* ��    ���� None
* ��������:  �����־��Ǻ���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2017-7-28 15:12:42
==================================================================================*/
void TaskRemarks(void)
{
	uint8_t i;
	for (i=0; i<TASKS_MAX; i++)          // �������ʱ�䴦��
	{
		if(TaskComps[i].RunState == TASK_ENABLE)		//���ʹ���˵�ǰ�����⣬����м�����
		{
			if (TaskComps[i].Timer)          // ʱ�䲻Ϊ0
			{
				TaskComps[i].Timer--;         // ��ȥһ������
				if (TaskComps[i].Timer == 0)       // ʱ�������
				{
					 TaskComps[i].Timer = TaskComps[i].ItvTime;       // �ָ���ʱ��ֵ��������һ��
					 TaskComps[i].run_power++;		//��Ȩֵ����
					 TaskComps[i].Run = 1;           // �����������
				}
			}
		}
  }
}

/*==================================================================================
* �� �� ���� TaskRemarks
* ��    ���� None
* ��������:  ������
* �� �� ֵ�� None
* ��    ע�� ��������ѭ���е��ã��ڶ�ʱ�ж��е���ע�����ʱ��
* ��    �ߣ�  
* ����ʱ�䣺 2017-7-28 15:12:42
==================================================================================*/
void TaskProcess(void *who)
{
	uint8_t i; 
	for (i=0; i<TASKS_MAX; i++)           // �������ʱ�䴦��
	{
		if (TaskComps[i].Run)           // ʱ�䲻Ϊ0
		{
			 TaskComps[i].TaskHook(who);         // ��������
			 TaskComps[i].Run = 0;          // ��־��0
			 return;
		}
	}
}

