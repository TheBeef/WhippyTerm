/*******************************************************************************
 * FILENAME: Form_ManBookmarkAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
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
 *    Paul Hutchinson (23 Nov 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_ManBookmark.h"
#include "main.h"
#include "ui_Form_ManBookmark.h"
#include "UI/UIManBookmark.h"
#include <QTreeWidgetItem>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

class Form_ManBookmark *g_ManBookmarkDialog;

/*******************************************************************************
 * NAME:
 *    UIAlloc_ManBookmark
 *
 * SYNOPSIS:
 *    bool UIAlloc_ManBookmark(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the manage bookmarks dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_ManBookmark(void)
{
    try
    {
        g_ManBookmarkDialog=new Form_ManBookmark(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_ManBookmarkDialog=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_ManBookmark
 *
 * SYNOPSIS:
 *    bool UIShow_ManBookmark(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the new connection dialog that was allocated.
 *    You can only have 1 new connection dialog active at a time.
 *
 *    This is a blocking call.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_ManBookmark(void)
{
    return g_ManBookmarkDialog->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_ManBookmark
 *
 * SYNOPSIS:
 *    void UIFree_ManBookmark(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the current new connection dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_ManBookmark(void)
{
    delete g_ManBookmarkDialog;

    g_ManBookmarkDialog=NULL;
}

t_UIButtonCtrl *UIDMB_GetButton(e_UIDMB_Button bttn)
{
    switch(bttn)
    {
        case e_UIDMB_Button_Rename:
            return (t_UIButtonCtrl *)g_ManBookmarkDialog->ui->Rename_pushButton;
        case e_UIDMB_Button_Delete:
            return (t_UIButtonCtrl *)g_ManBookmarkDialog->ui->Delete_pushButton;
        case e_UIDMB_Button_Options:
            return (t_UIButtonCtrl *)g_ManBookmarkDialog->ui->Options_pushButton;
        case e_UIDMB_Button_Settings:
            return (t_UIButtonCtrl *)g_ManBookmarkDialog->ui->Settings_pushButton;
        case e_UIDMB_ButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}

t_UITextInputCtrl *UIDMB_GetTextInput(e_UIDMB_TextInput Input)
{
    switch(Input)
    {
        case e_UIDMB_TextInput_BookmarkName:
            return (t_UITextInputCtrl *)g_ManBookmarkDialog->ui->BookmarkName_lineEdit;
        case e_UIDMB_TextInput_URI:
            return (t_UITextInputCtrl *)g_ManBookmarkDialog->ui->URI_lineEdit;
        case e_UIDMB_TextInputMAX:
        default:
            return NULL;
    }
    return NULL;
}

t_UITreeView *UIDMB_GetTreeView(e_UIDMB_TreeView TreeView)
{
    switch(TreeView)
    {
        case e_UIDMB_TreeView_Folders:
            return (t_UITreeView *)g_ManBookmarkDialog->ui->Folder_treeWidget;
        case e_UIDMB_TreeViewMAX:
        default:
            return NULL;
    }
    return NULL;
}

t_UICheckboxCtrl *UIDMB_GetCheckbox(e_UIDMB_CheckboxType Checkbox)
{
    switch(Checkbox)
    {
        case e_UIDMB_Checkbox_UseGlobalSettings:
            return (t_UICheckboxCtrl *)g_ManBookmarkDialog->ui->GlobalSettings_checkBox;
        case e_UIDMB_CheckboxMAX:
        default:
            return NULL;
    }
    return NULL;
}

t_UITreeItem *UIDMB_AddFolderName(const char *Name)
{
    QTreeWidgetItem *NewItem;

    try
    {
        NewItem=new QTreeWidgetItem(g_ManBookmarkDialog->ui->Folder_treeWidget);
        NewItem->setText(0,Name);
        g_ManBookmarkDialog->ui->Folder_treeWidget->addTopLevelItem(NewItem);
    }
    catch(...)
    {
        return NULL;
    }
    return (t_UITreeItem *)NewItem;
}

t_UITreeItem *UIDMB_AddBookmarkEntry(t_UITreeItem *Parent,const char *Name)
{
    QTreeWidgetItem *NewItem;
    QTreeWidgetItem *ParentItem;

    try
    {
        ParentItem=(QTreeWidgetItem *)Parent;

        NewItem=new QTreeWidgetItem(ParentItem);
        NewItem->setText(0,Name);
        ParentItem->addChild(NewItem);
    }
    catch(...)
    {
        return NULL;
    }
    return (t_UITreeItem *)NewItem;
}

t_UITreeItem *UIDMB_GetSelectedFolderItem(void)
{
    return (t_UITreeItem *)g_ManBookmarkDialog->ui->Folder_treeWidget->currentItem();
}

void UIDMB_SetBookmarkName(const char *Name)
{
    g_ManBookmarkDialog->ui->BookmarkName_lineEdit->setText(Name);
}

void UIDMB_GetBookmarkName(std::string &Name)
{
    Name=g_ManBookmarkDialog->ui->BookmarkName_lineEdit->text().toStdString();
}

void UIDMB_SetURIInput(const char *URI)
{
    g_ManBookmarkDialog->ui->URI_lineEdit->setText(URI);
}

void UIDMB_GetURIInput(std::string &URI)
{
    URI=g_ManBookmarkDialog->ui->URI_lineEdit->text().toStdString();
}

