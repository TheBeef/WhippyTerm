/*******************************************************************************
 * FILENAME: UIControl.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has program control UI function in it.
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
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UICONTROL_H_
#define __UICONTROL_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/KeyDefines.h"
#include <list>
#include <string>
#include <stdint.h>

/***  DEFINES                          ***/
#define UISTYLE_UNDERLINE       0x0001
#define UISTYLE_STRIKETHROUGH   0x0002
#define UISTYLE_ITALIC          0x0004
#define UISTYLE_BOLD            0x0008

/* TreeView drop positions */
#define TREEVIEW_DROP_ABOVE     1
#define TREEVIEW_DROP_BELOW     2
#define TREEVIEW_DROP_ON        3
#define TREEVIEW_DROP_WIDGET    4

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/* ListView */
struct ListViewItem
{
    std::string Label;
    uintptr_t ID;
};

typedef std::list<struct ListViewItem> t_ListViewItemListType;
typedef t_ListViewItemListType::iterator i_ListViewItemListType;
typedef t_ListViewItemListType::const_iterator ci_ListViewItemListType;

struct ComboBoxItem
{
    std::string Label;
    uintptr_t ID;
};

typedef std::list<struct ComboBoxItem> t_ComboBoxItemListType;
typedef t_ComboBoxItemListType::iterator i_ComboBoxItemListType;
typedef t_ComboBoxItemListType::const_iterator ci_ComboBoxItemListType;

struct UIMenu {int x;};
typedef struct UIMenu e_UIMenuCtrl;     // DEBUG PAUL: This starts a e_ should be a t_

struct UISubMenu {int x;};
typedef struct UISubMenu e_UISubMenuCtrl;

struct UIContextMenu {int x;};
typedef struct UIContextMenu t_UIContextMenuCtrl;

struct UICheckbox {int x;};
typedef struct UICheckbox t_UICheckboxCtrl;

struct UIListView {int x;};
typedef struct UIListView t_UIListViewCtrl;

struct UIButton {int x;};
typedef struct UIButton t_UIButtonCtrl;

struct UITabCtrl {int x;};
typedef struct UITabCtrl t_UITabCtrl;

struct UITab {int x;};
typedef struct UITab t_UITab;

struct UIComboBoxCtrl {int x;};
typedef struct UIComboBoxCtrl t_UIComboBoxCtrl;

struct UIScrollBarCtrl {int x;};
typedef struct UIScrollBarCtrl t_UIScrollBarCtrl;

struct UINumberInput {int x;};
typedef struct UINumberInput t_UINumberInput;

struct UIDoubleInput {int x;};
typedef struct UIDoubleInput t_UIDoubleInput;

struct UIToolbar {int x;};
typedef struct UIToolbar t_UIToolbarCtrl;

struct UIGroupBox {int x;};
typedef struct UIGroupBox t_UIGroupBox;

struct UITextInput {int x;};
typedef struct UITextInputCtrl t_UITextInputCtrl;

struct UIMuliListTextInput {int x;};
typedef struct UIMuliListTextInput t_UIMuliLineTextInputCtrl;

struct UIHTMLViewCtrl {int x;};
typedef struct UIHTMLViewCtrl t_UIHTMLViewCtrl;

struct UIRadioBttnCtrl {int x;};
typedef struct UIRadioBttn t_UIRadioBttnCtrl;

struct UILabelCtrl {int x;};
typedef struct UILabelCtrl t_UILabelCtrl;

struct UIContainerCtrl {int x;};
typedef struct UIContainerCtrl t_UIContainerCtrl;

struct UITreeItem {int x;};
typedef struct UITreeItem t_UITreeItem;

struct UITreeView {int x;};
typedef struct UITreeView t_UITreeView;

struct UIColumnView {int x;};
typedef struct UIColumnView t_UIColumnView;

struct UIProgressBarCtrl {int x;};
typedef struct UIProgressBarCtrl t_UIProgressBarCtrl;

struct UIColorPreviewCtrl {int x;};
typedef struct UIColorPreviewCtrl t_UIColorPreviewCtrl;

struct UIContainerFrame {int x;};
typedef struct UIContainerFrame t_UIContainerFrameCtrl;

struct UIPanel {int x;};
typedef struct UIPanel t_UIPanelCtrl;

struct UIPagePanel {int x;};
typedef struct UIPagePanel t_UIPagePanelCtrl;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void UIExit(int ExitCode);  /* DEBUG PAUL: Not part of UIControl */

/* Menu */
void UIEnableMenu(e_UIMenuCtrl *Menu,bool Enable);
void UICheckMenu(e_UIMenuCtrl *Menu,bool Checked);
bool UIGetMenuCheckStatus(e_UIMenuCtrl *Menu);
void UISetMenuKeySeq(e_UIMenuCtrl *Menu,uint8_t Mod,e_UIKeys Key,char Letter);
void UISetMenuLabel(e_UIMenuCtrl *Menu,const char *NewLabel);

/* UIContextMenuCtrl */
void UIEnableContextMenu(t_UIContextMenuCtrl *Menu,bool Enable);
void UICheckContextMenu(t_UIContextMenuCtrl *Menu,bool Checked);
bool UIGetContextMenuCheckStatus(t_UIContextMenuCtrl *Menu);
void UISetContextMenuLabel(t_UIContextMenuCtrl *Menu,const char *NewLabel);
void UISetContextMenuVisible(t_UIContextMenuCtrl *Menu,bool Show);

/* Checkbox */
void UIEnableCheckbox(t_UICheckboxCtrl *Checkbox,bool Enable);
void UICheckCheckbox(t_UICheckboxCtrl *Checkbox,bool Checked);
bool UIGetCheckboxCheckStatus(t_UICheckboxCtrl *Checkbox);
void UISetCheckboxLabel(t_UICheckboxCtrl *Checkbox,const char *NewLabel);
void UISetCheckboxVisible(t_UICheckboxCtrl *Checkbox,bool Show);
void UICheckboxVisible(t_UICheckboxCtrl *Checkbox,bool Show);

/* Button */
void UIEnableButton(t_UIButtonCtrl *Bttn,bool Enable);
void UISetButtonLabel(t_UIButtonCtrl *Bttn,const char *Text);

/* ComboBox */
void UIEnableComboBox(t_UIComboBoxCtrl *ComboBox,bool Enable);
void UIClearComboBox(t_UIComboBoxCtrl *ComboBox);
void UIAddItem2ComboBox(t_UIComboBoxCtrl *ComboBox,const struct ComboBoxItem *Item);
void UIAddItem2ComboBox(t_UIComboBoxCtrl *ComboBox,const char *Label,uintptr_t ID);
void UIAddItem2ComboBox(t_UIComboBoxCtrl *ComboBox,const std::string &Label,uintptr_t ID);
void UIAddList2ComboBox(t_UIComboBoxCtrl *ComboBox,const t_ComboBoxItemListType &List);
void UIStyleComboBoxItem(t_UIComboBoxCtrl *ComboBox,uintptr_t ID,
        uint32_t Styles);
void UISetComboBoxSelectedEntry(t_UIComboBoxCtrl *ComboBox,uintptr_t ID);
uintptr_t UIGetComboBoxSelectedEntry(t_UIComboBoxCtrl *ComboBox);
int UIGetComboBoxSelectedIndex(t_UIComboBoxCtrl *ComboBox);
void UISetComboBoxItemToolTip(t_UIComboBoxCtrl *ComboBox,uintptr_t ID,
        const char *Tip);
void UIGetComboBoxText(t_UIComboBoxCtrl *ComboBox,std::string &Text);
void UISetComboBoxText(t_UIComboBoxCtrl *ComboBox,const char *Text);
void UISortComboBox(t_UIComboBoxCtrl *ComboBox);

/* ListView */
void UIEnableListView(t_UIListViewCtrl *ListView,bool Enable);
void UIClearListView(t_UIListViewCtrl *ListView);
void UIAddItem2ListView(t_UIListViewCtrl *ListView,struct ListViewItem *Item);
void UIAddItem2ListView(t_UIListViewCtrl *ListView,const char *Label,uintptr_t ID);
void UIAddItem2ListView(t_UIListViewCtrl *ListView,const std::string &Label,uintptr_t ID);
void UIAddList2ListView(t_UIListViewCtrl *ListView,
        const t_ListViewItemListType &List);
void UIStyleListViewItem(t_UIListViewCtrl *ListView,uintptr_t ID,
        uint32_t Styles);
void UISetListViewSelectedEntry(t_UIListViewCtrl *ListView,uintptr_t ID);
uintptr_t UIGetListViewSelectedEntry(t_UIListViewCtrl *ListView);
bool UIListViewHasSelectedEntry(t_UIListViewCtrl *ListView);
void UIClearListViewSelectedEntry(t_UIListViewCtrl *ListView);
void UISetListViewEntryCheckable(t_UIListViewCtrl *ListView,uintptr_t ID,
        bool Checkable);
void UISetListViewEntryCheckedState(t_UIListViewCtrl *ListView,uintptr_t ID,
        bool Checked);
bool UIGetListViewEntryCheckedState(t_UIListViewCtrl *ListView,uintptr_t ID);
void UISetListViewEntryToolTip(t_UIListViewCtrl *ListView,uintptr_t ID,
                const char *Tip);
void UIScrollListViewToBottom(t_UIListViewCtrl *ListView);
void UIReplaceItemInListView(t_UIListViewCtrl *ListView,uintptr_t ID,const char *Label);

/* TabCtrl */
void UIEnableTabCtrl(t_UITabCtrl *TabCtrl,bool Enable);
void UIShowHideTabCtrl(t_UITabCtrl *TabCtrl,bool Show);
t_UITab *UITabCtrlAddTab(t_UITabCtrl *TabCtrl,const char *Name,uintptr_t ID);
void UITabCtrlRemoveTab(t_UITabCtrl *TabCtrl,uintptr_t ID);
void UITabCtrlRemoveTab(t_UITabCtrl *TabCtrl,t_UITab *Tab);
int UITabCtrlGetTabCount(t_UITabCtrl *TabCtrl);
void UITabCtrlClearAllTabs(t_UITabCtrl *TabCtrl);
void UITabCtrlChangeTabID(t_UITabCtrl *TabCtrl,uintptr_t OldID,uintptr_t NewID);
void UITabCtrlSetTabID(t_UITabCtrl *TabCtrl,t_UITab *Tab,uintptr_t NewID);
uintptr_t UITabCtrlGetActiveTabID(t_UITabCtrl *TabCtrl);
t_UITab *UITabCtrlGetActiveTabHandle(t_UITabCtrl *TabCtrl);
void UITabCtrlMakeTabActive(t_UITabCtrl *TabCtrl,uintptr_t ID);
void UITabCtrlMakeTabActive(t_UITabCtrl *TabCtrl,t_UITab *Tab);
void UITabCtrlMakeTabActiveUsingIndex(t_UITabCtrl *TabCtrl,int Index);
int UITabCtrlGetActiveTabIndex(t_UITabCtrl *TabCtrl);
t_UITab *UITabCtrlGetTabFromIndex(t_UITabCtrl *TabCtrl,int Index);
t_UITab *UITabCtrlGetTabFromID(t_UITabCtrl *TabCtrl,uintptr_t ID);
uintptr_t UITabCtrlGetID(t_UITabCtrl *TabCtrl,t_UITab *Tab);
void UITabCtrlSetTabLabel(t_UITabCtrl *TabCtrl,t_UITab *Tab,const char *Label);
void UITabCtrlSetTabVisibleByIndex(t_UITabCtrl *TabCtrl,int Index,bool Hidden);
void UITabCtrlSetTabVisible(t_UITabCtrl *TabCtrl,t_UITab *Tab,bool Hidden);

/* ScrollBarCtrl */
void UIEnableScrollBarCtrl(t_UIScrollBarCtrl *ScrollBarCtrl,bool Enable);
void UIShowHideScrollBarCtrl(t_UIScrollBarCtrl *ScrollBarCtrl,bool Show);
int UIGetScrollBarPos(t_UIScrollBarCtrl *ScrollBarCtrl);
void UISetScrollBarPos(t_UIScrollBarCtrl *ScrollBarCtrl,int NewPos);
void UISetScrollBarTotalSize(t_UIScrollBarCtrl *ScrollBarCtrl,int Max);
void UISetScrollBarPageSize(t_UIScrollBarCtrl *ScrollBarCtrl,int PageSize);
void UISetScrollBarPageSizeAndMax(t_UIScrollBarCtrl *ScrollBarCtrl,int PageSize,int Max);
int UIGetScrollBarTotalSize(t_UIScrollBarCtrl *ScrollBarCtrl);
void UISetScrollBarStepSize(t_UIScrollBarCtrl *ScrollBarCtrl,int StepSize);

/* Number Input */
void UIEnableNumberInputCtrl(t_UINumberInput *NumberInputCtrl,bool Enable);
void UISetNumberInputCtrlValue(t_UINumberInput *NumberInputCtrl,int Number);
long UIGetNumberInputCtrlValue(t_UINumberInput *NumberInputCtrl);
void UISetNumberInputCtrlMin(t_UINumberInput *NumberInputCtrl,int Min);
void UISetNumberInputCtrlMax(t_UINumberInput *NumberInputCtrl,int Max);

/* Double Input */
void UIEnableDoubleInputCtrl(t_UIDoubleInput *DoubleInputCtrl,bool Enable);
void UISetDoubleInputCtrlValue(t_UIDoubleInput *DoubleInputCtrl,double Double);
double UIGetDoubleInputCtrlValue(t_UIDoubleInput *DoubleInputCtrl);
void UISetDoubleInputCtrlMin(t_UIDoubleInput *DoubleInputCtrl,double Min);
void UISetDoubleInputCtrlMax(t_UIDoubleInput *DoubleInputCtrl,double Max);
void UISetDoubleInputCtrlDecimals(t_UIDoubleInput *DoubleInputCtrl,int Points);

/* Toolbar */
void UIEnableToolbar(t_UIToolbarCtrl *Toolbar,bool Enable);
void UICheckToolbar(t_UIToolbarCtrl *Toolbar,bool Checked);
bool UIGetToolbarCheckStatus(t_UIToolbarCtrl *Toolbar);

/* GroupBox */
void UIEnableGroupBox(t_UIGroupBox *GroupBox,bool Enable);
void UIGroupBoxVisible(t_UIGroupBox *GroupBox,bool Show);
void UISetGroupBoxTitle(t_UIGroupBox *GroupBox,const char *Title);

/* RadioBttnCtrl */
void UISelectRadioBttn(t_UIRadioBttnCtrl *RadioBttn);
bool UIIsRadioBttnSelected(t_UIRadioBttnCtrl *RadioBttn);
void UIUnselectRadioBttn(t_UIRadioBttnCtrl *RadioBttn);
void UIEnableRadioBttn(t_UIRadioBttnCtrl *RadioBttn,bool Enable);

/* UITextInputCtrl */
void UIEnableTextCtrl(t_UITextInputCtrl *TextInput,bool Enable);
void UISetTextCtrlText(t_UITextInputCtrl *TextInput,const char *Text);
void UIGetTextCtrlText(t_UITextInputCtrl *TextInput,char *Text,int MaxLen);
void UIGetTextCtrlText(t_UITextInputCtrl *TextInput,std::string &Text);

/* t_UIMuliLineTextInputCtrl */
void UISetMuliLineTextCtrlText(t_UIMuliLineTextInputCtrl *TextInput,const char *Text);
void UIGetMuliLineTextCtrlText(t_UIMuliLineTextInputCtrl *TextInput,char *Text,int MaxLen);
void UIGetMuliLineTextCtrlText(t_UIMuliLineTextInputCtrl *TextInput,std::string &Text);
void UIEnableMuliLineTextCtrl(t_UIMuliLineTextInputCtrl *TextInput,bool Enable);

/* t_UIHTMLViewCtrl */
void UISetHTMLViewCtrlText(t_UIHTMLViewCtrl *HTMLView,const char *HTMLText);

/* UITreeViewCtrl */
void UIClearTreeView(t_UITreeView *Tree);
void UISetTreeViewFoldState(t_UITreeView *Tree,bool Fold);
void UIRemoveTreeItem(t_UITreeItem *Item);
void UISelectTreeItem(t_UITreeItem *Item);
void UIGetTreeItemText(t_UITreeItem *Item,std::string &ItemStr);
void UISetTreeItemText(t_UITreeItem *Item,const char *ItemStr);
void UIClearTreeViewSelection(t_UITreeView *Tree);

/* UILabelCtrl */
void UIEnableLabel(t_UILabelCtrl *Label,bool Enable);
void UISetLabelText(t_UILabelCtrl *Label,const char *Text);
void UISetLabelBackgroundColor(t_UILabelCtrl *Label,uint32_t Color);
void UIMakeLabelVisible(t_UILabelCtrl *Label,bool Visible);

/* UIProgressBar */
void UIEnableProgressBar(t_UIProgressBarCtrl *Bar,bool Enable);
void UISetProgressBarSteps(t_UIProgressBarCtrl *Bar,int Steps);
void UISetProgressBarLevel(t_UIProgressBarCtrl *Bar,int CurrentStep);
void UIProgressBarVisible(t_UIProgressBarCtrl *Bar,bool Show);

/* UIContainerCtrl */
void UIEnableContainerCtrl(t_UIContainerCtrl *Container,bool Enable);

/* UIColorPreviewCtrl */
void UISetColorPreviewColor(t_UIColorPreviewCtrl *CP,uint32_t RGB);

/* UIPanelCtrl */
void UIPanelCtrlEnable(t_UIPanelCtrl *Panel,bool Enable);
void UIPanelCtrlVisible(t_UIPanelCtrl *Panel,bool Show);

/* t_UIPagePanelCtrl */
void UIPagePanelCtrlEnable(t_UIPagePanelCtrl *PagePanel,bool Enable);
void UIPagePanelCtrlVisible(t_UIPagePanelCtrl *PagePanel,bool Show);
void UIPagePanelCtrlSetPage(t_UIPagePanelCtrl *PagePanel,int Page);
int UIPagePanelCtrlGetPage(t_UIPagePanelCtrl *PagePanel);

/* UIColumnViewCtrl */
void UIColumnViewClear(t_UIColumnView *ColumnView);
void UIColumnViewRemoveRow(t_UIColumnView *ColumnView,int Row);
int UIColumnViewAddRow(t_UIColumnView *ColumnView);
void UIColumnViewSetColumnText(t_UIColumnView *ColumnView,int Column,int Row,const char *Str);
void UIColumnViewSelectRow(t_UIColumnView *ColumnView,int Row);
void UIColumnViewClearSelection(t_UIColumnView *ColumnView);
bool UIColumnViewHasSelectedEntry(t_UIColumnView *ColumnView);
int UIGetColumnViewSelectedEntry(t_UIColumnView *ColumnView);

#endif   /* end of "#ifndef __UICONTROL_H_" */
