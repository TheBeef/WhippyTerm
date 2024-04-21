/*******************************************************************************
 * FILENAME: Form_AddBookmarkAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access functions for the add bookmark dialog.
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (16 Nov 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_AddBookmark.h"
#include "main.h"
#include "ui_Form_AddBookmark.h"
#include <QTreeWidgetItem>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_AddBookmark *g_AddBookmarkDialog;

/*******************************************************************************
 * NAME:
 *    UIAlloc_AddBookmark
 *
 * SYNOPSIS:
 *    bool UIAlloc_AddBookmark(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the about dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_AddBookmark(void)
{
    try
    {
        g_AddBookmarkDialog=new Form_AddBookmark(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_AddBookmarkDialog=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_AddBookmark
 *
 * SYNOPSIS:
 *    bool UIShow_AddBookmark(void);
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
bool UIShow_AddBookmark(void)
{
    return g_AddBookmarkDialog->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_AddBookmark
 *
 * SYNOPSIS:
 *    void UIFree_AddBookmark(void);
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
void UIFree_AddBookmark(void)
{
    delete g_AddBookmarkDialog;

    g_AddBookmarkDialog=NULL;
}

t_UITreeItem *UIAB_AddFolderName(const char *Name)
{
    QTreeWidgetItem *NewItem;

    try
    {
        NewItem=new QTreeWidgetItem(g_AddBookmarkDialog->ui->Folder_treeWidget);
        NewItem->setText(0,Name);
        g_AddBookmarkDialog->ui->Folder_treeWidget->addTopLevelItem(NewItem);
    }
    catch(...)
    {
        return NULL;
    }
    return (t_UITreeItem *)NewItem;
}

void UIAB_SetBookmarkName(const char *Name)
{
    g_AddBookmarkDialog->ui->BookmarkName_lineEdit->setText(Name);
}

void UIAB_GetBookmarkName(std::string &Name)
{
    Name=g_AddBookmarkDialog->ui->BookmarkName_lineEdit->text().toStdString();
}

t_UITreeItem *UIAB_GetSelectedFolderItem(void)
{
    return (t_UITreeItem *)g_AddBookmarkDialog->ui->Folder_treeWidget->currentItem();
}
