#ifndef __APPSIGNALDETECTOR_H
    #define __APPSIGNALDETECTOR_H

// DEFINE FUNCTIONS TO EXPORT INSIDE OF HERE

enum SignalStrength{
    SIG_NONE = 0,
    SIG_MEDIUM = 1,
    SIG_HIGH = 2,
    SIG_STRONG = 3
};

// referenced AppStopWatch.h to build this
typedef struct{ 

} DetectedSignal;


void AppEnterSignalDetectionMode(void); // built using stopwatch as an example 
//extern void KeyProcess_Search(U8 keyEvent)

//extern void WeatherInit(U8 num); // reference AppScanQT
// DEFINE THIS ONLY IF ITS NOT ALREADY DEFINED
#endif