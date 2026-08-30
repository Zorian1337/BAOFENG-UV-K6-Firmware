#ifndef __APPSCANQT_H
    #define __APPSCANQT_H

typedef enum 
{
    SCANQT_OFF=0,SCANQT_STEPUP,SCANQT_WAIT,SCANQT_STOP
}ENUM_SCANQT;

enum 
{
    SCAN_SUBAUDIO_NONE, SCAN_SUBAUDIO_CTCS, SCAN_SUBAUDIO_DCS
};


typedef struct
{
    ENUM_SCANQT state;                           // Sub-audio scanning state
    U16 timeOut;                                 // Scan timeout
    U8  dcsCtsType;                              // Sub-audio type
    U32 dcsCtsDat;
    U8  isStandarDCS;
    U16 dcsIndex;
}STR_REMOTESCANQT;

extern void EnterRemoteScanQTMode(void);
extern void ExitRemoteScanQTMode(void);
extern void TaskRemoteScanQT(void);    
extern void KeyProcess_ScanQt(U8 keyEvent);

#endif
