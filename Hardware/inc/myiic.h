#ifndef __MYIIC_H
#define __MYIIC_H
#include "stdint.h"
#include "includes.h"
#include "hwioconfig.h"

typedef uint8_t u8;
typedef uint32_t u32;

#define SDA_GROUP		NTC_SDA_GPIO_Port
#define SDA_PIN			NTC_SDA_Pin

#define SCL_GROUP		NTC_SCL_GPIO_Port
#define SCL_PIN			NTC_SCL_Pin

//IO�������� 
#define SDA_IN()  my_SDA_IN()
#define SDA_OUT() my_SDA_OUT()

//IIC���в�������
void MYIIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  
#endif
















