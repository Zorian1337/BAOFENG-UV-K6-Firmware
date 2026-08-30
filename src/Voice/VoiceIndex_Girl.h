#ifndef _VOICEINDEX_H
    #define _VOICEINDEX_H

/***********************************************************/
/***** Define voice prompt command IDs ************************/
/***********************************************************/
//                             Command code      Prompt text
#define  vo_Null               0x00
#define  vo_0                  0x10         // 0
#define  vo_1         	       0x11         // 1
#define  vo_2         	       0x12         // 2
#define  vo_3         	       0x13         // 3
#define  vo_4         	       0x14         // 4
#define  vo_5         	       0x15         // 5
#define  vo_6         	       0x16         // 6
#define  vo_7         	       0x17         // 7
#define  vo_8         	       0x18         // 8
#define  vo_9         	       0x19         // 9
#define  vo_10         	       0x1A         // 10
#define  vo_100         	   0x1B         // Hundred
                               
#define  vo_Welcome	           0x1C         // Welcome
#define  vo_keylock            0x1D         // Keyboard lock
#define  vo_unlock             0x1E         // Keyboard unlock
#define  vo_scanbegin          0x1F         // Start scan
#define  vo_scanstop           0x20         // Stop scan
#define  vo_CTCSS              0x21         // CTCSS
#define  vo_DCS                0x22         // DCS
#define  vo_power              0x23         // Transmit power
#define  vo_savemode           0x24         // Power-save mode
#define  vo_Memorychl          0x25         // Memory channel
#define  vo_Deletechl          0x26         // Delete channel
#define  vo_Step               0x27         // Step frequency
#define  vo_Squelch            0x28         // Squelch level
#define  vo_BackLight          0x2A         // Backlight selection
#define  vo_Txovertime         0x29         // Transmit timeout
#define  vo_VOX                0x2B         // VOX transmit
#define  vo_Freqdir            0x2C         // Frequency offset direction
#define  vo_Offsetfreq         0x2D         // Offset frequency
#define  vo_Txmemory           0x2E         // Transmit memory
#define  vo_Rxmemory           0x2F         // Receive memory
#define  vo_EmerCall           0x30         // Emergency call
#define  vo_Lowvoltage         0x31         // Low battery
#define  vo_Channelmode        0x32         // Channel mode
#define  vo_Freqmode           0x33         // Frequency mode
#define  vo_Voicepromrt        0x34         // Voice language selection
#define  vo_bandselect         0x35         // Band selection
#define  vo_Dualstandby        0x36         // Dual standby
#define  vo_Chlbandwidth       0x37         // Channel bandwidth
#define  vo_Optsignal          0x38         // Optional signaling
#define  vo_Mutemode           0x39         // Mute mode
#define  vo_Busylockout        0x3A         // Busy lockout
#define  vo_Beepprompt         0x3B         // Prompt tone
#define  vo_ANIcode            0x3C         // ANI code
#define  vo_initialization     0x3D         // Initialization
#define  vo_Confirm            0x3E         // Confirm
#define  vo_Cancel             0x3F         // Cancel
#define  vo_ON                 0x40         // Enable
#define  vo_OFF                0x41         // Disable
#define  vo_Menu               0x42         // Menu settings
#define  vo_FreqScan           0x43         // Frequency scan


#define  vo_zero               0x44         // 
#define  vo_ten                0x4E         // 
#define  vo_hundred            0x4F         // 

                               
#define  vo_eleven             0x78         // 
#define  vo_twelve             0x79         // 
#define  vo_thirteen           0x7A         // 
#define  vo_fourteen           0x7B         // 
#define  vo_fifteen            0x7C         // 
#define  vo_sixteen            0x7D         // 
#define  vo_seventeen          0x7E         // 
#define  vo_eighteen           0x7F         // 
#define  vo_nineteen           0x80         // 
#define  vo_twenty             0x82         // 
#define  vo_thirty             0x83         // 
#define  vo_forty              0x84         // 
#define  vo_fifty              0x85         // 
#define  vo_sixty              0x86         // 
#define  vo_seventy            0x87         // 
#define  vo_eighty             0x88         // 
#define  vo_ninety             0x89         // 
#define  vo_and                0x89         // 
                               
#define  vo_switch_en          0x34         // Switch Chinese voice prompt to English
#define  vo_index_start        0x10         // Voice prompt index address

#endif

