
#include <stdio.h>
#include <string.h>

#include "motor_control.h"
#include "hmc80x5_api.h"
#include "application.h"


motor_queue_t motorMsgSt;//��������ṹ��

/*==================================================================================
* �� �� ���� motor_position_zero
* ��    ���� None
* ��������:  hk ���λ�ù���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-05-29 170658
==================================================================================*/
void motor_position_zero(void)
{
	motor_queue_t pMotorMsgSt;
	
	printf("\n �����Ὺʼ��ʼ��\r\n");
	Set_Axs(0,0,0,0,0,0);//�Ƚ���ʹ��
	Set_Axs(0,1,0,0,0,0);
	Set_Axs(0,2,0,0,0,0);   
	Set_Axs(0,3,0,0,0,0);

	Set_Axs(0,0,1,1,1,1);//������ʹ�ܣ��߼�λ�û�0
	Set_Axs(0,1,1,1,1,1);
	Set_Axs(0,2,1,1,1,1);
	Set_Axs(0,3,1,1,1,1);

	Set_Encorder(0,0,3,1,0,0,0);//�Ƚ���ʹ��
	Set_Encorder(0,1,3,1,0,0,0);
	Set_Encorder(0,2,3,1,0,0,0);
	Set_Encorder(0,3,3,1,0,0,0);

	Set_Encorder(0,0,3,1,0,0,1);//������ʹ�ܣ��߼�λ�û�0
	Set_Encorder(0,1,3,1,0,0,1);
	Set_Encorder(0,2,3,1,0,0,1);
	Set_Encorder(0,3,3,1,0,0,1);

	
	DeltMov(0,0,0,0,0,1000,5000,10000,0,500,500,0,0);
	DeltMov(0,1,0,0,0,1000,5000,10000,0,500,500,0,0);
	DeltMov(0,2,0,0,0,1000,5000,10000,0,500,500,0,0);
	DeltMov(0,3,0,0,0,1000,5000,10000,0,500,500,0,0);

	HAL_Delay(5000);
	
	//������ص�ԭ��
	//FL_ContinueMov(0,1,0,0,1000,2000);
	MovToOrg(0,0,1,0,ZERO_SPEED);
	MovToOrg(0,1,1,0,ZERO_SPEED);
	MovToOrg(0,2,1,0,ZERO_SPEED);
	MovToOrg(0,3,1,0,ZERO_SPEED);
//	
//	//�ȴ������λ
	while(pMotorMsgSt.queue[0].Io_state != negative_limit)//������︺���޿���
	{
		pMotorMsgSt = get_motor_current_state();
	}
	while(pMotorMsgSt.queue[1].Io_state != negative_limit)//������︺���޿���
	{
		pMotorMsgSt = get_motor_current_state();
		//AxsStop(0,1);
	}
	while(pMotorMsgSt.queue[2].Io_state != negative_limit)//������︺���޿���
	{
		pMotorMsgSt = get_motor_current_state();
	}
	while(pMotorMsgSt.queue[3].Io_state != negative_limit)//������︺���޿���
	{
		pMotorMsgSt = get_motor_current_state();
	}

	//���λ�ø�λ����������߼�λ�ù���
	get_motor_current_state();
	Set_Axs(0,0,0,0,0,0);//�Ƚ���ʹ��
	Set_Axs(0,1,0,0,0,0);
	Set_Axs(0,2,0,0,0,0);   
	Set_Axs(0,3,0,0,0,0);

	Set_Axs(0,0,1,1,1,1);//������ʹ�ܣ��߼�λ�û�0
	Set_Axs(0,1,1,1,1,1);
	Set_Axs(0,2,1,1,1,1);
	Set_Axs(0,3,1,1,1,1);
	pMotorMsgSt = get_motor_current_state();
		
	//�����λ���޺���������һЩ����
	DeltMov(0,0,0,0,0,1000,5000,ZREO_PLUS,0,500,500,0,0);
	DeltMov(0,1,0,1,0,1000,5000,ZREO_PLUS,0,500,500,0,0);
	DeltMov(0,2,0,0,0,1000,5000,ZREO_PLUS,0,500,500,0,0);
	DeltMov(0,3,0,0,0,1000,5000,ZREO_PLUS,0,500,500,0,0);
	HAL_Delay(5000);
	printf("�������ʼ�����\r\n");
}

/*==================================================================================
* �� �� ���� motor_emergency_stop
* ��    ���� None
* ��������:  hk �����ͣ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-29 170658
==================================================================================*/
void motor_emergency_stop(void)
{
	AxsStop(0,0);
	AxsStop(0,1);
	AxsStop(0,2);
	AxsStop(0,3);
}

/*==================================================================================
* �� �� ���� get_motor_current_state
* ��    ���� None
* ��������:  ��ȡ�����ǰ״̬
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-29 170658
==================================================================================*/
motor_queue_t get_motor_current_state(void)
{	
	uint8_t SyncIO,i;
	int code_num = 0;
	motor_queue_t pMotorMsgSt;
	
	for(i=0;i<MOTOR_NUM;i++)
	{
		Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
		debug_print("postion[%d] = %d, run_state[%d] = %d, Io_state[%d] = %d \r\n",i,pMotorMsgSt.queue[i].position, i,pMotorMsgSt.queue[i].run_state, i,pMotorMsgSt.queue[i].Io_state);
		if(pMotorMsgSt.queue[i].Io_state == negative_limit)
		{
			Set_Axs(0,i,0,0,0,0);
			Set_Axs(0,i,1,1,1,1);//������ʹ�ܣ��߼�λ�û�0
		}
	}

	return pMotorMsgSt;
}
