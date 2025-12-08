/*******************************************************************************
 * FILENAME: PluginUI.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
 *    Paul Hutchinson (10 Jul 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __PLUGINUI_H_
#define __PLUGINUI_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/PluginTypes.h"
#include <stdint.h>

/***  DEFINES                          ***/
/* Versions of struct PI_UIAPI */
#define PI_UIAPI_API_VERSION_1          1
#define PI_UIAPI_API_VERSION_2          2
#define PI_UIAPI_API_VERSION_3          3

/* Ask Types */
#define PIUI_ASK_OK             0x0001
#define PIUI_ASK_OKCANCEL       0x0002
#define PIUI_ASK_YESNO          0x0003
#define PIUI_ASK_RETRYCANCEL    0x0004
#define PIUI_ASK_IGNORECANCEL   0x0005

/* Ask return values */
#define PIUI_ASK_OK_BTTN        1
#define PIUI_ASK_CANCEL_BTTN    0
#define PIUI_ASK_YES_BTTN       1
#define PIUI_ASK_NO_BTTN        0
#define PIUI_ASK_RETRY_BTTN     1
#define PIUI_ASK_IGNORE_BTTN    1

#define SUGGESTED_STYLE_DATA_STR_BUFFER_LEN             512

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/* What this really is depends on the sub system */
struct WidgetSysHandle {int PrivateData;};
typedef struct WidgetSysHandle t_WidgetSysHandle;

struct PIUIComboBoxCtrl {int PrivateData;};
typedef struct PIUIComboBoxCtrl t_PIUIComboBoxCtrl;

struct PIUITextInputCtrl {int PrivateData;};
typedef struct PIUITextInputCtrl t_PIUITextInputCtrl;

struct PIUINumberInputCtrl {int PrivateData;};
typedef struct PIUINumberInputCtrl t_PIUINumberInputCtrl;

struct PIUIDoubleInputCtrl {int PrivateData;};
typedef struct PIUIDoubleInputCtrl t_PIUIDoubleInputCtrl;

struct PIUIColumnViewInputCtrl {int PrivateData;};
typedef struct PIUIColumnViewInputCtrl t_PIUIColumnViewInputCtrl;

struct PIUIButtonInputCtrl {int PrivateData;};
typedef struct PIUIButtonInputCtrl t_PIUIButtonInputCtrl;

struct PIUIIndicatorCtrl {int PrivateData;};
typedef struct PIUIIndicatorCtrl t_PIUIIndicatorCtrl;

struct PIUILabelCtrl {int PrivateData;};
typedef struct PIUILabelCtrl t_PIUILabelCtrl;

struct PIUIRadioBttnCtrl {int PrivateData;};
typedef struct PIUIRadioBttnCtrl t_PIUIRadioBttnCtrl;

struct PIUICheckboxCtrl {int PrivateData;};
typedef struct PIUICheckboxCtrl t_PIUICheckboxCtrl;

struct PIUITextBoxCtrl {int PrivateData;};
typedef struct PIUITextBoxCtrl t_PIUITextBoxCtrl;

struct PIUIGroupBoxCtrl {int PrivateData;};
typedef struct PIUIGroupBoxCtrl t_PIUIGroupBoxCtrl;

struct PIUIColorPickCtrl {int PrivateData;};
typedef struct PIUIColorPickCtrl t_PIUIColorPickCtrl;

struct PIUIStylePickCtrl {int PrivateData;};
typedef struct PIUIStylePickCtrl t_PIUIStylePickCtrl;

/* PI_Event_ComboxBox */
typedef enum
{
    e_PIECB_IndexChanged,
    e_PIECB_TextInputChanged,
    e_PIECB_TextInputEditFinished,          // PI_UIAPI_API_VERSION_3
    e_PIECBMAX
} e_PIECBType;

struct PICBEvent
{
    e_PIECBType EventType;
};

/* PI_Event_RadioBox */
typedef enum
{
    e_PIERB_Changed,
    e_PIERBMAX
} e_PIERBType;

struct PIRBEvent
{
    e_PIERBType EventType;
    struct PI_RadioBttn *Bttn;
};

/* PI_Event_Checkbox */
typedef enum
{
    e_PIECheckbox_Changed,
    e_PIECheckboxMAX
} e_PIECheckboxType;

struct PICheckboxEvent
{
    e_PIECheckboxType EventType;
    struct PI_Checkbox *CheckBox;
    PG_BOOL Checked;
};

/* PI_Event_ColumnView */
typedef enum
{
    e_PIECV_IndexChanged,
    e_PIECVMAX
} e_PIECVType;

struct PICVEvent
{
    e_PIECVType EventType;
    int Index;
};

/* PI_Event_Button */
typedef enum
{
    e_PIEButton_Press,
    e_PIEButtonMAX
} e_PIEButtonType;

struct PIButtonEvent
{
    e_PIEButtonType EventType;
    int Index;      // Not used.  Seems to have just been copied and wasn't cleaned up, so now it lives on forever...
};

/* PIColorPickEvent */
typedef enum
{
    e_PIEColorPick_Press,
    e_PIEColorPickMAX
} e_PIEColorPickType;

struct PIColorPickEvent
{
    e_PIEColorPickType EventType;
};

/* PIStylePickEvent */
typedef enum
{
    e_PIEStylePick_NewStyle,
    e_PIEStylePickMAX
} e_PIEStylePickType;

struct PIStylePickEvent
{
    e_PIEStylePickType EventType;
};

struct PI_ComboBox
{
    t_PIUIComboBoxCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_RadioBttn
{
    t_PIUIRadioBttnCtrl *Ctrl;
    void *UIData;
};

struct PI_Checkbox
{
    t_PIUICheckboxCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_RadioBttnGroup {int PrivateData;};

struct PI_TextInput
{
    t_PIUITextInputCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_NumberInput
{
    t_PIUINumberInputCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_DoubleInput
{
    t_PIUIDoubleInputCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_ColumnViewInput
{
    t_PIUIColumnViewInputCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_ButtonInput
{
    t_PIUIButtonInputCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_Indicator
{
    t_PIUIIndicatorCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_TextBox
{
    t_PIUITextBoxCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_GroupBox
{
    t_PIUIGroupBoxCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    t_WidgetSysHandle *GroupWidgetHandle;
    void *UIData;
};

struct PI_ColorPick
{
    t_PIUIColorPickCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

struct PI_StylePick
{
    t_PIUIStylePickCtrl *Ctrl;
    t_PIUILabelCtrl *Label;
    void *UIData;
};

typedef enum
{
    e_FileReqType_Load,
    e_FileReqType_Save,
    e_FileReqTypeMAX
} e_FileReqTypeType;

typedef enum
{
    e_TextBoxProp_FontMode,
    e_TextBoxPropMAX
} e_TextBoxPropType;

typedef enum
{
    e_ComboBoxPropMAX
} e_ComboBoxPropType;

typedef enum
{
    e_RadioBttnPropMAX
} e_RadioBttnPropType;

typedef enum
{
    e_CheckboxPropMAX
} e_CheckboxPropType;

typedef enum
{
    e_TextInputPropMAX
} e_TextInputPropType;

typedef enum
{
    e_NumberInputPropMAX
} e_NumberInputPropType;

typedef enum
{
    e_DoubleInputPropMAX
} e_DoubleInputPropType;

typedef enum
{
    e_ColumnViewInputPropMAX
} e_ColumnViewInputPropType;

typedef enum
{
    e_ButtonInputPropMAX
} e_ButtonInputPropType;

typedef enum
{
    e_IndicatorPropMAX
} e_IndicatorPropType;

typedef enum
{
    e_GroupBoxPropMAX
} e_GroupBoxPropType;

typedef enum
{
    e_ColorPickPropMAX
} e_ColorPickPropType;

typedef enum
{
    e_StylePickPropMAX
} e_StylePickPropType;

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct PI_UIAPI
{
    /********* Start of PI_UIAPI_API_VERSION_1 *********/
    /* Combobox Input */
    struct PI_ComboBox *(*AddComboBox)(t_WidgetSysHandle *WidgetHandle,PG_BOOL UserEditable,const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),void *UserData);
    void (*FreeComboBox)(t_WidgetSysHandle *WidgetHandle,struct PI_ComboBox *UICtrl);
    void (*ClearComboBox)(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox);
    void (*AddItem2ComboBox)(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,const char *Label,uintptr_t ID);
    void (*SetComboBoxSelectedEntry)(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,uintptr_t ID);
    uintptr_t (*GetComboBoxSelectedEntry)(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox);
    const char *(*GetComboBoxText)(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox);
    void (*SetComboBoxText)(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,const char *Txt);
    void (*EnableComboBox)(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,PG_BOOL Enabled);

    /* Radio button Input */
    struct PI_RadioBttnGroup *(*AllocRadioBttnGroup)(t_WidgetSysHandle *WidgetHandle,const char *Label);
    void (*FreeRadioBttnGroup)(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttnGroup *UICtrl);
    struct PI_RadioBttn *(*AddRadioBttn)(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttnGroup *RBGroup,const char *Label,void (*EventCB)(const struct PIRBEvent *Event,void *UserData),void *UserData);
    void (*FreeRadioBttn)(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *UICtrl);
    PG_BOOL (*IsRadioBttnChecked)(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *Bttn);
    void (*SetRadioBttnChecked)(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *Bttn,PG_BOOL Checked);
    void (*EnableRadioBttn)(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *Bttn,PG_BOOL Enabled);

    /* Checkbox Input */
    struct PI_Checkbox *(*AddCheckbox)(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData),void *UserData);
    void (*FreeCheckbox)(t_WidgetSysHandle *WidgetHandle,struct PI_Checkbox *UICtrl);
    PG_BOOL (*IsCheckboxChecked)(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn);
    void (*SetCheckboxChecked)(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn,PG_BOOL Checked);
    void (*EnableCheckbox)(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn,PG_BOOL Enabled);

    /* Text Input */
    struct PI_TextInput *(*AddTextInput)(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),void *UserData);
    void (*FreeTextInput)(t_WidgetSysHandle *WidgetHandle,struct PI_TextInput *UICtrl);
    const char *(*GetTextInputText)(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *TextInput);
    void (*SetTextInputText)(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *TextInput,const char *Txt);
    void (*EnableTextInput)(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *TextInput,PG_BOOL Enabled);

    /* Number Input */
    struct PI_NumberInput *(*AddNumberInput)(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),void *UserData);
    void (*FreeNumberInput)(t_WidgetSysHandle *WidgetHandle,struct PI_NumberInput *UICtrl);
    uint64_t (*GetNumberInputValue)(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput);
    void (*SetNumberInputValue)(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput,int64_t Value);
    void (*SetNumberInputMinMax)(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput,int64_t Min,int64_t Max);
    void (*EnableNumberInput)(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput,PG_BOOL Enabled);

    /* Double Input */
    struct PI_DoubleInput *(*AddDoubleInput)(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),void *UserData);
    void (*FreeDoubleInput)(t_WidgetSysHandle *WidgetHandle,struct PI_DoubleInput *UICtrl);
    double (*GetDoubleInputValue)(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput);
    void (*SetDoubleInputValue)(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,double Value);
    void (*SetDoubleInputMinMax)(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,double Min,double Max);
    void (*SetDoubleInputDecimals)(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,int Points);
    void (*EnableDoubleInput)(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,PG_BOOL Enabled);

    /* List View */
    struct PI_ColumnViewInput *(*AddColumnViewInput)(t_WidgetSysHandle *WidgetHandle,const char *Label,int Columns,const char *ColumnNames[],void (*EventCB)(const struct PICVEvent *Event,void *UserData),void *UserData);
    void (*FreeColumnViewInput)(t_WidgetSysHandle *WidgetHandle,struct PI_ColumnViewInput *UICtrl);
    void (*ColumnViewInputClear)(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl);
    void (*ColumnViewInputRemoveRow)(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Row);
    int (*ColumnViewInputAddRow)(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl);
    void (*ColumnViewInputSetColumnText)(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Column,int Row,const char *Str);
    void (*ColumnViewInputSelectRow)(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Row);
    void (*ColumnViewInputClearSelection)(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl);

    /* Button */
    struct PI_ButtonInput *(*AddButtonInput)(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PIButtonEvent *Event,void *UserData),void *UserData);
    void (*FreeButtonInput)(t_WidgetSysHandle *WidgetHandle,struct PI_ButtonInput *UICtrl);

    /* Indicator */
    struct PI_Indicator *(*AddIndicator)(t_WidgetSysHandle *WidgetHandle,const char *Label);
    void (*FreeIndicator)(t_WidgetSysHandle *WidgetHandle,struct PI_Indicator *UICtrl);
    void (*SetIndicator)(t_WidgetSysHandle *WidgetHandle,t_PIUIIndicatorCtrl *UICtrl,bool On);

    /* Ask */
    int (*Ask)(const char *Message,int Type);
    /********* End of PI_UIAPI_API_VERSION_1 *********/

    /********* Start of PI_UIAPI_API_VERSION_2 *********/
    PG_BOOL (*FileReq)(e_FileReqTypeType Req,const char *Title,char **Path,char **Filename,const char *Filters,int SelectedFilter);
    void (*FreeFileReqPathAndFile)(char **Path,char **Filename);

    /* TextBox (display only) */
    struct PI_TextBox *(*AddTextBox)(t_WidgetSysHandle *WidgetHandle,const char *Label,const char *Text);
    void (*FreeTextBox)(t_WidgetSysHandle *WidgetHandle,struct PI_TextBox *BoxHandle);
    void (*SetTextBox)(t_WidgetSysHandle *WidgetHandle,t_PIUITextBoxCtrl *UICtrl,const char *Text);

    /* GroupBox */
    struct PI_GroupBox *(*AddGroupBox)(t_WidgetSysHandle *WidgetHandle,const char *Label);
    void (*FreeGroupBox)(t_WidgetSysHandle *WidgetHandle,struct PI_GroupBox *BoxHandle);
    void (*SetGroupBoxLabel)(t_WidgetSysHandle *WidgetHandle,t_PIUIGroupBoxCtrl *UICtrl,const char *Label);

    /* Color Pick */
    struct PI_ColorPick *(*AddColorPick)(t_WidgetSysHandle *WidgetHandle,const char *Label,uint32_t RGB,void (*EventCB)(const struct PIColorPickEvent *Event,void *UserData),void *UserData);
    void (*FreeColorPick)(t_WidgetSysHandle *WidgetHandle,struct PI_ColorPick *Handle);
    uint32_t (*GetColorPickValue)(t_WidgetSysHandle *WidgetHandle,t_PIUIColorPickCtrl *UICtrl);
    void (*SetColorPickValue)(t_WidgetSysHandle *WidgetHandle,t_PIUIColorPickCtrl *UICtrl,uint32_t RGB);
    /********* End of PI_UIAPI_API_VERSION_2 *********/

    /********* Start of PI_UIAPI_API_VERSION_3 *********/
    struct PI_StylePick *(*AddStylePick)(t_WidgetSysHandle *WidgetHandle,const char *Label,struct StyleData *SD,void (*EventCB)(const struct PIStylePickEvent *Event,void *UserData),void *UserData);
    void (*FreeStylePick)(t_WidgetSysHandle *WidgetHandle,struct PI_StylePick *Handle);
    void (*GetStylePickValue)(t_WidgetSysHandle *WidgetHandle,t_PIUIStylePickCtrl *UICtrl,struct StyleData *SD);
    void (*SetStylePickValue)(t_WidgetSysHandle *WidgetHandle,t_PIUIStylePickCtrl *UICtrl,struct StyleData *SD);
    PG_BOOL (*Style2StrHelper)(struct StyleData *SD,char *Str,int MaxLen);
    void (*Str2StyleHelper)(struct StyleData *SD,const char *Str);

    /* Widget Properties */
    PG_BOOL (*ChangeTextBoxProp)(t_WidgetSysHandle *WidgetHandle,t_PIUITextBoxCtrl *UICtrl,e_TextBoxPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeComboBoxProp)(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *UICtrl,e_ComboBoxPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeRadioBttnProp)(t_WidgetSysHandle *WidgetHandle,t_PIUIRadioBttnCtrl *UICtrl,e_RadioBttnPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeCheckboxProp)(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *UICtrl,e_CheckboxPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeTextInputProp)(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *UICtrl,e_TextInputPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeNumberInputProp)(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *UICtrl,e_NumberInputPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeDoubleInputProp)(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *UICtrl,e_DoubleInputPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeColumnViewInputProp)(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,e_ColumnViewInputPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeButtonInputProp)(t_WidgetSysHandle *WidgetHandle,t_PIUIButtonInputCtrl *UICtrl,e_ButtonInputPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeIndicatorProp)(t_WidgetSysHandle *WidgetHandle,t_PIUIIndicatorCtrl *UICtrl,e_IndicatorPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeGroupBoxProp)(t_WidgetSysHandle *WidgetHandle,t_PIUIGroupBoxCtrl *UICtrl,e_GroupBoxPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeColorPickProp)(t_WidgetSysHandle *WidgetHandle,t_PIUIColorPickCtrl *UICtrl,e_ColorPickPropType Prop,uint32_t Value,void *Ptr);
    PG_BOOL (*ChangeStylePickProp)(t_WidgetSysHandle *WidgetHandle,t_PIUIStylePickCtrl *UICtrl,e_StylePickPropType Prop,uint32_t Value,void *Ptr);

    void (*SetLabelText)(t_WidgetSysHandle *WidgetHandle,t_PIUILabelCtrl *UILabel,const char *Txt);

    /********* End of PI_UIAPI_API_VERSION_3 *********/
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
