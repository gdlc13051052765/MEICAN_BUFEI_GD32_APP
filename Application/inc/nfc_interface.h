#ifndef __NFC_INTERFACE_H
#define __NFC_INTERFACE_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MAX_READ_ZERO_NUM		5
#define MAX_TAG_NUM		    	1
#define TAG_UID_LENS				8
#define TAG_LENGTH		    	TAG_UID_LENS
#define TAG_MAX_TRY_TIME		3			//最大尝试次数，超过则认为标签离开

/*! NFC-V INVENTORY_RES format   Digital 2.0  9.6.2 */
typedef struct 
{
    uint8_t RES_FLAG;                   /*!< Response Flags                 */
    uint8_t DSFID;                      /*!< Data Storage Format Identifier */
    uint8_t UID[8];     /*!< NFC-V device UID               */
    uint8_t crc[2];          /*!< CRC                            */
} rfalNfcvInventoryRes;

/*! NFC-V listener device (VICC) struct  */
typedef struct
{
    rfalNfcvInventoryRes    InvRes;     /*!< INVENTORY_RES                  */
    bool                    isSleep;    /*!< Device sleeping flag           */
} rfalNfcvListenDevice;

typedef enum
{
	NEW_ENTRY_STA = 0x01,		//新接入标签
	LOST_EXIT_STA = 0x81,		//丢失退出的标签
	LOST_ALL_STA = 0x82,		//删除所有标签
}_Tag_Sta;

typedef enum
{
	NULL_STA = 0x00,		//没有标签
	ERR_CARD_STA,				// 非美餐标识卡
	EXIT_STA,											//离开
	EXIST_STA,										//已经存在的标签
	ENTRY_BLOCK_OK_STA,						//新放入的标签,读取block成功
	ENTRY_BLOCK_FAIL_STA,					//新放入的标签,读取block失败
	
}_Tag_State,*_pTag_State;

typedef union
{
	struct
	{
		uint8_t id_code:4;		//唯一编号
		uint8_t cstm_crc:4;		//标签号CRC4校验
	}_bit;
	uint8_t cstm_id;				//
}_Tag_Cstm_Id;

typedef struct
{
	//读卡参数
	uint8_t is_use;			//是否为有效标签
	uint8_t try_time;		//尝试次数，如果超过最大次数则认为标签离开
	_Tag_State tag_state;				//标签是否已经存在    EXIT_STA-->EXIT_STA-->NULL_STA  标签离开(连续状态循环)    

	//更新
	_Tag_Sta tag_up_state;			//标签更新状态，根据此信息进行删除操作
	uint8_t is_update;					//是否更新到主机  0：没有  0x01:更新中   0x02:更新完成, 更新中标签的信息不允许更新，只能在更新完或者没有更新的状态下改变
	_Tag_Cstm_Id cstm_tag_id;		//自定义标签ID，函数（每读取到一个新标签1，标签个数为0时候，清空为1）
	
	//标签信息
	uint8_t uid[TAG_LENGTH];		//存放标签的uid
	uint8_t block[TAG_LENGTH];	//block数据
}_Tag_Info,*_pTag_Info;

typedef struct
{
	uint16_t total_tag_num;	//总标签数量，包含EXIT_STA状态的标签
	uint16_t read_tag_num;	//读取到的标签数量，每一次读取的标签数量
	uint16_t use_state;			//每一位代表一个存储， 0：当前为空  1：代表有标签数据

	_Tag_Info new_tag_info[MAX_TAG_NUM];
	_Tag_Info old_tag_info[MAX_TAG_NUM];
}_Tag_Context,*_pTag_Context;

typedef struct
{
	uint8_t antiShake;
	uint8_t scanMode;
	uint8_t recursion_depth;
	uint8_t slotNoRespondTime; 
}_Rfal_Param,*_pRfal_Param;


/*==================================================================================
* 函 数 名： nfc_init
* 参    数： None
* 功能描述:  射频硬件初始化
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void nfc_init(void);

/*==================================================================================
* 函 数 名： read_card_task
* 参    数： None
* 功能描述:  定时读写卡任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void read_card_task(void* argv);

#endif
