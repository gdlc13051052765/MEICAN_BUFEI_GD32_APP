#include "gd25q_driver.h"
#include "application.h"
#include "stdio.h"

SPI_HandleTypeDef w25qxx_hspi;
/*==================================================================================
* �� �� ���� delay_us
* ��    ���� None
* ��������:  ����ȷ��ʱ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2021-01-04 043651
==================================================================================*/
static void delay_us(uint16_t us)
{
	for(int i=9; i<us; i++)
		for(int j=0; j<72; j++)
			{
				__NOP();
			}
}

/*==================================================================================
* �� �� ���� W25QXX_Spi_Init
* ��    ���� None
* ��������:  w25q spi ��ʼ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2021-01-04 043646
==================================================================================*/
void w25qxx_spi_init(void)
{
	w25qxx_hspi.Instance = W25QXX_SPI;
	w25qxx_hspi.Init.Mode = SPI_MODE_MASTER;
	w25qxx_hspi.Init.Direction = SPI_DIRECTION_2LINES;
	w25qxx_hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	w25qxx_hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
	w25qxx_hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
	w25qxx_hspi.Init.NSS = SPI_NSS_SOFT;
	w25qxx_hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	w25qxx_hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	w25qxx_hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	w25qxx_hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	w25qxx_hspi.Init.CRCPolynomial = 7;
	if (HAL_SPI_Init(&w25qxx_hspi) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI2_Init 2 */
	__HAL_SPI_ENABLE(&w25qxx_hspi);
	
	W25QXX_ReadID();	
}

/*==================================================================================
* �� �� ���� spi1_sed_rev
* ��    ���� None
* ��������:  SPI1�ӿڷ�������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ�  
* ����ʱ�䣺 2021-01-04 043646
==================================================================================*/
static uint8_t sed_rev_byte(uint8_t value)
{
	uint8_t ret = 0; 
	while((W25QXX_SPI->SR & SPI_FLAG_TXE) == RESET);	//�ȴ����ͻ�������
	W25QXX_SPI->DR = value; 	//����һ���ֽ�  

	while((W25QXX_SPI->SR & SPI_FLAG_RXNE) == RESET);	 //�ȴ��Ƿ���յ�һ���ֽ�  
	ret = (W25QXX_SPI->DR); 													//��ø��ֽ�
	
	return ret; //�����յ����ֽ�
}

//��ȡ״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;   
	W25QXX_CS_0;                            //ʹ������
	sed_rev_byte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ������� 
	byte=sed_rev_byte(0Xff);             //��ȡһ���ֽ�����
	W25QXX_CS_1;                                 
	return byte;   
} 
//д״̬�Ĵ���
void W25QXX_Write_SR(uint8_t sr)   
{   
	W25QXX_CS_0;                            //ʹ������
	sed_rev_byte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������  
	sed_rev_byte(sr);               //д��һ���ֽ�
	W25QXX_CS_1;                            //ȡ��Ƭѡ  	      
}   
//дʹ��	   
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_WriteEnable);       
	W25QXX_CS_1;                                	      
} 
//дʧ��
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS_0;                            
  sed_rev_byte(W25X_WriteDisable);     
	W25QXX_CS_1;                               
} 		
//��ȡоƬID  
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
	W25QXX_CS_0;				    
	sed_rev_byte(0x90);//��ȡоƬID����    
	sed_rev_byte(0x00); 	    
	sed_rev_byte(0x00); 	    
	sed_rev_byte(0x00); 	 			   
	Temp|=sed_rev_byte(0xFF)<<8;  
	Temp|=sed_rev_byte(0xFF);	 
	W25QXX_CS_1;			
	debug_print("w25q id = %4x \r\n", Temp );
	return Temp;
}   		    
//��ȡSPI FLASH  
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	W25QXX_CS_0;                              
	sed_rev_byte(W25X_ReadData);          
	sed_rev_byte((uint8_t)((ReadAddr)>>16));      
	sed_rev_byte((uint8_t)((ReadAddr)>>8));   
	sed_rev_byte((uint8_t)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=sed_rev_byte(0XFF);   
	}
	W25QXX_CS_1;  				    	      
}  
//SPI��һҳ(0~65535)��д������256�ֽڵ�����
//pBuffer�����ݴ洢��
//WriteAddr����ʼҪд��ĵ�ַ
//Ҫд����ֽ���(���256�ֽ�)
void W25QXX_Write_Page(const uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
	W25QXX_Write_Enable();                  
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_PageProgram);        
	sed_rev_byte((uint8_t)((WriteAddr)>>16)); 
	sed_rev_byte((uint8_t)((WriteAddr)>>8));   
	sed_rev_byte((uint8_t)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)
	{	
		sed_rev_byte(pBuffer[i]);
	}
		
	W25QXX_CS_1;                           
	W25QXX_Wait_Busy();					  
} 

//�޼���дSPI_FLASH
//����ͬ��,NumByteToWrite���Ϊ65535
void W25QXX_Write_NoCheck(const uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; 	 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  
			if(NumByteToWrite>256)pageremain=256; 
			else pageremain=NumByteToWrite; 	  
		}
	};	    
} 
//дSPI FLASH  
//����ͬ�ϣ�NumByteToWrite���Ϊ65535
static uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write(const uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
	W25QXX_BUF=W25QXX_BUFFER;	     
	secpos=WriteAddr/4096;
	secoff=WriteAddr%4096;
	secremain=4096-secoff;
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);
		for(i=0;i<secremain;i++)
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;  
		}
		if(i<secremain)
		{
			W25QXX_Erase_Sector(secpos);
			for(i=0;i<secremain;i++)	 
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096); 

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);			   
		if(NumByteToWrite==secremain)break;
		else
		{
			secpos++;
			secoff=0;	 

			pBuffer+=secremain;  
			WriteAddr+=secremain;   
			NumByteToWrite-=secremain;				
			if(NumByteToWrite>4096)secremain=4096;	
			else secremain=NumByteToWrite;			
		}
	}
}

//������ƬоƬ
void W25QXX_Erase_Chip(void)   
{                                   
	W25QXX_Write_Enable();                 
	W25QXX_Wait_Busy();   
	W25QXX_CS_0;                           
	sed_rev_byte(W25X_ChipErase);        
	W25QXX_CS_1;                                 
	W25QXX_Wait_Busy();   				   
}   
//����һ������
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{   
	Dst_Addr*=4096;
	W25QXX_Write_Enable();                
	W25QXX_Wait_Busy();   
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_SectorErase);      
	sed_rev_byte((uint8_t)((Dst_Addr)>>16));     
	sed_rev_byte((uint8_t)((Dst_Addr)>>8));   
	sed_rev_byte((uint8_t)Dst_Addr);  
	W25QXX_CS_1;                                
	W25QXX_Wait_Busy();   				   
}  

//����һ������
void W25QXX_Erase(uint32_t Dst_Addr, uint32_t length)   
{   
	uint32_t sector_num = (uint32_t)((length/4096)+1);
	
	for(int i=0; i<sector_num; i++)
	{
		W25QXX_Erase_Sector(Dst_Addr+(i*4096));
	}
	
}  

//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   
}  
//�������ģʽ
void W25QXX_PowerDown(void)   
{ 
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_PowerDown);        
	W25QXX_CS_1;                                	      
	delay_us(3);                               
}   
//����
void W25QXX_WAKEUP(void)   
{  
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_ReleasePowerDown);     
	W25QXX_CS_1;                            	      
	delay_us(3);                               
}   

