/*******************************************************************************
 * FILENAME: DataProcessors.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file is part of the Plugin SDK.  It has things needed for
 *    the Data Processors System (encoders, decoders, ect).
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
 *
 *    This program is free software: you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation, either version 3 of the License, or (at your
 *    option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program. If not, see https://www.gnu.org/licenses/.
 *
 * HISTORY:
 *    Paul Hutchinson (30 Aug 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __DATAPROCESSORS_H_
#define __DATAPROCESSORS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginTypes.h"
#include "PluginUI.h"
#include "KeyDefines.h"

/***  DEFINES                          ***/
/* Versions of struct DataProcessorAPI */
#define DATA_PROCESSORS_API_VERSION_1       1
#define DATA_PROCESSORS_API_VERSION_2       2
#define DATA_PROCESSORS_API_VERSION_3       3

/* Versions of struct DPS_API */
#define DPS_API_VERSION_1                   1
#define DPS_API_VERSION_2                   2
#define DPS_API_VERSION_3                   3

#define TXT_ATTRIB_UNDERLINE                0x0001
#define TXT_ATTRIB_UNDERLINE_DOUBLE         0x0002
#define TXT_ATTRIB_UNDERLINE_DOTTED         0x0004
#define TXT_ATTRIB_UNDERLINE_DASHED         0x0008
#define TXT_ATTRIB_UNDERLINE_WAVY           0x0010
#define TXT_ATTRIB_OVERLINE                 0x0020
#define TXT_ATTRIB_LINETHROUGH              0x0040
#define TXT_ATTRIB_BOLD                     0x0080
#define TXT_ATTRIB_ITALIC                   0x0100
#define TXT_ATTRIB_OUTLINE                  0x0200
#define TXT_ATTRIB_BOX                      0x0400  // Future
#define TXT_ATTRIB_ROUNDBOX                 0x0800
#define TXT_ATTRIB_REVERSE                  0x1000
#define TXT_ATTRIB_RESERVED                 0x8000  // Do not use

/* Goes back to version 1.0 (just here for compatibility) */
#define TXT_ATTRIB_LINETHROUGHT             TXT_ATTRIB_LINETHROUGH

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_SysCol_Black=0,
    e_SysCol_Red,
    e_SysCol_Green,
    e_SysCol_Yellow,
    e_SysCol_Blue,
    e_SysCol_Magenta,
    e_SysCol_Cyan,
    e_SysCol_White,
    e_SysColMAX
} e_SysColType;

typedef enum
{
    e_SysColShade_Normal,
    e_SysColShade_Bright,
    e_SysColShade_Dark,
    e_SysColShadeMAX,
} e_SysColShadeType;

typedef enum
{
    e_DefaultColors_BG,
    e_DefaultColors_FG,
    e_DefaultColorsMAX
} e_DefaultColorsType;

typedef struct DataProcessorHandle {int PrivateDataHere;} t_DataProcessorHandleType;            // Fake type holder
typedef struct DataProSettingsWidgets {int PrivateDataHere;} t_DataProSettingsWidgetsType;      // Fake type holder
typedef struct DataProMark {int PrivateDataHere;} t_DataProMark;                                // Fake type holder

typedef enum
{
    e_TextDataProcessorClass_Other,
    e_TextDataProcessorClass_CharEncoding,
    e_TextDataProcessorClass_TermEmulation,
    e_TextDataProcessorClass_Highlighter,
    e_TextDataProcessorClass_Logger,
    e_TextDataProcessorClassMAX
} e_TextDataProcessorClassType;

typedef enum
{
    e_BinaryDataProcessorClass_Other,
    e_BinaryDataProcessorClass_Decoder,
    e_BinaryDataProcessorClassMAX
} e_BinaryDataProcessorClassType;

typedef enum
{
    e_BinaryDataProcessorMode_Text,
    e_BinaryDataProcessorMode_Hex,
//    e_BinaryDataProcessorMode_Table,
    e_BinaryDataProcessorModeMAX
} e_BinaryDataProcessorModeType;

typedef enum
{
    e_DataProcessorType_Text,
    e_DataProcessorType_Binary,
    e_DataProcessorTypeMAX
} e_DataProcessorTypeType;

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct DataProcessorInfo
{
    const char *DisplayName;
    const char *Tip;
    const char *Help;
    e_DataProcessorTypeType ProType;

    union
    {
        e_TextDataProcessorClassType TxtClass;      // Only applies to text processors
        e_BinaryDataProcessorClassType BinClass;    // Only applies to binary processors
    };
    e_BinaryDataProcessorModeType BinMode;      // Only applies to binary processors
};

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct DataProcessorAPI
{
    /********* Start of DATA_PROCESSORS_API_VERSION_1 *********/
    t_DataProcessorHandleType *(*AllocateData)(void);
    void (*FreeData)(t_DataProcessorHandleType *DataHandle);
    const struct DataProcessorInfo *(*GetProcessorInfo)(unsigned int *SizeOfInfo);
    PG_BOOL (*ProcessKeyPress)(t_DataProcessorHandleType *DataHandle,
            const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
            uint8_t Mod);
    void (*ProcessIncomingTextByte)(t_DataProcessorHandleType *DataHandle,
            const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
            PG_BOOL *Consumed);
    void (*ProcessIncomingBinaryByte)(t_DataProcessorHandleType *DataHandle,
            const uint8_t Byte);
    /********* End of DATA_PROCESSORS_API_VERSION_1 *********/
    /********* Start of DATA_PROCESSORS_API_VERSION_2 *********/
    void (*ProcessOutGoingData)(t_DataProcessorHandleType *DataHandle,
            const uint8_t *TxData,int Bytes);
    /********* End of DATA_PROCESSORS_API_VERSION_2 *********/
    /********* Start of DATA_PROCESSORS_API_VERSION_3 *********/
    t_DataProSettingsWidgetsType *(*AllocSettingsWidgets)(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Settings);
    void (*FreeSettingsWidgets)(t_DataProSettingsWidgetsType *PrivData);
    void (*SetSettingsFromWidgets)(t_DataProSettingsWidgetsType *PrivData,t_PIKVList *Settings);
    void (*ApplySettings)(t_DataProcessorHandleType *DataHandle,t_PIKVList *Settings);
    /********* End of DATA_PROCESSORS_API_VERSION_3 *********/
};

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct DPS_API
{
    /********* Start of DPS_API_VERSION_1 *********/
    PG_BOOL (*RegisterDataProcessor)(const char *ProID,const struct DataProcessorAPI *ProAPI,int SizeOfProAPI);
    const struct PI_UIAPI *(*GetAPI_UI)(void);
    void (*WriteData)(const uint8_t *Data,int Bytes);
    uint32_t (*GetSysColor)(uint32_t SysColShade,uint32_t SysColor);
    uint32_t (*GetSysDefaultColor)(uint32_t DefaultColor);
    void (*SetFGColor)(uint32_t FGColor);
    uint32_t (*GetFGColor)(void);
    void (*SetBGColor)(uint32_t BGColor);
    uint32_t (*GetBGColor)(void);
    void (*SetULineColor)(uint32_t ULineColor);
    uint32_t (*GetULineColor)(void);
    void (*SetAttribs)(uint32_t Attribs);
    uint32_t (*GetAttribs)(void);
    void (*SetTitle)(const char *Title);
    void (*DoNewLine)(void);
    void (*DoReturn)(void);
    void (*DoBackspace)(void);
    void (*DoClearScreen)(void);
    void (*DoClearArea)(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2);
    void (*DoTab)(void);
    void (*DoPrevTab)(void);
    void (*DoSystemBell)(int VisualOnly);
    void (*DoScrollArea)(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,int32_t DeltaX,int32_t DeltaY);
    void (*DoClearScreenAndBackBuffer)(void);
    void (*GetCursorXY)(int32_t *RetCursorX,int32_t *RetCursorY);
    void (*SetCursorXY)(uint32_t X,uint32_t Y);
    void (*GetScreenSize)(int32_t *RetRows,int32_t *RetColumns);
    void (*NoteNonPrintable)(const char *CodeStr);
    void (*SendBackspace)(void);
    void (*SendEnter)(void);
    void (*BinaryAddText)(const char *Str);
    void (*BinaryAddHex)(uint8_t Byte);
    void (*InsertString)(uint8_t *Str,uint32_t Len);
    /********* End of DPS_API_VERSION_1 *********/
    /********* Start of DPS_API_VERSION_2 *********/
    void (*SetCurrentSettingsTabName)(const char *Name);
    t_WidgetSysHandle *(*AddNewSettingsTab)(const char *Name);
    t_DataProMark *(*AllocateMark)(void);
    void (*FreeMark)(t_DataProMark *Mark);
    PG_BOOL (*IsMarkValid)(t_DataProMark *Mark);
    void (*SetMark2CursorPos)(t_DataProMark *Mark);
    void (*ApplyAttrib2Mark)(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
    void (*RemoveAttribFromMark)(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
    void (*ApplyFGColor2Mark)(t_DataProMark *Mark,uint32_t FGColor,uint32_t Offset,uint32_t Len);
    void (*ApplyBGColor2Mark)(t_DataProMark *Mark,uint32_t BGColor,uint32_t Offset,uint32_t Len);
    void (*MoveMark)(t_DataProMark *Mark,int Amount);
    void (*FreezeStream)(void);
    void (*ClearFrozenStream)(void);
    void (*ReleaseFrozenStream)(void);
    /********* End of DPS_API_VERSION_2 *********/
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
