#ifndef  __COMMAND_H
#define  __COMMAND_H
#include <stdint.h>
#include "SwitchConfig.h"

#define MAIN_READ_CMD  0x02//主读指令
#define MAIN_WRITE_CMD 0x03//主写指令

#define CRC32_DATA_LEN      4 //CRC32所占的字节数

#define DATA_CHECK_OK       0 //数据校验成功
#define DATA_CHECK_ERR_1    1 //数据校验失败1：字头错误
#define DATA_CHECK_ERR_2    2 //数据校验失败2：CRC校验错误
#define DATA_CHECK_ERR_3    3 //数据校验失败3：主指令错误
#define DATA_CHECK_ERR_4    4 //数据校验失败4：餐格ID地址号错误
//========================应答包当前状态宏定义======================================
#define PROTOCOL_ACK_OK		            0x00 //执行成功
#define PROTOCOL_ACK_ERROR_CRC32			0x01 //CRC校验错误
#define PROTOCOL_ACK_ERROR_PARA		    0x02 //参数错误
#define PROTOCOL_ACK_ERROR_INS		    0x03 //无法解析指令

//CAN通信命令
typedef enum
{

	//安卓发起到CC的命令 
	ANDROID_CC_GET_SW_AND_HW_VER	= 0xA1,//获取版本号
	ANDROID_CC_GET_WEIGH_UID,//获取当前重量值跟卡号
	ANDROID_CC_GET_WEIGH,//获取当前重量
	ANDROID_CC_GET_UID,//获取当前重卡号

	//固件升级命令
	ANDROID_CC_UPDATE_INFO  = 0x80,//下发升级包信息
	ANDROID_CC_UPDATE_DATA  = 0x81,//下发升级包数据
	ANDROID_CC_GET_VERSION  = 0x82,//获取软硬件版本
	ANDROID_CC_UPDATE_CHECK = 0x84,//检查确认升级
	ANDROID_CC_UPDATE_RESET = 0x88,//重启
	ANDROID_CC_UPDATE_LOST  = 0x8A,//下发缺失升级包
	
}_T113_Cmd;

//天津景园称重传感器
#ifdef JINGYUAN_MODE	
	//485通信命令对接丽景称重传感器
	#define	READ_MID_CMD					 0x02//读设备MID, MID是设备唯一标识，出厂时设置，8位16进制数
	#define	READ_DATA_CMD					 0x3C//读数据 读取内存变量
	#define WRITE_DATA_CMD				 0x3D//写数据 修改内存变量
	#define CLEAR_ZERO_CMD				 0x3E//清零 设零点修正数值为当前原始重量值
#endif

//BT559A称重传感器 返回数据长度7
#ifdef BT559A_MODE
	#define GET_CURRENT_WEIGH_CMD			0xA9//获取当前重量
#endif

//主控T113 跟单片机的通讯协议
#define GET_UUID_WEIGH_CMD		0xB0//获取卡号跟重量


typedef union test_struct
{
	uint64_t data;
	uint8_t buff[8];
}test_struct_t;

//uuid 重量结构体
typedef union uuidWeigh_struct
{
	uint8_t flag;
	uint8_t uuid[8];
	int weigh;
}uuidWeigh_struct_t;

void p485_push_data_msg(void* p_msg);
/*==================================================================================
* 函 数 名： p485_instance_init
* 参    数： None
* 功能描述:  485接收初始化
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-10-28 170617
=================================================================================*/
void p485_instance_init(void);
/*==================================================================================
* 函 数 名： p485_send_data_task
* 参    数： None
* 功能描述:  485 发送数据任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void p485_send_data_task(void* argv);
/*==================================================================================
* 函 数 名： p485_rec_decode_task
* 参    数： None
* 功能描述:  485 接收数据解析任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void p485_rec_decode_task(void* argv);


/*==================================================================================
* 函 数 名： pt113_instance_init
* 参    数： None
* 功能描述:  主控T113串口接收队列初始化
* 返 回 值： None
* 备    注： None
* 作    者：  
* 创建时间： 2019-10-28 170617
=================================================================================*/
void pt113_instance_init(void);

/*==================================================================================
* 函 数 名： pt113_push_data_msg
* 参    数： None
* 功能描述:  主控T1113 接收到数据后调用此函数把接收的数据添加的队列里面
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void pt113_push_data_msg(void* ret_msg);

/*==================================================================================
* 函 数 名： p485_send_data_task
* 参    数： None
* 功能描述:  485 发送数据任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 170658
==================================================================================*/
void pt113_send_data_task(void* argv);

/*==================================================================================
* 函 数 名： pt113_rec_decode_task
* 参    数： None
* 功能描述:  解析T113串口发过来的命令任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2022-06-08 170658
==================================================================================*/
void pt113_rec_decode_task(void* argv);
	
#endif
