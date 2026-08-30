#include "includes.h"

const U8  addrMap[8] = {0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00};

U8  powerOnMsg[17] = {0};
STR_RF_MODELE g_rfMoudel;
STR_BAND bandRang;
/*********************************************************************
* Function name: Flash_ModifyChannelData
* Function description: Saves channel information, including data and channel name
* Input parameters: channelNum: channel number  chData: data pointer  chName: channel name data pointer
* Output parameters:
* Return value:
* Notes:
***********************************************************************/
extern void Flash_ModifyChannelData(U16 channelNum,U8 * chData,U8 * chName)
{
    U8  buf[4*1024]; // 4K
    U32 indexSector; // Index
    U32 indexChannelNum;
    U32 addr = CHAN_ADDR;
    
    // Calculate the logical flash address for channel storage
    indexSector = channelNum / 128; // 4k / 32byte = 128
    addr = addr + (indexSector * 0x1000);

    // Convert the channel position
    indexChannelNum = channelNum % 128;

    // Read data / erase FLASH
    SpiFlash_ReadBytes(addr, buf, 4*1024);
    SpiFlash_EraseSector(addr);

    // Modify channel data without changing the channel name
    memcpy(buf + (indexChannelNum * CHAN_SIZE), chData, sizeof(STR_CHANNEL));
    // Save the channel name
    memcpy(buf + (indexChannelNum * CHAN_SIZE + NAME_ADDR_SHIFT), chName, NAME_SIZE);
    
    SpiFlash_WriteBytes(addr, buf, 4*1024);
}

/*********************************************************************
* Function description: Saves channel information, including channel data and channel name
* Input parameters: channelNum: channel number  chData: channel data pointer chName: channel name data pointer
* Output parameters:
* Return value:
* Notes: Data is stored only when it has changed
***********************************************************************/
extern void Flash_SaveChannelData(U16 channelNum,U8 * chData,U8 * chName)
{
    U32 addr;
    U8  channelBuf[32] = {0x00};

    addr = g_ChannelVfoInfo.currentChannelNum*CHAN_SIZE+CHAN_ADDR;
    SpiFlash_ReadBytes(addr,channelBuf, CHAN_SIZE);

    // Check whether the data has changed
    if(memcmp(channelBuf,chData,sizeof(STR_CHANNEL)) != 0 || memcmp(chName,&chData[NAME_ADDR_SHIFT],NAME_SIZE) != 0)
    {
        Flash_ModifyChannelData(channelNum,chData,chName);
    }
}

/*********************************************************************
* Function description: Delete the specified channel data
* Input parameters: channelNum: channel number
* Output parameters:
* Return value:
* Notes: Only the first 8 bytes of frequency data are cleared to 0
***********************************************************************/
extern void Flash_DeleteChannelData(U16 channelNum)
{
    U32 addr;
    U8  resetBuf[8];

    addr = channelNum*CHAN_SIZE+CHAN_ADDR;
    memset(resetBuf,0x00,8);
    SpiFlash_WriteBytes(addr,resetBuf, 8);
}

/*********************************************************************
* Function name: Flash_GetLogicAddrShift
* Function description: Gets the address offset position
* Input parameters: addr: storage block address   useByte: required byte count / 8
* Output parameters:
* Return value:
* Notes:
***********************************************************************/
U16 Flash_GetLogicAddrShift(U32 addr,U8  useByte)
{
    U8  i,j;
    U8  addrMask;
    U8  addrMap[32];
    U16 addrOffset = 0xFF;  // Address offset

    SpiFlash_ReadBytes(addr, addrMap,useByte);

    for(i = 0; i < useByte; i++)
    {
        addrMask = 0x01;
        for(j = 0; j < 8; j++)
        {
            if(addrMap[i] & addrMask)
            {
                addrOffset = i * 8 + j;

                return addrOffset;
            }
            else
            {
                addrMask <<= 1;
            }
        }
    }
    return addrOffset;
}

// Save walkie-talkie radio function information / radio channel information
extern void Flash_SaveRadioImfosData(void)
{
    U8  addrOffset;
    U8  dataMap;
    U32 logicAddr;
    U16 checkSum,CheckSumRd;
    U8  buf[96] = {0x00};// Radio function info 1 + radio function info 2 + radio info = 94 bytes + 2 bytes (checksum)
                            // (84 - 16 bytes) / 96 = 42  decay count: 42
    U8  cmpBuf[80];
    
    // Read storage location information
    addrOffset = Flash_GetLogicAddrShift(RADIO_IMFOS_ADDR,6);

    memcpy(buf, (U8  *)&g_radioInform.sqlLevel, sizeof(STR_RADIOINFORM));
    // Used to store startup display information
    memcpy(buf+64,powerOnMsg, 16);

    // Calculate CRC checksum
    checkSum = CRC_ValidationCalc(buf,94);
    memcpy(buf+94,(U8  *)&checkSum,2);

    if(addrOffset < 41)
    {
        logicAddr = addrOffset * 96 + RADIO_IMFOS_ADDR + 16;
        // Read the current checksum and compare it
        SpiFlash_ReadBytes(logicAddr+94, (U8  *)&CheckSumRd, 2);
        if(CheckSumRd == checkSum)
        {
            SpiFlash_ReadBytes(logicAddr, cmpBuf, 80);
            if(memcmp(cmpBuf,buf,80) == 0)
            {// Data is the same, return immediately
                return;
            }
        }
        // Save only if the data changed
        logicAddr += 96;
        SpiFlash_WriteBytes(logicAddr, buf, 96);
        
        // Mark the active address block
        dataMap = addrMap[(addrOffset%8)];
        SpiFlash_WriteBytes((RADIO_IMFOS_ADDR + (addrOffset/8)), &dataMap, 1);
    }
    else
    {// Erase
        SpiFlash_EraseSector(RADIO_IMFOS_ADDR);
        SpiFlash_WriteBytes((RADIO_IMFOS_ADDR + 16), buf, 96);
    }
}

// Read walkie-talkie frequency mode data / walkie-talkie function info / radio info
extern void Flash_ReadRadioImfosData(void)
{
    U8  addrOffset;
    U32 logicAddr;
    U16 checkSum;
    U8  buf[96]={0x00};// Radio function info 1 + radio function info 2 + radio info = 94 bytes + 2 bytes (checksum)
                           // (4K - 16 bytes) / 96 = 42  decay count: 42
    // Read storage location information
    addrOffset = Flash_GetLogicAddrShift(RADIO_IMFOS_ADDR,6);
    
    if(addrOffset < 42)
    {
        logicAddr = addrOffset* 96 + RADIO_IMFOS_ADDR + 16;
        SpiFlash_ReadBytes(logicAddr, buf, 96);
        
        memcpy((U8  *)&checkSum,&buf[94],2);
        if(checkSum == CRC_ValidationCalc(buf,94))
        {// Check passed
            memcpy((U8  *)&g_radioInform.sqlLevel, buf, sizeof(STR_RADIOINFORM));
            memcpy(powerOnMsg, buf+64,16);
            return;
        }
    }
    // Data error, checksum failed / recover from backup area
    SpiFlash_EraseSector(RADIO_IMFOS_ADDR);
    ResetRadioFunData();

}

// Save walkie-talkie frequency mode data; uses one 4K flash block, first 16 bytes mark the active block
// A-segment frequency mode + B-segment frequency mode = 64 + 2 bytes (checksum)
// (4K - 16 bytes) / 66 = 61  decay count: 61
// The first 16 bytes are used by the decay algorithm; actually 41 bits are used
U8  VfoBuf[66]={0x00};
extern void Flash_SaveVfoData(U8  workAB)
{
    U8  addrOffset;
    U8  dataMap;
    U32 logicAddr;
    U16 checkSum;

    if(workAB == 1)
    {
        if(memcmp(VfoBuf+32,g_ChannelVfoInfo.vfoInfo[1].freq,32) == 0)
        {// Data unchanged; do not save
            return;
        }
        memcpy(VfoBuf+32, g_ChannelVfoInfo.vfoInfo[1].freq, 32);
    }
    else if(workAB == 0)
    {
        if(memcmp(VfoBuf,g_ChannelVfoInfo.vfoInfo[0].freq,32) == 0)
        {// Data unchanged; do not save
            return;
        }
        memcpy(VfoBuf, g_ChannelVfoInfo.vfoInfo[0].freq,32);
    }
    else if(workAB == 0XFF)
    {// Write frequency used
        
    }
    else
    {// Reset only
        memcpy(VfoBuf, g_ChannelVfoInfo.vfoInfo[0].freq,32);
        memcpy(VfoBuf+32, g_ChannelVfoInfo.vfoInfo[1].freq, 32);
    }
    checkSum = CRC_ValidationCalc(VfoBuf,64);

    memcpy(VfoBuf+64,(U8  *)&checkSum,2);

    addrOffset = Flash_GetLogicAddrShift(VFO_INFO_ADDR,8);

    if(addrOffset < 60)
    {// When it reaches the last block address, erase is required
        logicAddr = (addrOffset + 1) * 66+ VFO_INFO_ADDR + 16;
        SpiFlash_WriteBytes(logicAddr, VfoBuf, 66);
        // Mark the active address block
        dataMap = addrMap[(addrOffset%8)];
        SpiFlash_WriteBytes((VFO_INFO_ADDR + (addrOffset/8)), &dataMap, 1);

    }
    else
    {// Write after a full erase
        SpiFlash_EraseSector(VFO_INFO_ADDR);
        SpiFlash_WriteBytes((VFO_INFO_ADDR + 16), VfoBuf, 66);
    }
}

// Read walkie-talkie frequency mode data
extern void Flash_ReadVfoData(U8  workAB)
{
    U8  addrOffset;
    U32 logicAddr;
    U16 checkSum;

    addrOffset = Flash_GetLogicAddrShift(VFO_INFO_ADDR,8);
    if(addrOffset < 61)
    {
        logicAddr = (addrOffset * 66) + VFO_INFO_ADDR + 16;
        SpiFlash_ReadBytes(logicAddr, VfoBuf, 66);

        memcpy((U8  *)&checkSum,&VfoBuf[64],2);
        if(checkSum == CRC_ValidationCalc(VfoBuf,64))
        {// CRC check passed
            if(workAB == 1)
            {
                memcpy(&g_ChannelVfoInfo.vfoInfo[1], VfoBuf+32, 32);
            }
            else
            {
                memcpy(&g_ChannelVfoInfo.vfoInfo[0], VfoBuf, 32);
            }
            return;
        }
    }
    // Data error, restore defaults from initialization
    SpiFlash_EraseSector(VFO_INFO_ADDR);
    ResetVfoModeData();

}

// Save walkie-talkie channel number, weather channel number, and current radio frequency
// Reserved 14 bytes + 2 bytes (checksum)
// (4K - 32 bytes) / 16 = 254  decay count: 254
// The first 16 bytes are used by the decay algorithm; actually 61 bits are used
extern void Flash_SaveSystemRunData(void)
{
    U16 addrOffset;
    U8  dataMap;
    U32 logicAddr;
    U16 checkSum;
    U8  buf[16]={0x00};

    addrOffset = Flash_GetLogicAddrShift(SYSTEMRAN_ADDR,32);

    // Extract data
    memcpy(buf, (U8  *)&g_ChannelVfoInfo.channelNum, 2);

    // Calculate CRC checksum
    checkSum = CRC_ValidationCalc(buf,14);
    memcpy(buf+14,(U8  *)&checkSum,2);

    if(addrOffset < 253)
    {
        logicAddr = (addrOffset + 1) * 16 + SYSTEMRAN_ADDR + 32;
        SpiFlash_WriteBytes(logicAddr, buf, 16);
        // Mark the active address block
        dataMap = addrMap[(addrOffset%8)];
        SpiFlash_WriteBytes((SYSTEMRAN_ADDR + (addrOffset/8)), &dataMap, 1);
    }
    else
    {// Write after a full erase
        SpiFlash_EraseSector(SYSTEMRAN_ADDR);
        SpiFlash_WriteBytes((SYSTEMRAN_ADDR + 32), buf, 16);
    }
}

// Read walkie-talkie channel number, weather channel number, and current radio frequency
// Reserved 14 bytes + 2 bytes (checksum)
// (4K - 32 bytes) / 16 = 254  decay count: 254
// The first 16 bytes are used by the decay algorithm; actually 61 bits are used
extern void Flash_ReadSystemRunData(void)
{
    U16 addrOffset;
    U32 logicAddr;
    U16 checkSum;
    U8  buf[16]={0x00};

    addrOffset = Flash_GetLogicAddrShift(SYSTEMRAN_ADDR,32);

    if(addrOffset < 254)
    {
        logicAddr = (addrOffset * 16) + SYSTEMRAN_ADDR + 32;
        SpiFlash_ReadBytes(logicAddr, buf, 16);

        memcpy((U8  *)&checkSum,&buf[14],2);
        if(checkSum == CRC_ValidationCalc(buf,14))
        {// Data checksum correct
            memcpy((U8  *)&g_ChannelVfoInfo.channelNum, buf, 2);
            
            return;
        }
    }
    // Data error, restore default values from initialization
    g_ChannelVfoInfo.channelNum[0] = 0;
    g_ChannelVfoInfo.channelNum[1] = 0;
}

// Save local DTMF ID
extern void Flash_SaveDtmfInfo(void)
{
     U8  buf[288]={0xFF};
     
     SpiFlash_ReadBytes(DTMFINFOR_ADDR, buf, 288); //32 + 16 * 16

     memcpy(buf,(U8  *)&g_dtmfStore, sizeof(g_dtmfStore));
     SpiFlash_EraseSector(DTMFINFOR_ADDR);

     SpiFlash_WriteBytes(DTMFINFOR_ADDR, buf, 288);
}

// Define fixed frequency information
const U8  fixBand1[][16] = 
{
  {0x01,0x01,0x36,0x01,0x74,0x01,0x04,0x00,0x06,0x00,0x01,0x02,0x00,0x02,0x60,0x00},
}; 

const U8  fixBand2[][8] = 
{
  {0x00,0x03,0x50,0x03,0x90,0x00,0x00,0x00},
}; 

// Read walkie-talkie debug parameters
extern void Flash_ReadDebugImfosData(void)
{
    U8  bandData[16];
    U8  bandData1[16];
    U8  i;

    SpiFlash_ReadBytes(RF_PWR_H_U_400_ADDR, &i, 1);
    if(i != 0xFF)
    {
        SpiFlash_ReadBytes(RF_PWR_H_U_400_ADDR, TXPWR_H_U_400, 16);
        SpiFlash_ReadBytes(RF_PWR_H_V_136_ADDR, TXPWR_H_V_136, 16);
        SpiFlash_ReadBytes(RF_PWR_H_V_200_ADDR, TXPWR_H_V_200, 16);
    
        SpiFlash_ReadBytes(RF_PWR_L_U_400_ADDR, TXPWR_L_U_400, 16);
        SpiFlash_ReadBytes(RF_PWR_L_V_136_ADDR, TXPWR_L_V_136, 16);
        SpiFlash_ReadBytes(RF_PWR_L_V_200_ADDR, TXPWR_L_V_200, 16);
        
        SpiFlash_ReadBytes(RF_SQL_TAB_ADDR, TH_SQL_TAB, 10 );
        SpiFlash_ReadBytes(RF_SQL_TAB_MUTE_ADDR, TH_SQL_TAB_MUTE, 10);
    
        SpiFlash_ReadBytes(RF_SQL_U_400_ADDR, OFFSET_SQL_U_400, 16);
        SpiFlash_ReadBytes(RF_SQL_V_136_ADDR, OFFSET_SQL_V_136, 16);
        SpiFlash_ReadBytes(RF_SQL_V_200_ADDR, OFFSET_SQL_V_200, 16);
        SpiFlash_ReadBytes(RF_SQL_U_350_ADDR, OFFSET_SQL_U_350, 16);

        SpiFlash_ReadBytes(RF_MODULATION_ADDR, OFFSET_MODULATION, 16);

        //SpiFlash_ReadBytes(DEV_BATT_ADDR, battery.voltList, BAT_LEVEL_NUM);
    }
    
    // Read model code
    Flash_ReadRfMoudelType();
    g_sysRunPara.moduleType = g_rfMoudel.moduleType;
    
    if(g_sysRunPara.moduleType >= 0x30 && g_sysRunPara.moduleType <= 0x35)
    {
        g_sysRunPara.moduleType &= 0x0F;
    }
    else
    {
        g_sysRunPara.moduleType = 0;
    }
    memcpy(bandData,fixBand1[0],16);
    memcpy(bandData1,fixBand2[0],8);
    
    g_sysRunPara.rfTxFlag.txEnable[0] = bandData[0];
    g_sysRunPara.rfTxFlag.txEnable[1] = bandData[5];
    g_sysRunPara.rfTxFlag.txEnable[2] = bandData[10];

    // Calculate frequency range
    for(i = 0; i < 16; i++)
    {
       bandData[i] = changeHexToInt(bandData[i]);
    }
    // U-band frequency range
    bandRang.bandFreq.vhfL   = bandData[1]*1000+bandData[2]*10;
    bandRang.bandFreq.vhfH   = bandData[3]*1000+bandData[4]*10;
    bandRang.bandFreq.freqVL = bandRang.bandFreq.vhfL*10000;
    bandRang.bandFreq.freqVH = bandRang.bandFreq.vhfH*10000;

    // V-band frequency range
    bandRang.bandFreq.uhfL   = bandData[6]*1000+bandData[7]*10;
    bandRang.bandFreq.uhfH   = bandData[8]*1000+bandData[9]*10;
    bandRang.bandFreq.freqUL = bandRang.bandFreq.uhfL*10000;
    bandRang.bandFreq.freqUH = bandRang.bandFreq.uhfH*10000;
    
    // 200M frequency range
    bandRang.bandFreq.vhf2L   = bandData[11]*1000+bandData[12]*10;
    bandRang.bandFreq.vhf2H   = bandData[13]*1000+bandData[14]*10;
    bandRang.bandFreq.freqV2L = bandRang.bandFreq.vhf2L*10000;
    bandRang.bandFreq.freqV2H = bandRang.bandFreq.vhf2H*10000;

    // 350M frequency range
    for(i = 0; i < 8; i++)
    {
       bandData1[i] = changeHexToInt(bandData1[i]);
    }
    bandRang.bandFreq.B350ML = bandData1[1]*1000+bandData1[2]*10;
    bandRang.bandFreq.B350MH = bandData1[3]*1000+bandData1[4]*10;
    bandRang.bandFreq.freq350ML = bandRang.bandFreq.B350ML*10000;
    bandRang.bandFreq.freq350MH = bandRang.bandFreq.B350MH*10000;
}

// Save transmission enable and band selection settings
// Reserved 14 bytes + 2 bytes (checksum)
// (4K - 32 bytes) / 16 = 254  decay count: 254
// The first 16 bytes are used by the decay algorithm; actually 61 bits are used
extern void Flash_SaveRfMoudelType(void)
{
    U16 addrOffset;
    U8  dataMap;
    U32 logicAddr;
    U16 checkSum;
    U8  buf[16]={0x00};

    addrOffset = Flash_GetLogicAddrShift(RF_MODEL_ADDR,32);

    // Extract data
    memcpy(buf, (U8  *)&g_rfMoudel.moduleType, 5);   

    // Calculate CRC checksum
    checkSum = CRC_ValidationCalc(buf,14);
    memcpy(buf+14,(U8  *)&checkSum,2);

    if(addrOffset < 253)
    {
        logicAddr = (addrOffset + 1) * 16 + RF_MODEL_ADDR + 32;
        SpiFlash_WriteBytes(logicAddr, buf, 16);
        // Mark the active address block
        dataMap = addrMap[(addrOffset%8)];
        SpiFlash_WriteBytes((RF_MODEL_ADDR + (addrOffset/8)), &dataMap, 1);
    }
    else
    {// Write after a full erase
        SpiFlash_EraseSector(RF_MODEL_ADDR);
        SpiFlash_WriteBytes((RF_MODEL_ADDR + 32), buf, 16);
    }
}

// Read walkie-talkie transmission enable and band selection settings
// Reserved 14 bytes + 2 bytes (checksum)
// (4K - 32 bytes) / 16 = 254  decay count: 254
// The first 16 bytes are used by the decay algorithm; actually 61 bits are used
extern void Flash_ReadRfMoudelType(void)
{
    U16 addrOffset;
    U32 logicAddr;
    U16 checkSum;
    U8  buf[16]={0x00};

    addrOffset = Flash_GetLogicAddrShift(RF_MODEL_ADDR,32);
    if(addrOffset < 254)
    {
        logicAddr = (addrOffset * 16) + RF_MODEL_ADDR + 32;
        SpiFlash_ReadBytes(logicAddr, buf, 16);

        memcpy((U8  *)&checkSum,&buf[14],2);
        if(checkSum == CRC_ValidationCalc(buf,14))
        {// CRC check passed
            memcpy((U8  *)&g_rfMoudel.moduleType, buf, 5);
            return;
        }
    }
    // Default factory mode; transmission is not allowed on either band
    memset((U8  *)&g_rfMoudel.moduleType,0x00,5);

    // Extract data
    memset(buf,0x00,16);

    // Calculate CRC checksum
    checkSum = CRC_ValidationCalc(buf,14);
    memcpy(buf+14,(U8  *)&checkSum,2);
    SpiFlash_EraseSector(RF_MODEL_ADDR);
    SpiFlash_WriteBytes((RF_MODEL_ADDR + 32), buf, 16);
}

// Save walkie-talkie radio mode channel data
// Radio data 64 bytes + 2 bytes (checksum)
// (4K - 16 bytes) / 66 = 61  decay count: 61
// The first 16 bytes are used by the decay algorithm; actually 120 bits are used
extern void Flash_SaveFmData(void)
{
    U16 addrOffset;
    U8  dataMap;
    U32 logicAddr;
    U16 checkSum,CheckSumRd;
    U8  buf[66] = {0x00};
    U8  cmpBuf[66];
    
    // Read storage location information
    addrOffset = Flash_GetLogicAddrShift(FM_IMFOS_ADDR,8);

    // Extract data
    memcpy(buf,   (U8  *)&g_FMInform.FmCurFreq, sizeof(STR_FMINFOS));
    // Calculate CRC checksum
    checkSum = CRC_ValidationCalc(buf,64);
    memcpy(&buf[64], (U8  *)&checkSum,2);

    if(addrOffset < 60)
    {
        logicAddr = addrOffset * 66 + FM_IMFOS_ADDR + 16;
        // Read current checksum and compare it
        SpiFlash_ReadBytes(logicAddr+64, (U8  *)&CheckSumRd, 2);
        if(CheckSumRd == checkSum)
        {
            SpiFlash_ReadBytes(logicAddr, cmpBuf, 64);
            if(memcmp(cmpBuf,buf,64) == 0)
            {// Data is the same, return immediately
                return;
            }
        }
        // Save only if the data changed
        SpiFlash_WriteBytes(logicAddr + 66, buf, 66);
        
        // Mark the active address block
        dataMap = addrMap[(addrOffset%8)];
        SpiFlash_WriteBytes((FM_IMFOS_ADDR + (addrOffset/8)), &dataMap, 1);
    }
    else
    {// Erase
        SpiFlash_EraseSector(FM_IMFOS_ADDR);
        SpiFlash_WriteBytes((FM_IMFOS_ADDR + 16), buf, 66);
    }
}

// Read current radio frequency and channel data
// Radio data 64 bytes + 2 bytes (checksum)
// (4K - 16 bytes) / 66 = 61  decay count: 61
// The first 16 bytes are used by the decay algorithm; actually 120 bits are used
extern void Flash_ReadFmData(void)
{
    U16 addrOffset;
    U32 logicAddr;
    U16 checkSum;
    U8  buf[66]={0x00};

    addrOffset = Flash_GetLogicAddrShift(FM_IMFOS_ADDR,8);

    if(addrOffset < 61)
    {
        logicAddr = (addrOffset * 66) + FM_IMFOS_ADDR + 16;
        SpiFlash_ReadBytes(logicAddr, buf, 66);

        memcpy((U8  *)&checkSum,&buf[64],2);
        if(checkSum == CRC_ValidationCalc(buf,64))
        {// CRC check passed
            memcpy((U8  *)&g_FMInform.FmCurFreq, buf, 64); 
            return;
        }
    }
    
    // Data error; restore from initialization
    g_FMInform.FmCurFreq = 889;
    g_FMInform.fmChNum = 0;
    g_FMInform.fmChVfo = VFO_MODE;

    memcpy(buf,   (U8  *)&g_FMInform.FmCurFreq, sizeof(STR_FMINFOS));
    SpiFlash_EraseSector(FM_IMFOS_ADDR);
    SpiFlash_WriteBytes((FM_IMFOS_ADDR + 16), buf, 66);
}

