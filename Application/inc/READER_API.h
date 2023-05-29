#ifndef _READERAPI_H_
#define _READERAPI_H_
//#include "DEVICE_CFG.h"
#include "stm32f10x_type.h"
#include "stm32f1xx.h"
#include "nfc_interface.h"

static const unsigned char RF_CMD_REQA = 0x26;
static const unsigned char RF_CMD_WUPA	= 0x52;
static const unsigned char RF_CMD_ANTICOL[3] = {0x93,0x95,0x97} ;
static const unsigned char RF_CMD_SELECT[3] = {0x93,0x95,0x97} ;

static const unsigned char RF_CMD_KEYA = 0x60;
static const unsigned char RF_CMD_KEYB = 0x61;
//发射参数设置
#define MODWIDTH 0x26 //默认设置为 0x26 ，0x26 = 106K，0x13 = 212K， 0x09 = 424K ，0x04 = 848K

//接收参数配置 TYPE A
#define GAIN_A	0		//设置范围0~3
#define HPCF_A	0		//设置范围0~3
#define AMPLITUDE_A 3 //设置范围0~3
//接收参数配置 TYPE B
#define GAIN_B	2		//设置范围0~3
#define HPCF_B	2		//设置范围0~3
#define AMPLITUDE_B 3 //设置范围0~3
#define MODULATION	12//设置范围0~31
//接收参数配置 TYPE V
#define GAIN_V	1//设置范围0~3
#define HPCF_V  0//设置范围0~3
#define AMPLITUDE_V 1 //设置范围0~7
//0 = TVDD-100mV,1 = TVDD-175mV,2 = TVDD-250mV,3 = TVDD-350mV,4 = TVDD-500mV,5 = TVDD-650mV,6=TVDD-1000mV,7 = TVDD-850mV

#define GSNON	15		//设置范围0~15
#define MODGSNON 8 	//设置范围0~15
#define GSP 31			//设置范围0~63
#define MODGSP 31		//设置范围0~63
#define COLLLEVEL 4	//设置范围0~7
#define MINLEVEL  8	//设置范围0~15
#define RXWAIT 4		//设置范围0~63
#define UARTSEL 2		//默认设置为2  设置范围0~3 0:固定低电平 1:TIN包络信号 2:内部接收信号 3:TIN调制信号

typedef enum {CRC_NONE = 0,CRC_TX = 1, CRC_RX = 2,CRC_TX_RX = 3} crc_type;

struct picc_b_struct
{
unsigned char ATQB[12];//REQB应答
unsigned char	PUPI[4];
unsigned char	APPLICATION_DATA[4];
unsigned char	PROTOCOL_INF[3];
unsigned char CID[1];//ATTRIB应答
unsigned char Answer_to_HALT[1];//HALT应答
unsigned char SN[8];//身份证SN代码
};
extern struct picc_b_struct PICC_B; 

struct picc_a_struct
{
unsigned char ATQA[2];
unsigned char CASCADE_LEVEL;
unsigned char UID_Length;	
unsigned char UID[15];
unsigned char BCC[3];
unsigned char SAK[3];
};


extern struct picc_a_struct PICC_A; 

struct picc_v_struct
{
	unsigned char UID[8];
};

extern struct picc_v_struct PICC_V;

#define FM176XX_SUCCESS					0x00
#define FM176XX_RESET_ERR				0xF1
#define FM176XX_PARAM_ERR 			0xF2	//输入参数错误
#define FM176XX_TIMEROUT_ERR		0xF3	//接收超时
#define FM176XX_COMM_ERR				0xF4	//
#define FM176XX_COLL_ERR				0xF5	//冲突错误
#define FM176XX_FIFO_ERR				0xF6	//FIFO
#define FM176XX_CRC_PARITY_ERR	0xF7
#define FM176XX_PROTOCOL_ERR		0xF9
#define FM176XX_EEPROM_ERR			0xFA

#define FM176XX_AUTH_ERR		0xE1
#define FM176XX_LOADKEY_ERR	0xE2
#define FM176XX_RATS_ERR 		0xD1
#define FM176XX_PPS_ERR 		0xD2
#define FM176XX_PCB_ERR 		0xD3

typedef struct
{
	unsigned char Cmd;                 	// 命令代码
	crc_type PCRC;
	unsigned char nBitsToSend;					//准备发送的位数	
	unsigned char nBytesToSend;        	//准备发送的字节数
	unsigned char nBytesToReceive;			//准备接收的字节数
	unsigned char nBitsToReceive;				//准备接收的位数
	unsigned char nBytesReceived;      	// 已接收的字节数
	unsigned char nBitsReceived;       	// 已接收的位数
	unsigned char *pSendBuf;						//发送数据缓冲区
	unsigned char *pReceiveBuf;					//接收数据缓冲区
	unsigned char CollPos;             	// 碰撞位置
	unsigned char Error;								// 错误状态
	unsigned long Cycles;								//超时时间
} command_struct;
extern unsigned char SetBaudRate(unsigned char baudrate);

extern void SetTAPPS(unsigned char pps1);
extern void SetTBPPS(unsigned char pps1);
extern void SetParity(FunctionalState state);
extern void SetTimer(unsigned long cycles);
extern unsigned char Command_Execute(command_struct * comm_status);
extern unsigned char FM176XX_Initial_ReaderA(void);
extern unsigned char FM176XX_Initial_ReaderB(void);
extern unsigned char FM176XX_Initial_ReaderV(void);
extern unsigned char ReaderA_Halt(void);
extern unsigned char ReaderA_Request(struct picc_a_struct *type_a);
extern unsigned char ReaderA_Wakeup(struct picc_a_struct *type_a);

extern unsigned char ReaderA_AntiColl(struct picc_a_struct *type_a);
extern unsigned char ReaderA_Select(struct picc_a_struct *type_a);
extern unsigned char ReaderA_CardActivate(struct picc_a_struct *type_a);
extern unsigned char SetRf(FunctionalState state);
extern void SetSendCRC(unsigned char mode);
extern void SetReceiveCRC(unsigned char mode);
extern unsigned char ReaderB_Setting(unsigned char *value);
extern unsigned char ReaderB_Request(struct picc_b_struct *picc_b);
extern unsigned char ReaderB_Wakeup(struct picc_b_struct *picc_b);
extern unsigned char ReaderB_Attrib(unsigned char *param,struct picc_b_struct *picc_b);
extern unsigned char ReaderB_Halt(struct picc_b_struct *picc_b);
extern unsigned char ReaderB_Get_SN(struct picc_b_struct *picc_b);
extern unsigned char FM175XX_Polling(unsigned char *polling_card,struct picc_a_struct *picc_a,struct picc_b_struct *picc_b);

extern unsigned char ReaderV_SendSlot(void);
extern unsigned char ReaderV_Inventory(struct picc_v_struct *picc_v);
unsigned char ReaderV_Select(struct picc_v_struct *picc_v,unsigned char *response);
unsigned char ReaderV_ReadSingleBlock(unsigned char block_num,unsigned char *response);
unsigned char ReaderV_WriteSingleBlock(unsigned char block_num,unsigned char *block_data,unsigned char *response);

extern unsigned char FM176XX_TPDU(unsigned char *send_param,unsigned char *receive_param);


//读卡信息
 _Tag_Info fm17660_read_card_block(void);
#endif

