#ifndef  __APPLICATION_H
#define  __APPLICATION_H
#include <stdint.h>
#include "board_info.h"
 

#define MAX_FRAME_LENGTH			160			//最大数据帧长度，数据域


#define  MAX_FLASH_SIZE				256			//flash存储长度
#define  MAX_USE_FLASH_SIZE		64			//实际参数使用的空间
#define  CONFIG_DATA_LEN      12      //配置数据长度
 

#pragma pack(1)
typedef union
{
	struct
	{
		//固定格式
		uint8_t crc_data_len;	//crc校验数据的长度
		uint8_t crc_data_len_n;	//crc校验数据的长度
		
		//剩下数据和校验占64字节
		uint8_t can_addr;			//和安卓间的CAN通信地址
		uint8_t can_addr_n;		//和安卓间的CAN通信地址  取反
		  
		uint8_t soft_ver;					//软件版本
		uint8_t soft_ver_n;				//软件版本  取反
 
		uint8_t hard_ver;					//硬件地址
		uint8_t hard_ver_n;				//硬件地址  取反
		
		uint8_t  is_heat;				//是否加热
		uint8_t  is_heat_n;				//是否加热 取反

		uint8_t rfid_retry_count;		//rfid重复读卡次数
		uint8_t rfid_retry_count_n;
		
		uint16_t heat_set_temp;	//加热控制,设定温度值
		uint16_t heat_set_temp_n;	//加热控制,设定温度值 取反
		
		uint16_t ant_adjust;	//RFID天线调整参数 
		uint16_t ant_adjust_n;	//RFID天线调整参数  取反
		 
 
		//上面占用8byte
		
		//待添加的数据
		
		uint32_t crc32;				//校验
		
		uint8_t res_buff[MAX_USE_FLASH_SIZE-12];		//保留空间, 20指已经占用的空间
	}_Use_Param;
	
	struct
	{
		uint8_t null_buff[MAX_USE_FLASH_SIZE];	//存储区
		uint8_t copy_param[MAX_USE_FLASH_SIZE];	//备份参数区
	}_copy_Param;
	
	uint8_t flash_buff[MAX_FLASH_SIZE];
}_App_Config_Param,*_pApp_Config_Param;

#pragma pack()
typedef struct
{
	uint8_t can_addr;					//和安卓间的通信地址
	uint8_t soft_ver;					//软件版本
	uint8_t hard_ver;					//硬件地址	

	uint8_t  is_heat;				//是否加热
	uint16_t heat_set_temp;	//加热控制,设定温度值
	uint16_t ant_adjust;	//RFID天线调整参数
	uint8_t rfid_retry_count;
	_App_Config_Param config_param;	//如果参数改变，需要更新到config_param，在更新到flash
		
	//全局变量
	uint8_t sys_init_complete;		//系统初始化完成
	
}_App_Param, *_pApp_Param;

 extern _App_Param mApp_Param;

#define 	EN_DEBUG                (1)   //0:调试          1:正常运行
#define 	EN_WWDG                 (0)   //0:关闭看门狗     1:使能看门狗
#define   EN_AUTO_CAN_ADDR_CFG		(1)		//0:不是能can地址自动分配  1：使能CAN地址自动分配

#if EN_DEBUG == 1
	#define debug_print		printf
	#define debug_print_hex  print_hex
#else
	#define debug_print	printf
	#define debug_print_hex  	print_hex
#endif



void systerm_init(void);
void si_sed_loop_task(void); 
void app_dispatch(void);

uint8_t si_sed_buff_to(uint8_t to_addr, uint8_t cmd, uint8_t *buff, uint8_t len, uint8_t try_time);
uint8_t si_sed_buff_to_master(uint8_t cmd, uint8_t *buff, uint8_t len, uint8_t try_time);
#endif

