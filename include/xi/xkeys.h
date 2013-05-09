/*
 * Copyright 2013 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _XKEYS_H_
#define _XKEYS_H_

/**
 * @brief XKey Definitions
 *
 * @file xkeys.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

/**
 * @defgroup xkeys XKey Definitions
 * @ingroup XI
 * @{
 */

/* Unknown Key */
#define IK_UNDEF                         0x0000   ///< Undefined Key Code

/* Mouse Keys */
#define IK_MBTN_1                        0x0001   ///< Mouse : Button Left
#define IK_MBTN_2                        0x0002   ///< Mouse : Button Middle
#define IK_MBTN_3                        0x0003   ///< Mouse : Button Right
#define IK_MBTN_4                        0x0004   ///< Mouse : Typically Wheel Up
#define IK_MBTN_5                        0x0005   ///< Mouse : Typically Wheel Down
#define IK_MBTN_6                        0x0006   ///< Mouse : extended button #6
#define IK_MBTN_7                        0x0007   ///< Mouse : extended button #7
#define IK_MBTN_8                        0x0008   ///< Mouse : extended button #8


/* STB Remote-controller Keys */
#define IK_RC_Color_0                    0x0193   ///< Remote-Controller : Color#1
#define IK_RC_Color_1                    0x0194   ///< Remote-Controller : Color#2
#define IK_RC_Color_2                    0x0195   ///< Remote-Controller : Color#3
#define IK_RC_Color_3                    0x0196   ///< Remote-Controller : Color#4
#define IK_RC_Color_4                    0x0197   ///< Remote-Controller : Color#5
#define IK_RC_Color_5                    0x0198   ///< Remote-Controller : Color#6
#define IK_RC_Power                      0x0199   ///< Remote-Controller : Power
#define IK_RC_Dimmer                     0x019a   ///< Remote-Controller : Dimmer
#define IK_RC_Wink                       0x019b   ///< Remote-Controller : Wink
#define IK_RC_Rewind                     0x019c   ///< Remote-Controller : Rewind
#define IK_RC_Stop                       0x019d   ///< Remote-Controller : Stop
#define IK_RC_Eject                      0x019e   ///< Remote-Controller : Eject
#define IK_RC_Play                       0x019f   ///< Remote-Controller : Play
#define IK_RC_Record                     0x01a0   ///< Remote-Controller : Record
#define IK_RC_FastForward                0x01a1   ///< Remote-Controller : FastForward
#define IK_RC_Play_SpeedUp               0x01a2   ///< Remote-Controller : Play Speed - Up
#define IK_RC_Play_SpeedDown             0x01a3   ///< Remote-Controller : Play Speed - Down
#define IK_RC_Play_SpeedReset            0x01a4   ///< Remote-Controller : Play Speed - Reset
#define IK_RC_Record_SpeedNext           0x01a5   ///< Remote-Controller : Record_Speed - Next
#define IK_RC_Goto_Start                 0x01a6   ///< Remote-Controller : Goto Start
#define IK_RC_Goto_End                   0x01a7   ///< Remote-Controller : Goto End
#define IK_RC_PrevTrack                  0x01a8   ///< Remote-Controller : Previous Track
#define IK_RC_NextTrack                  0x01a9   ///< Remote-Controller : Next Track
#define IK_RC_RandomTrack                0x01aa   ///< Remote-Controller : Random Track
#define IK_RC_ChannelUp                  0x01ab   ///< Remote-Controller : Channel Up
#define IK_RC_ChannelDown                0x01ac   ///< Remote-Controller : Channel Down
#define IK_RC_Store_Favorite0            0x01ad   ///< Remote-Controller : Store Favorite #0
#define IK_RC_Store_Favorite1            0x01ae   ///< Remote-Controller : Store Favorite #1
#define IK_RC_Store_Favorite2            0x01af   ///< Remote-Controller : Store Favorite #2
#define IK_RC_Store_Favorite3            0x01b0   ///< Remote-Controller : Store Favorite #3
#define IK_RC_Recall_Favorite0           0x01b1   ///< Remote-Controller : Recall Favorite #0
#define IK_RC_Recall_Favorite1           0x01b2   ///< Remote-Controller : Recall Favorite #1
#define IK_RC_Recall_Favorite2           0x01b3   ///< Remote-Controller : Recall Favorite #2
#define IK_RC_Recall_Favorite3           0x01b4   ///< Remote-Controller : Recall Favorite #3
#define IK_RC_Clear_Favorite0            0x01b5   ///< Remote-Controller : Clear Favorite #0
#define IK_RC_Clear_Favorite1            0x01b6   ///< Remote-Controller : Clear Favorite #1
#define IK_RC_Clear_Favorite2            0x01b7   ///< Remote-Controller : Clear Favorite #2
#define IK_RC_Clear_Favorite3            0x01b8   ///< Remote-Controller : Clear Favorite #3
#define IK_RC_Scan_Channel               0x01b9   ///< Remote-Controller : Scan Channel
#define IK_RC_Pinpoint                   0x01ba   ///< Remote-Controller : Pinpoint
#define IK_RC_Split_Screen               0x01bb   ///< Remote-Controller : Split Screen
#define IK_RC_Display_Swap               0x01bc   ///< Remote-Controller : Display Swap (PIP)
#define IK_RC_ScreenMode_Next            0x01bd   ///< Remote-Controller : Screen Mode - Next
#define IK_RC_VideoMode_Next             0x01be   ///< Remote-Controller : Video Mode - Next
#define IK_RC_VolumeUp                   0x01bf   ///< Remote-Controller : Volume Up
#define IK_RC_VolumeDown                 0x01c0   ///< Remote-Controller : Volume Down
#define IK_RC_Mute                       0x01c1   ///< Remote-Controller : Mute
#define IK_RC_SurroundMode_Next          0x01c2   ///< Remote-Controller : Surround Mode - Next
#define IK_RC_Balance_Right              0x01c3   ///< Remote-Controller : Balance - Right
#define IK_RC_Balance_Left               0x01c4   ///< Remote-Controller : Balance - Left
#define IK_RC_Fader_Front                0x01c5   ///< Remote-Controller : Fader - Front
#define IK_RC_Fader_Rear                 0x01c6   ///< Remote-Controller : Fader - Rear
#define IK_RC_Bass_BoostUp               0x01c7   ///< Remote-Controller : Bass - Boost Up
#define IK_RC_Bass_BoostDown             0x01c8   ///< Remote-Controller : Bass - Boost Down
#define IK_RC_Info                       0x01c9   ///< Remote-Controller : Information
#define IK_RC_Guide                      0x01ca   ///< Remote-Controller : Guide
#define IK_RC_Teletext                   0x01cb   ///< Remote-Controller : Teletext
#define IK_RC_Subtitle                   0x01cc   ///< Remote-Controller : Subtitle

#define IK_RC_RF_Bypass                  0x0258   ///< Remote-Controller : RF Bypass
#define IK_RC_Exit                       0x0259   ///< Remote-Controller : Exit
#define IK_RC_Menu                       0x025a   ///< Remote-Controller : Menu
#define IK_RC_NextDay                    0x025b   ///< Remote-Controller : Next Day
#define IK_RC_PrevDay                    0x025c   ///< Remote-Controller : Previous Day
#define IK_RC_Apps                       0x025d   ///< Remote-Controller : Applications
#define IK_RC_Link                       0x025e   ///< Remote-Controller : Link
#define IK_RC_Last                       0x025f   ///< Remote-Controller : Last
#define IK_RC_Back                       0x0260   ///< Remote-Controller : Back
#define IK_RC_Forward                    0x0261   ///< Remote-Controller : Forward
#define IK_RC_Zoom                       0x0262   ///< Remote-Controller : Zoom
#define IK_RC_Settings                   0x0263   ///< Remote-Controller : Settings
#define IK_RC_NextFavorite_Channel       0x0264   ///< Remote-Controller : Next Favorite Channel
#define IK_RC_Reserve_1                  0x0265   ///< Remote-Controller : Reserve #1
#define IK_RC_Reserve_2                  0x0266   ///< Remote-Controller : Reserve #2
#define IK_RC_Reserve_3                  0x0267   ///< Remote-Controller : Reserve #3
#define IK_RC_Reserve_4                  0x0268   ///< Remote-Controller : Reserve #4
#define IK_RC_Reserve_5                  0x0269   ///< Remote-Controller : Reserve #5
#define IK_RC_Reserve_6                  0x026a   ///< Remote-Controller : Reserve #6
#define IK_RC_Lock                       0x026b   ///< Remote-Controller : Lock
#define IK_RC_Skip                       0x026c   ///< Remote-Controller : Skip
#define IK_RC_List                       0x026d   ///< Remote-Controller : List
#define IK_RC_Live                       0x026e   ///< Remote-Controller : Live
#define IK_RC_OnDemand                   0x026f   ///< Remote-Controller : On Demand
#define IK_RC_Pinpoint_Move              0x0270   ///< Remote-Controller : Pinpoint - Move
#define IK_RC_Pinpoint_Up                0x0271   ///< Remote-Controller : Pinpoint - Up
#define IK_RC_Pinpoint_Down              0x0272   ///< Remote-Controller : Pinpoint - Down

#define IK_RC_Reserve_7                  0x028a   ///< Remote-Controller : Reserve #7
#define IK_RC_Reserve_8                  0x028b   ///< Remote-Controller : Reserve #8
#define IK_RC_Reserve_9                  0x028c   ///< Remote-Controller : Reserve #9
#define IK_RC_Reserve_10                 0x028d   ///< Remote-Controller : Reserve #10
#define IK_RC_Reserve_11                 0x028e   ///< Remote-Controller : Reserve #11

#define IK_RC_Genre_1                    0x02bc   ///< Remote-Controller : Genre #1
#define IK_RC_Genre_2                    0x02bd   ///< Remote-Controller : Genre #2
#define IK_RC_Genre_3                    0x02be   ///< Remote-Controller : Genre #3
#define IK_RC_Genre_4                    0x02bf   ///< Remote-Controller : Genre #4
#define IK_RC_Genre_5                    0x02c0   ///< Remote-Controller : Genre #5
#define IK_RC_Genre_6                    0x02c1   ///< Remote-Controller : Genre #6
#define IK_RC_Genre_7                    0x02c2   ///< Remote-Controller : Genre #7
#define IK_RC_Genre_8                    0x02c3   ///< Remote-Controller : Genre #8
#define IK_RC_Genre_9                    0x02c4   ///< Remote-Controller : Genre #9

#define IK_RC_Record_List                0x02c6   ///< Remote-Controller : Record - List

/*
 * TTY function keys, cleverly chosen to map to ASCII, for convenience of
 * programming, but could have been arbitrary (at the cost of lookup
 * tables in client code).
 */
#define IK_BackSpace                     0xff08  ///< Keyboard : Back space, back char
#define IK_Tab                           0xff09  ///< Keyboard : Tab space
#define IK_Linefeed                      0xff0a  ///< Keyboard : Line-feed, LF
#define IK_Clear                         0xff0b  ///< Keyboard : Clear
#define IK_Enter                         0xff0d  ///< Keyboard : Return, enter, carriage return
#define IK_Pause                         0xff13  ///< Keyboard : Pause, hold
#define IK_ScrollLock                    0xff14  ///< Keyboard : Scroll Lock
#define IK_SysReq                        0xff15  ///< Keyboard : System Request
#define IK_Escape                        0xff1b  ///< Keyboard : Escape
#define IK_Delete                        0xffff  ///< Keyboard : Delete, rubout

/* International & multi-key character composition */
#define IK_MultiKey                      0xff20  ///< Keyboard : Multi-key character compose
#define IK_Codeinput                     0xff37  ///< Keyboard : Code input
#define IK_SingleCandidate               0xff3c  ///< Keyboard : Single Candidate
#define IK_MultipleCandidate             0xff3d  ///< Keyboard : Multiple Candidate
#define IK_PreviousCandidate             0xff3e  ///< Keyboard : Previous Candidate

/* Korean keyboard support */
#define IK_Hangul                        0xff31 ///< Keyboard : Hangul Toggle
#define IK_Hanja                         0xff34 ///< Keyboard : Hangul->Hanja Convert

/* Japanese keyboard support */
#define IK_Kanji                         0xff21  ///< Keyboard : Kanji, Kanji convert
#define IK_Muhenkan                      0xff22  ///< Keyboard : Cancel Conversion
#define IK_Henkan_Mode                   0xff23  ///< Keyboard : Start/Stop Conversion
#define IK_Henkan                        0xff23  ///< Keyboard : Alias for Henkan_Mode
#define IK_Romaji                        0xff24  ///< Keyboard : to Romaji
#define IK_Hiragana                      0xff25  ///< Keyboard : to Hiragana
#define IK_Katakana                      0xff26  ///< Keyboard : to Katakana
#define IK_Hiragana_Katakana             0xff27  ///< Keyboard : Hiragana/Katakana toggle
#define IK_Zenkaku                       0xff28  ///< Keyboard : to Zenkaku
#define IK_Hankaku                       0xff29  ///< Keyboard : to Hankaku
#define IK_Zenkaku_Hankaku               0xff2a  ///< Keyboard : Zenkaku/Hankaku toggle
#define IK_Touroku                       0xff2b  ///< Keyboard : Add to Dictionary
#define IK_Massyo                        0xff2c  ///< Keyboard : Delete from Dictionary
#define IK_Kana_Lock                     0xff2d  ///< Keyboard : Kana Lock
#define IK_Kana_Shift                    0xff2e  ///< Keyboard : Kana Shift
#define IK_Eisu_Shift                    0xff2f  ///< Keyboard : Alphanumeric Shift
#define IK_Eisu_toggle                   0xff30  ///< Keyboard : Alphanumeric toggle
#define IK_Kanji_Bangou                  0xff37  ///< Keyboard : Kanji_Bangou, Codeinput
#define IK_Zen_Koho                      0xff3d  ///< Keyboard : Zen Koho, Multiple/All Candidate(s)
#define IK_Mae_Koho                      0xff3e  ///< Keyboard : Mae Koho, Previous Candidate

/* Cursor control & motion */
#define IK_Home                          0xff50  ///< Keyboard : Home
#define IK_Left                          0xff51  ///< Keyboard : Move left, left arrow
#define IK_Up                            0xff52  ///< Keyboard : Move up, up arrow
#define IK_Right                         0xff53  ///< Keyboard : Move right, right arrow
#define IK_Down                          0xff54  ///< Keyboard : Move down, down arrow
#define IK_Prior                         0xff55  ///< Keyboard : Prior, previous
#define IK_PageUp                        0xff55  ///< Keyboard : Page Up
#define IK_Next                          0xff56  ///< Keyboard : Next
#define IK_PageDown                      0xff56  ///< Keyboard : Page Down
#define IK_End                           0xff57  ///< Keyboard : End of Line
#define IK_Begin                         0xff58  ///< Keyboard : Begin of Line

/* Misc functions */
#define IK_Select                        0xff60  ///< Keyboard : Select, mark
#define IK_Print                         0xff61  ///< Keyboard : Print
#define IK_Execute                       0xff62  ///< Keyboard : Execute, run, do
#define IK_Insert                        0xff63  ///< Keyboard : Insert, insert here
#define IK_Undo                          0xff65  ///< Keyboard : Undo
#define IK_Redo                          0xff66  ///< Keyboard : Redo, again
#define IK_Menu                          0xff67  ///< Keyboard : Menu
#define IK_Find                          0xff68  ///< Keyboard : Find, search
#define IK_Cancel                        0xff69  ///< Keyboard : Cancel, stop, abort, exit
#define IK_Help                          0xff6a  ///< Keyboard : Help
#define IK_Break                         0xff6b  ///< Keyboard : Break
#define IK_ModeSwitch                    0xff7e  ///< Keyboard : Character set switch
#define IK_ScriptSwitch                  0xff7e  ///< Keyboard : Alias for ModeSwitch
#define IK_NumLock                       0xff7f  ///< Keyboard : Numerical Lock

/* Keypad functions, keypad numbers cleverly chosen to map to ASCII */
#define IK_KP_Space                      0xff80  ///< Keypad : Space
#define IK_KP_Tab                        0xff89  ///< Keypad : Tab
#define IK_KP_Enter                      0xff8d  ///< Keypad : Enter
#define IK_KP_F1                         0xff91  ///< Keypad : PF1, KP_A, ...
#define IK_KP_F2                         0xff92  ///< Keypad : F2
#define IK_KP_F3                         0xff93  ///< Keypad : F3
#define IK_KP_F4                         0xff94  ///< Keypad : F4
#define IK_KP_Home                       0xff95  ///< Keypad : Home
#define IK_KP_Left                       0xff96  ///< Keypad : Left
#define IK_KP_Up                         0xff97  ///< Keypad : Up
#define IK_KP_Right                      0xff98  ///< Keypad : Right
#define IK_KP_Down                       0xff99  ///< Keypad : Down
#define IK_KP_Prior                      0xff9a  ///< Keypad : Prior
#define IK_KP_PageUp                     0xff9a  ///< Keypad : Page Up
#define IK_KP_Next                       0xff9b  ///< Keypad : Next
#define IK_KP_PageDown                   0xff9b  ///< Keypad : Page Down
#define IK_KP_End                        0xff9c  ///< Keypad : End of Line
#define IK_KP_Begin                      0xff9d  ///< Keypad : Begin of Line
#define IK_KP_Insert                     0xff9e  ///< Keypad : Insert
#define IK_KP_Delete                     0xff9f  ///< Keypad : Delete
#define IK_KP_Equal                      0xffbd  ///< Keypad : Equals
#define IK_KP_Multiply                   0xffaa  ///< Keypad : Multiply, *
#define IK_KP_Add                        0xffab  ///< Keypad : Add, +
#define IK_KP_Separator                  0xffac  ///< Keypad : Separator, often comma
#define IK_KP_Subtract                   0xffad  ///< Keypad : Subtract, -
#define IK_KP_Decimal                    0xffae  ///< Keypad : Decimal
#define IK_KP_Divide                     0xffaf  ///< Keypad : Divide, /

#define IK_KP_0                          0xffb0  ///< Keypad : 0
#define IK_KP_1                          0xffb1  ///< Keypad : 1
#define IK_KP_2                          0xffb2  ///< Keypad : 2
#define IK_KP_3                          0xffb3  ///< Keypad : 3
#define IK_KP_4                          0xffb4  ///< Keypad : 4
#define IK_KP_5                          0xffb5  ///< Keypad : 5
#define IK_KP_6                          0xffb6  ///< Keypad : 6
#define IK_KP_7                          0xffb7  ///< Keypad : 7
#define IK_KP_8                          0xffb8  ///< Keypad : 8
#define IK_KP_9                          0xffb9  ///< Keypad : 9

/*
 * Auxiliary functions; note the duplicate definitions for left and right
 * function keys;  Sun keyboards and a few other manufacturers have such
 * function key groups on the left and/or right sides of the keyboard.
 * We've not found a keyboard with more than 35 function keys total.
 */
#define IK_F1                            0xffbe  ///< Keyboard : F1
#define IK_F2                            0xffbf  ///< Keyboard : F2
#define IK_F3                            0xffc0  ///< Keyboard : F3
#define IK_F4                            0xffc1  ///< Keyboard : F4
#define IK_F5                            0xffc2  ///< Keyboard : F5
#define IK_F6                            0xffc3  ///< Keyboard : F6
#define IK_F7                            0xffc4  ///< Keyboard : F7
#define IK_F8                            0xffc5  ///< Keyboard : F8
#define IK_F9                            0xffc6  ///< Keyboard : F9
#define IK_F10                           0xffc7  ///< Keyboard : F10
#define IK_F11                           0xffc8  ///< Keyboard : F11
#define IK_F12                           0xffc9  ///< Keyboard : F12
#define IK_F13                           0xffca  ///< Keyboard : F13
#define IK_F14                           0xffcb  ///< Keyboard : F14
#define IK_F15                           0xffcc  ///< Keyboard : F15
#define IK_F16                           0xffcd  ///< Keyboard : F16
#define IK_F17                           0xffce  ///< Keyboard : F17
#define IK_F18                           0xffcf  ///< Keyboard : F18
#define IK_F19                           0xffd0  ///< Keyboard : F19
#define IK_F20                           0xffd1  ///< Keyboard : F20
#define IK_F21                           0xffd2  ///< Keyboard : F21
#define IK_F22                           0xffd3  ///< Keyboard : F22
#define IK_F23                           0xffd4  ///< Keyboard : F23
#define IK_F24                           0xffd5  ///< Keyboard : F24

/* Modifiers */
#define IK_Shift_L                       0xffe1  ///< Keyboard : Left shift
#define IK_Shift_R                       0xffe2  ///< Keyboard : Right shift
#define IK_Control_L                     0xffe3  ///< Keyboard : Left control
#define IK_Control_R                     0xffe4  ///< Keyboard : Right control
#define IK_CapsLock                      0xffe5  ///< Keyboard : Caps lock
#define IK_ShiftLock                     0xffe6  ///< Keyboard : Shift lock

#define IK_Meta_L                        0xffe7  ///< Keyboard : Left meta
#define IK_Meta_R                        0xffe8  ///< Keyboard : Right meta
#define IK_Alt_L                         0xffe9  ///< Keyboard : Left alt
#define IK_Alt_R                         0xffea  ///< Keyboard : Right alt
#define IK_Super_L                       0xffeb  ///< Keyboard : Left super
#define IK_Super_R                       0xffec  ///< Keyboard : Right super
#define IK_Hyper_L                       0xffed  ///< Keyboard : Left hyper
#define IK_Hyper_R                       0xffee  ///< Keyboard : Right hyper


/* Main Keys */

// Line#1
#define IK_Grave                         0x0060  ///< Keyboard : BACK_QUOTE
#define IK_1                             0x0031  ///< Keyboard : 1
#define IK_2                             0x0032  ///< Keyboard : 2
#define IK_3                             0x0033  ///< Keyboard : 3
#define IK_4                             0x0034  ///< Keyboard : 4
#define IK_5                             0x0035  ///< Keyboard : 5
#define IK_6                             0x0036  ///< Keyboard : 6
#define IK_7                             0x0037  ///< Keyboard : 7
#define IK_8                             0x0038  ///< Keyboard : 8
#define IK_9                             0x0039  ///< Keyboard : 9
#define IK_0                             0x0030  ///< Keyboard : 0
#define IK_Minus                         0x002d  ///< Keyboard : -
#define IK_Equal                         0x003d  ///< Keyboard : =

// Line#2
#define IK_Q                             0x0071  ///< Keyboard : q
#define IK_W                             0x0077  ///< Keyboard : w
#define IK_E                             0x0065  ///< Keyboard : e
#define IK_R                             0x0072  ///< Keyboard : r
#define IK_T                             0x0074  ///< Keyboard : t
#define IK_Y                             0x0079  ///< Keyboard : y
#define IK_U                             0x0075  ///< Keyboard : u
#define IK_I                             0x0069  ///< Keyboard : i
#define IK_O                             0x006f  ///< Keyboard : o
#define IK_P                             0x0070  ///< Keyboard : p
#define IK_BracketLeft                   0x005b  ///< Keyboard : [
#define IK_BracketRight                  0x005d  ///< Keyboard : ]
#define IK_BackSlash                     0x005c  ///< Keyboard : Backslash "\\"

// Line#3
#define IK_A                             0x0061  ///< Keyboard : a
#define IK_S                             0x0073  ///< Keyboard : s
#define IK_D                             0x0064  ///< Keyboard : d
#define IK_F                             0x0066  ///< Keyboard : f
#define IK_G                             0x0067  ///< Keyboard : g
#define IK_H                             0x0068  ///< Keyboard : h
#define IK_J                             0x006a  ///< Keyboard : j
#define IK_K                             0x006b  ///< Keyboard : k
#define IK_L                             0x006c  ///< Keyboard : l
#define IK_SemiColon                     0x003b  ///< Keyboard : ;
#define IK_Apostrophe                    0x0027  ///< Keyboard : SINGLE_QUOTE

// Line#4
#define IK_Z                             0x007a  ///< Keyboard : z
#define IK_X                             0x0078  ///< Keyboard : x
#define IK_C                             0x0063  ///< Keyboard : c
#define IK_V                             0x0076  ///< Keyboard : v
#define IK_B                             0x0062  ///< Keyboard : b
#define IK_N                             0x006e  ///< Keyboard : n
#define IK_M                             0x006d  ///< Keyboard : m
#define IK_Comma                         0x002c  ///< Keyboard : ,
#define IK_Period                        0x002e  ///< Keyboard : .
#define IK_Slash                         0x002f  ///< Keyboard : /

// Line#5
#define IK_Space                         0x0020  ///< Keyboard : " "


/* Constant for European keyboards - less usage!! */
#define IK_dead_grave                    0xfe50  ///< Keyboard(dead) : grave
#define IK_dead_acute                    0xfe51  ///< Keyboard(dead) : acute
#define IK_dead_circumflex               0xfe52  ///< Keyboard(dead) : circumflex
#define IK_dead_tilde                    0xfe53  ///< Keyboard(dead) : tilde
#define IK_dead_macron                   0xfe54  ///< Keyboard(dead) : macron
#define IK_dead_breve                    0xfe55  ///< Keyboard(dead) : breve
#define IK_dead_abovedot                 0xfe56  ///< Keyboard(dead) : above dot
#define IK_dead_diaeresis                0xfe57  ///< Keyboard(dead) : diaeresis
#define IK_dead_abovering                0xfe58  ///< Keyboard(dead) : above ring
#define IK_dead_doubleacute              0xfe59  ///< Keyboard(dead) : double acute
#define IK_dead_caron                    0xfe5a  ///< Keyboard(dead) : caron
#define IK_dead_cedilla                  0xfe5b  ///< Keyboard(dead) : cedilla
#define IK_dead_ogonek                   0xfe5c  ///< Keyboard(dead) : ogonek
#define IK_dead_iota                     0xfe5d  ///< Keyboard(dead) : iota
#define IK_dead_voiced_sound             0xfe5e  ///< Keyboard(dead) : voiced sound
#define IK_dead_semivoiced_sound         0xfe5f  ///< Keyboard(dead) : semivoiced sound
#define IK_dead_belowdot                 0xfe60  ///< Keyboard(dead) : below dot
#define IK_dead_hook                     0xfe61  ///< Keyboard(dead) : hook
#define IK_dead_horn                     0xfe62  ///< Keyboard(dead) : horn
#define IK_dead_stroke                   0xfe63  ///< Keyboard(dead) : stroke

/**
 * @}  // end of xkeys
 */

#endif // _XKEYS_H_
