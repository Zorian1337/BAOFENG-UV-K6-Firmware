#include "includes.h"

STR_ALARM alarmDat;

/*********************************************************************************************************************/
extern void CheckAlarmDelay(void)
{
    if(alarmDat.freqSwTime)
    {
        alarmDat.freqSwTime--;
    }
    if(alarmDat.alarmTime)
    {
        alarmDat.alarmTime--;
    }
    if(alarmDat.ledFlashTime)
    {
        alarmDat.ledFlashTime--;
    }
}

void AlarmOut(ENUM_ONOFF flag)
{
    if(flag == ON)
    {
        alarmDat.alarmFreq = 100;  
        Rfic_SetToneFreq(alarmDat.alarmFreq);
        if( g_radioInform.alarmMode == ALARM_TONE && g_rfState == RF_TX)
        {
            Rfic_TxSingleTone_On(1);
        }
        else
        {
            Rfic_TxSingleTone_On(0);
        }
   
    }
    else
    {
        alarmDat.alarmFreq = 0; 
        // Turn off the alarm tone
        Rfic_TxSingleTone_Off();
    }    
}

Boolean SetAlarmCode(void)
{
    if(alarmDat.alarmStates == ON)
    {
        if(g_radioInform.alarmMode == ALARM_CODE)        
        {
            DtmfSendCodeOn(DTMF_ALARMCODE);
            g_rfTxState  = ALARM_TXID;
        }
        else 
        {
            AlarmOut(ON);
        }
        return TRUE;
    }
    return FALSE;
}

extern void AlarmTask(void)
{
    // Enable alarm mode
    if(alarmDat.alarmStates == ON)
    {      
        if(alarmDat.freqSwTime == 0)
        {
            alarmDat.freqSwTime = 12;

            if(alarmDat.toneFlag)
            {
                alarmDat.alarmFreq = alarmDat.alarmFreq + 12;

                if(alarmDat.alarmFreq > 250)
                {
                    alarmDat.toneFlag = 0;
                    // Display alarm information
                }
            }
            else
            {
                alarmDat.alarmFreq = alarmDat.alarmFreq - 12;
                if(alarmDat.alarmFreq < 70)
                {
                    alarmDat.toneFlag = 1;
                    // Display alarm information
                }
            }

            if(!(g_rfState == RF_TX && g_radioInform.alarmMode == ALARM_CODE))
            {// Do not send alarm tone
                if(alarmDat.ledFlashTime == 0)
                {
                    alarmDat.ledFlashTime = 100;
                    LedTxSwitch(LED_FLASH);
                    LightSwitch(LED_FLASH);
                }
                Rfic_SetToneFreq(alarmDat.alarmFreq);
                
                // Allow local alarm tone output
                if((g_radioInform.alarmMode == 0) || g_radioInform.alarmLocal)
                {
                    SpeakerSwitch(ON);
                }
            }
        }

        // Send alarm tone and alarm code mode
        if(g_radioInform.alarmMode)
        {
            if(alarmDat.alarmTime == 0)
            {
                if(g_rfState == RF_RX)
                {
                    Radio_EnterTxMode();
                    if(g_radioInform.alarmMode == ALARM_CODE)
                    {
                        AlarmOut(OFF);
                    }
                    ResetTimeKeyLockAndPowerSave();
                }
                else
                {
                    AlarmOut(OFF);
                    RF_TxRoger();
                    Rfic_SetPA(0);;
                    Rfic_RxTxOnOffSetup(RFIC_IDLE);
                    RF_PowerSet(g_ChannelVfoInfo.BandFlag,PWR_OFF);
                    LedRxSwitch(LED_ON);
                    AlarmOut(ON);
                }
                alarmDat.alarmTime = 3000;
            }
        }
    }
}


extern void AlarmFuncSwitch(ENUM_ONOFF flag)
{
    DualStandbyWorkOFF();
        
    if(flag == ON)
    {
    	/* Display AM indicator */
        if(g_ChannelVfoInfo.chVfoInfo[g_ChannelVfoInfo.switchAB].freqRx.frequency >= 10800000 && g_ChannelVfoInfo.chVfoInfo[g_ChannelVfoInfo.switchAB].freqRx.frequency < 13600000 )
        {
           if(g_radioInform.alarmMode != ALARM_LOCAL)
           {
               BeepOut(BEEP_ERROR);
               return;
           }
        }
    
        ExitAllFunction(1);
        
        // Disable transmit timeout
        g_sysRunPara.rfTxFlag.totTime = 500;
        
        alarmDat.freqSwTime   = 12;
        alarmDat.ledFlashTime = 100;
        alarmDat.alarmTime    = 3000;
        Rfic_TxSingleTone_Off();// Turn off alarm tone output to prevent local noise while waiting to send the alarm code
        
        if(g_radioInform.alarmMode)
        {
            if(g_radioInform.alarmLocal == OFF)
            {
                SpeakerSwitch(OFF);
            }

            Radio_EnterTxMode();
            Rfic_RxTxOnOffSetup(RFIC_IDLE);
        }
        else
        {
            AlarmOut(ON);
        }

        if(g_radioInform.alarmMode != ALARM_CODE)
        {
            LedRxSwitch(LED_ON);
        }
        LedTxSwitch(LED_OFF);

        LightSwitch(LED_ON);
        alarmDat.alarmStates = ON;
    }
    else
    {
        alarmDat.freqSwTime = 0;
        alarmDat.ledFlashTime = 0;
        alarmDat.alarmTime = 0;
    
        LedRxSwitch(LED_OFF);
        LedTxSwitch(LED_OFF);
        LightSwitch(LED_OFF);

        AlarmOut(OFF);

        if(g_radioInform.alarmMode)
        {
            RF_TxEnd();
            DisplayRadioHome();
        }

        alarmDat.alarmStates = OFF;        
        SpeakerSwitch(OFF); 
    }
}

