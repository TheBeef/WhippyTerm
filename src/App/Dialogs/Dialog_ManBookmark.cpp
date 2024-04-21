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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (23 Nov 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_ManBookmark.h"
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
static void DMB_SetupOptionsInputs(void);
static void DMB_StoreOptionsInputs(void);
static void DMB_ClearOptionsInputs(void);
static void DMB_RebuildFolderTree(void);
static void DMB_ClearSelection(void);
static void DMB_HandleRenameFolder(void);
static void DMB_HandleDeleteItem(void);
static void DMB_HandleBookmarkNameChange(const char *NewName);
static void DMB_ChangeSelectedItemsURIText(const char *Text);
static void DMB_ChangeSelectedItem(t_UITreeItem *Item);
static void DMB_HandleDragAndDropped(t_UITreeItem *ItemDragged,t_UITreeItem *DroppedOn,
        int DropPlacement);
static void DMB_OptionsUIChanged(void *UserData);

/*** VARIABLE DEFINITIONS     ***/
t_DMB_ItemLookup m_DMB_ItemLookup;
t_BookmarkList *m_DMB_EditList;
i_DMB_ItemLookup m_SelectedEntry;
t_ConnectionOptionsDataType *m_DMB_ConOptions;

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

    m_DMB_ConOptions=NULL;
    try
    {
        m_DMB_EditList=&EditList;

        if(!UIAlloc_ManBookmark())
            return false;

        m_SelectedEntry=m_DMB_ItemLookup.end();
        DMB_RebuildFolderTree();

        RetValue=UIShow_ManBookmark();

        DMB_StoreOptionsInputs();
    }
    catch(...)
    {
        RetValue=false;
    }

    DMB_ClearOptionsInputs();
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

    DMB_StoreOptionsInputs();

    m_SelectedEntry=m_DMB_ItemLookup.find(Item);
    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    RenameBttn=UIDMB_GetButton(e_UIDMB_Button_Rename);
    DeleteBttn=UIDMB_GetButton(e_UIDMB_Button_Delete);
    BookmarkNameInput=UIDMB_GetTextInput(e_UIDMB_TextInput_BookmarkName);
    URIInput=UIDMB_GetTextInput(e_UIDMB_TextInput_URI);

    if(m_SelectedEntry->second==m_DMB_EditList->end())
    {
        /* Folder */
        UIEnableButton(RenameBttn,true);

        UIEnableTextCtrl(BookmarkNameInput,false);
        UIEnableTextCtrl(URIInput,false);
        UISetTextCtrlText(BookmarkNameInput,"");
        UISetTextCtrlText(URIInput,"");

        DMB_ClearOptionsInputs();
    }
    else
    {
        /* Bookmark */
        UIEnableButton(RenameBttn,false);

        UIEnableTextCtrl(BookmarkNameInput,true);
        UIEnableTextCtrl(URIInput,true);

        UISetTextCtrlText(BookmarkNameInput,
                m_SelectedEntry->second->Name.c_str());
        UISetTextCtrlText(URIInput,m_SelectedEntry->second->URI.c_str());

        DMB_SetupOptionsInputs();
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
    t_UITreeView *Folders;

    RenameBttn=UIDMB_GetButton(e_UIDMB_Button_Rename);
    DeleteBttn=UIDMB_GetButton(e_UIDMB_Button_Delete);
    BookmarkNameInput=UIDMB_GetTextInput(e_UIDMB_TextInput_BookmarkName);
    URIInput=UIDMB_GetTextInput(e_UIDMB_TextInput_URI);
    Folders=UIDMB_GetTreeView(e_UIDMB_TreeView_Folders);

    UIEnableButton(RenameBttn,false);
    UIEnableButton(DeleteBttn,false);
    UIEnableTextCtrl(BookmarkNameInput,false);
    UIEnableTextCtrl(URIInput,false);
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

    /* Update the options */
    IOS_UpdateOptionsFromURI(Text,m_SelectedEntry->second->Options);

    /* Set the UI with the updated options */
    IOS_SetUI2ConnectionOptions(m_DMB_ConOptions,
            m_SelectedEntry->second->Options);
}

/*******************************************************************************
 * NAME:
 *    DMB_SetupOptionsInputs
 *
 * SYNOPSIS:
 *    static void DMB_SetupOptionsInputs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the options and adds in new ones with the
 *    currently selected bookmarks data.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMB_SetupOptionsInputs(void)
{
    string UniqueID;

    DMB_ClearOptionsInputs();

    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
        return;

    if(!IOS_GetUniqueIDFromURI(m_SelectedEntry->second->URI.c_str(),UniqueID))
        return;

    m_DMB_ConOptions=IOS_AllocConnectionOptionsFromUniqueID(UniqueID.c_str(),
            UIDMB_GetOptionsFrameContainer(),m_SelectedEntry->second->Options,
            DMB_OptionsUIChanged,NULL);
}

/*******************************************************************************
 * NAME:
 *    DMB_ClearOptionsInputs()
 *
 * SYNOPSIS:
 *    static void DMB_ClearOptionsInputs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears (and frees) the options from the options area.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMB_ClearOptionsInputs(void)
{
    if(m_DMB_ConOptions!=NULL)
    {
        IOS_FreeConnectionOptions(m_DMB_ConOptions);
        m_DMB_ConOptions=NULL;
    }
}

/*******************************************************************************
 * NAME:
 *    DMB_StoreOptionsInputs
 *
 * SYNOPSIS:
 *    static void DMB_StoreOptionsInputs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function stores the connection options back into the current
 *    selected bookmark.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMB_StoreOptionsInputs(void)
{
    string UniqueID;
    string URI;
    t_UITextInputCtrl *URIInput;

    if(m_SelectedEntry==m_DMB_ItemLookup.end())
        return;

    if(m_SelectedEntry->second==m_DMB_EditList->end())
        return;

    if(m_DMB_ConOptions==NULL)
        return;

    if(!IOS_GetUniqueIDFromURI(m_SelectedEntry->second->URI.c_str(),UniqueID))
        return;

    IOS_StoreConnectionOptions(m_DMB_ConOptions,
            m_SelectedEntry->second->Options);

    /* We need to rebuild the URI (to apply the options if needed) */
    if(!IOS_GetURIFromUniqueID(UniqueID.c_str(),
            m_SelectedEntry->second->Options,URI))
    {
        return;
    }
    m_SelectedEntry->second->URI=URI;

    URIInput=UIDMB_GetTextInput(e_UIDMB_TextInput_URI);
    UISetTextCtrlText(URIInput,m_SelectedEntry->second->URI.c_str());
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
        case e_DMBEventMAX:
        default:
        break;
    }
    return AcceptEvent;
}

/*******************************************************************************
 * NAME:
 *    DMB_OptionsUIChanged
 *
 * SYNOPSIS:
 *    static void DMB_OptionsUIChanged(void *UserData);
 *
 * PARAMETERS:
 *    UserData [I] -- Ignored.
 *
 * FUNCTION:
 *    This function gets called when one of the options for a connection
 *    changes.
 *
 *    We rebuild the URI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMB_OptionsUIChanged(void *UserData)
{
    DMB_StoreOptionsInputs();
}

