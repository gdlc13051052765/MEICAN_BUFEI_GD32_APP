#ifndef  __BOARD_INFO_H
#define  __BOARD_INFO_H

#define  IAP_INFO_SIZE           2048
//#define  ST_FLASH_CACHE_ADDR		 0x08008C00		//�ڲ�FLASH�����ַ
#define  EX_FLASH_CACHE_ADDR		 0x0800							//�ⲿ�����ַ

/*==================================================================================
* ����˵���� ���ļ�Ϊmcu_tools.exe����������������
* ��    ע�� ÿһ��궨���ֵ������Ҫ��()��������Ϊ�˷��������������ò�Ҫ�䶯��������Ҫ�޸Ĺ���
* ��    �ߣ�  
* ����ʱ�䣺 2021-01-05 113517
==================================================================================*/
#define  SOFT_VERSION					(0x03)		//����汾 1���ֽ�
#define  HARD_VERSION					(0x02)		//Ӳ���汾 1���ֽ�
#define  MACHINE_TYPE					(0x05)		//���������� 2���ֽ�
#define  BOARD_TYPE						(0x02)		//�忨���� 1���ֽ�

/*==========================FLASH��ַ�����=========================*/
#define   APP_START_ADDR			0x08002000					//�û�������ʼ��ַ
#define   APP_CACHE_ADDR			EX_FLASH_CACHE_ADDR					//�������򻺴��ַ
#define   IAP_FLAG_ADDR				0x00000000					//�������ò�����ַ

#define  APP_START_ADDRESS 		0x2000

#endif
