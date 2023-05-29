#ifndef _MIFARE_H_
#define _MIFARE_H_
#define KEY_A_M1 0
#define KEY_B_M1 1
#define KEY_A_SH 3
#define KEY_B_SH 4
extern unsigned char SECTOR,BLOCK,BLOCK_NUM;
extern unsigned char BLOCK_DATA[16];
extern unsigned char KEY_A[16][6];
extern unsigned char KEY_B[16][6];
extern void Mifare_Clear_Crypto(void);
unsigned char Mifare_LoadKey(unsigned char *mifare_key);
extern unsigned char Mifare_Transfer(unsigned char block,unsigned char *ack);
extern unsigned char Mifare_Restore(unsigned char block,unsigned char *ack);
extern unsigned char Mifare_Blockset(unsigned char block,unsigned char *data_buff);
extern unsigned char Mifare_Blockinc(unsigned char block,unsigned char *data_buff,unsigned char *ack);
extern unsigned char Mifare_Blockdec(unsigned char block,unsigned char *data_buff,unsigned char *ack);
extern unsigned char Mifare_Blockwrite(unsigned char block,unsigned char *data_buff);
extern unsigned char Mifare_Blockread(unsigned char block,unsigned char *data_buff);
extern unsigned char Mifare_Auth(unsigned char key_mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid);
#endif
