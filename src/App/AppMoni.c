#include "includes.h"

extern void EnterMoniMode(void)
{
    ExitAllFunction(0);
    g_sysRunPara.moniFlag = 1;
    g_sysRunPara.sysRunMode = MODE_MONI;

    if(g_sysRunPara.rfRxFlag.rxReceiveOn == ON)
    {// If already receiving, directly monitor the current channel
         // Enable related receive peripherals
         LedRxSwitch(LED_ON);
         Rfic_SetAfout(ON);
         SpeakerSwitch(ON);
         g_rfRxState = RX_MONI;
    }
    else
    {
        // Exit sleep state
        Rfic_WakeUp();
        Rfic_TxSingleTone_Off();// Turn off the local single-tone path
        DualStandbyWorkOFF();
    }
    DisplaySingalFlag(4,1);  
}

void ExitMoniMode(void)
{
    g_sysRunPara.sysRunMode = MODE_MAIN;
    g_sysRunPara.moniFlag = 0;
    RF_RxEnd();
    SpeakerSwitch(OFF);
    RF_PowerSet(g_ChannelVfoInfo.BandFlag,PWR_OFF); 
    g_rfRxState = RX_READY;
    DisplayHomePage();
}

extern void KeyProcess_Moni(U8 keyEvent)
{
    switch(keyEvent)
    {
        case KEYID_SIDEKEYL2:
        case KEYID_MONIEXIT:
            ExitMoniMode();
            break;
        default:
            BeepOut(BEEP_NULL);
            break;
    }
}


