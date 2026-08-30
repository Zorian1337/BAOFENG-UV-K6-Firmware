#ifndef __APPFM_H
    #define __APPFM_H

// Define FM timing parameters
#define FM_FREQSW_TIME    10      // FM frequency switching time in 10 ms units
#define FM_SEEK_TIMEOUT   1000    // FM auto-search timeout 10 s
#define FM_SEEK_TIME      FM_SEEK_TIMEOUT - 20 // FM auto-search timeout
#define FM_RETURN_TIME    200     // Time to return after exiting FM mode

#define FM_MAX_CH_NUM     30      // Define maximum stored channel count

// Define radio operating states
enum {FM_STOP=0,FM_SLEEP,FM_READY, FM_SEEK, FM_PLAY};              
typedef struct
{
    U8  mode;                 // FM operating state
    U8  band;                 // Operating band 0:76-108  1:65-76
    U16 freq;                 // Current FM frequency
    U16 timeOut;              // Timeout used for search and return after exit
    U8  fmChList[4];          // Valid FM channel list
    U8  fmChActive;           // Whether FM channel mode is valid
}STR_FMSTATE;

extern STR_FMSTATE    fmInfo;
/********************************************************************/
extern Boolean CheckFmChActive(U8 curChanNum);
extern void ResumeFmMode(void);
extern void FmBandConfig(void);
extern Boolean CheckFmVfoMode(void);
extern void FmCheckChannelActive(void);
extern void FmCheckTimeOut(void);
extern void FmEnterSleepMode(void);
extern void FMSwitchExit(void);
extern void EnterFmMode(void);
extern void ExitFmMode(void);
extern void FmTaskFunc(void);
extern void ResetFmSleepTime(void);
extern void KeyProcess_Fm(U8 keyEvent);

#endif
