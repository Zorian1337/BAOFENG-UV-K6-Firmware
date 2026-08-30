#ifndef _DATAADDRMAPTYPE_H_
    #define _DATAADDRMAPTYPE_H_

// Define the Flash addresses used
#define CHAN_SIZE                 32       // Channel information size
#define CHAN_ADDR                 0x0000   // Channel start address
#define NAME_ADDR_SHIFT           20       // Channel name address offset
#define NAME_SIZE                 12       // Channel name information size is 16 bytes

#define VFO_INFO_ADDR             0x8000   // VFO storage address
#define VFO_SIZE                  32       // VFO channel information size
#define VFO1_ADDR                 0x8000   // VFO address
#define VFO2_ADDR                 0x8020   // VFO address

#define RADIO_IMFOS_ADDR          0X9000   // Radio frequency mode / function information / receiver address
#define RADIO_SIZE                64       // Optional information size

#define PWR_MSG_ADDR              0x9040   // Power-on display information

// Dual-tone multi-frequency [DTMF]
#define DTMFINFOR_ADDR            0xA000

// Remote kill code address, 16 bytes
#define DTMF_KILLED_ADDR          0xA010

// Define DTMF storage address 16
#define DTMF_SIZE                 10
#define DTMF_CODE_ADDR            0xA020


// Scan list usage address
#define SCAN_LIST_ADDR            0xB000

/************************************************************************/
#define FM_IMFOS_ADDR             0XC000   // Radio function information / receiver address
#define FM_SIDZE                  32
#define FM_ADDR                   0xC000   // Receiver frequency storage address

/************************************************************************/
#define RF_MODEL_ADDR             0xD000  // Model code storage address

#define RF_TXEN_ADDR              0xD001  // Transmit enable: 220M / 350M / 520M transmit enable, aviation band receive enable
/************************************************************************/
/* Local runtime information storage */
/************************************************************************/
// Define channel number, receiver, and related addresses
#define SYSTEMRAN_ADDR            0xE000
#define F1CHAN_ADDR               0xE000
#define F2CHAN_ADDR               0xE002

/************************************************************************/
/* Debug information addresses begin at 0xF000 */
/************************************************************************/
#define DEBUG_ADDR                0XF000

// Battery voltage
#define DEV_BATT_ADDR             0XF200
// Modulation
#define RF_MODULATION_ADDR        0XF210
// Chinese/English
#define DEV_LANGUAGE_ADDR         0XF220
// Bluetooth enable
#define DEV_BTEN_ADDR             0XF221

// APC output
#define RF_ADJUST_BASE_ADDR       0XF000
#define RF_PWR_H_U_400_ADDR       0XF000
#define RF_PWR_H_V_136_ADDR       0XF010
#define RF_PWR_H_V_200_ADDR       0XF020
#define RF_PWR_H_U_350_ADDR       0XF030
#define RF_PWR_M_U_400_ADDR       0XF040
#define RF_PWR_M_V_136_ADDR       0XF050
#define RF_PWR_M_V_200_ADDR       0XF060
#define RF_PWR_M_U_350_ADDR       0XF070
#define RF_PWR_L_U_400_ADDR       0XF080
#define RF_PWR_L_V_136_ADDR       0XF090
#define RF_PWR_L_V_200_ADDR       0XF0A0
#define RF_PWR_L_U_350_ADDR       0XF0B0

// SQL threshold
#define RF_SQL_TAB_ADDR           0XF0C0
#define RF_SQL_TAB_MUTE_ADDR      0XF0D0
#define RF_SQL_U_400_ADDR         0XF0E0  // Squelch correction value
#define RF_SQL_V_136_ADDR         0XF0F0  // Squelch correction value
#define RF_SQL_V_200_ADDR         0XF100  // Squelch correction value
#define RF_SQL_U_350_ADDR         0XF110  // Squelch correction value

#define BAND_ADDR                 0XF230  // V/U/200M frequency range
#define BAND2_ADDR                0XF240  // 300M frequency range

#define MODEL_ADDR                0xF250  // Model code storage address
/************************************************************************/

#endif

