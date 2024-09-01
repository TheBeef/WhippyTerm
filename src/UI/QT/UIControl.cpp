/*******************************************************************************
 * FILENAME:
 *    UIControl.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has GUI control functions in it.
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
 * CREATED BY:
 *    Paul Hutchinson (05 Jul 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UIControl.h"
#include "QTKeyMappings.h"
#include <QAction>
#include <QCheckBox>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QScrollBar>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QProgressBar>
#include <QFormLayout>
#include <QApplication>
#include <QPalette>
#include <QWidget>
#include <QStackedWidget>
#include <QtGlobal>

//include <stdio.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*************************************/
/* Menus                             */
/*************************************/
void UIEnableMenu(e_UIMenuCtrl *Menu,bool Enable)
{
    QAction *action=(QAction *)Menu;

    action->setEnabled(Enable);
}

void UICheckMenu(e_UIMenuCtrl *Menu,bool Checked)
{
    QAction *action=(QAction *)Menu;

    action->setChecked(Checked);
}

bool UIGetMenuCheckStatus(e_UIMenuCtrl *Menu)
{
    QAction *action=(QAction *)Menu;

    return action->isChecked();
}

void UISetMenuLabel(e_UIMenuCtrl *Menu,const char *NewLabel)
{
    QAction *action=(QAction *)Menu;

    action->setText(NewLabel);
}

void UISetMenuKeySeq(e_UIMenuCtrl *Menu,uint8_t Mod,e_UIKeys Key,char Letter)
{
    QAction *action=(QAction *)Menu;
    int QKey;
    QKeySequence ShortCut;

    if(Key==e_UIKeysMAX && Letter==0)
    {
        /* Clear it */
        action->setShortcut(QKeySequence());
    }
    else
    {
        QKey=ConvertUIKey2QTKey(Key);

        if(QKey==Qt::Key_unknown)
        {
            if(Letter>='a' && Letter<'z')
            {
                /* Uppercase only */
                Letter-=32;
            }
            if(Letter>=' ' && Letter<127)
            {
                /* Use Letter */
                QKey=Letter;
            }
            else
            {
                /* Can't do it */
                return;
            }
        }

        if(Mod&KEYMOD_SHIFT)
            QKey+=Qt::SHIFT;
        if(Mod&KEYMOD_CONTROL)
            QKey+=Qt::CTRL;
        if(Mod&KEYMOD_ALT)
            QKey+=Qt::ALT;
        if(Mod&KEYMOD_LOGO)
            QKey+=Qt::META;

        action->setShortcut(QKeySequence(QKey));
    }
}

/*************************************/
/* Checkbox                          */
/*************************************/

void UIEnableCheckbox(t_UICheckboxCtrl *Checkbox,bool Enable)
{
    QCheckBox *cb=(QCheckBox *)Checkbox;

    cb->setEnabled(Enable);
}

void UICheckCheckbox(t_UICheckboxCtrl *Checkbox,bool Checked)
{
    QCheckBox *cb=(QCheckBox *)Checkbox;

    cb->setChecked(Checked);
}

bool UIGetCheckboxCheckStatus(t_UICheckboxCtrl *Checkbox)
{
    QCheckBox *cb=(QCheckBox *)Checkbox;

    return cb->isChecked();
}

void UISetCheckboxLabel(t_UICheckboxCtrl *Checkbox,const char *NewLabel)
{
    QCheckBox *cb=(QCheckBox *)Checkbox;

    cb->setText(NewLabel);
}

void UISetCheckboxVisible(t_UICheckboxCtrl *Checkbox,bool Show)
{
    QCheckBox *cb=(QCheckBox *)Checkbox;

    cb->setVisible(Show);
}

/*************************************/
/* ComboBox                          */
/*************************************/
void UIEnableComboBox(t_UIComboBoxCtrl *ComboBox,bool Enable)
{
    QComboBox *box=(QComboBox *)ComboBox;

    box->setEnabled(Enable);
}

void UIClearComboBox(t_UIComboBoxCtrl *ComboBox)
{
    QComboBox *box=(QComboBox *)ComboBox;

    return box->clear();
}

void UIAddItem2ComboBox(t_UIComboBoxCtrl *ComboBox,const struct ComboBoxItem *Item)
{
    QComboBox *box=(QComboBox *)ComboBox;

    try
    {
        box->blockSignals(true);
        box->addItem(Item->Label.c_str(),(qulonglong)Item->ID);
    }
    catch(...)
    {
    }
    box->blockSignals(false);
}

void UIAddItem2ComboBox(t_UIComboBoxCtrl *ComboBox,const char *Label,uintptr_t ID)
{
    QComboBox *box=(QComboBox *)ComboBox;

    try
    {
        box->blockSignals(true);
        box->addItem(Label,(qulonglong)ID);
    }
    catch(...)
    {
    }
    box->blockSignals(false);
}

void UIAddItem2ComboBox(t_UIComboBoxCtrl *ComboBox,const std::string &Label,
        uintptr_t ID)
{
    QComboBox *box=(QComboBox *)ComboBox;

    try
    {
        box->blockSignals(true);
        box->addItem(Label.c_str(),(qulonglong)ID);
    }
    catch(...)
    {
    }
    box->blockSignals(false);
}

void UIAddList2ComboBox(t_UIComboBoxCtrl *ComboBox,
        const t_ComboBoxItemListType &List)
{
    QComboBox *box=(QComboBox *)ComboBox;
    ci_ComboBoxItemListType Item;

    try
    {
        box->blockSignals(true);

        for(Item=List.begin();Item!=List.end();Item++)
            box->addItem(Item->Label.c_str(),(qulonglong)Item->ID);
    }
    catch(...)
    {
    }
    box->blockSignals(false);
}

void UIStyleComboBoxItem(t_UIComboBoxCtrl *ComboBox,uintptr_t ID,
        uint32_t Styles)
{
    QComboBox *box=(QComboBox *)ComboBox;
    int Index;   // The index of the entry with 'ID' 
    QFont font;

    /* Find the entry with this ID */
    Index=box->findData((qulonglong)ID);
    if(Index<0)
        return;

    if(Styles&UISTYLE_UNDERLINE)
        font.setUnderline(true);
    if(Styles&UISTYLE_STRIKETHROUGH)
        font.setStrikeOut(true);
    if(Styles&UISTYLE_ITALIC)
        font.setItalic(true);
    if(Styles&UISTYLE_BOLD)
        font.setBold(true);

    box->setItemData(Index,QVariant(font), Qt::FontRole);
}

void UISetComboBoxSelectedEntry(t_UIComboBoxCtrl *ComboBox,uintptr_t ID)
{
    QComboBox *box=(QComboBox *)ComboBox;
    int Index;   // The index of the entry with 'ID' 

    /* Find the entry with this ID */
    Index=box->findData((qulonglong)ID);
    if(Index<0)
        return;

    try
    {
        box->blockSignals(true);
        box->setCurrentIndex(Index);
    }
    catch(...)
    {
    }
    box->blockSignals(false);
}

uintptr_t UIGetComboBoxSelectedEntry(t_UIComboBoxCtrl *ComboBox)
{
    QComboBox *box=(QComboBox *)ComboBox;
    int CurrentIndex;   // The currently selected index

    CurrentIndex=box->currentIndex();

    return (uintptr_t)box->itemData(CurrentIndex).toULongLong();
}

void UISetComboBoxItemToolTip(t_UIComboBoxCtrl *ComboBox,uintptr_t ID,
        const char *Tip)
{
    QComboBox *box=(QComboBox *)ComboBox;
    int Index;   // The index of the entry with 'ID' 

    /* Find the entry with this ID */
    Index=box->findData((qulonglong)ID);
    if(Index<0)
        return;

    box->setItemData(Index,Tip,Qt::ToolTipRole);
}

void UIGetComboBoxText(t_UIComboBoxCtrl *ComboBox,std::string &Text)
{
    QComboBox *box=(QComboBox *)ComboBox;

    try
    {
        if(!box->isEditable())
        {
            Text="";
        }
        else
        {
            Text=qPrintable(box->currentText());
        }
    }
    catch(...)
    {
    }
}

void UISetComboBoxText(t_UIComboBoxCtrl *ComboBox,const char *Text)
{
    QComboBox *box=(QComboBox *)ComboBox;

    try
    {
        box->blockSignals(true);

        if(box->isEditable())
            box->setEditText(Text);
    }
    catch(...)
    {
    }
    box->blockSignals(false);
}

void UISortComboBox(t_UIComboBoxCtrl *ComboBox)
{
    QComboBox *box=(QComboBox *)ComboBox;

    box->model()->sort(0);
}

/*************************************/
/* ListViewCtrl                      */
/*************************************/
void UIEnableListView(t_UIListViewCtrl *ListView,bool Enable)
{
    QListWidget *lv=(QListWidget *)ListView;

    lv->setEnabled(Enable);
}

void UIClearListView(t_UIListViewCtrl *ListView)
{
    QListWidget *lv=(QListWidget *)ListView;

    lv->clear();
}

void UIAddItem2ListView(t_UIListViewCtrl *ListView,struct ListViewItem *Item)
{
    QListWidget *lv=(QListWidget *)ListView;
    QListWidgetItem *NewQItem;

    NewQItem=new QListWidgetItem(Item->Label.c_str());
    NewQItem->setData(Qt::UserRole,(qulonglong)Item->ID);

    try
    {
        lv->blockSignals(true);

        lv->addItem(NewQItem);
    }
    catch(...)
    {
    }
    lv->blockSignals(false);
}

void UIAddItem2ListView(t_UIListViewCtrl *ListView,const char *Label,uintptr_t ID)
{
    QListWidget *lv=(QListWidget *)ListView;
    QListWidgetItem *NewQItem;

    NewQItem=new QListWidgetItem(Label);
    NewQItem->setData(Qt::UserRole,(qulonglong)ID);

    try
    {
        lv->blockSignals(true);

        lv->addItem(NewQItem);
    }
    catch(...)
    {
    }
    lv->blockSignals(false);
}

void UIAddItem2ListView(t_UIListViewCtrl *ListView,const std::string &Label,uintptr_t ID)
{
    QListWidget *lv=(QListWidget *)ListView;
    QListWidgetItem *NewQItem;

    NewQItem=new QListWidgetItem(Label.c_str());
    NewQItem->setData(Qt::UserRole,(qulonglong)ID);

    try
    {
        lv->blockSignals(true);

        lv->addItem(NewQItem);
    }
    catch(...)
    {
    }
    lv->blockSignals(false);
}

void UIAddList2ListView(t_UIListViewCtrl *ListView,
        const t_ListViewItemListType &List)
{
    QListWidget *lv=(QListWidget *)ListView;
    ci_ListViewItemListType Item;
    QListWidgetItem *NewQItem;

    try
    {
        lv->blockSignals(true);

        for(Item=List.begin();Item!=List.end();Item++)
        {
            NewQItem=new QListWidgetItem(Item->Label.c_str());
            NewQItem->setData(Qt::UserRole,(qulonglong)Item->ID);

            lv->addItem(NewQItem);
        }
    }
    catch(...)
    {
    }
    lv->blockSignals(false);
}

void UIReplaceItemInListView(t_UIListViewCtrl *ListView,uintptr_t ID,const char *Label)
{
    QListWidget *lv=(QListWidget *)ListView;
    int Index;   // The index of the entry with 'ID' 

    /* Find the entry with this ID */
    for(Index=0;Index<lv->count();Index++)
        if((lv->item(Index)->data(Qt::UserRole))==(qulonglong)ID)
            break;
    if(Index==lv->count())
        return;

    lv->item(Index)->setText(QString::fromUtf8(Label));
}

void UIStyleListViewItem(t_UIListViewCtrl *ListView,uintptr_t ID,
        uint32_t Styles)
{
    QListWidget *lv=(QListWidget *)ListView;
    int Index;   // The index of the entry with 'ID' 
    QFont font;

    /* Find the entry with this ID */
    for(Index=0;Index<lv->count();Index++)
        if((lv->item(Index)->data(Qt::UserRole))==(qulonglong)ID)
            break;
    if(Index==lv->count())
        return;

    if(Styles&UISTYLE_UNDERLINE)
        font.setUnderline(true);
    if(Styles&UISTYLE_STRIKETHROUGH)
        font.setStrikeOut(true);
    if(Styles&UISTYLE_ITALIC)
        font.setItalic(true);
    if(Styles&UISTYLE_BOLD)
        font.setBold(true);

    lv->item(Index)->setData(Qt::FontRole,QVariant(font));
}

void UISetListViewSelectedEntry(t_UIListViewCtrl *ListView,uintptr_t ID)
{
    QListWidget *lv=(QListWidget *)ListView;
    int Index;   // The index of the entry with 'ID' 

    /* Find the entry with this ID */
    for(Index=0;Index<lv->count();Index++)
        if((lv->item(Index)->data(Qt::UserRole))==(qulonglong)ID)
            break;
    if(Index==lv->count())
        return;

    lv->setCurrentRow(Index);
}

void UIClearListViewSelectedEntry(t_UIListViewCtrl *ListView)
{
    QListWidget *lv=(QListWidget *)ListView;

    lv->clearSelection();
}

uintptr_t UIGetListViewSelectedEntry(t_UIListViewCtrl *ListView)
{
    QListWidget *lv=(QListWidget *)ListView;

    return (uintptr_t)(lv->item(lv->currentRow())->data(Qt::UserRole).toULongLong());
}

bool UIListViewHasSelectedEntry(t_UIListViewCtrl *ListView)
{
    QListWidget *lv=(QListWidget *)ListView;

    return lv->currentItem()!=NULL;
}

void UISetListViewEntryCheckable(t_UIListViewCtrl *ListView,uintptr_t ID,
        bool Checkable)
{
    QListWidget *lv=(QListWidget *)ListView;
    int Index;   // The index of the entry with 'ID' 

    /* Find the entry with this ID */
    for(Index=0;Index<lv->count();Index++)
        if((lv->item(Index)->data(Qt::UserRole))==(qulonglong)ID)
            break;
    if(Index==lv->count())
        return;

    lv->item(Index)->setFlags(lv->item(Index)->flags()|Qt::ItemIsUserCheckable);
    lv->item(Index)->setCheckState(Qt::Unchecked);
}

void UISetListViewEntryCheckedState(t_UIListViewCtrl *ListView,uintptr_t ID,
        bool Checked)
{
    QListWidget *lv=(QListWidget *)ListView;
    int Index;   // The index of the entry with 'ID' 

    /* Find the entry with this ID */
    for(Index=0;Index<lv->count();Index++)
        if((lv->item(Index)->data(Qt::UserRole))==(qulonglong)ID)
            break;
    if(Index==lv->count())
        return;

    lv->item(Index)->setCheckState(Checked?Qt::Checked:Qt::Unchecked);
}

bool UIGetListViewEntryCheckedState(t_UIListViewCtrl *ListView,uintptr_t ID)
{
    QListWidget *lv=(QListWidget *)ListView;
    int Index;   // The index of the entry with 'ID' 

    /* Find the entry with this ID */
    for(Index=0;Index<lv->count();Index++)
        if((lv->item(Index)->data(Qt::UserRole))==(qulonglong)ID)
            break;
    if(Index==lv->count())
        return false;

    return (lv->item(Index)->checkState()==Qt::Checked)?true:false;
}

void UISetListViewEntryToolTip(t_UIListViewCtrl *ListView,uintptr_t ID,
        const char *Tip)
{
    QListWidget *lv=(QListWidget *)ListView;
    int Index;   // The index of the entry with 'ID' 

    /* Find the entry with this ID */
    for(Index=0;Index<lv->count();Index++)
        if((lv->item(Index)->data(Qt::UserRole))==(qulonglong)ID)
            break;
    if(Index==lv->count())
        return;

    lv->item(Index)->setToolTip(Tip);
}

void UIScrollListViewToBottom(t_UIListViewCtrl *ListView)
{
    QListWidget *lv=(QListWidget *)ListView;
    QListWidgetItem *LastEntry;
    int TotalRows;

    TotalRows=lv->count();
    if(TotalRows>0)
    {
        LastEntry=lv->item(TotalRows-1);
        lv->scrollToItem(LastEntry);
    }
}

/*************************************/
/* ButtonCtrl                        */
/*************************************/
void UIEnableButton(t_UIButtonCtrl *Bttn,bool Enable)
{
    QPushButton *PButton=(QPushButton *)Bttn;

    PButton->setEnabled(Enable);
}

void UISetButtonLabel(t_UIButtonCtrl *Bttn,const char *Text)
{
    QPushButton *PButton=(QPushButton *)Bttn;

    PButton->setText(Text);
}

/*************************************/
/* TabCtrl                           */
/*************************************/
void UIEnableTabCtrl(t_UITabCtrl *TabCtrl,bool Enable)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

    tabc->setEnabled(Enable);
}

void UIShowHideTabCtrl(t_UITabCtrl *TabCtrl,bool Show)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

    tabc->setVisible(Show);
}

t_UITab *UITabCtrlAddTab(t_UITabCtrl *TabCtrl,const char *Name,uintptr_t ID)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;
    QWidget *NewTab;

    NewTab = new QWidget();
    NewTab->setObjectName(QString::number(ID));

    tabc->addTab(NewTab,QString(QString::fromUtf8(Name)));

    return (t_UITab *)NewTab;
}

void UITabCtrlRemoveTab(t_UITabCtrl *TabCtrl,uintptr_t ID)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;
    QWidget *Tab;

    Tab=tabc->findChild<QWidget *>(QString::number(ID));
    if(Tab==NULL)
        return;

    tabc->removeTab(tabc->indexOf(Tab));

    delete Tab;
}

void UITabCtrlRemoveTab(t_UITabCtrl *TabCtrl,t_UITab *Tab)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;
    QWidget *tabwidget=(QWidget *)Tab;

    tabc->removeTab(tabc->indexOf(tabwidget));

    delete tabwidget;
}

int UITabCtrlGetTabCount(t_UITabCtrl *TabCtrl)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

    return tabc->count();
}

void UITabCtrlClearAllTabs(t_UITabCtrl *TabCtrl)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;
    QWidget *Tab;

    while(tabc->count()>0)
    {
        Tab=tabc->widget(0);
        tabc->removeTab(0);
        if(Tab!=NULL)
            delete Tab;
    }
}

void UITabCtrlChangeTabID(t_UITabCtrl *TabCtrl,uintptr_t OldID,uintptr_t NewID)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;
    QWidget *Tab;

    Tab=tabc->findChild<QWidget *>(QString::number(OldID));
    if(Tab==NULL)
        return;

    Tab->setObjectName(QString::number(NewID));
}

void UITabCtrlSetTabID(t_UITabCtrl *TabCtrl,t_UITab *Tab,uintptr_t NewID)
{
//    QTabWidget *tabc=(QTabWidget *)TabCtrl;
    QWidget *tabwidget=(QWidget *)Tab;

    tabwidget->setObjectName(QString::number(NewID));
}

uintptr_t UITabCtrlGetActiveTabID(t_UITabCtrl *TabCtrl)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

    if(tabc->currentWidget()==NULL)
        return 0;

    return (uintptr_t)tabc->currentWidget()->objectName().toULongLong();
}

void UITabCtrlMakeTabActive(t_UITabCtrl *TabCtrl,uintptr_t ID)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;
    QWidget *Tab;

    Tab=tabc->findChild<QWidget *>(QString::number(ID));
    if(Tab==NULL)
        return;

    tabc->setCurrentWidget(Tab);
}

void UITabCtrlMakeTabActive(t_UITabCtrl *TabCtrl,t_UITab *Tab)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;
    QWidget *tabwidget=(QWidget *)Tab;

    tabc->setCurrentWidget(tabwidget);
}

void UITabCtrlMakeTabActiveUsingIndex(t_UITabCtrl *TabCtrl,int Index)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

    tabc->setCurrentIndex(Index);
}

/* -1 if none */
int UITabCtrlGetActiveTabIndex(t_UITabCtrl *TabCtrl)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

    return (int)tabc->currentIndex();
}

t_UITab *UITabCtrlGetTabFromIndex(t_UITabCtrl *TabCtrl,int Index)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

    if(Index>=tabc->count())
        return NULL;

    return (t_UITab *)tabc->widget(Index);
}

t_UITab *UITabCtrlGetTabFromID(t_UITabCtrl *TabCtrl,uintptr_t ID)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;
    QWidget *Tab;

    Tab=tabc->findChild<QWidget *>(QString::number(ID));
    if(Tab==NULL)
        return NULL;

    return (t_UITab *)Tab;
}

uintptr_t UITabCtrlGetID(t_UITabCtrl *TabCtrl,t_UITab *Tab)
{
    QWidget *tabwidget=(QWidget *)Tab;

    return (uintptr_t)tabwidget->objectName().toULongLong();
}

t_UITab *UITabCtrlGetActiveTabHandle(t_UITabCtrl *TabCtrl)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

    if(tabc->currentWidget()==NULL)
        return 0;

    return (t_UITab *)tabc->currentWidget();
}

void UITabCtrlSetTabLabel(t_UITabCtrl *TabCtrl,t_UITab *Tab,const char *Label)
{
    QWidget *tabwidget=(QWidget *)Tab;
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

    tabc->setTabText(tabc->indexOf(tabwidget),Label);
}

void UITabCtrlSetTabVisibleByIndex(t_UITabCtrl *TabCtrl,int Index,bool Show)
{
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    tabc->setTabVisible(Index,Show);       // QT 5.15+
#else
    tabc->setTabEnabled(Index,Show);
    if(Show)
        tabc->setStyleSheet("");
    else
        tabc->setStyleSheet("QTabBar::tab::disabled {width: 0; height: 0; margin: 0; padding: 0; border: none;} ");
#endif
}

void UITabCtrlSetTabVisible(t_UITabCtrl *TabCtrl,t_UITab *Tab,bool Show)
{
    QWidget *tabwidget=(QWidget *)Tab;
    QTabWidget *tabc=(QTabWidget *)TabCtrl;

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    tabc->setTabVisible(tabc->indexOf(tabwidget),Show);        // QT 5.15+
#else
    tabc->setTabEnabled(tabc->indexOf(tabwidget),Show);
    if(Show)
        tabc->setStyleSheet("");
    else
        tabc->setStyleSheet("QTabBar::tab::disabled {width: 0; height: 0; margin: 0; padding: 0; border: none;} ");
#endif
}

/*************************************/
/* ScrollBarCtrl                     */
/*************************************/
void UIEnableScrollBarCtrl(t_UIScrollBarCtrl *ScrollBarCtrl,bool Enable)
{
    QScrollBar  *scrollbar=(QScrollBar *)ScrollBarCtrl;

    scrollbar->setEnabled(Enable);
}

void UIShowHideScrollBarCtrl(t_UIScrollBarCtrl *ScrollBarCtrl,bool Show)
{
    QScrollBar  *scrollbar=(QScrollBar *)ScrollBarCtrl;

    scrollbar->setVisible(Show);
}

int UIGetScrollBarPos(t_UIScrollBarCtrl *ScrollBarCtrl)
{
    QScrollBar  *scrollbar=(QScrollBar *)ScrollBarCtrl;

    return scrollbar->sliderPosition();
}

void UISetScrollBarPos(t_UIScrollBarCtrl *ScrollBarCtrl,int NewPos)
{
    QScrollBar  *scrollbar=(QScrollBar *)ScrollBarCtrl;

    scrollbar->setSliderPosition(NewPos);
}

//void UISetScrollBarTotalSize(t_UIScrollBarCtrl *ScrollBarCtrl,int Max)
//{
//    QScrollBar  *scrollbar=(QScrollBar *)ScrollBarCtrl;
//    int PageSize;
//    int NewValue;
//
//    PageSize=scrollbar->pageStep();
//
//    if(Max<=PageSize)
//    {
//        /* Disable the widget */
//        if(scrollbar->isEnabled())
//            scrollbar->setEnabled(false);
//        NewValue=0;
//    }
//    else
//    {
//        NewValue=Max-PageSize;
//        if(!scrollbar->isEnabled())
//            scrollbar->setEnabled(true);
//    }
//
//    scrollbar->setMaximum(NewValue);
//}
//
//void UISetScrollBarPageSize(t_UIScrollBarCtrl *ScrollBarCtrl,int PageSize)
//{
//    QScrollBar  *scrollbar=(QScrollBar *)ScrollBarCtrl;
//    int OldMax;
//    int NewMax;
//
//    OldMax=scrollbar->maximum()+scrollbar->pageStep();
//
//    if(OldMax<=PageSize)
//    {
//        /* Disable the widget */
//        if(scrollbar->isEnabled())
//            scrollbar->setEnabled(false);
//        PageSize=0;
//        NewMax=0;
//    }
//    else
//    {
//        NewMax=OldMax-PageSize;
//        if(!scrollbar->isEnabled())
//            scrollbar->setEnabled(true);
//    }
//
//    scrollbar->setPageStep(PageSize);
//    scrollbar->setMaximum(NewMax);
//}

void UISetScrollBarTotalSize(t_UIScrollBarCtrl *ScrollBarCtrl,int Max)
{
    QScrollBar *scrollbar=(QScrollBar *)ScrollBarCtrl;
    int PageSize;
    int NewValue;

    /* Store the max */
    scrollbar->setProperty("TotalSize",Max);

    PageSize=scrollbar->pageStep();

    if(Max<=PageSize)
    {
        /* Disable the widget */
        if(scrollbar->isEnabled())
            scrollbar->setEnabled(false);
        NewValue=0;
    }
    else
    {
        NewValue=Max-PageSize;
        if(!scrollbar->isEnabled())
            scrollbar->setEnabled(true);
    }

    scrollbar->setMaximum(NewValue);
}

void UISetScrollBarPageSize(t_UIScrollBarCtrl *ScrollBarCtrl,int PageSize)
{
    QScrollBar  *scrollbar=(QScrollBar *)ScrollBarCtrl;
    int Max;
    int NewMax;

    Max=scrollbar->property("TotalSize").toInt();

    if(Max<=PageSize)
    {
        /* Disable the widget */
        if(scrollbar->isEnabled())
            scrollbar->setEnabled(false);
        NewMax=0;
    }
    else
    {
        NewMax=Max-PageSize;
        if(!scrollbar->isEnabled())
            scrollbar->setEnabled(true);
    }

    scrollbar->setPageStep(PageSize);
    scrollbar->setMaximum(NewMax);
}

void UISetScrollBarPageSizeAndMax(t_UIScrollBarCtrl *ScrollBarCtrl,int PageSize,int Max)
{
    QScrollBar  *scrollbar=(QScrollBar *)ScrollBarCtrl;
    int NewMax;

    if(Max<=PageSize)
    {
        /* Disable the widget */
        if(scrollbar->isEnabled())
            scrollbar->setEnabled(false);
        NewMax=0;
    }
    else
    {
        NewMax=Max-PageSize;
        if(!scrollbar->isEnabled())
            scrollbar->setEnabled(true);
    }

    scrollbar->setPageStep(PageSize);
    scrollbar->setMaximum(NewMax);
}

int UIGetScrollBarTotalSize(t_UIScrollBarCtrl *ScrollBarCtrl)
{
    QScrollBar  *scrollbar=(QScrollBar *)ScrollBarCtrl;

    return scrollbar->maximum()+scrollbar->pageStep();
}

void UISetScrollBarStepSize(t_UIScrollBarCtrl *ScrollBarCtrl,int StepSize)
{
    QScrollBar *scrollbar=(QScrollBar *)ScrollBarCtrl;

    scrollbar->setSingleStep(StepSize);
}

/*************************************/
/* NumberInputCtrl                   */
/*************************************/
void UIEnableNumberInputCtrl(t_UINumberInput *NumberInputCtrl,bool Enable)
{
    QSpinBox *spinbox=(QSpinBox *)NumberInputCtrl;

    spinbox->setEnabled(Enable);
}

void UISetNumberInputCtrlValue(t_UINumberInput *NumberInputCtrl,int Number)
{
    QSpinBox *spinbox=(QSpinBox *)NumberInputCtrl;

    try
    {
        spinbox->blockSignals(true);
        spinbox->setValue(Number);
    }
    catch(...)
    {
    }
    spinbox->blockSignals(false);
}

long UIGetNumberInputCtrlValue(t_UINumberInput *NumberInputCtrl)
{
    QSpinBox *spinbox=(QSpinBox *)NumberInputCtrl;

    return spinbox->cleanText().toLong();
}

void UISetNumberInputCtrlMin(t_UINumberInput *NumberInputCtrl,int Min)
{
    QSpinBox *spinbox=(QSpinBox *)NumberInputCtrl;

    try
    {
        spinbox->blockSignals(true);
        spinbox->setMinimum(Min);
    }
    catch(...)
    {
    }
    spinbox->blockSignals(false);
}

void UISetNumberInputCtrlMax(t_UINumberInput *NumberInputCtrl,int Max)
{
    QSpinBox *spinbox=(QSpinBox *)NumberInputCtrl;

    try
    {
        spinbox->blockSignals(true);
        spinbox->setMaximum(Max);
    }
    catch(...)
    {
    }
    spinbox->blockSignals(false);
}

/*************************************/
/* DoubleInputCtrl                   */
/*************************************/
void UIEnableDoubleInputCtrl(t_UIDoubleInput *DoubleInputCtrl,bool Enable)
{
    QDoubleSpinBox *dspinbox=(QDoubleSpinBox *)DoubleInputCtrl;

    dspinbox->setEnabled(Enable);
}

void UISetDoubleInputCtrlValue(t_UIDoubleInput *DoubleInputCtrl,double Double)
{
    QDoubleSpinBox *dspinbox=(QDoubleSpinBox *)DoubleInputCtrl;

    try
    {
        dspinbox->blockSignals(true);
        dspinbox->setValue(Double);
    }
    catch(...)
    {
    }
    dspinbox->blockSignals(false);
}

double UIGetDoubleInputCtrlValue(t_UIDoubleInput *DoubleInputCtrl)
{
    QDoubleSpinBox *dspinbox=(QDoubleSpinBox *)DoubleInputCtrl;

    return dspinbox->value();
}

void UISetDoubleInputCtrlMin(t_UIDoubleInput *DoubleInputCtrl,double Min)
{
    QDoubleSpinBox *dspinbox=(QDoubleSpinBox *)DoubleInputCtrl;

    try
    {
        dspinbox->blockSignals(true);
        dspinbox->setMinimum(Min);
    }
    catch(...)
    {
    }
    dspinbox->blockSignals(false);
}

void UISetDoubleInputCtrlMax(t_UIDoubleInput *DoubleInputCtrl,double Max)
{
    QDoubleSpinBox *dspinbox=(QDoubleSpinBox *)DoubleInputCtrl;

    try
    {
        dspinbox->blockSignals(true);
        dspinbox->setMaximum(Max);
    }
    catch(...)
    {
    }
    dspinbox->blockSignals(false);
}

void UISetDoubleInputCtrlDecimals(t_UIDoubleInput *DoubleInputCtrl,int Points)
{
    QDoubleSpinBox *dspinbox=(QDoubleSpinBox *)DoubleInputCtrl;

    try
    {
        dspinbox->blockSignals(true);
        dspinbox->setDecimals(Points);
    }
    catch(...)
    {
    }
    dspinbox->blockSignals(false);
}

/*************************************/
/* Toolbar                           */
/*************************************/
void UIEnableToolbar(t_UIToolbarCtrl *Toolbar,bool Enable)
{
    QAction *action=(QAction *)Toolbar;

    action->setEnabled(Enable);
}

void UICheckToolbar(t_UIToolbarCtrl *Toolbar,bool Checked)
{
    QAction *action=(QAction *)Toolbar;

    action->setChecked(Checked);
}

bool UIGetToolbarCheckStatus(t_UIToolbarCtrl *Toolbar)
{
    QAction *action=(QAction *)Toolbar;

    return action->isChecked();
}

/*************************************/
/* GroupBoxCtrl                      */
/*************************************/
void UIEnableGroupBox(t_UIGroupBox *GroupBox,bool Enable)
{
    QGroupBox *groupbox=(QGroupBox *)GroupBox;

    groupbox->setEnabled(Enable);
}

void UIGroupBoxVisible(t_UIGroupBox *GroupBox,bool Show)
{
    QGroupBox *groupbox=(QGroupBox *)GroupBox;

    groupbox->setVisible(Show);
}

void UISetGroupBoxTitle(t_UIGroupBox *GroupBox,const char *Title)
{
    QGroupBox *groupbox=(QGroupBox *)GroupBox;

    groupbox->setTitle(Title);
}

/*************************************/
/* RadioBttnCtrl                     */
/*************************************/
void UISelectRadioBttn(t_UIRadioBttnCtrl *RadioBttn)
{
    QRadioButton *radiobttn=(QRadioButton *)RadioBttn;

    radiobttn->setChecked(true);
}

bool UIIsRadioBttnSelected(t_UIRadioBttnCtrl *RadioBttn)
{
    QRadioButton *radiobttn=(QRadioButton *)RadioBttn;

    return radiobttn->isChecked();
}

void UIUnselectRadioBttn(t_UIRadioBttnCtrl *RadioBttn)
{
    QRadioButton *radiobttn=(QRadioButton *)RadioBttn;

    radiobttn->setAutoExclusive(false);
    radiobttn->setChecked(false);
    radiobttn->setAutoExclusive(true);
}

void UIEnableRadioBttn(t_UIRadioBttnCtrl *RadioBttn,bool Enable)
{
    QRadioButton *radiobttn=(QRadioButton *)RadioBttn;

    radiobttn->setEnabled(Enable);
}

/*************************************/
/* UITextInputCtrl                   */
/*************************************/
void UISetTextCtrlText(t_UITextInputCtrl *TextInput,const char *Text)
{
    QLineEdit *textinput=(QLineEdit *)TextInput;

    try
    {
        textinput->blockSignals(true);
        textinput->setText(Text);
    }
    catch(...)
    {
    }
    textinput->blockSignals(false);
}

void UIGetTextCtrlText(t_UITextInputCtrl *TextInput,char *Text,int MaxLen)
{
    QLineEdit *textinput=(QLineEdit *)TextInput;

    strncpy(Text,textinput->text().toUtf8().data(),MaxLen-1);
    Text[MaxLen-1]=0;
}

void UIGetTextCtrlText(t_UITextInputCtrl *TextInput,std::string &Text)
{
    QLineEdit *textinput=(QLineEdit *)TextInput;

    Text=textinput->text().toStdString();
}

void UIEnableTextCtrl(t_UITextInputCtrl *TextInput,bool Enable)
{
    QLineEdit *textinput=(QLineEdit *)TextInput;

    textinput->setEnabled(Enable);
}

/*************************************/
/* t_UIMuliLineTextInputCtrl         */
/*************************************/
void UISetMuliLineTextCtrlText(t_UIMuliLineTextInputCtrl *TextInput,const char *Text)
{
    QTextEdit *textinput=(QTextEdit *)TextInput;

    try
    {
        textinput->blockSignals(true);
        textinput->setText(Text);
    }
    catch(...)
    {
    }
    textinput->blockSignals(false);
}

void UIGetMuliLineTextCtrlText(t_UIMuliLineTextInputCtrl *TextInput,char *Text,int MaxLen)
{
    QTextEdit *textinput=(QTextEdit *)TextInput;

    strncpy(Text,textinput->toPlainText().toUtf8().data(),MaxLen-1);
    Text[MaxLen-1]=0;
}

void UIGetMuliLineTextCtrlText(t_UIMuliLineTextInputCtrl *TextInput,std::string &Text)
{
    QTextEdit *textinput=(QTextEdit *)TextInput;

    Text=textinput->toPlainText().toStdString();
}

void UIEnableMuliLineTextCtrl(t_UIMuliLineTextInputCtrl *TextInput,bool Enable)
{
    QTextEdit *textinput=(QTextEdit *)TextInput;

    textinput->setEnabled(Enable);
}

/*************************************/
/* UITreeViewCtrl                    */
/*************************************/
void UIRemoveTreeItem(t_UITreeItem *Item)
{
    QTreeWidgetItem *TreeItem=(QTreeWidgetItem *)Item;

    delete TreeItem;
}

void UISelectTreeItem(t_UITreeItem *Item)
{
    QTreeWidgetItem *TreeItem=(QTreeWidgetItem *)Item;
    QTreeWidget *TreeWid=TreeItem->treeWidget();

    TreeWid->setCurrentItem(TreeItem);
}

void UIGetTreeItemText(t_UITreeItem *Item,std::string &ItemStr)
{
    QTreeWidgetItem *TreeItem=(QTreeWidgetItem *)Item;

    ItemStr=TreeItem->text(0).toStdString();
}

void UISetTreeItemText(t_UITreeItem *Item,const char *ItemStr)
{
    QTreeWidgetItem *TreeItem=(QTreeWidgetItem *)Item;

    TreeItem->setText(0,ItemStr);
}

void UIClearTreeView(t_UITreeView *Tree)
{
    QTreeWidget *TreeWidget=(QTreeWidget *)Tree;
    TreeWidget->clear();
}

void UIClearTreeViewSelection(t_UITreeView *Tree)
{
    QTreeWidget *TreeWidget=(QTreeWidget *)Tree;

    TreeWidget->setCurrentItem(NULL);
}

void UISetTreeViewFoldState(t_UITreeView *Tree,bool Fold)
{
    QTreeWidget *TreeWidget=(QTreeWidget *)Tree;

    if(Fold)
        TreeWidget->collapseAll();
    else
        TreeWidget->expandAll();
}

/*************************************/
/* UILabelCtrl                       */
/*************************************/
void UIEnableLabel(t_UILabelCtrl *Label,bool Enable)
{
    QLabel *lab=(QLabel *)Label;

    lab->setEnabled(Enable);
}

void UISetLabelText(t_UILabelCtrl *Label,const char *Text)
{
    QLabel *lab=(QLabel *)Label;

    lab->setText(Text);
}

void UISetLabelBackgroundColor(t_UILabelCtrl *Label,uint32_t Color)
{
    QLabel *lab=(QLabel *)Label;
    QPalette NewPalette;

    if(Color==0xFFFFFFFF)
    {
        lab->setAutoFillBackground(false);
    }
    else
    {
        NewPalette.setColor(QPalette::Window, Color);
        lab->setAutoFillBackground(true);
        lab->setPalette(NewPalette);
    }
}

void UIMakeLabelVisible(t_UILabelCtrl *Label,bool Visible)
{
    QLabel *lab=(QLabel *)Label;

    lab->setVisible(Visible);
}

/*************************************/
/* UIProgressBarCtrl                 */
/*************************************/
void UIEnableProgressBar(t_UIProgressBarCtrl *Bar,bool Enable)
{
    QProgressBar *pb=(QProgressBar *)Bar;

    pb->setEnabled(Enable);
}

void UISetProgressBarSteps(t_UIProgressBarCtrl *Bar,int Steps)
{
    QProgressBar *pb=(QProgressBar *)Bar;

    pb->setMaximum(Steps);
}

void UISetProgressBarLevel(t_UIProgressBarCtrl *Bar,int CurrentStep)
{
    QProgressBar *pb=(QProgressBar *)Bar;

    pb->setValue(CurrentStep);
}

void UIProgressBarVisible(t_UIProgressBarCtrl *Bar,bool Show)
{
    QProgressBar *pb=(QProgressBar *)Bar;

    pb->setVisible(Show);
}

/*************************************/
/* UIContainerCtrl                   */
/*************************************/
void UIEnableContainerCtrl(t_UIContainerCtrl *Container,bool Enable)
{
    QFormLayout *fl=(QFormLayout *)Container;
    QWidget *parent;

    parent=fl->parentWidget();

    parent->setEnabled(Enable);
}

/*************************************/
/* UIColorPreviewCtrl                */
/*************************************/
void UISetColorPreviewColor(t_UIColorPreviewCtrl *CP,uint32_t RGB)
{
    QFrame *frame;
    QPalette pal;
    QColor col;

    frame=(QFrame *)CP;

    col.setRgb(RGB);
    pal=QApplication::palette();
    pal.setColor(QPalette::Window,col);

    frame->setPalette(pal);
}

/*************************************/
/* UIPanelCtrl                       */
/*************************************/
void UIPanelCtrlEnable(t_UIPanelCtrl *Panel,bool Enable)
{
    QWidget *wid=(QWidget *)Panel;

    wid->setEnabled(Enable);
}

void UIPanelCtrlVisible(t_UIPanelCtrl *Panel,bool Show)
{
    QWidget *wid=(QWidget *)Panel;

    wid->setVisible(Show);
}

/*************************************/
/* t_UIPagePanelCtrl                 */
/*************************************/
void UIPagePanelCtrlEnable(t_UIPagePanelCtrl *PagePanel,bool Enable)
{
    QStackedWidget *stack=(QStackedWidget *)PagePanel;

    stack->setEnabled(Enable);
}

void UIPagePanelCtrlVisible(t_UIPagePanelCtrl *PagePanel,bool Show)
{
    QStackedWidget *stack=(QStackedWidget *)PagePanel;

    stack->setVisible(Show);
}

void UIPagePanelCtrlSetPage(t_UIPagePanelCtrl *PagePanel,int Page)
{
    QStackedWidget *stack=(QStackedWidget *)PagePanel;

    stack->setCurrentIndex(Page);
}

int UIPagePanelCtrlGetPage(t_UIPagePanelCtrl *PagePanel)
{
    QStackedWidget *stack=(QStackedWidget *)PagePanel;

    return stack->currentIndex();
}

/*************************************/
/* UIColumnViewCtrl                  */
/*************************************/
void UIColumnViewClear(t_UIColumnView *ColumnView)
{
    QTreeWidget *TreeWidget=(QTreeWidget *)ColumnView;
    TreeWidget->clear();
}

void UIColumnViewRemoveRow(t_UIColumnView *ColumnView,int Row)
{
    QTreeWidget *TreeWidget=(QTreeWidget *)ColumnView;
    QTreeWidgetItem *TreeItem;

    TreeItem=TreeWidget->takeTopLevelItem(Row);
    if(TreeItem!=NULL)
        delete TreeItem;
}

int UIColumnViewAddRow(t_UIColumnView *ColumnView)
{
    QTreeWidget *TreeWidget=(QTreeWidget *)ColumnView;
    QTreeWidgetItem *TreeItem;
    int RetValue;

    try
    {
        TreeItem=new QTreeWidgetItem(TreeWidget);

        TreeWidget->addTopLevelItem(TreeItem);

        RetValue=TreeWidget->topLevelItemCount()-1;
    }
    catch(...)
    {
        RetValue=-1;
    }

    return RetValue;
}

void UIColumnViewSetColumnText(t_UIColumnView *ColumnView,int Column,int Row,const char *Str)
{
    QTreeWidget *TreeWidget=(QTreeWidget *)ColumnView;
    QTreeWidgetItem *TreeItem;

    if(Row>=TreeWidget->topLevelItemCount())
        return;

    TreeItem=TreeWidget->topLevelItem(Row);

    TreeItem->setText(Column,Str);
}

void UIColumnViewSelectRow(t_UIColumnView *ColumnView,int Row)
{
    QTreeWidget *TreeWidget=(QTreeWidget *)ColumnView;
    QTreeWidgetItem *TreeItem;

    if(Row>=TreeWidget->topLevelItemCount())
        return;

    TreeItem=TreeWidget->topLevelItem(Row);

    TreeWidget->blockSignals(true);
    TreeWidget->setCurrentItem(TreeItem);
    TreeWidget->blockSignals(false);
}

void UIColumnViewClearSelection(t_UIColumnView *ColumnView)
{
    QTreeWidget *TreeWidget=(QTreeWidget *)ColumnView;

    TreeWidget->blockSignals(true);
    TreeWidget->selectionModel()->clearSelection();
    TreeWidget->blockSignals(false);
}
