#ifndef __VOICEBROADCAST_H
    #define __VOICEBROADCAST_H

/**************************************************************************************************/
/***** Define voice playback index constants *******************************************************/
/**************************************************************************************************/
#include "VoiceIndex_Girl.h"

/**************************************************************************************************/
typedef struct
{
    U32 length;          // Total audio file length
    U32 dataAddr;        // Audio file start address
}STR_VOICE_INDEX;

// Define the structure used for DMA playback
typedef struct
{
    U32 logicAddr;       // Address actually read during playback
    U32 usedLen;         // Amount of data already played
    volatile U8  finishFlag;      // Playback completion flag
    volatile U8  dmaBufUsed;      // Current buffer region in use
    volatile U8  dmaBufAUseFlag;  // Whether buffer A has started being used
    volatile U8  dmaBufBUseFlag;  // Whether buffer B has started being used
    U16 dmaBufA[1024];
    U16 dmaBufB[1024];
    U8  lastPackage;
}STR_VOICE_PLAY;

typedef struct
{
    STR_VOICE_INDEX voiceIndex;
    STR_VOICE_PLAY voicePlay;
}STR_VOICE_ONFO;

// Structure used for voice announcements
typedef struct
{
    U8  voiceState;     // Voice playback flag indicating whether voice mode is active
    U8  busyFlag;    // Whether playback is in progress
    U8  voiceCnt;       // Number of voice prompts currently queued
    U8  voiceBuf[6];    // Current voice prompt command buffer
}STR_VOICE;

extern STR_VOICE voice;
extern STR_VOICE_ONFO g_voiceInform;
extern U8 FastChangeVoice;

extern void VoiceOutput_Interrupt(void);
extern void AudioHard_Init(void);

extern void Audio_PlayVoice(U8 Data);
extern void Audio_PlayVoiceLock(U8 Data);
extern void Audio_PlayNumInQueue(U8 Data);
extern void Audio_PlayChanNum(U8 Data);	

extern U8 Audio_CheckBusy(void);
extern void Audio_PlayStop(void);
extern void Audio_PlayTask(void);
extern void VoiceBroadcastWithBeepLock(U8 voiceDat,ENUM_BEEPMODE beepData);

#endif
