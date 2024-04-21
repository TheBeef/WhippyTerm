/*******************************************************************************
 * FILENAME: Bookmarks.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the bookmark system in it.
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (12 Nov 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Bookmarks.h"
#include "OS/FilePaths.h"
#include "OS/Directorys.h"
#include "ThirdParty/TinyCFG/TinyCFG.h"
#include "App/Dialogs/Dialog_AddBookmark.h"
#include "App/Dialogs/Dialog_ManBookmark.h"
#include "App/Util/StorageHelpers.h"
#include <string>

using namespace std;

/*** DEFINES                  ***/
#define BOOKMARKS_FILE       "Bookmarks.cfg"

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
t_BookmarkList g_BookmarkList;
int g_BookmarkNextUID;

class Bookmarks_TinyCFG : public TinyCFGBaseData
{
    public:
        t_BookmarkList *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};

/*******************************************************************************
 * NAME:
 *    InitBookmarks
 *
 * SYNOPSIS:
 *    void InitBookmarks(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's the bookmark system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void InitBookmarks(void)
{
    g_BookmarkNextUID=1;    // We start the UID at 1 so we can use 0 as none
}

bool Bookmarks_TinyCFG::LoadElement(class TinyCFG *CFG)
{
    struct Bookmark NewData;
    class TinyCFG SubCFG("Bookmark");

    Ptr->clear();

    SubCFG.Register("MenuName",NewData.MenuName);
    SubCFG.Register("Name",NewData.Name);
    SubCFG.Register("URI",NewData.URI);
    RegisterKVList_TinyCFG(SubCFG,"Option",NewData.Options);
    SubCFG.Register("UseCustomSettings",NewData.UseCustomSettings);
    SubCFG.StartBlock("CustomSettings");
    NewData.CustomSettings.RegisterAllMembers(SubCFG);
    SubCFG.EndBlock();

    NewData.MenuName="";
    NewData.Name="";
    NewData.URI="";
    NewData.UseCustomSettings=false;
    NewData.CustomSettings.DefaultSettings();

    SubCFG.ConnectToParentCFGForReading(CFG);

    while(SubCFG.ReadNextCFG())
    {
        NewData.BookmarkUID=g_BookmarkNextUID;
        g_BookmarkNextUID++;

        Ptr->push_back(NewData);
    }

    return true;
}

bool Bookmarks_TinyCFG::SaveElement(class TinyCFG *CFG)
{
    i_BookmarkList i;
    string MenuName;
    string Name;
    string URI;
    t_KVList Options;
    class TinyCFG SubCFG("Bookmark");
    class ConSettings CustomSettings;
    bool UseCustomSettings;

    SubCFG.Register("MenuName",MenuName);
    SubCFG.Register("Name",Name);
    SubCFG.Register("URI",URI);
    RegisterKVList_TinyCFG(SubCFG,"Option",Options);
    SubCFG.Register("UseCustomSettings",UseCustomSettings);
    SubCFG.StartBlock("CustomSettings");
    CustomSettings.RegisterAllMembers(SubCFG);
    SubCFG.EndBlock();

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        MenuName=i->MenuName;
        Name=i->Name;
        URI=i->URI;
        Options=i->Options;
        UseCustomSettings=i->UseCustomSettings;
        CustomSettings=i->CustomSettings;

        SubCFG.WriteCFGUsingParentCFG(CFG);
    }
    return true;
}

bool RegisterBookmarksList_TinyCFG(class TinyCFG &cfg,const char *XmlName,
      t_BookmarkList &Data)
{
    class Bookmarks_TinyCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new Bookmarks_TinyCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}

bool LoadBookmarks(void)
{
    class TinyCFG cfg("BookMarkData");
    string Path;
    string BookmarksFilename;

    try
    {
         if(GetAppDataPath(Path)==false)
            return false;

        /* See if this path exists */
        if(!PathExists(Path.c_str()))
        {
            /* Try making it */
            if(!MakePathDir(Path.c_str()))
                return false;
        }

        BookmarksFilename=Path;
        BookmarksFilename+=BOOKMARKS_FILE;

        RegisterBookmarksList_TinyCFG(cfg,"Bookmarks",g_BookmarkList);

        cfg.LoadCFGFile(BookmarksFilename.c_str());
    }
    catch(...)
    {
        return false;
    }
    return true;
}

bool SaveBookmarks(void)
{
    class TinyCFG cfg("BookMarkData");
    string Path;
    string BookmarksFilename;

    try
    {
         if(GetAppDataPath(Path)==false)
            return false;

        /* See if this path exists */
        if(!PathExists(Path.c_str()))
        {
            /* Try making it */
            if(!MakePathDir(Path.c_str()))
                return false;
        }

        BookmarksFilename=Path;
        BookmarksFilename+=BOOKMARKS_FILE;

        RegisterBookmarksList_TinyCFG(cfg,"Bookmarks",g_BookmarkList);

        cfg.SaveCFGFile(BookmarksFilename.c_str());
    }
    catch(...)
    {
        return false;
    }
    return true;
}

bool BookmarkConnection(class Connection *Con)
{
    struct Bookmark NewBookmark;
    i_BookmarkList bm;

    try
    {
        NewBookmark.MenuName="";
        Con->GetDisplayName(NewBookmark.Name);
        if(!Con->GetConnectionOptions(NewBookmark.Options))
            return false;

        if(!Con->GetURI(NewBookmark.URI))
            return false;

        NewBookmark.UseCustomSettings=Con->UsingCustomSettings;
        NewBookmark.CustomSettings=Con->CustomSettings;

        if(!RunAddBookmarkDialog(NewBookmark))
            return true;

        /* Ok, find the insert position for this bookmark */
        if(NewBookmark.MenuName=="")
        {
            /* No menu just stick it on the end */
            NewBookmark.BookmarkUID=g_BookmarkNextUID;
            g_BookmarkNextUID++;

            g_BookmarkList.push_back(NewBookmark);
        }
        else
        {
            /* See if this menu already exists */
            for(bm=g_BookmarkList.begin();bm!=g_BookmarkList.end();bm++)
            {
                if(bm->MenuName==NewBookmark.MenuName)
                {
                    /* Found it, continue until we hit a different name */
                    while(bm!=g_BookmarkList.end() &&
                            bm->MenuName==NewBookmark.MenuName)
                    {
                        bm++;
                    }
                    if(bm!=g_BookmarkList.end())
                        g_BookmarkList.insert(bm,NewBookmark);

                    break;
                }
            }
            if(bm==g_BookmarkList.end())
            {
                /* This is a new folder name or there wasn't another folder
                   after the requested folder */
                g_BookmarkList.push_back(NewBookmark);
            }
        }
    }
    catch(...)
    {
        return false;
    }

    SaveBookmarks();

    return true;
}

void ManageBookmarks(void)
{
    t_BookmarkList EditBookmarkList;

    /* Copy the bookmarks so the user can edit without changing everything */
    EditBookmarkList=g_BookmarkList;

    if(RunManBookmarkDialog(EditBookmarkList))
    {
        /* Take the changes the user made */
        g_BookmarkList=EditBookmarkList;

        SaveBookmarks();
    }
}

i_BookmarkList FindBookmarkByUID(int UID)
{
    i_BookmarkList bm;

    for(bm=g_BookmarkList.begin();bm!=g_BookmarkList.end();bm++)
        if(bm->BookmarkUID==UID)
            break;

    return bm;
}

