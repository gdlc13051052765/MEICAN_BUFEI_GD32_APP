#include "iap_protocols.h"
#include "board_info.h"
#include "frame_parse.h"
#include "crc.h" 
#include "stm32f1xx_hal.h"

_Iap_Param mIap_Param = 
{
    .boot_msg_addr	= IAP_FLAG_ADDR,
    .app_run_addr	= APP_START_ADDR,
    .app_cache_addr	= APP_CACHE_ADDR,		//stFlash/exFlash

    .is_valid_fw_msg	= 0,
    .frame_step			= 0, 
    .update_state		= iap_null,

    .w_flash = &mEx_Flash_Func,
    .r_flash = &mEx_Flash_Func,
    //.msg_flash = &stFlash_Func,
};

static void iap_init(void);
static uint8_t iap_frame_info(const uint8_t* src_buff, uint16_t *ret_result);
static uint8_t iap_frame_data(const uint8_t* src_buff, uint16_t *ret_result);
static uint8_t iap_frame_patch(const uint8_t* src_buff, uint16_t *ret_result);
static uint8_t iap_firmware_complete_check(uint16_t *ret_result);
static uint8_t iap_reset_system(void);

_Iap_Func mIap_Func = 
{
	.init 		= iap_init,
	.info_opt	= iap_frame_info,
	.data_opt	= iap_frame_data,
	.patch_opt	= iap_frame_patch,
	.check_opt	= iap_firmware_complete_check,
	.reset_opt	= iap_reset_system,
};

_pIap_Func pIap_Func = &mIap_Func;

IAP_StatusTypeDef IAP_Status;
IAP_StatusTypeDef *p_IAP_Status = &IAP_Status;

/*==================================================================================
* 函 数 名： write_firware
* 参    数： None
* 功能描述:  写固件
* 返 回 值： None
* 备    注： 
* 作    者：  
* 创建时间： 2021-01-07 025540
==================================================================================*/
static uint8_t write_firware(uint32_t addr, const uint8_t* buff, uint32_t length)
{ 
	uint8_t r_buff[FRAME_LENGTH] = {0};
	uint8_t try_count = 3;
	
	do{
			if(mIap_Param.w_flash->write(addr, buff, length) == 0x01)
			{
				if(mIap_Param.r_flash->read(addr, r_buff, FRAME_LENGTH) == 0x01)
				{
					if(msg_frame_compare(r_buff, buff, FRAME_LENGTH) == 0x01)
					{
							return ret_ok;
					}
				}
			}
	}while(try_count--);
	
	return ret_flash_lost;
}

/*==================================================================================
* 函 数 名： iap_init
* 参    数： None
* 功能描述:  初始化
* 返 回 值： None
* 备    注：  
* 作    者：  
* 创建时间： 2021-01-06 042608
==================================================================================*/
static void iap_init(void)
{ 
	IAP_Status.status = IAP_IDLE;
	
	IAP_Status.firmwareDataAddress = IAP_INFO_SIZE;  //2048       
	IAP_Status.appAddress = APP_START_ADDR;          //0x8000000   +     8192  
	IAP_Status.infoAddress = IAP_FLAG_ADDR;          // 0
	
	IAP_Status.magicNumber = 0;
	
	IAP_Status.firmwareSize = 0 ;    // 0
	IAP_Status.Crc32 = 0;            // 0
	
	IAP_Status.firmwareByteWritten = 0;     // 0
	IAP_Status.firmwareCurrentWritePtr = IAP_FLAG_ADDR +  IAP_INFO_SIZE; // 0 + 2048
	
	IAP_Status.Key32 = 0x8542;
	IAP_Status.Update_Times = 0;
	
	mIap_Param.w_flash->write(IAP_Status.infoAddress, (uint8_t *)p_IAP_Status, sizeof(IAP_Status));
}

/*==================================================================================
* 函 数 名： check_firware_is_complete
* 参    数： None
* 功能描述:  根据当前版本值去校验缓存固件是否一致
* 返 回 值： None
* 备    注：  
* 作    者：  
* 创建时间： 2021-01-08 034506
==================================================================================*/
static uint8_t  check_firware_complete_ver(uint8_t soft_ver, uint16_t *fw_id)
{
	uint8_t ret = 0; 
	uint8_t r_buff[106 + 2] = {0}; 
	 
	//重新初始化变量 
	mIap_Param.frame_step = 0;
	mIap_Param.update_state = iap_null;

	//检查信息帧是否存在
	if(mIap_Param.r_flash->read(mIap_Param.app_cache_addr, r_buff, FRAME_MAX_LENS) == 0x01)
	{
		//校验解析信息帧
		if(msg_frame_check(&mIap_Param))
		{
			//判断版本是否合法
			if(soft_ver != mIap_Param.fw_msg.MsgFrame.AppVer)
			{
				return ret_ver_error;
			}

			//有效信息帧
			mIap_Param.is_valid_fw_msg = 0x01;	//接收到有效信息帧

			//检查当前接收到的帧ID
			ret = find_firmware_lost_step(&mIap_Param, fw_id);		//校验当前接收的的包ID 

			if(ret == ret_need_pkg)
			{
				mIap_Param.frame_step = *fw_id - 1;
				return ret_rev_pkg_id;
			}
			 
			return ret;
		}
		else
		{
			return ret_check_error;
		}
	}
	else
	{
		return ret_flash_lost;
	}
}

/*==================================================================================
* 函 数 名： iap_frame_info
* 参    数： None
* 功能描述:  信息帧处理
* 返 回 值： None
* 备    注：  
* 作    者：  
* 创建时间： 2021-01-06 042608
==================================================================================*/
static uint8_t iap_frame_info(const uint8_t* src_buff, uint16_t *ret_result)
{
	uint8_t ret = 0;  
	
	//校验数据帧是否合法
	memcpy(mIap_Param.fw_msg.FrameBuff,src_buff,FRAME_LENGTH);
	 
//	DEBUG("信息头数据：\r\n");	
//	for(u8 i = 0; i < 106; i++)
//	{
//		DEBUG("%2x",src_buff[i]);
//	}
	//校验信息帧是否合法 
	if(msg_frame_check(&mIap_Param) == 0)
	{
		//不合法
		return ret_info_error;
	}
	
	if(mIap_Param.update_state != iap_null)
	{
		//printf("update_state 不为空，上电后不是第一次接收信息包");
		//缓存已经有升级包
		if(msg_frame_compare(mIap_Param.fw_msg.FrameBuff, src_buff, FRAME_MAX_LENS) == 0x01)	//信息帧比较
		{
			//相同信息帧 
			mIap_Param.update_state = iap_info_state;
			mIap_Param.is_valid_fw_msg = 0x01;	//接收到有效信息帧
			mIap_Param.frame_step = 0;
			
			ret = find_firmware_lost_step(&mIap_Param, ret_result);		//校验当前接收的的包ID 
			printf("info iap_frame_info()-> ret0:%04x; fw_id=%04x\r\n", ret_rev_pkg_id, *ret_result);
            
			if(ret == ret_need_pkg)
			{
				mIap_Param.frame_step = *ret_result - 1;
				return ret_rev_pkg_id;
			}
			else if (ret == 0)
			{
				*ret_result = mIap_Param.fw_msg.MsgFrame.AppTotalPackage;
				mIap_Param.frame_step = *ret_result - 1;
				write_firware(mIap_Param.boot_msg_addr, src_buff, FRAME_LENGTH); //将信息帧写入boot_msg_addr
				return ret_rev_pkg_id;
			}
            
			write_firware(mIap_Param.boot_msg_addr, src_buff, FRAME_LENGTH); //将信息帧写入boot_msg_addr 
			return ret;
		}
		else
		{
			//校验信息帧是否合法
			mIap_Param.update_state = iap_info_state;
			mIap_Param.is_valid_fw_msg = 0x01; //接收到有效信息帧
			mIap_Param.frame_step = 0;
			
			memcpy(mIap_Param.fw_msg.FrameBuff, src_buff, FRAME_LENGTH); //使用新的信息帧

			//不同信息帧，擦除存储中的缓存升级包，重新开始
			//先进行存储器擦除
			if(mIap_Param.w_flash->erase(mIap_Param.app_cache_addr, (uint32_t)(mIap_Param.fw_msg.MsgFrame.AppTotalPackage * FRAME_LENGTH)) == 0x01)
			{
				if(write_firware(mIap_Param.app_cache_addr, src_buff, FRAME_LENGTH) == ret_ok) //写入信息帧
				{
          write_firware(mIap_Param.boot_msg_addr, src_buff, FRAME_LENGTH); //将信息帧写入boot_msg_addr
					return ret_ok;
				}
				else
				{
					return ret_flash_lost;
				}
			}
			else
			{
				return ret_flash_lost;
			}
		}
	}
	else
	{
		//printf("update_state 为空，上电后第一次接收信息包");
		//缓存无数据
		if(mIap_Param.r_flash->read(mIap_Param.app_cache_addr, mIap_Param.fw_msg.FrameBuff, FRAME_MAX_LENS) == 0x01)	//从存储中同步信息帧
		{
			if(msg_frame_compare(mIap_Param.fw_msg.FrameBuff, src_buff, FRAME_MAX_LENS) == 0x01)	//信息帧
			{
				//有效信息帧
				mIap_Param.update_state = iap_info_state;
				mIap_Param.is_valid_fw_msg = 0x01;	//接收到有效信息帧
				mIap_Param.frame_step = 0;
				
				ret = find_firmware_lost_step(&mIap_Param, ret_result);		//校验当前接收的的包ID 
				printf("info iap_frame_info()-> ret:%04x; fw_id=%04x\r\n", ret, *ret_result);
                
				if(ret == ret_need_pkg )
				{ 
					mIap_Param.frame_step = *ret_result - 1;
					//write_firware(mIap_Param.boot_msg_addr, src_buff, FRAME_LENGTH); //将信息帧写入boot_msg_addr
					printf("从第%d包开始升级\r\n",ret);
					return ret_rev_pkg_id;
				}
				else if (ret == 0)
				{
					*ret_result = mIap_Param.fw_msg.MsgFrame.AppTotalPackage;
					mIap_Param.frame_step = *ret_result - 1;
					//write_firware(mIap_Param.boot_msg_addr, src_buff, FRAME_LENGTH); //将信息帧写入boot_msg_addr
					return ret_rev_pkg_id;
				}
                
        //write_firware(mIap_Param.boot_msg_addr, src_buff, FRAME_LENGTH); //将信息帧写入boot_msg_addr
				return ret;
			}
			else
			{
				//有效信息帧
				mIap_Param.update_state = iap_info_state;
				mIap_Param.is_valid_fw_msg = 0x01;	//接收到有效信息帧
				mIap_Param.frame_step = 0;
				
				memcpy(mIap_Param.fw_msg.FrameBuff, src_buff, FRAME_LENGTH);	
				//使用新的信息帧
				//先进行存储器擦除
				if(mIap_Param.w_flash->erase(mIap_Param.app_cache_addr, (uint32_t)(mIap_Param.fw_msg.MsgFrame.AppTotalPackage*FRAME_LENGTH)) == 0x01)
				{
					if(write_firware(mIap_Param.app_cache_addr, src_buff, FRAME_LENGTH) == ret_ok) //写入信息帧
					{
            write_firware(mIap_Param.boot_msg_addr, src_buff, FRAME_LENGTH); //将信息帧写入boot_msg_addr
						printf("从第一包开始升级");
						return ret_ok;
					}
					else
					{
						return ret_flash_lost;
					}
				}
				else
				{
					return ret_flash_lost;
				}
			}
		}
		else
		{
			return ret_flash_lost;
		}
	}
}

/*==================================================================================
* 函 数 名： iap_frame_data
* 参    数： None
* 功能描述:  数据帧处理
* 返 回 值： None
* 备    注：  
* 作    者：  
* 创建时间： 2021-01-06 042608
==================================================================================*/
static uint8_t iap_frame_data(const uint8_t* src_buff, uint16_t *ret_result)
{ 
	uint32_t w_addr = 0; 
	
	//校验数据帧是否合法
	pBootAppFrame pmsg = (pBootAppFrame)src_buff;
	 
	//判断是否为断点续传
	if(mIap_Param.update_state == iap_null) //没有接收到信息帧
	{
		//从外部存储中校验固件信息
		 return check_firware_complete_ver(pmsg->DataFrame.SoftVer, ret_result);
	}
	else if( mIap_Param.update_state == iap_info_state) //没有接收到信息帧
	{
		mIap_Param.update_state = iap_data_state;
	}
	 
	w_addr = mIap_Param.app_cache_addr + pmsg->DataFrame.FrameStep * FRAME_LENGTH;	//计算写入地址
	
	printf("iap_frame_data()-> FrameStep:%04x; frame_step=%04x\r\n", pmsg->DataFrame.FrameStep, mIap_Param.frame_step);
	if((pmsg->DataFrame.FrameStep == (mIap_Param.frame_step + 1))&&
		 pmsg->DataFrame.CRC16 == crc16_ccitt(src_buff, FRAME_LENGTH - 2)&&
		 pmsg->DataFrame.SoftVer == mIap_Param.fw_msg.MsgFrame.AppVer)
	{
		mIap_Param.frame_step++;	//自己计算帧ID，
//				mIap_Param.frame_step = pmsg->DataFrame.FrameStep;	//拷贝帧ID
		return write_firware(w_addr, src_buff, FRAME_LENGTH);
	}
	else
	{
		return  ret_data_error; 
	} 
}

/*==================================================================================
* 函 数 名： iap_firmware_check
* 参    数： None
* 功能描述:  数据帧处理
* 返 回 值： None
* 备    注：  
* 作    者：  
* 创建时间： 2021-01-06 042608
==================================================================================*/
static uint8_t iap_firmware_complete_check(uint16_t *ret_result)
{
//	uint8_t ret = 0;
	
	//判断是否接收完数据包
//	if(mIap_Param.update_state == iap_data_state)
	{
		//校验是否缺包，并返回缺少的包数据
		return check_firmware_lost(&mIap_Param, ret_result);
	}
//	return ret;
}

/*==================================================================================
* 函 数 名： iap_frame_patch
* 参    数： None
* 功能描述:  补包流程处理
* 返 回 值： None
* 备    注： 补包要注意flash操作，需要先整个扇区读取，擦除，插入，回写
* 作    者：  
* 创建时间： 2021-01-06 042608
==================================================================================*/
static uint8_t iap_frame_patch(const uint8_t* src_buff, uint16_t *ret_result)
{
	uint8_t ret = 0;
	uint16_t needPkg = 0;
	uint32_t w_addr = 0;
	pBootAppFrame pmsg = (pBootAppFrame)src_buff;
	
	printf("d fram step=%04x, param step=%04x\r\n", pmsg->DataFrame.FrameStep, mIap_Param.frame_step+1);
	//校验当前包是否合法
	if((pmsg->DataFrame.FrameStep == (mIap_Param.frame_step + 1))&&
	 pmsg->DataFrame.CRC16 == crc16_ccitt(src_buff, FRAME_LENGTH - 2)&&
	 pmsg->DataFrame.SoftVer == mIap_Param.fw_msg.MsgFrame.AppVer)
	{
			w_addr = pmsg->DataFrame.FrameStep * pmsg->DataFrame.FrameStep; 
			if((ret=write_firware(w_addr, src_buff, FRAME_LENGTH)) == ret_ok) //下入方式必须用带页校验的
			{
				//获取下一包
				ret = find_firmware_lost_step(&mIap_Param, &needPkg);
				
				mIap_Param.frame_step++;
				if(ret == 0)
				{
					return ret_ok;
				}
				else
				{
					*ret_result = needPkg;	//需要的包ID
					printf("needPkg=%04x\r\n", needPkg);
					return ret_need_pkg;
				}
				//返回需要的下一包数据
				
			}
			else
			{
				return ret;
			}
	}
	else
	{
		return  ret_data_error; 
	}  
}

/*==================================================================================
* 函 数 名： write_flag_to_iap
* 参    数： None
* 功能描述:	 写入升级跳转标志
* 返 回 值： None
* 备    注： 
* 作    者：  
* 创建时间： 2021-01-11 105718
==================================================================================*/
static void write_flag_to_iap(void)
{ 
	uint32_t jump_flag_value = TO_IAP_FLAG; 
	//写入标志 
	mIap_Param.msg_flash->write(mIap_Param.boot_msg_addr, (uint8_t*)&jump_flag_value, 4);
}

/*==================================================================================
* 函 数 名： iap_reset_system
* 参    数： None
* 功能描述:  系统复位
* 返 回 值： None
* 备    注： 
* 作    者：  
* 创建时间： 2021-01-11 105718
==================================================================================*/
static uint8_t iap_reset_system(void)
{ 
	//重写标志位
	write_flag_to_iap();

	HAL_NVIC_SystemReset();
	//SOFT_SYSTEM_RESET(); //软件复位系统

	while(1);	//等待复位完成
} 
