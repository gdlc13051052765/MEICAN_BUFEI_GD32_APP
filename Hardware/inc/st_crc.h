#ifndef  __ST_CRC_H
#define  __ST_CRC_H
#include <stdint.h>


void st_crc_init(void);
uint32_t st_crc32(uint8_t *buff, uint16_t len);
void st_crc32_reset(void);
uint32_t st_crc32_no_reset(uint8_t *buff, uint16_t len);
/*********************************************************
// 函数名称：CRC32DataCheckFun
// 函数功能：数据CRC32校验函数
// 入口参数：checkDataBuf：待校验的数组；checkDataLen：待校验的数据长度
// 出口参数：返回值：CRC32校验码
// 注    释：
*********************************************************/
uint32_t CRC32DataCheckFun(uint8_t *checkDataBuf, uint16_t checkDataLen);

/*
  * 函数功能: Modbus CRC16 校验计算函数
  * 输入参数: pushMsg:待计算的数据首地址,usDataLen:数据长度
  * 返 回 值: CRC16 计算结果
  * 说    明: 计算结果是高位在前,需要转换才能发送
  */
uint16_t MB_CRC16(uint8_t *_pushMsg,uint8_t _usDataLen);
#endif
