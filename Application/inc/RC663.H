#ifndef __RC663_H
#define __RC663_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "nfc_interface.h"
#include "hwioconfig.h"
//#include "stm32f10x.h"
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t  s8;

#define UART_PRINT

#define RC663_NSS_GROUP				RC663_CS_GPIO_Port
#define RC663_NSS_PIN					RC663_CS_Pin

#define RC663_RST_GROUP				RC663_RST_GPIO_Port
#define RC663_RST_PIN					RC663_RST_Pin

#define RC663_IRQ_GROUP				RC663_IRQT_GPIO_Port
#define RC663_IRQ_PIN					RC663_IRQT_Pin

#define RC663_LED_GROUP				LED1_GPIO_Port
#define RC663_LED_PIN					LED1_Pin


#define	 RC663_NSS_1	 do{ HAL_GPIO_WritePin(RC663_NSS_GROUP, RC663_NSS_PIN, GPIO_PIN_SET);}while(0)
#define	 RC663_NSS_0	 do{ HAL_GPIO_WritePin(RC663_NSS_GROUP, RC663_NSS_PIN, GPIO_PIN_RESET);}while(0)

#define	 PDOWN_1	 do{ HAL_GPIO_WritePin(RC663_RST_GROUP, RC663_RST_PIN, GPIO_PIN_SET);}while(0)
#define	 PDOWN_0	 do{ HAL_GPIO_WritePin(RC663_RST_GROUP, RC663_RST_PIN, GPIO_PIN_RESET);}while(0)

#define	 LED_1	 do{ HAL_GPIO_WritePin(RC663_LED_GROUP, RC663_LED_PIN, GPIO_PIN_SET);}while(0)
#define	 LED_0	 do{ HAL_GPIO_WritePin(RC663_LED_GROUP, RC663_LED_PIN, GPIO_PIN_RESET);}while(0)


/////////////////////////////////////////////
#define rRegCommand 					0x00 // Starts and stops command execution
#define rRegHostCtrl 				0x01 // Host control register
#define rRegFIFOControl 			0x02 // Control register of the FIFO
#define rRegWaterLevel 			0x03 // Level of the FIFO underflow and overflow warning
#define rRegFIFOLength 			0x04 // Length of the FIFO
#define rRegFIFOData 				0x05 // Data In/Out exchange register of FIFO buffer
#define rRegIRQ0 						0x06 // Interrupt register 0
#define rRegIRQ1 						0x07 // Interrupt register 1
#define rRegIRQ0En 					0x08 // Interrupt enable register 0
#define rRegIRQ1En 					0x09 // Interrupt enable register 1
#define rRegError 						0x0A // Error bits showing the error status of the last command execution
#define rRegStatus 					0x0B // Contains status of the communication
#define rRegRxBitCtrl 				0x0C // Control register for anticollision adjustments for bit oriented protocols
#define rRegRxColl 					0x0D // Collision position register
#define rRegTControl 				0x0E // Control of Timer 0..3
#define rRegT0Control 				0x0F // Control of Timer0
#define rRegT0ReloadHi 			0x10 // High register of the reload value of Timer0
#define rRegT0ReloadLo 			0x11 // Low register of the reload value of Timer0
#define rRegT0CounterValHi 	0x12 // Counter value high register of Timer0
#define rRegT0CounterValLo 	0x13 // Counter value low register of Timer0
#define rRegT1Control 				0x14 // Control of Timer1
#define rRegT1ReloadHi 			0x15 // High register of the reload value of Timer1
#define rRegT1ReloadLo 			0x16 // Low register of the reload value of Timer1
#define rRegT1CounterValHi 	0x17 // Counter value high register of Timer1
#define rRegT1CounterValLo 	0x18 // Counter value low register of Timer1
#define rRegT2Control 				0x19 // Control of Timer2
#define rRegT2ReloadHi 			0x1A // High byte of the reload value of Timer2
#define rRegT2ReloadLo 			0x1B // Low byte of the reload value of Timer2
#define rRegT2CounterValHi 	0x1C // Counter value high byte of Timer2
#define rRegT2CounterValLo 	0x1D // Counter value low byte of Timer2
#define rRegT3Control 				0x1E // Control of Timer3
#define rRegT3ReloadHi 			0x1F // High byte of the reload value of Timer3
#define rRegT3ReloadLo 			0x20 // Low byte of the reload value of Timer3
#define rRegT3CounterValHi 	0x21 // Counter value high byte of Timer3
#define rRegT3CounterValLo 	0x22 // Counter value low byte of Timer3
#define rRegT4Control 				0x23 // Control of Timer4
#define rRegT4ReloadHi 			0x24 // High byte of the reload value of Timer4
#define rRegT4ReloadLo 			0x25 // Low byte of the reload value of Timer4
#define rRegT4CounterValHi 	0x26 // Counter value high byte of Timer4
#define rRegT4CounterValLo 	0x27 // Counter value low byte of Timer4
#define rRegDrvMod 					0x28 // Driver mode register
#define rRegTxAmp 						0x29 // Transmitter amplifier register
#define rRegDrvCon 					0x2A // Driver configuration register
#define rRegTxl 							0x2B // Transmitter register
#define rRegTxCrcPreset 			0x2C // Transmitter CRC control register, preset value
#define rRegRxCrcPreset 			0x2D // Receiver CRC control register, preset value
#define rRegTxDataNum 				0x2E // Transmitter data number register
#define rRegTxModWidth 			0x2F // Transmitter modulation width register
#define rRegTxSym10BurstLen 	0x30 // Transmitter symbol 1 + symbol 0 burst length register
#define rRegTXWaitCtrl 			0x31 // Transmitter wait control
#define rRegTxWaitLo 				0x32 // Transmitter wait low
#define rRegFrameCon 				0x33 // Transmitter frame control
#define rRegRxSofD 					0x34 // Receiver start of frame detection
#define rRegRxCtrl 					0x35 // Receiver control register
#define rRegRxWait 					0x36 // Receiver wait register
#define rRegRxThreshold 			0x37 // Receiver threshold register
#define rRegRcv 							0x38 // Receiver register
#define rRegRxAna 						0x39 // Receiver analog register
#define rRegRFU_3A						0x3A // -
#define rRegSerialSpeed 			0x3B // Serial speed register
#define rRegLFO_Trimm 				0x3C // Low-power oscillator trimming register
#define rRegPLL_Ctrl 				0x3D // IntegerN PLL control register, for microcontroller clock output adjustment
#define rRegPLL_DivOut 			0x3E // IntegerN PLL control register, for microcontroller clock output adjustment
#define rRegLPCD_QMin 				0x3F // Low-power card detection Q channel minimum threshold
#define rRegLPCD_QMax 				0x40 // Low-power card detection Q channel maximum threshold
#define rRegLPCD_IMin 				0x41 // Low-power card detection I channel minimum threshold
#define rRegLPCD_I_Result 		0x42 // Low-power card detection I channel result register
#define rRegLPCD_Q_Result 		0x43 // Low-power card detection Q channel result register
#define rRegPadEn 						0x44 // PIN enable register
#define rRegPadOut 					0x45 // PIN out register
#define rRegPadIn 						0x46 // PIN in register
#define rRegSigOut 					0x47 // Enables and controls the SIGOUT Pin
#define rRegTxBitMod 				0x48 // Transmitter bit mode register
#define rRegRFU_49						0x49 // -
#define rRegTxDataCon 				0x4A // Transmitter data configuration register
#define rRegTxDataMod 				0x4B // Transmitter data modulation register
#define rRegTxSymFreq 				0x4C // Transmitter symbol frequency
#define rRegTxSym0H 					0x4D // Transmitter symbol 0 high register
#define rRegTxSym0L 					0x4E // Transmitter symbol 0 low register
#define rRegTxSym1H 					0x4F // Transmitter symbol 1 high register
#define rRegTxSym1L 					0x50 // Transmitter symbol 1 low register
#define rRegTxSym2 					0x51 // Transmitter symbol 2 register
#define rRegTxSym3 					0x52 // Transmitter symbol 3 register
#define rRegTxSym10Len 			0x53 // Transmitter symbol 1 + symbol 0 length register
#define rRegTxSym32Len 			0x54 // Transmitter symbol 3 + symbol 2 length register
#define rRegTxSym10BurstCtrl 0x55 // Transmitter symbol 1 + symbol 0 burst control register
#define rRegTxSym10Mod 			0x56 // Transmitter symbol 1 + symbol 0 modulation register
#define rRegTxSym32Mod 			0x57 // Transmitter symbol 3 + symbol 2 modulation register
#define rRegRxBitMod 				0x58 // Receiver bit modulation register
#define rRegRxEofSym 				0x59 // Receiver end of frame symbol register
#define rRegRxSyncValH 			0x5A // Receiver synchronisation value high register
#define rRegRxSyncValL 			0x5B // Receiver synchronisation value low register
#define rRegRxSyncMod 				0x5C // Receiver synchronisation mode register
#define rRegRxMod 						0x5D // Receiver modulation register
#define rRegRxCorr 					0x5E // Receiver correlation register
#define rRegFabCal 					0x5F // Calibration register of the receiver, calibration performed at production
#define rReg_60 							0x60 //
#define rReg_61 							0x61 //
#define rReg_66 							0x66 //
#define rReg_6A 							0x6A //
#define rReg_6B 							0x6B //
#define rReg_6C 							0x6C //
#define rReg_6D 							0x6D //
#define rReg_6E 							0x6E //
#define rReg_6F 							0x6F //
#define rRegVersion 					0x7F // Version and subversion register
//////////////////////////////////////////////////////////
//		Command 			No. Parameter (bytes) 	Short description
#define RC663_Idle 				0x00 //- 					no action, cancels current command execution
#define RC663_LPCD 				0x01 //- 					low-power card detection
#define RC663_LoadKey 		0x02 //(keybyte1..6); 		reads a MIFARE key (size of 6 bytes) from FIFO buffer and puts it into Key buffer
#define RC663_MFAuthent 	0x03 //60h or 61h,(block address),(card serial number byte0..3) 	performs the MIFARE standard authentication in MIFARE read/write mode only
#define RC663_AckReq 			0x04 //- 					performs a query, an Ack and a Req-Rn for ISO/IEC 18000-3 mode 3/ EPC Class-1 HF
#define RC663_Receive 		0x05 //- 					activates the receive circuit
#define RC663_Transmit 		0x06 //- 					transmits data from the FIFO buffer
#define RC663_Transceive 	0x07 //- 					transmits data from the FIFO buffer and automatically activates the receiver after transmission finished
#define RC663_WriteE2 		0x08 //addressH, addressL, data; 	gets one byte from FIFO buffer and writes it to the internal EEPROM, 
#define RC663_WriteE2Page 0x09 //(page Address), data0, [data1..data63]; 	gets up to 64 bytes (one EEPROM page) from the FIFO buffer and writes it to the EEPROM
#define RC663_ReadE2 			0x0A // address H, addressL,length; 	reads data from the EEPROM and copies it into the FIFO buffer
#define RC663_LoadReg 		0x0C //(EEPROM addressL), (EEPROM addressH), RegAdr, (number of Register to be copied);  reads data from the internal EEPROM and initializes the CLRC663 registers. EEPROM address needs to be within EEPROM sector 2
#define RC663_LoadProtocol 0x0D //(Protocol number RX), (Protocol number TX);		reads data from the internal EEPROM and initializes the CLRC663 registers needed for a Protocol change
#define RC663_LoadKeyE2 	0x0E //KeyNr; 				copies a key of the EEPROM into the key buffer
#define RC663_StoreKeyE2 	0x0F //KeyNr, byte1..6;	stores a MIFARE key (size of 6 bytes) into the EEPROM
#define RC663_ReadRNR 		0x1C //- 					Copies bytes from the Random Number generator into the FIFO until the FiFo is full
#define RC663_Soft_Reset 	0x1F //- 					resets the CLRC663

//////////////////////////////////////////////////////////
void delay_init(u8 SYSCLK);
void delay_us(u32);
void delay_ms(u16);

void RC663_Init(void);
u8 RC663_ReadReg(u8);
void RC663_WriteReg(u8, u8);

s8 RC663_CMD_ReadE2(u16 addr,u8 len,u8 *pdat);
s8 RC663_CMD_WriteE2(u16 addr,u8 dat);
s8 RC663_CMD_LoadProtocol(u8 rx,u8 tx);
s8 RC663_PcdConfigISOType(u8 type);
void RC663_MifareClassic(void);
void RC663_ID2(void);
void RC663_Felica(void);
void RC663_ISO15693(void);

s8 RC663_Lpcd_Calib(u8 *pI,u8 *pQ);
s8 RC663_Lpcd_Det(u8 ValueI,u8 ValueQ);


/*==================================================================================
* 函 数 名： get_card_tag_msg
* 参    数： None
* 功能描述:  获取卡片信息
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
_Tag_Info get_card_tag_msg(void);

int read_card_block( uint8_t* uid, uint8_t* block_buff, uint16_t* read_block_len);

_Tag_Info rc663_read_card_block(void);

#endif
