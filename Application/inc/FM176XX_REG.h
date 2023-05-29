/*********************************************************************
*                                                                    *
*   Copyright (c) 2010 Shanghai FuDan MicroElectronic Inc, Ltd.      *
*   All rights reserved. Licensed Software Material.                 *
*                                                                    *
*   Unauthorized use, duplication, or distribution is strictly       *
*   prohibited by law.                                               *
*                                                                    *
**********************************************************************/
#ifndef _FM176XX_REG_H
#define _FM176XX_REG_H

#define  REG_COMMAND         0x00    //
#define  REG_HOSTCTRL        0x01    //
#define  REG_FIFOCONTROL     0x02    //
#define  REG_WATERLEVEL      0x03    //
#define  REG_FIFOLENGTH      0x04    //
#define  REG_FIFODATA        0x05    //
#define  REG_IRQ0            0x06    //
#define  REG_IRQ1            0x07    //
#define  REG_IRQ0EN          0x08    //
#define  REG_IRQ1EN          0x09    //
#define  REG_ERROR           0x0A    //    
#define  REG_STATUS          0x0B    //
#define  REG_RXBITCTRL       0x0C    //
#define  REG_RXCOLL       	 0x0D    //
#define  REG_TCONTROL        0x0E    //
#define  REG_T0CONTROL       0x0F    //                            
                               
                               
#define  REG_T0RELOADHI      0x10    //
#define  REG_T0RELOADLO      0x11    //
#define  REG_T0COUNTERVALHI  0x12    //                           
#define  REG_T0COUNTERVALLO  0x13    //                 
#define  REG_T1CONTROL       0x14    //                      
#define  REG_T1RELOADHI      0x15    //
#define  REG_T1RELOADLO      0x16    //
#define  REG_T1COUNTERVALHI  0x17    //                
#define  REG_T1COUNTERVALLO  0x18    //  
#define  REG_T2CONTROL       0x19    // 
#define  REG_T2RELOADHI      0x1A    //
#define  REG_T2RELOADLO      0x1B    //      
#define  REG_T2COUNTERVALHI  0x1C    //
#define  REG_T2COUNTERVALLO  0x1D    //                          
#define  REG_T3CONTROL       0x1E    //                       
#define  REG_T3RELOADHI      0x1F    //
                               
                               
#define  REG_T3RELOADLO      0x20    //
#define  REG_T3COUNTERVALHI  0x21    //                
#define  REG_T3COUNTERVALLO  0x22    //                
#define  REG_T4CONTROL       0x23    //
#define  REG_T4RELOADHI      0x24    //                    
#define  REG_T4RELOADLO      0x25    //
#define  REG_T4COUNTERVALHI  0x26    //
#define  REG_T4COUNTERVALLO  0x27    //
#define  REG_TXMODE          0x28    
#define  REG_TXAMP           0x29
#define  REG_TXCON           0x2A    //
#define  REG_TXI			       0x2B    //
#define  REG_TXCRCCON        0x2C    //
#define  REG_RXCRCCON        0x2D    //  
#define  REG_TXDATANUM       0x2E
#define  REG_TXMODWIDTH  		 0x2F    // 
                               
                               
#define  REG_TXSYM10BURSTLEN 0x30    //
#define  REG_TXWAITCTRL      0x31    //                             
#define  REG_TXWAITLO        0x32    //                            
#define  REG_FRAMECON        0x33    //                        
#define  REG_RXSOFD			     0x34    //                             
#define  REG_RXCTRL        	 0x35    //                         
#define  REG_RXWAIT          0x36    //                             
#define  REG_RXTHRESHOLD     0x37    //
#define  REG_RCV             0x38    //                             
#define  REG_RXANA           0x39    //                              
#define  REG_LPCD_OPTIONS    0x3A    //                             
#define  REG_SERIALSPEED     0x3B    //                              
#define  REG_LFO_TRIMM       0x3C    //                              
#define  REG_CLKOUT_CTRL     0x3D    //                              
#define  REG_LPCD_THRESHOLD  0x3E    //                              
#define  REG_LPCD_QMIN       0x3F    //                            

#define  REG_THNADJ          0x5F
#define  REG_THNSET				   0x61
#define  REG_THNMIN				   0x62
#define  REG_RXTXCON			   0x77
#define  REG_VERSION				 0x7F

#define  CMD_MASK          	 0x1F

#define     CMD_IDLE         0x00
#define     CMD_LPCD     		 0x01 
#define     CMD_LOADKEY   	 0x02
#define     CMD_AUTHENT      0x03 
#define     CMD_RECEIVE      0x05 
#define     CMD_TRANSMIT   	 0x06 
#define     CMD_TRANSCEIVE   0x07 
#define			CMD_WRITEE2			 0x08
#define			CMD_WRITEE2PAGE	 0x09
#define			CMD_READE2			 0x0A
#define     CMD_LOADREG		   0x0C 
#define     CMD_LOADPROTOCOL 0x0D 
#define     CMD_LOADKEYE2    0x0E 
#define     CMD_STOREKEYE2   0x0F 
#define     CMD_CRCCALC      0x1B
#define     CMD_READRNR      0x1C
#define     CMD_SOFTRESET    0x1F 


/** \name Host-Control Register Contents (0x00)
*/
/*@{*/
#define BIT_STANDBY			0x80U   /**< Standby bit; If set, the IC transits to standby mode. */
#define BIT_MODEMOFF		0x40U  
/*@{*/
/** \name Host-Control Register Contents (0x01)
*/
/*@{*/
#define BIT_I2CFORCEHS		0x01U
//#define BIT_REGEN                0x80U
//#define BIT_BUSHOST              0x40U
//#define BIT_BUSSAM               0x20U
//#define MASK_SAMINTERFACE        0x0CU
/*@}*/

/** \name FIFO-Control Register Contents (0x02)
*/
/*@{*/
#define BIT_FIFOSIZE             0x80U
#define BIT_HIALERT              0x40U
#define BIT_LOALERT              0x20U
#define BIT_FIFOFLUSH            0x10U
#define BIT_WATERLEVEL_HI        0x04U
#define MASK_FIFOLENGTH_HI       0x03U
/*@}*/

/** \name IRQ0 Register(s) Contents (0x06/0x08)
*/
/*@{*/
#define BIT_SET                  0x80U
#define BIT_IRQINV               0x80U
#define BIT_HIALERTIRQ           0x40U
#define BIT_LOALERTIRQ           0x20U
#define BIT_IDLEIRQ              0x10U
#define BIT_TXIRQ                0x08U
#define BIT_RXIRQ                0x04U
#define BIT_ERRIRQ               0x02U
#define BIT_RXSOFIRQ               0x01U
/*@}*/

/** \name IRQ1 Register(s) Contents (0x07/0x09)
*/
/*@{*/
/* #define BIT_SET                  0x80U */
#define BIT_IRQPUSHPULL          0x80U
#define BIT_GLOBALIRQ            0x40U
#define BIT_IRQPINEN             0x40U
#define BIT_LPCDIRQ              0x20U
#define BIT_TIMER4IRQ            0x10U
#define BIT_TIMER3IRQ            0x08U
#define BIT_TIMER2IRQ            0x04U
#define BIT_TIMER1IRQ            0x02U
#define BIT_TIMER0IRQ            0x01U
/*@}*/

/** \name Error Register Contents (0x0A)
*/
/*@{*/
#define BIT_CMDEE_ERR            0x80U
#define BIT_FIFOWRERR            0x40U
#define BIT_FIFOOVL              0x20U
#define BIT_MINFRAMEERR          0x10U
#define BIT_NODATAERR            0x08U
#define BIT_COLLDET              0x04U
#define BIT_PROTERR              0x02U
#define BIT_INTEGERR             0x01U
/*@}*/

/** \name Status Register Contents (0x0B)
*/
/*@{*/
#define BIT_CRYPTO1ON            0x20U
#define MASK_COMMSTATE           0x07U
/*@}*/

/** \name Rx-Bit-Control Register Contents (0x0C)
*/
/*@{*/
#define BIT_VALUESAFTERCOLL      0x80U
#define BIT_NOCOLL               0x08U
#define MASK_RXALIGN             0x70U
#define MASK_RXLASTBITS          0x07U
/*@}*/

/** \name Rx-Coll Register Contents (0x0D)
*/
/*@{*/
#define BIT_COLLPOSVALID         0x80U
#define MASK_COLLPOS             0x7FU
/*@}*/

/** \name Timer-Control Register Contents (0x0E)
*/
/*@{*/
#define BIT_T3RUNNING            0x80U
#define BIT_T2RUNNING            0x40U
#define BIT_T1RUNNING            0x20U
#define BIT_T0RUNNING            0x10U
#define BIT_T3STARTSTOPNOW       0x08U
#define BIT_T2STARTSTOPNOW       0x04U
#define BIT_T1STARTSTOPNOW       0x02U
#define BIT_T0STARTSTOPNOW       0x01U
/*@}*/

/** \name T[0-3]-Control Register Contents (0x0F/0x14/0x19/0x1E)
*/
/*@{*/
#define BIT_TSTOP_RX             0x80U   /**< Stop timer on receive interrupt. */
#define BIT_TAUTORESTARTED       0x08U   /**< Auto-restart timer after underflow. */
#define BIT_TSTART_TX            0x10U   /**< Start timer on transmit interrupt. */
//#define BIT_TSTART_LFO           0x20U   /**< Use this timer for LFO trimming. */
//#define BIT_TSTART_LFO_UV        0x30U   /**< Use this timer for LFO trimming (generate UV at a trimming event). */
#define MASK_TSTART              0x30U   /**< Mask for TSTART bits. */
#define VALUE_TCLK_1356_MHZ      0x00U   /**< Use 13.56MHz as input clock. */
#define VALUE_TCLK_212_KHZ       0x01U   /**< Use 212KHz as input clock. */
#define VALUE_TCLK_T0            0x02U   /**< Use timer0 as input clock. */
#define VALUE_TCLK_T1            0x03U   /**< Use timer1 as input clock. */
/*@}*/

/** \name T4-Control Register Contents (0x23)
*/
/*@{*/
#define BIT_T4RUNNING            0x80U
#define BIT_T4STARTSTOPNOW       0x40U
#define BIT_T4AUTOTRIMM          0x20U
#define BIT_T4AUTOLPCD           0x10U
#define BIT_T4AUTORESTARTED      0x08U
#define BIT_T4AUTOWAKEUP         0x04U
/*#define MASK_TSTART              0x30U*/
#define VALUE_TCLK_LFO_64_KHZ    0x00U
#define VALUE_TCLK_LFO_8_KHZ     0x01U
#define VALUE_TCLK_LFO_4_KHZ     0x02U
#define VALUE_TCLK_LFO_2_KHZ     0x03U
/*@}*/

/** \name Driver Mode Register Contents (0x28)
*/
/*@{*/
#define BIT_TX2INV               0x80U
#define BIT_TX1INV               0x40U
#define BIT_TXEN                 0x08U
#define VALUE_TXCLKMODE_HIGHIMPEDANCE		0x00U
#define VALUE_TXCLKMODE_OUTPULL0				0x01U
#define VALUE_TXCLKMODE_OUTPULL1				0x02U
#define VALUE_TXCLKMODE_RFLOWPULL				0x05U
#define VALUE_TXCLKMODE_RFHIGHPUSH			0x06U
#define VALUE_TXCLKMODE_PUSHPULL				0x07U
#define BIT_RFON                 0x04U
#define BIT_TPUSHON              0x02U
#define BIT_TPULLON              0x01U
/*@}*/

/** \name Tx Amplifier Register Contents (0x29)
*/
/*@{*/
#define MASK_CW_AMPLITUDE        0x00U
#define MASK_RESIDUAL_CARRIER    0x1FU
/*@}*/

/** \name Driver Control Register Contents (0x2A)
*/
/*@{*/
#define BIT_CWMAX                0x08U
#define BIT_DRIVERINV            0x04U
#define VALUE_DRIVERSEL_LOW      0x00U
#define VALUE_DRIVERSEL_TXENV    0x01U
#define VALUE_DRIVERSEL_SIGIN    0x02U
/*@}*/

/** \name Tx-/Rx-CRC Control Register Contents (0x2C/0x2D)
*/
/*@{*/
#define BIT_RXFORCECRCWRITE      0x80U
#define BIT_CRCINVERT            0x02U
#define BIT_CRCEN                0x01U
#define MASK_CRCPRESETVAL        0x70U
#define MASK_CRCTYPE             0x0CU
#define MASK_CRCTYPE5            0x00U
#define MASK_CRCTYPE16           0x08U
/*@}*/

/** \name Tx-DataNum Register Contents (0x2E)
*/
/*@{*/
#define BIT_KEEPBITGRID          0x10U
#define BIT_DATAEN               0x08U
#define MASK_TXLASTBITS          0x07U
#define MASK_SYMBOL_SEND         0x08U
/*@}*/

/** \name Tx-Wait Control Register Contents (0x31)
*/
/*@{*/
#define BIT_TXWAIT_START_RX      0x80U
#define BIT_TXWAIT_DBFREQ        0x40U
#define MASK_TXWAITHI            0x38U
#define MASK_TXSTOPBITLEN        0x07U
/*@}*/

/** \name Frame Control Register Contents (0x33)
*/
/*@{*/
#define BIT_TXPARITYEN           0x80U
#define BIT_RXPARITYEN           0x40U
#define VALUE_STOP_SYM3          0x0CU
#define VALUE_STOP_SYM2          0x08U
#define VALUE_STOP_SYM1          0x04U
#define VALUE_START_SYM3		 0x03U
#define VALUE_START_SYM2         0x02U
#define VALUE_START_SYM1         0x01U
#define MASK_STARTSYM            0x03U
#define MASK_STOPSYM             0x0CU
/*@}*/

/** \name Rx Control Register Contents (0x35)
*/
/*@{*/
#define BIT_RXALLOWBITS          0x80U
#define BIT_RXMULTIPLE           0x40U
#define BIT_RXEOFTYPE            0x20U
#define BIT_EGT_CHECK            0x10U
#define BIT_EMD_SUPPRESSION      0x08U
#define MASK_RXBAUDRATE          0x07U
/*@}*/

/** \name Rx-Wait Register Contents (0x36)
*/
/*@{*/
#define BIT_RXWAITDBFREQ         0x80U
#define MASK_RXWAIT              0x7FU
/*@}*/

/** \name Rx-Threshold Register Contents (0x37)
*/
/*@{*/
#define MASK_MINLEVEL            0xF0U
#define MASK_MINLEVELP           0x0FU
/*@}*/

/** \name Rx-Receiver Register Contents (0x38)
*/
/*@{*/
#define BIT_RX_SINGLE            0x80U
#define BIT_RX_SHORT_MIX2ADC     0x40U
#define BIT_USE_SMALL_EVAL       0x04U
#define MASK_RX_SIGPRO_IN_SEL    0x30U
#define MASK_COLLLEVEL           0x03U
/*@}*/

/** \name Rx-Analog Register Contents (0x39)
*/
/*@{*/
#define BIT_RX_OC_ENABLE         0x20U
#define BIT_RX_HP_LOWF           0x10U
#define MASK_VMID_R_SEL          0xC0U
#define MASK_RCV_HPCF            0x0CU
#define MASK_RCV_GAIN            0x03U
/*@}*/

/** \name Serial-Speed Register Contents (0x3B)
*/
/*@{*/
#define MASK_BR_T0               0xE0U
#define MASK_BR_T1               0x1FU
/*@}*/


/** \name LPCD Result(Q) Register Contents (0x43)
*/
/*@{*/
#define BIT_LPCDIRQ_CLR          0x40U
/*@}*/

/** \name Tx-BitMod Register Contents (0x48)
*/
/*@{*/
#define BIT_TXMSBFIRST           0x80U
#define BIT_TXPARITYTYPE         0x20U
#define BIT_TXSTOPBITTYPE        0x08U
#define BIT_TXSTARTBITTYPE       0x02U
#define BIT_TXSTARTBITEN         0x01U
/*@}*/

/** \name Rx-BitMod Register Contents (0x58)
*/
/*@{*/
#define BIT_RXSTOPONINVPAR       0x20U
#define BIT_RXSTOPONLEN          0x10U
#define BIT_RXMSBFIRST           0x08U
#define BIT_RXSTOPBITEN          0x04U
#define BIT_RXPARITYTYPE         0x02U
/*@}*/

/** \name Rx-Mod Register Contents (0x5D)
*/
/*@{*/
#define BIT_PREFILTER            0x20U
#define BIT_RECTFILTER           0x10U
#define BIT_SYNCHIGH             0x08U
#define BIT_CORRINV              0x04U
#define BIT_FSK                  0x02U
#define BIT_BPSK                 0x01U
/*@}*/

/** \name RxSupCfg Register Contents (0x6E)
*/
/*@{*/
#define BIT_RXNOERR            0x80U
/*@}*/
/** \name RxTxConReg Register Contents (0x77)
*/
/*@{*/
#define BIT_SHMODE				0x08U	//上海算法
/*@}*/
#define LPCD_OPTION2 			0x1DF


//---------------------------------------------------------------
//定义接收协议号
#define	RX_TYPEA_106			0 
#define	RX_TYPEA_212			1
#define	RX_TYPEA_424			2
#define	RX_TYPEA_848			3

#define	RX_TYPEB_106			4
#define	RX_TYPEB_212			5
#define	RX_TYPEB_424			6
#define	RX_TYPEB_848			7

#define	RX_TYPEV_26				10
#define	RX_TYPEV_53				11

#define	RX_FELICA_212			19
#define	RX_FELICA_424			20

//定义发射协议号
#define	TX_TYPEA_106			0 
#define	TX_TYPEA_212			1
#define	TX_TYPEA_424			2
#define	TX_TYPEA_848			3

#define	TX_TYPEB_106			4
#define	TX_TYPEB_212			5
#define	TX_TYPEB_424			6
#define	TX_TYPEB_848			7

#define	TX_TYPEV_26				10
#define	TX_TYPEV_53				11

#define	TX_FELICA_212			19
#define	TX_FELICA_424			20

#endif 


