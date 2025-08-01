/*******************************************************************************
 * FILENAME: Dialog_ManBookmark.cpp
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
#include "App/Dialogs/Dialog_ConnectionOptions.h"
#include "App/Dialogs/Dialog_ManBookmark.h"
#include "App/Dialogs/Dialog_Settings.h"
#include "UI/UIManBookmark.h"
#include "UI/UIDebug.h"
#include "UI/UIAsk.h"
#include "UI/UITextInputBox.h"
#include <string>
#include <map>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef map<t_UITreeItem *,i_BookmarkList> t_DMB_ItemLookup;
typedef t_DMB_ItemLookup::iterator i_DMB_ItemLookup;

/*** FUNCTION PROTOTYPES      ***/
static void DMB_RebuildFolderTree(void);
static void DMB_ClearSelection(void);
static void DMB_HandleRenameFolder(void);
static void DMB_HandleDeleteItem(void);
static void DMB_HandleBookmarkNameChange(const char *NewName);
static void DMB_ChangeSelectedItemsURIText(const char *Text);
static void DMB_ChangeSelectedItem(t_UITreeItem *Item);
static void DMB_HandleDragAndDropped(t_UITreeItem *ItemDragged,t_UITreeItem *DroppedOn,
        int DropPlacement);
static void DMB_RunSettings(void);
static void DMB_RestoreSettings(void);
static void DMB_DoOptions(void);
static void DMB_HandleUseGlobalSettingsChange(void);
static void DMB_RethinkUseGlobalSettings(void);

/*** VARIABLE DEFINITIONS     ***/
t_DMB_ItemLookup m_DMB_ItemLookup;
t_BookmarkList *m_DMB_EditList;
i_DMB_ItemLookup m_SelectedEntry;

/*******************************************************************************
 * NAME:
 *    RunManBookmarkDialog
 *
 * SYNOPSIS:
 *    bool RunManBookmarkDialog(t_BookmarkList &EditList);
 *
 * PARAMETERS:
 *    EditList [I/O] -- The list of bookmarks to edit.  This list will be
 *                      displayed and the user will make changes to this
 *                      list.
 *
 * FUNCTION:
 *    This function shows the manage bookmark dialog.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunManBookmarkDialog(t_BookmarkList &EditList)
{
    bool RetValue;

    try
    {
        m_DMB_EditList=&EditList;

        if(!UIAlloc_ManBookmark())
            return false;

        m_SelectedEntry=m_DMB_ItemLookup.end();
        DMB_RebuildFolderTree();

        RetValue=UIShow_ManBookmark();
    }
    catch(...)
    {
        RetValue=false;
    }

    UIFree_ManBookmark();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    DMB_ChangeSelectedItem
 *
 * SYNOPSIS:
 *    void DMB_ChangeSelectedItem(t_UITreeItem *Item);
 *
 * PARAMETERS:
 *    Item [I] -- The new item to select
 *
 * FUNCTION:
 *    This function changes what item is currently selected item.  It doesn't
 *    update the UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DMB_ChangeSelectedItem(t_UITreeItem *Item)
{
    t_UIButtonCtrl *RenameBttn;
    t_UITextInputCtrl *URIInput;
    t_UITextInputCtrl *BookmarkNameInput;
    t_UIButtonCtrl *DeleteBttn;
    t_UIButtonCtrl *OptionsBttn;
    t_UIButtonCtrl *SettingsBttn;
    t_UICheckboxCtrl *UseGlobalSettings;

    RenameBttn=UIDMB_GetButton(e_UIDMB_Button_Rename);
    DeleteBttn=UIDMB_GetButton(e_UIDMB_Button_Delete);
    OptionsBttn=UIDMB_GetButton(e_UIDMB_Button_Options);
    SettingsBttn=UIDMB_GetButton(e_UIDMB_Button_Settings);
    BookmarkNameInput=UIDMB_GetTextInput(e_UIDMB_TextInput_BookmarkName);
    URIInput=UIDMB_GetTextInput(e_UIDMB_TextInput_URI);
    UseGlobalSettings=UIDMB_GetCheckbox(e_UIDMB_Checkbox_UseGlobalSettings);

    m_SelectedEntry=m_DMB_ItemLookup.find(Item);
    if(m_SelectedEntry==m_DMB_ItemLookup.end())
    {
        UIEnableButton(RenameBttn,false);
        UIEnableButton(OptionsBttn,false);
        UIEnableButton(SettingsBttn,false);
        UIEnableTextCtrl(BookmarkNameInput,false);
        UIEnableTextCtrl(URIInput,false);
        UISetTextCtrlText(BookmarkNameInput,"");
        UISetTextCtrlText(URIInput,"");
        UIEnableCheckbox(UseGlobalSettings,false);

        return;
    }

    if(m_SelectedEntry->second==m_DMB_EditList->end())
    {
        /* Folder */
        UIEnableButton(RenameBttn,true);
        UIEnableButton(OptionsBttn,false);
        UIEnableButton(SettingsBttn,false);
        UIEnableCheckbox(UseGlobalSettings,false);

        UIEnableTextCtrl(BookmarkNameInput,false);
        UIEnableTextCtrl(URIInput,false);
        UISetTextCtrlText(BookmarkNameInput,"");
        UISetTextCtrlText(URIInput,"");
    }
    else
    {
        /* Bookmark */
        UIEnableButton(RenameBttn,false);
        UIEnableButton(OptionsBttn,true);
        UIEnableCheckbox(UseGlobalSettings,true);

        UIEnableTextCtrl(BookmarkNameInput,true);
        UIEnableTextCtrl(URIInput,true);

        UISetTextCtrlText(BookmarkNameInput,
                m_SelectedEntry->second->Name.c_str());
        UISetTextCtrlText(URIInput,m_SelectedEntry->second->URI.c_str());

        DMB_RethinkUseGlobalSettings();
    }
    UIEnableButton(DeleteBttn,true);
}

/*******************************************************************************
 * NAME:
 *    DMB_HandleDragAndDropped
 *
 * SYNOPSIS:
 *    void DMB_HandleDragAndDropped(t_UITreeItem *ItemDragged,
 *              t_UITreeItem *DroppedOn,int DropPlacement);
 *
 * PARAMETERS:
 *    ItemDragged [I] -- The item that was selected and dragged
 *    DroppedOn [I] -- The place where the drag item was dropped (the
 *                     item it was dropped on).  This maybe NULL if the
 *                     item wasn't dropped on an item.
 *    DropPlacement [I] -- Where on the dropped item was the drag item dropped.
 *                         Supported values:
 *                              TREEVIEW_DROP_ABOVE -- 'ItemDragged' was dropped
 *                                  just above 'DroppedOn'
 *                              TREEVIEW_DROP_BELOW -- 'ItemDragged' was dropped
 *                                  just below 'DroppedOn'
 *                              TREEVIEW_DROP_ON -- 'ItemDragged' was dropped
 *                                  directly on 'DroppedOn'
 *                              TREEVIEW_DROP_WIDGET -- 'ItemDragged' was
 *                                  dropped on the tree widget (in the blank
 *                                  area of the widget).
 *
 * FUNCTION:
 *    This function is called when an item in the tree view is dragged and
 *    dropped in the tree view.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DMB_HandleDragAndDropped(t_UITreeItem *ItemDragged,t_UITreeItem *DroppedOn,
        int DropPlacement)
{
    i_DMB_ItemLookup ItemDraggedBM;
    i_DMB_ItemLookup DroppedOnBM;
    i_BookmarkList bm;
    i_BookmarkList FirstBM;
    i_BookmarkList EndBM;
    i_BookmarkList InsertPoint;
    i_BookmarkList Savedbm;
    string FolderName;
    bool DraggedFolder;
    bool DroppedOnFolder;

    ItemDraggedBM=m_DMB_ItemLookup.find(ItemDragged);
    DroppedOnBM=m_DMB_ItemLookup.find(DroppedOn);

    if(ItemDraggedBM->second==m_DMB_EditList->end())
        DraggedFolder=true;
    else
        DraggedFolder=false;

    if(DroppedOnBM->second==m_DMB_EditList->end())
        DroppedOnFolder=true;
    else
        DroppedOnFolder=false;

    if(DropPlacement==TREEVIEW_DROP_WIDGET)
    {
        if(DraggedFolder)
        {
            /* We need to move all items that have this menu name to the end
               of the list */
            UIGetTreeItemText(ItemDragged,FolderName);

            /* Find the last item */
            EndBM=m_DMB_EditList->end();
            EndBM--;
            for(bm=m_DMB_EditList->begin();bm!=EndBM;)
            {
                if(bm->MenuName==FolderName)
                {
                    Savedbm=bm;
                    bm++;
                    m_DMB_EditList->splice(m_DMB_EditList->end(),
                            *m_DMB_EditList,Savedbm);
                }
                else
                {
                    bm++;
                }
            }
        }
        else
        {
            ItemDraggedBM->second->MenuName="";  // Move to the root

            /* Move this item to the end of the list */
            m_DMB_EditList->splice(m_DMB_EditList->end(),*m_DMB_EditList,
                    ItemDraggedBM->second);
        }
    }
    else
    {
        if(DroppedOn==NULL)
        {
            /* Hu? */
            return;
        }

        if(DraggedFolder)
        {
            if(DroppedOnFolder)
            {
                /* Find the first bookmark with this menu name */
                UIGetTreeItemText(DroppedOn,FolderName);
                for(bm=m_DMB_EditList->begin();bm!=m_DMB_EditList->end();bm++)
                    if(bm->MenuName==FolderName)
                        break;
                if(bm==m_DMB_EditList->end())
                {
                    /* Hu? */
                    return;
                }

                switch(DropPlacement)
                {
                    case TREEVIEW_DROP_ABOVE:
                    case TREEVIEW_DROP_ON:
                        /* Move above this folder */
                        InsertPoint=bm;
                    break;
                    case TREEVIEW_DROP_BELOW:
                    default:
                        return;
                    break;
                }
            }
            else
            {
                InsertPoint=DroppedOnBM->second;
                if(InsertPoint->MenuName!="")
                {
                    /* User just tried to drag a folder into another folder */
                    return;
                }
            }

            UIGetTreeItemText(ItemDragged,FolderName);

            /* Find the the start and end of them folders items */
            FirstBM=m_DMB_EditList->end();
            EndBM=m_DMB_EditList->end();
            for(bm=m_DMB_EditList->begin();bm!=m_DMB_EditList->end();bm++)
            {
                if(bm->MenuName==FolderName)
                {
                    if(FirstBM==m_DMB_EditList->end())
                        FirstBM=bm;
                    EndBM=bm;
                    EndBM++;    // Doesn't copy the last item so we need to move 1 past
                }
            }

            /* Move the whole thing to the new insert point */
            m_DMB_EditList->splice(InsertPoint,*m_DMB_EditList,FirstBM,EndBM);
        }
        else
        {
            if(DroppedOnFolder)
            {
                UIGetTreeItemText(DroppedOn,FolderName);

                switch(DropPlacement)
                {
                    case TREEVIEW_DROP_ABOVE:
                        /* Move above the folder */
                        /* Find the first item with this folder name */
                        for(bm=m_DMB_EditList->begin();bm!=m_DMB_EditList->end();bm++)
                        {
                            if(bm->MenuName==FolderName)
                                break;
                        }
                        if(bm==m_DMB_EditList->end())
                        {
                            /* Didn't find this folder? */
                            return;
                        }

                        /* Move it just before the folder */
                        m_DMB_EditList->splice(bm,*m_DMB_EditList,
                                ItemDraggedBM->second);

                        /* Add it to the root */
                        ItemDraggedBM->second->MenuName="";
                        
                    break;
                    case TREEVIEW_DROP_ON:
                    case TREEVIEW_DROP_BELOW:
                    default:
                        /* Move to this under this folder */
                        /* Find the last entry in this folder */
                        EndBM=m_DMB_EditList->end();
                        for(bm=m_DMB_EditList->begin();bm!=m_DMB_EditList->end();
                                bm++)
                        {
                            if(bm->MenuName==FolderName)
                                EndBM=bm;
                        }

                        if(EndBM!=m_DMB_EditList->end())
                            EndBM++;    // Insert after the insert point

                        /* Move it to the end of the folder list */
                        m_DMB_EditList->splice(EndBM,*m_DMB_EditList,
                                ItemDraggedBM->second);

                        /* Add it to the folder */
                        ItemDraggedBM->second->MenuName=FolderName;
                    break;
                }
            }
            else
            {
                /* Check if we are going to just insert to the same spot */
                EndBM=ItemDraggedBM->second;
                EndBM++;
                if(EndBM==DroppedOnBM->second &&
                        ItemDraggedBM->second->MenuName==
                        DroppedOnBM->second->MenuName)
                {
                    /* Ok, swap them */
                    m_DMB_EditList->splice(ItemDraggedBM->second,*m_DMB_EditList,
                            DroppedOnBM->second);
                }
                else
                {
                    switch(DropPlacement)
                    {
                        case TREEVIEW_DROP_ABOVE:
                        case TREEVIEW_DROP_ON:
                            /* Move it just before this item */
                            m_DMB_EditList->splice(DroppedOnBM->second,
                                    *m_DMB_EditList,ItemDraggedBM->second);
                        break;
                        case TREEVIEW_DROP_BELOW:
                        default:
                            /* Move it just after this item */
                            InsertPoint=DroppedOnBM->second;
                            InsertPoint++;
                            m_DMB_EditList->splice(InsertPoint,*m_DMB_EditList,
                                    ItemDraggedBM->second);
                        break;
                    }
                }

                /* Make sure they are in the same folder */
                ItemDraggedBM->second->MenuName=DroppedOnBM->second->MenuName;
            }
        }
    }

    /* Clear the lookup and rebuild */
    m_DMB_ItemLookup.clear();

    DMB_RebuildFolderTree();
}

/*******************************************************************************
 * NAME:
 *    DMB_RebuildFolderTree
 *
 * SYNOPSIS:
 *    static void DMB_RebuildFolderTree(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the folder tree and rebuilds it.  It also rebuilds
 *    the lookup map.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMB_RebuildFolderTree(void)
{
    t_UITreeItem *LastFolderItem;
    string LastMenuName;
    t_UITreeItem *AddedItem;
    i_BookmarkList bm;
    t_UITreeView *Folders;

    Folders=UIDMB_GetTreeView(e_UIDMB_TreeView_Folders);
    UIClearTreeView(Folders);
    m_DMB_ItemLookup.clear();

    m_SelectedEntry=m_DMB_ItemLookup.end();

    LastMenuName="";
    LastFolderItem=NULL;
    for(bm=m_DMB_EditList->begin();bm!=m_DMB_EditList->end();bm++)
    {
        if(bm->MenuName!=LastMenuName)
        {
            /* Ok, we are switching menus */
            if(bm->MenuName!="")
            {
                LastFolderItem=UIDMB_AddFolderName(bm->MenuName.c_str());
                m_DMB_ItemLookup.insert(make_pair(LastFolderItem,
                        m_DMB_EditList->end()));
            }

            LastMenuName=bm->MenuName;
        }

        /* Add the bookmark to the list */
        if(bm->MenuName=="")
            AddedItem=UIDMB_AddFolderName(bm->Name.c_str());
        else
            AddedItem=UIDMB_AddBookmarkEntry(LastFolderItem,bm->Name.c_str());
        m_DMB_ItemLookup.insert(make_pair(AddedItem,bm));
    }
    UISetTreeViewFoldState(Folders,false);
    DMB_ClearSelection();
}

/*******************************************************************************
 * NAME:
 *    DMB_ClearSelection
 *
 * SYNOPSIS:
 *    static void DMB_ClearSelection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This funciton clears the selection from the tree view.  It also fixes
 *    up the other inputs.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMB_ClearSelection(void)
{
    t_UIButtonCtrl *RenameBttn;
    t_UIButtonCtrl *DeleteBttn;
    t_UITextInputCtrl *URIInput;
    t_UITextInputCtrl *BookmarkNameInput;
    t_UIButtonCtrl *OptionsBttn;
    t_UIButtonCtrl *SettingsBttn;
    t_UITreeView *Folders;
    t_UICheckboxCtrl *UseGlobalSettings;

    RenameBttn=UIDMB_GetButton(e_UIDMB_Button_Rename);
    DeleteBttn=UIDMB_GetButton(e_UIDMB_Button_Delete);
    OptionsBttn=UIDMB_GetButton(e_UIDMB_Button_Options);
    SettingsBttn=UIDMB_GetButton(e_UIDMB_Button_Settings);
    BookmarkNameInput=UIDMB_GetTextInput(e_UIDMB_TextInput_BookmarkName);
    URIInput=UIDMB_GetTextInput(e_UIDMB_TextInput_URI);
    Folders=UIDMB_GetTreeView(e_UIDMB_TreeView_Folders);
    UseGlobalSettings=UIDMB_GetCheckbox(e_UIDMB_Checkbox_UseGlobalSettings);

    UIEnableButton(RenameBttn,false);
    UIEnableButton(DeleteBttn,false);
    UIEnableButton(OptionsBttn,false);
    UIEnableButton(SettingsBttn,false);
    UIEnableTextCtrl(BookmarkNameInput,false);
    UIEnableTextCtrl(URIInput,false);
    UIEnableCheckbox(UseGlobalSettings,false);
    UISetTextCtrlText(BookmarkNameInput,"");
    UISetTextCtrlText(URIInput,"");
    UIClearTreeViewSelection(Folders);
}

/*******************************************************************************
 * NAME:
 *    DMB_HandleDeleteItem
 *
 * SYNOPSIS:
 *    void DMB_HandleDeleteItem(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is deletes the current selected folder.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DMB_HandleDeleteItem(void)
{
    string FolderName;
    i_BookmarkList bm;
    i_BookmarkList FirstBM;
    i_BookmarkList EndBM;

    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
    {
        /* Deleting a folder */
        if(UIAsk("Warning","Delete this folder and it's contents?",
                e_AskBox_Warning,e_AskBttns_YesNo)==e_AskRet_Yes)
        {
            /* Delete all the bookmarks with this folder name */
            UIGetTreeItemText(m_SelectedEntry->first,FolderName);

            /* Find the the start and end of them folders items */
            FirstBM=m_DMB_EditList->end();
            EndBM=m_DMB_EditList->end();
            for(bm=m_DMB_EditList->begin();bm!=m_DMB_EditList->end();bm++)
            {
                if(bm->MenuName==FolderName)
                {
                    if(FirstBM==m_DMB_EditList->end())
                        FirstBM=bm;
                    EndBM=bm;
                    EndBM++;    // erase doesn't delete this one so we need to move 1 past
                }
            }
            m_DMB_EditList->erase(FirstBM,EndBM);
        }
    }
    else
    {
        /* Just delete the bookmark */
        m_DMB_EditList->erase(m_SelectedEntry->second);
    }
    DMB_RebuildFolderTree();
}

/*******************************************************************************
 * NAME:
 *    DMB_HandleBookmarkNameChange
 *
 * SYNOPSIS:
 *    void DMB_HandleBookmarkNameChange(const char *NewName);
 *
 * PARAMETERS:
 *    NewName [I] -- The name to change the bookmarks name to
 *
 * FUNCTION:
 *    This function changes the currently selected bookmarks name.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DMB_HandleBookmarkNameChange(const char *NewName)
{
    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
        return;

    m_SelectedEntry->second->Name=NewName;
    UISetTreeItemText(m_SelectedEntry->first,NewName);
}

/*******************************************************************************
 * NAME:
 *    DMB_HandleRenameFolder
 *
 * SYNOPSIS:
 *    void DMB_HandleRenameFolder(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles the rename folder function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DMB_HandleRenameFolder(void)
{
    string OldName;
    string NewName;
    i_BookmarkList bm;

    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second!=m_DMB_EditList->end())
        return;

    UIGetTreeItemText(m_SelectedEntry->first,OldName);

    NewName=OldName;
    if(!UITextInputBox("Rename","Rename this folder to",NewName))
        return;

    if(NewName=="")
        return;

    /* Find all menus under the old name and change them */
    for(bm=m_DMB_EditList->begin();bm!=m_DMB_EditList->end();bm++)
        if(bm->MenuName==OldName)
            bm->MenuName=NewName;

    DMB_RebuildFolderTree();
}

/*******************************************************************************
 * NAME:
 *    DMB_ChangeSelectedItemsURIText
 *
 * SYNOPSIS:
 *    void DMB_ChangeSelectedItemsURIText(const char *Text);
 *
 * PARAMETERS:
 *    Text [I] -- The new text for the URI
 *
 * FUNCTION:
 *    This function changes the currently selected item's URI text.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DMB_ChangeSelectedItemsURIText(const char *Text)
{
    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
        return;

    m_SelectedEntry->second->URI=Text;

    /* Now update the options */
    IOS_UpdateOptionsFromURI(Text,m_SelectedEntry->second->Options);
}

/*******************************************************************************
 * NAME:
 *    DMB_Event
 *
 * SYNOPSIS:
 *    bool DMB_Event(const struct DMBEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event we should process.
 *
 * FUNCTION:
 *    This function is called from the UI to tell us there was a UI event.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DMB_Event(const struct DMBEvent *Event)
{
    bool AcceptEvent;

    AcceptEvent=true;
    switch(Event->EventType)
    {
        case e_DMBEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_UIDMB_Button_Rename:
                    DMB_HandleRenameFolder();
                break;
                case e_UIDMB_Button_Delete:
                    DMB_HandleDeleteItem();
                break;
                case e_UIDMB_Button_Options:
                    DMB_DoOptions();
                break;
                case e_UIDMB_Button_Settings:
                    DMB_RunSettings();
                break;
                case e_UIDMB_ButtonMAX:
                default:
                break;
            }
        break;
        case e_DMBEvent_BookmarkNameChanged:
            DMB_HandleBookmarkNameChange(Event->Info.StringChange.NewText);
        break;
        case e_DMBEvent_URITextEditFinished:
            DMB_ChangeSelectedItemsURIText(Event->Info.StringChange.NewText);
        break;
        case e_DMBEvent_SelectedFolderItemChanged:
            DMB_ChangeSelectedItem(Event->Info.Item.NewItem);
        break;
        case e_DMBEvent_FolderDragAndDropped:
            DMB_HandleDragAndDropped(Event->Info.Drag.ItemDragged,
                    Event->Info.Drag.DroppedOn,Event->Info.Drag.DropPlacement);
        break;
        case e_DMBEvent_CheckboxChange:
            switch(Event->Info.Checkbox.BoxID)
            {
                case e_UIDMB_Checkbox_UseGlobalSettings:
                    DMB_HandleUseGlobalSettingsChange();
                break;
                case e_UIDMB_CheckboxMAX:
                default:
                break;
            }
        break;
        case e_DMBEventMAX:
        default:
        break;
    }
    return AcceptEvent;
}

/*******************************************************************************
 * NAME:
 *    DMB_RunSettings
 *
 * SYNOPSIS:
 *    static void DMB_RunSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    Runs the settings dialog to set the connection settings for the selected
 *    bookmark.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMB_RunSettings(void)
{
    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
        return;

    if(RunSettingsDialog(NULL,&m_SelectedEntry->second->CustomSettings,
            e_SettingsJump2_Default))
    {
        m_SelectedEntry->second->UseCustomSettings=true;
    }
}

/*******************************************************************************
 * NAME:
 *    DMB_RestoreSettings
 *
 * SYNOPSIS:
 *    static void DMB_RestoreSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    Asks if the user wants to restore the settings for the current bookmark
 *    and then resets the settings.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMB_RestoreSettings(void)
{
    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
        return;

    if(UIAsk("Reset","Restore this bookmark to global settings?",
            e_AskBox_Question,e_AskBttns_YesNo)==e_AskRet_Yes)
    {
        m_SelectedEntry->second->CustomSettings=g_Settings.DefaultConSettings;
        m_SelectedEntry->second->UseCustomSettings=false;
    }
}

/*******************************************************************************
 * NAME:
 *    DMB_DoOptions
 *
 * SYNOPSIS:
 *    static void DMB_DoOptions(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the connection options for the selected bookmark.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMB_DoOptions(void)
{
    std::string UniqueID;
    t_UITextInputCtrl *URIInput;
    string URI;

    URIInput=UIDMB_GetTextInput(e_UIDMB_TextInput_URI);

    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
        return;

    if(!IOS_GetUniqueIDFromURI(m_SelectedEntry->second->URI.c_str(),UniqueID))
    {
        UIAsk("URI error","Unable to find correct driver for URI",
                e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    if(RunConnectionOptionsDialog(UniqueID,m_SelectedEntry->second->Options))
    {
        if(!IOS_GetURIFromUniqueID(UniqueID.c_str(),
                m_SelectedEntry->second->Options,URI))
        {
            return;
        }
        m_SelectedEntry->second->URI=URI;

        UISetTextCtrlText(URIInput,m_SelectedEntry->second->URI.c_str());
    }
}

/*******************************************************************************
 * NAME:
 *    DMB_HandleUseGlobalSettingsChange
 *
 * SYNOPSIS:
 *    void DMB_HandleUseGlobalSettingsChange(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the use global settings is clicked on.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DMB_HandleUseGlobalSettingsChange(void)
{
    t_UICheckboxCtrl *UseGlobalSettings;

    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
        return;

    UseGlobalSettings=UIDMB_GetCheckbox(e_UIDMB_Checkbox_UseGlobalSettings);

    if(UIGetCheckboxCheckStatus(UseGlobalSettings))
    {
        /* They are turning global settings back on, if they have changed
           the settings then we warn them. */
        if(!AreConSettingsEqual(m_SelectedEntry->second->CustomSettings,
                g_Settings.DefaultConSettings))
        {
            DMB_RestoreSettings();
        }
        else
        {
            /* They haven't changed anything, just turn it off */
            m_SelectedEntry->second->UseCustomSettings=false;
        }
    }
    else
    {
        /* They want to use custom settings, copy the global settings and
           turn it on */
        m_SelectedEntry->second->CustomSettings=g_Settings.DefaultConSettings;
        m_SelectedEntry->second->UseCustomSettings=true;
    }

    DMB_RethinkUseGlobalSettings();
}

/*******************************************************************************
 * NAME:
 *    DMB_RethinkUseGlobalSettings
 *
 * SYNOPSIS:
 *    void DMB_RethinkUseGlobalSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the GUI to reflect the current state of the
 *    selected bookmarks custom settings.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DMB_RethinkUseGlobalSettings(void)
{
    t_UICheckboxCtrl *UseGlobalSettings;
    t_UIButtonCtrl *SettingsBttn;

    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
        return;

    UseGlobalSettings=UIDMB_GetCheckbox(e_UIDMB_Checkbox_UseGlobalSettings);
    SettingsBttn=UIDMB_GetButton(e_UIDMB_Button_Settings);

    /* Set the global use global settings checkbox and enable/disable the
       settings button */
    if(m_SelectedEntry->second->UseCustomSettings)
    {
        UIEnableButton(SettingsBttn,true);
        UICheckCheckbox(UseGlobalSettings,false);
    }
    else
    {
        UIEnableButton(SettingsBttn,false);
        UICheckCheckbox(UseGlobalSettings,true);
    }
}
