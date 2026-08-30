#ifndef __INC_PROGROMTYPE_H__
    #define __INC_PROGROMTYPE_H__
/***************************************************************************************/
#define FLASH_PRO_HEAD           0xA5       // Command packet header

#define FLASH_PRO_CMD_ADDR       0x01
#define FLASH_PRO_PACKED_ADDR    0x02
#define FLASH_PRO_LEN_ADDR       0x04
#define FLASH_PRO_DATA_ADDR      0x06

// Define commands
#define PRO_CMD_HANDSHAKE        0x02
#define PRO_CMD_ADDR             0x03
#define PRO_CMD_EARSE            0x04
#define PRO_CMD_END              0x06      // Restart command
#define PRO_CMD_WRITE            0x57

#define PRO_CMD_ERROR            0xEE

// Define error codes
#define ERROR_CODE_OK            0X59       // Data correct
#define ERROR_CODE_HEAD          0x01       // Header error
#define ERROR_CODE_HAND          0x02       // Handshake error
#define ERROR_CODE_CMD           0x03       // Command error
#define ERROR_CODE_DATA          0x04       // Data error
#define ERROR_CODE_WRITE         0x05       // Flash write error
#define ERROR_CODE_ADDR          0x06       // Flash address error

// Define erase modes
#define ERASE_MODE               0X45
#define ERASE_MODE_CHIP          0X01       // Full chip erase
#define ERASE_MODE_4K            0X02       // Erase 4K
#define ERASE_MODE_32K           0X03       // Erase 32K
#define ERASE_MODE_64K           0X04       // Erase 64K

enum{FLASH_PRG_CHECKHEAD=0,FLASH_PRG_CMD,FLASH_PRG_END,FLASH_PRG_ERROR};

typedef struct
{
    U8  cmd;                  // Current command received
    U32 StartAddr;            // Starting flash edit address
    U16 packageNum;           // Number of data packets received so far
    U16 length;               // Length of currently received data
    U16 txLength;             // Length of data to send
    U8  txBuf[64];
}STR_FLASH_PROGROM;

extern STR_FLASH_PROGROM    flashProgrom;    
/***************************************************************************************/
#define UART_MAX_NUM           1040        // Data buffer size; other bytes are used for flash editing

#define UARTBUF_NUM      144//72        // Data buffer size; 72 bytes

#define UART_TIMEOUT     2000      // Exit if no data is received within 2 s

#define PROGROMLEN       64        // Write length fixed at 32 bytes

enum{PRG_CHECKHEAD=0,PRG_FRQRANGE,PRG_WR,PRG_END,PRG_ERROR};

typedef  void (*pFunction)(void);

typedef struct
{
    U8  states;                 // Frequency-write state
    U8  errCnt;                 // Error count
    U8  enterMode;              // Flag indicating whether the read/write frequency mode has been entered
    U8  recFlag;                // Data received flag
    U16 dataNum;                // Length of data received
    U16 timeOut;                // Read/write frequency timeout
    U8  packageTime;            // Receive packet timing
    U8  rxBuf[1040];    // Receive data buffer
}STR_PROGROM;

extern STR_PROGROM progrom;

/***************************************************************************************/
extern const U8  strModelType[];
/***************************************************************************************/
extern Boolean CheckLinkHead(void);
extern void ProgromInit(void);
extern void CheckProgromMode(U8  rxData);
extern void UartSendBuf(U8  *buf,U16 len);
extern void EnterProgromMode(void);
extern void EnterFlashProgromMode(void);

#endif
