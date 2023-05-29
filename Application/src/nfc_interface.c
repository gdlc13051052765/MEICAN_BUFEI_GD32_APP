#include <stdio.h>
#include <string.h>
#include "FM176XX_REG.h"
#include "READER_API.h"
#include "FM176XX.h"
#include "define.h"
#include "CPU_CARD.h"
#include "RC663.h"
#include "nfc.h"
#include "includes.h"
#include "delay.h"

//NFC FM17660
//#define FM17660_MODE

//NFC RC663
#define RC663_MODE

static _Tag_Info pTag;//卡信息结构体
/*==================================================================================
* 函 数 名： nfc_init
* 参    数： None
* 功能描述:  射频硬件初始化
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void nfc_init(void)
{
	#ifdef RC663_MODE
		RC663_Init();
	#endif
	
	#ifdef FM17660_MODE
		FM17660_Init();
	#endif
}

/*==================================================================================
* 函 数 名： get_card_tag_msg
* 参    数： None
* 功能描述:  获取卡片信息
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
_Tag_Info get_card_tag_msg(void)
{
	return pTag;
}

/*==================================================================================
* 函 数 名： read_card_task
* 参    数： None
* 功能描述:  定时读写卡任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void read_card_task(void* argv)
{
	#ifdef RC663_MODE
		pTag = rc663_read_card_block();
	#endif
	
	#ifdef FM17660_MODE
		pTag = fm17660_read_card_block();
	#endif
}
