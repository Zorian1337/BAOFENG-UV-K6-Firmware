#include "includes.h"

STR_PROGROM progrom;
STR_FLASH_PROGROM    flashProgrom;

const U8 linkHead[]     = {"PROGRAMBF-K6PROU"};
const U8 updateFlag[]   = {"UPDATE"};
const U8 strModelType[] = {"UVK6"};

#define CONNECT_START          linkHead[3]
#define PRG_CMD_FREQ           'F'              // Send the supported frequency range command for the radio
#define PRG_CMD_ACK            0x06
#define PRG_CMD_MODEL          'M'
#define PRG_CMD_FLASH          'D'               // Update the startup screen
#define PRG_CMD_WRITE          'W'               // Write-frequency command
#define PRG_CMD_READ           'R'               // Read-frequency command
#define PRG_CMD_END            'E'               // End command

/*********************************************************************/
// Define command flags
#define progromCmd             progrom.rxBuf[0]
#define progromAdr0            progrom.rxBuf[1]
#define progromAdr1            progrom.rxBuf[2]
#define progromLen             progrom.rxBuf[3]
#define progromData0           progrom.rxBuf[4]

/*********************************************************************
* Function name: ProgromInit
* Function description: Initializes the parameters for write-frequency mode
* Global variables:
* Input parameters:
* Output parameters:
* Return value:
* Notes:
***********************************************************************/
extern void ProgromInit(void)
{
    memset(&progrom,0x00,sizeof(STR_PROGROM));
}
/*********************************************************************
* Function name: UartSendBuf
* Function description: Sends a serial data packet
* Global variables:
* Input parameters: *buf: data pointer   len: data length
* Output parameters:
* Return value:
* Notes:
***********************************************************************/
extern void UartSendBuf(U8 *buf,U16 len)
{
    U16 i;
    
    for(i=0;i<len;i++)
    {
        uartSendChar(buf[i]);
    }
    
}
/*********************************************************************
* Function name: CheckProgromMode
* Function description: Checks whether the radio is in write-frequency mode and whether data has been received
* Global variables:
* Input parameters: rxData: data received on the serial port
* Output parameters:
* Return value:
* Notes:
***********************************************************************/
extern void CheckProgromMode(U8 rxData)
{
    static U8 rxPreData;
    
    progrom.timeOut = UART_TIMEOUT;
    if(g_sysRunPara.sysRunMode == MODE_FLASH_PROGRAM)
    {
        progrom.rxBuf[progrom.dataNum] = rxData;
        progrom.dataNum = (progrom.dataNum + 1) % UART_MAX_NUM;
        progrom.packageTime = 5;
        progrom.errCnt  = 0;
    }
    else
    {
        if(!progrom.enterMode)
        {
        // Use a stricter method to determine entry into write-frequency mode
            if((rxData == linkHead[1] && rxPreData == linkHead[0]) || (rxData == linkHead[2] && rxPreData == linkHead[1]))
            {
                progrom.enterMode = 1;
                g_sysRunPara.sysRunMode = MODE_PROGRAM;
            }
            rxPreData = rxData;
            progrom.dataNum = 0;  
        }
        else
        {
            progrom.rxBuf[progrom.dataNum] = rxData;
            progrom.dataNum = (progrom.dataNum + 1) % UARTBUF_NUM;
            progrom.recFlag = 1;
            progrom.errCnt  = 0;
        }
    }
}
/*********************************************************************
* Function name: CheckProgromTimeout
* Function description: Delay function for write-frequency mode, used to measure time accurately
* Global variables:
* Input parameters:
* Output parameters:
* Return value:
* Notes:
***********************************************************************/
extern void CheckProgromTimeout(void)
{
    if(progrom.timeOut)
    {
        progrom.timeOut--;

        if(progrom.timeOut == 0)
        {// Timeout resets the device
            progrom.states = PRG_END;
        }
    }
}
/*********************************************************************
* Function name: ProgromSendAck
* Function description: Sends a response message
* Global variables:
* Input parameters:
* Output parameters:
* Return value:
* Notes:
***********************************************************************/
void ProgromSendAck(void)
{
    uartSendChar(PRG_CMD_ACK);
    progrom.dataNum = 0;
}

/*********************************************************************
* Function name: ProgromSendFreqRange
* Function description: Sends band information to the PC
* Global variables:
* Input parameters:
* Output parameters:
* Return value:
* Notes:
***********************************************************************/
void ProgromSendFreqRange(void)
{
    U8 bandData[21];
    U8 i,j;

    SpiFlash_ReadBytes(BAND_ADDR, bandData, 21);

    j = 0;
    for(i=1;i<21;i++)
    {
        if(i == 5 || i == 10  || i == 15 || i == 16)
        {
            i++;
        }
        progrom.rxBuf[j++] = bandData[i];
    }

    progrom.dataNum = 16;

    // Send data
    UartSendBuf(progrom.rxBuf,progrom.dataNum);
}
/*********************************************************************
* Function name: CheckLinkHead
* Function description: Verifies whether the handshake code is correct
* Global variables:
* Input parameters:
* Output parameters:
* Return value: Whether the verification succeeded
* Notes:
***********************************************************************/
Boolean CheckLinkHead(void)
{
    U8 i;

    for(i=0;i<progrom.dataNum;i++)
    {
        if(CONNECT_START == progrom.rxBuf[i])
        {
            break;
        }
    }

    if((progrom.dataNum - i) >= 8)
    {
        i++;
        if(memcmp(&linkHead[4],&progrom.rxBuf[i],7) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}
/*********************************************************************
* Function name: CheckLinkHead
* Function description: Verifies whether the handshake code is correct
* Global variables: progrom
* Input parameters: flag: 0 = read frequency, 1 = write frequency
* Output parameters:
* Return value:
* Notes:
***********************************************************************/
void ProgromWriteReadData(U8 flag)
{
    U16 address;
    static U8 buf[64];

    address = progromAdr0;
    address = (address<<8)+ progromAdr1;

    if(flag)
    {
        memcpy(buf,&progromData0,PROGROMLEN); 
        switch(address)
        {
            case 0x8000:
                memcpy(VfoBuf, buf, 64);
                Flash_SaveVfoData(0xFF);
                break;                
            case 0x9000:
                memcpy((U8 *)&g_radioInform.sqlLevel, buf, sizeof(STR_RADIOINFORM));
                break;
            case 0x9040:
                memcpy(powerOnMsg, buf, 16);
                Flash_SaveRadioImfosData();
                break;
            case FM_IMFOS_ADDR:
                memcpy((U8 *)&g_FMInform.FmCurFreq, buf,64);
                Flash_SaveFmData();
                break;
            case RF_MODEL_ADDR:
                memcpy((U8 *)&g_rfMoudel.moduleType, buf,5);
                Flash_SaveRfMoudelType();
                break;
            default:
                if((address % 0x1000) == 0)
                {
                    SpiFlash_EraseSector(address);
                }
                SpiFlash_WriteBytes(address, buf, PROGROMLEN);
                
                if(address == BAND2_ADDR)
                {
                    g_rfMoudel.moduleType = buf[21];
                    Flash_SaveRfMoudelType();
                }
                break;
        }
    }
    else
    {
        switch(address)
        {
            case 0x8000:
                Flash_ReadVfoData(0);
                memcpy(&progromData0, VfoBuf, 64);
                break;                

            case 0x9000:
                memcpy(&progromData0, &g_radioInform.sqlLevel, sizeof(STR_RADIOINFORM));
                break;
            case 0x9040:
                memset(&progromData0,0xFF,64);
                memcpy(&progromData0, powerOnMsg, 16);
                break;
            case FM_IMFOS_ADDR:
                memcpy(&progromData0,(U8 *)&g_FMInform.FmCurFreq, 64);
                break;
            case RF_MODEL_ADDR:
                memcpy(&progromData0,(U8 *)&g_rfMoudel.moduleType, 5);
                break;
            default:
                SpiFlash_ReadBytes(address, &progromData0, PROGROMLEN);
                if(address == BAND2_ADDR)
                {
                    buf[21] = g_rfMoudel.moduleType;
                }
                break;
        }
    }
}
/*********************************************************************
* 函 数 名: EnterProgromMode
* ^^^ ENGLISH TRANSLATION: Function name: EnterProgromMode ***
* 功能描述: 进入写频模式
* ^^^ ENGLISH TRANSLATION: Function description: Enter frequency writing mode ***
* 全局变量:
* ^^^ ENGLISH TRANSLATION: Global variables ***
* 输入参数：
* ^^^ ENGLISH TRANSLATION: Input parameters ***
* 输出参数:
* ^^^ ENGLISH TRANSLATION: Output parameters ***
* 返　　回:
* ^^^ ENGLISH TRANSLATION: Return value ***
* 说　　明：
* ^^^ ENGLISH TRANSLATION: Description ***
***********************************************************************/
extern void EnterProgromMode(void)
{

    RfOff();
// Show the write-frequency mode interface
    DisplayProgrom();

    progrom.states = PRG_CHECKHEAD;
    while(1)
    {
        if(g_msFlag)
        {// Delay function
            g_msFlag = 0;
            CheckProgromTimeout();
        }
        
        if(progrom.recFlag)
        {
            progrom.recFlag = 0;
            
            switch(progrom.states)
            {
                case PRG_FRQRANGE:
                    if(progromCmd == PRG_CMD_FREQ)
                    {
                        ProgromSendFreqRange();
                        progrom.dataNum = 0;
                        progrom.states = PRG_WR;   
                    }
                    else if(progromCmd == PRG_CMD_FLASH)
                    {
                        g_sysRunPara.sysRunMode = MODE_FLASH_PROGRAM;
                        progrom.enterMode = 0;
                        progrom.dataNum = 0;
                        progrom.timeOut = 0;

                        return;
                    }
                    else
                    {
                        progrom.dataNum = 0;
                    }
                    break;
                case PRG_WR:
                    if(progromCmd == PRG_CMD_READ)
                    {
                        if(progrom.dataNum >= 4)
                        {
                            ProgromWriteReadData(0);
                            progrom.dataNum = 4 + progromLen;
                            //发送数据
                            // ^^^ ENGLISH TRANSLATION: Send data ***
                            UartSendBuf(progrom.rxBuf,progrom.dataNum);
                            progrom.dataNum = 0;

                            LedTxSwitch(LED_FLASH);
                        }
                    }
                    else if(progromCmd == PRG_CMD_WRITE)
                    {
                        if(progrom.dataNum >= (PROGROMLEN + 4))
                        {
                            ProgromWriteReadData(1);
                            ProgromSendAck();
                            LedRxSwitch(LED_FLASH);
                        }
                    }
                    else
                    {
                        if(progromCmd == PRG_CMD_END)
                        {
                            progrom.states = PRG_END;
							ProgromSendAck();
                        }
                        progrom.dataNum = 0;
                    }
                    break;
                case PRG_END:
                case PRG_ERROR:
                    break;
                case PRG_CHECKHEAD:
                default:
                    if(CheckLinkHead() == TRUE)
                    {
						ProgromSendAck();
                        progrom.states = PRG_FRQRANGE;
                    }
                    break;
            }
        }
        else
        {
            if(progrom.states == PRG_ERROR)
            {
                progrom.states = PRG_CHECKHEAD;
                progrom.errCnt++;

                if(progrom.errCnt >= 3)
                {// Error count exceeds three
                    progrom.states = PRG_END;
                }
            }

            if(progrom.states == PRG_END)
            {
                LedTxSwitch(LED_OFF);
                LedRxSwitch(LED_OFF);
                progrom.dataNum = 0;
			    // Reset the system
                NVIC_SystemReset();
            }
        }
    }
}
void StatusAck(U8 cmd, U8 status)
{
    U16 crcTemp;
    
    flashProgrom.txLength = 0;
    flashProgrom.txBuf[flashProgrom.txLength++] = FLASH_PRO_HEAD;
    flashProgrom.txBuf[flashProgrom.txLength++] = cmd;
    // Data packet
    flashProgrom.txBuf[flashProgrom.txLength++] = 0x00;
    flashProgrom.txBuf[flashProgrom.txLength++] = 0x00;
    // Data length
    flashProgrom.txBuf[flashProgrom.txLength++] = 0x00;
    flashProgrom.txBuf[flashProgrom.txLength++] = 0x01;
    // Error code
    flashProgrom.txBuf[flashProgrom.txLength++] = status;

    crcTemp = CRC_ValidationCalc(&flashProgrom.txBuf[FLASH_PRO_CMD_ADDR], flashProgrom.txLength-1);

    flashProgrom.txBuf[flashProgrom.txLength++] = crcTemp>>8;
    flashProgrom.txBuf[flashProgrom.txLength++] = crcTemp;

    UartSendBuf(flashProgrom.txBuf, flashProgrom.txLength);
    flashProgrom.txLength = 0;

    progrom.timeOut = 0;
}

U8 CheckRxDataOk(void)
{
    U16 crcTemp,crcTemp2;
    
    if(progrom.rxBuf[0] != FLASH_PRO_HEAD)
    {
        StatusAck(PRO_CMD_ERROR, ERROR_CODE_HEAD);
        return FERROR;
    }

    flashProgrom.length = (progrom.rxBuf[FLASH_PRO_LEN_ADDR]<<8) + progrom.rxBuf[FLASH_PRO_LEN_ADDR+1];
    
    crcTemp = (progrom.rxBuf[FLASH_PRO_DATA_ADDR + flashProgrom.length]<<8) + progrom.rxBuf[FLASH_PRO_DATA_ADDR + flashProgrom.length + 1];

    crcTemp2 = CRC_ValidationCalc(&progrom.rxBuf[FLASH_PRO_CMD_ADDR], flashProgrom.length + 5);
    if(crcTemp != crcTemp2)
    {
        StatusAck(PRO_CMD_ERROR, ERROR_CODE_DATA);
        return FERROR;
    }

    flashProgrom.cmd = progrom.rxBuf[FLASH_PRO_CMD_ADDR];
    
    return OK;
}
void HandleProgromCmd(void)
{
    U8 temp;
    U16 packedNum;
    U32 addr;

    memcpy(flashProgrom.txBuf,progrom.rxBuf,4);
    flashProgrom.txLength = 4;
    switch(flashProgrom.cmd)
    {
        case PRO_CMD_ADDR:
            memcpy((U8 *)&flashProgrom.StartAddr,&progrom.rxBuf[FLASH_PRO_DATA_ADDR],4);
            flashProgrom.packageNum = 0;
            break;
        case PRO_CMD_EARSE:
            temp = progrom.rxBuf[FLASH_PRO_PACKED_ADDR + 1];

            // Get the data length
            if(flashProgrom.length)
            {
                memcpy((U8 *)&addr,&progrom.rxBuf[FLASH_PRO_DATA_ADDR],4); 
                packedNum = (progrom.rxBuf[FLASH_PRO_DATA_ADDR+4]<<8) + progrom.rxBuf[FLASH_PRO_DATA_ADDR+5];
            }

            if(temp == ERASE_MODE_4K)
            {
                for(temp = 0;temp<packedNum;temp++)
                {
                    SpiFlash_EraseSector(addr);
                    addr += SPIFLASH_PAGESIZE;
                }
            }
            else if(temp == ERASE_MODE_32K)
            {
                for(temp = 0;temp<packedNum;temp++)
                {
                    SpiFlash_Erase32kBlock(addr);
                    addr += 0x8000;
                }
            }
            else if(temp == ERASE_MODE_64K)
            {
                for(temp = 0;temp<packedNum;temp++)
                {
                    SpiFlash_Erase64kBlock(addr);
                    addr += 0x10000;
                }
            }
            else
            {
                SpiFlash_EraseChip();
            }
            break;
        case PRO_CMD_WRITE:
            packedNum = (progrom.rxBuf[FLASH_PRO_PACKED_ADDR]<<8) + progrom.rxBuf[FLASH_PRO_PACKED_ADDR+1]; 
            
            if(flashProgrom.packageNum != packedNum)
            {
                StatusAck(PRO_CMD_ERROR, ERROR_CODE_ADDR);
                return;
            }

            SpiFlash_WriteBytes(flashProgrom.StartAddr,&progrom.rxBuf[FLASH_PRO_DATA_ADDR],flashProgrom.length);

            flashProgrom.StartAddr += flashProgrom.length;
            flashProgrom.packageNum++;

            break;   
        case PRO_CMD_END:
            NVIC_SystemReset();
            break;
        default:
            break;
    }

    // Return the current command
    if(flashProgrom.txLength == 4)
    {
        flashProgrom.txBuf[flashProgrom.txLength++] = 0x00;
        flashProgrom.txBuf[flashProgrom.txLength++] = 0x01;
        flashProgrom.txBuf[flashProgrom.txLength++] = 0x59;
    }

    // Calculate the CRC checksum
    packedNum = CRC_ValidationCalc(&flashProgrom.txBuf[FLASH_PRO_CMD_ADDR], flashProgrom.txLength-1);

    flashProgrom.txBuf[flashProgrom.txLength++] = packedNum>>8;
    flashProgrom.txBuf[flashProgrom.txLength++] = packedNum;

    UartSendBuf(flashProgrom.txBuf, flashProgrom.txLength);
    flashProgrom.txLength = 0;

    // Clear the timeout after sending is complete; the end of one packet indicates the protocol is fully complete
    progrom.timeOut = 0;
}

extern void EnterFlashProgromMode(void)
{
    progrom.states = FLASH_PRG_CHECKHEAD;
    g_sysRunPara.sysRunMode = MODE_FLASH_PROGRAM;
    progrom.packageTime = 0;
    progrom.timeOut = 0;
    // Default the start address to 0
    flashProgrom.StartAddr = 0;

    while(1)
    {
        if(g_msFlag)
        {// Delay function
            g_msFlag = 0;
            if(progrom.timeOut)
            {
                progrom.timeOut--;
        
                if(progrom.timeOut == 0)
                {// Reset the device on timeout
                    progrom.states = FLASH_PRG_END;
                }
            }

            if(progrom.packageTime)
            {
                progrom.packageTime--;
                if(0 == progrom.packageTime)
                {
                    progrom.recFlag = 1;
                }
            }
            CheckPowerOff();
        }

        if(progrom.recFlag)
        {// One packet of data has been received
            progrom.recFlag = 0;

            if(CheckRxDataOk() == OK)
            {
                switch(progrom.states)
                {
                    case FLASH_PRG_CHECKHEAD:
                        if(flashProgrom.cmd == PRO_CMD_HANDSHAKE)
                        {
                            progrom.states = FLASH_PRG_CMD;
                            StatusAck(PRO_CMD_HANDSHAKE, ERROR_CODE_OK);
                        }
                        break;
                    case FLASH_PRG_CMD:
                        HandleProgromCmd();
                        break; 
                    default:
                        break;
                }
            }
            progrom.dataNum = 0;
        }
        else
        {
        
            if(progrom.states == FLASH_PRG_ERROR)
            {
                progrom.states = FLASH_PRG_CHECKHEAD;
                progrom.errCnt++;

                if(progrom.errCnt >= 3)
                {// Error count exceeds three
                    progrom.states = FLASH_PRG_END;
                }
            }

            if(progrom.states == FLASH_PRG_END)
            {
                LedTxSwitch(LED_OFF);
                LedRxSwitch(LED_OFF);
                progrom.dataNum = 0;

                // Reset the system
                NVIC_SystemReset();
            }
        }
    }   
}


