#include "includes.h"

#define STA_PWR_KEY                                 (GPIOA->IDR & GPIO_Pin_15)
#define POWER_OFF                                    GPIOA->BRR  = GPIO_Pin_8

extern void CheckPowerOff(void)
{
    if(STA_PWR_KEY)
    {
        DelayMs(50);
        if(STA_PWR_KEY)
        {
            RfOff();
            Rfic_Sleep();
            // Display the power-off screen
            if(g_rfState == RF_TX)
            {
                DelayMs(200);
            }

            // Turn off dual standby before switching back to the main channel
            DualStandbyWorkOFF();
        
            // Save configured data
            Flash_SaveRadioImfosData();

            if(g_ChannelVfoInfo.chVfoInfo[g_ChannelVfoInfo.switchAB].chVfoMode == CHAN_MODE)
            {
                // Save channel data / channel name
                Flash_SaveChannelData(g_ChannelVfoInfo.channelNum[g_ChannelVfoInfo.switchAB],(U8 *)&g_ChannelVfoInfo.channelInfo[g_ChannelVfoInfo.switchAB].rxFreq,g_ChannelVfoInfo.chVfoInfo[g_ChannelVfoInfo.switchAB].channelName);
            }
            else
            {
                // Save frequency mode data
                Flash_SaveVfoData(g_ChannelVfoInfo.switchAB);
            }
            
            // Save system runtime data
            Flash_SaveSystemRunData();
        
            // Delay 100 ms to wait for data write completion
            DelayMs(100);
            POWER_OFF;

            // Delay to ensure shutdown
            DelayMs(500);
            // Reset the system to avoid immediate power-on after shutdown
            NVIC_SystemReset();
        }
    }
}

extern void LcdBackLightSwitch(ENUM_LED flag)
{
    if(flag == LED_OFF)
    {
        //g_sysRunPara.lcdBackSwtch = OFF;
        GPIOF->BRR = GPIO_Pin_7;
    }
    else
    { 
        //g_sysRunPara.lcdBackSwtch = ON;
        GPIOF->BSRR = GPIO_Pin_7;
    }
}

extern void LedTxSwitch(ENUM_LED flag)
{
    static U8 state;
    
    if(flag == LED_OFF)
    {
        Rfic_GpioSetBit( RF_GPIO5, GPIOx_RFIC_L );
        state = 0;
    }
    else if(flag == LED_ON)
    {
        Rfic_GpioSetBit( RF_GPIO5, GPIOx_RFIC_H );
        state = 1;
    }
    else
    {
        if(state)
        {
            Rfic_GpioSetBit( RF_GPIO5, GPIOx_RFIC_L );
            state = 0;
        }
        else
        {
            Rfic_GpioSetBit( RF_GPIO5, GPIOx_RFIC_H );
            state = 1;
        }
    }
}

extern void LedRxSwitch(ENUM_LED flag)
{
    static U8 state;
    
    if(flag == LED_OFF)
    {
        GPIOA->BRR = GPIO_Pin_3;
        state = 0;
    }
    else if(flag == LED_ON)
    {
        GPIOA->BSRR = GPIO_Pin_3;
        state = 1;
    }
    else
    {
        if(state)
        {
            GPIOA->BRR = GPIO_Pin_3;
            state = 0;
        }
        else
        {
            GPIOA->BSRR = GPIO_Pin_3;
            state = 1;
        }
    }
}

extern void LightSwitch(U8 flag)
{
    static U8 state;

    if(flag == LED_OFF)
    {
        GPIOB->BRR  = GPIO_Pin_7;
        state = 0;
    }
    else if(flag == LED_ON)
    {
        GPIOB->BSRR  = GPIO_Pin_7;
        state = 1;
    }
    else
    {
        if(state)
        {
            GPIOB->BRR  = GPIO_Pin_7;
            state = 0;
        }
        else
        {
            GPIOB->BSRR  = GPIO_Pin_7;
            state = 1;
        }
    }
}

extern void LightFlashTask(void)
{
    if(g_sysRunPara.ledState != 2)
    {
        return;
    }

    if(alarmDat.alarmStates == ON)
    {// The LED also flashes in alarm mode
        return;
    }
    LightSwitch(LED_FLASH);
}


extern void LCD_BackLightSetOn(void)
{
    if(g_radioInform.autoBack < 5)
    {
        g_sysRunPara.lcdAutoLight = g_radioInform.autoBack * 50;
    }
    else if(g_radioInform.autoBack == 5)
    {
        g_sysRunPara.lcdAutoLight = 300;
    }
    else
    {
        g_sysRunPara.lcdAutoLight = (g_radioInform.autoBack - 5) * 600;
    }
    LcdBackLightSwitch(LED_ON); 
}

extern void LCD_CheckBackLight(void)
{
    if(g_radioInform.autoBack == 0)
    {// Always on
        return;
    }

    if(g_sysRunPara.sysRunMode == MODE_SEARCH || g_sysRunPara.sysRunMode == MODE_SCAN_QT)
    {
        return;
    }

    if(g_sysRunPara.lcdAutoLight == 0)
    {
        LcdBackLightSwitch(LED_OFF);
    }
}


void RF_PowerSet(U8 band,ENUM_RFPWR flag)
{
    if(flag == PWR_RXON)
    {
        switch(band)
        {
            case FREQ_BAND_350M:
            case FREQ_BAND_UHF:
                GPIOA->BRR = GPIO_Pin_14;
                GPIOA->BSRR = GPIO_Pin_13;
                break;

            case FREQ_BAND_200M:
            case FREQ_BAND_VHF:
            default:
                GPIOA->BRR = GPIO_Pin_13;
                GPIOA->BSRR = GPIO_Pin_14;
                break;
         }       	
    }
    else if(flag == PWR_TXON)
    {

        switch(band)
        {
            case FREQ_BAND_350M:
            case FREQ_BAND_UHF:
                Rfic_GpioSetBit( RF_GPIO3, GPIOx_RFIC_L );
                Rfic_GpioSetBit( RF_GPIO2, GPIOx_RFIC_H );
                break;

            case FREQ_BAND_200M:
            case FREQ_BAND_VHF:
                Rfic_GpioSetBit( RF_GPIO2, GPIOx_RFIC_L );
                Rfic_GpioSetBit( RF_GPIO3, GPIOx_RFIC_H );
                break;
            default:
                Rfic_GpioSetBit( RF_GPIO3, GPIOx_RFIC_L );
		        Rfic_GpioSetBit( RF_GPIO2, GPIOx_RFIC_L );
                break;
         } 
    }
    else
    {
        // Turn off all power
        Rfic_GpioSetBit( RF_GPIO2, GPIOx_RFIC_L );
		Rfic_GpioSetBit( RF_GPIO3, GPIOx_RFIC_L );
		GPIOA->BRR = GPIO_Pin_14;
		GPIOA->BRR = GPIO_Pin_13;   		
    }
}

extern void SpeakerSwitch(ENUM_ONOFF flag)
{
    if(flag == ON)
    {
        GPIOB->BSRR = GPIO_Pin_2;
    }
    else
    {
        GPIOB->BRR = GPIO_Pin_2;
    }
}

