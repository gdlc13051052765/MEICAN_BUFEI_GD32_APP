#ifndef  __ST_CRC_H
#define  __ST_CRC_H
#include <stdint.h>


void st_crc_init(void);
uint32_t st_crc32(uint8_t *buff, uint16_t len);
void st_crc32_reset(void);
uint32_t st_crc32_no_reset(uint8_t *buff, uint16_t len);
/*********************************************************
// �������ƣ�CRC32DataCheckFun
// �������ܣ�����CRC32У�麯��
// ��ڲ�����checkDataBuf����У������飻checkDataLen����У������ݳ���
// ���ڲ���������ֵ��CRC32У����
// ע    �ͣ�
*********************************************************/
uint32_t CRC32DataCheckFun(uint8_t *checkDataBuf, uint16_t checkDataLen);

/*
  * ��������: Modbus CRC16 У����㺯��
  * �������: pushMsg:������������׵�ַ,usDataLen:���ݳ���
  * �� �� ֵ: CRC16 ������
  * ˵    ��: �������Ǹ�λ��ǰ,��Ҫת�����ܷ���
  */
uint16_t MB_CRC16(uint8_t *_pushMsg,uint8_t _usDataLen);
#endif
