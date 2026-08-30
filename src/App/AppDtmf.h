#ifndef __APPDTMF_H
    #define __APPDTMF_H

enum{ CALLTYPE_NONE, CALLTYPE_NID, CALLTYPE_ID, CALLTYPE_GROUP, CALLTYPE_ALL };
    
#define DTMF_ANI_LEN       3      // Define local ID length
    
typedef struct 
{
    const U16 tone1Freq;
    const U16 tone2Freq;
}DTMFCODESTRUCT;

typedef struct
{
    U8  machineId[5];// Local device ID
    U8  dtmfAlarmWord;         // Alarm code
    U8  dtmfFlag;              // BIT1: send local ID when PTT is released
                                  // BIT0: send local ID when PTT is pressed
    U8  onTime;                // DTMF code duration  Range: 80-2000 ms (step 10 ms), value: 0 1 2 3 .... 195 default: 0
    U8  offTime;               // DTMF code gap  Range: 80-2000 ms (step 10 ms), value: 0 1 2 3 .... 195 default: 0
    U8  separator;            // Separator
    U8  groupCall;            // Group call symbol
}__attribute__( ( packed ) )STF_DTMFSTORE;

// Transmit mode
enum{DTMF_ONLINE=1,DTMF_OFFLINE=2,DTMF_ALARMCODE=4,DTMF_ALARMID,DTMF_TYPEIN,DTMF_ANI=8};

// DTMF operating state
enum {DTMF_OVER = 0,DTMF_SETUP,DTMF_FREQ,DTMF_STOP};
typedef struct
{
    U8  id[5];
    U8  name[11];
} STR_CONTACT;

typedef struct
{
    U8  code[16];              // DTMF transmitted code
    U8  cntRxDtmf;             // Number of DTMF codes decoded
    U8  state;                 // Operating state
    U8  enCodeNum;             // Current transmit sequence number
    U8  enCode;                // Current code to send
    U8  sendFlag;              // Transmit-in-progress flag
    U16 timeOut;               // DTMF transmit time or transmit wait time
    U16 detTime;               // DTMF detection time
    U8  timeRxOut;             // Receive decode timeout
    String aniCode[8];            // Caller number
    String callCode[8];           // Call information
    U8  callType;              // Call type   individual call / caller ID call / group call / all-call
    U8  matchTime[2];          // Matching duration
    U8  flagAck;               // Identity acknowledgment flag
    U8  timerDlyTxEnd;         // DTMF transmit-end delay

    U8  timerDtmfGroupRst;     // DTMF call reset time   No need to detect again within 30 s
    U8  flagDtmfMatch;         // DTMF call detection flag

    STR_CONTACT contact[20];

    U8  onlineCode[16];      // Online code
    U8  offlineCode[16];     // Offline code
    U8  killCode[16];         // Kill code
    U8  reliveCode[16];       // Wake code

}STR_DTMFINFO;

extern STF_DTMFSTORE g_dtmfStore;
extern STR_DTMFINFO dtmfInfo;
extern const DTMFCODESTRUCT DTMFCODE[21];
/************************************************************/
extern void EnterDtmfEditMode(void);
extern void ExitDtmfEditMode(void);
extern void GetDtmfEditCode(void);
extern void ResetDtmfEditCode(void);
extern void KeyProcess_DtmfInput(U8 keyEvent);

extern void DtmfSendKeypadCode(U8 code);
extern void DtmfSendTxOver(void);
extern void DtmfSendCodeOn(U8 type);
extern void DtmfTask(void);
extern void Task_HangUp(void);
extern void DtmfReceiveSetup(void);
extern void DtmfReceiveTask(void);
extern void DtmfClrMatchTimer(void);
extern void DtmfRstMatchTimer(U8 set);

extern void ClearDisANIFlag(void);
extern void DtmfSendANIAck(void);

extern void DtmfInfoInit(void);
extern U8 DtmfGetMatchStatue(void);


#endif
