/*******************************************************************************
 * FILENAME: FileRequesters.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UIFileReq.h"
#include "Form_MainWindow.h"
#include <QFileDialog>
#include "main.h"

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static string FileReq_ConvertFilterIntoQTStyle(const char *Filter,
        string &SelectedFilter,int SelectIndex);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UI_LoadFileReq
 *
 * SYNOPSIS:
 *    bool UI_LoadFileReq(std::string &Path,std::string &Filename);
 *
 * PARAMETERS:
 *    Path [I/O] -- This has the default path in it and returns the path of
 *                  the file the user selected.
 *    Filename [I/O] -- This has the default filename in it and returns the
 *                      filename the user selected.
 *    Filters [I] -- This is a list of filters split by \n chars.  The format
 *                   of the filters is: DisplayName|Filter\n.  For example:
 *                      All Files|*\n
 *                      Image|*.png;*.bmp;*.jpg;*.jpeg\n
 *    SelectedFilter [I] -- The filter selected by default.  This is an index
 *                          into the 'Filters' list.
 *
 * FUNCTION:
 *    This file asks the user to select an existing file for loading.
 *
 * RETURNS:
 *    true -- User selected a file
 *    false -- User canceled or there was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UI_LoadFileReq(const char *Title,std::string &Path,
        std::string &Filename,const char *Filters,int SelectedFilter)
{
    string QTFilter;
    QString RetFileName;
    string StartingPath;
    string SelectFilterStr;
    QFileInfo FileInfo;
    QString SelectFilterStrQTStr;

    QTFilter=FileReq_ConvertFilterIntoQTStyle(Filters,SelectFilterStr,
            SelectedFilter);

    SelectFilterStrQTStr=SelectFilterStr.c_str();

    StartingPath=Path;
    if(StartingPath!="" && (StartingPath[StartingPath.length()-1]!='/' &&
            StartingPath[StartingPath.length()-1]!='\\'))
    {
        StartingPath+="/";
    }
    StartingPath+=Filename;

    RetFileName=QFileDialog::getOpenFileName(g_MainApp->activeWindow(),Title,
                                                StartingPath.c_str(),
                                                QTFilter.c_str(),
                                                &SelectFilterStrQTStr);

    if(RetFileName=="")
        return false;

    FileInfo.setFile(RetFileName);
    Path=QDir::toNativeSeparators(FileInfo.path()).toUtf8().data();
    Filename=FileInfo.fileName().toUtf8().data();

    return true;
}

/*******************************************************************************
 * NAME:
 *    UI_SaveFileReq
 *
 * SYNOPSIS:
 *    bool UI_SaveFileReq(std::string &Path,std::string &Filename);
 *
 * PARAMETERS:
 *    Path [I/O] -- This has the default path in it and returns the path of
 *                  the file the user selected.  This is not guaranteed to end
 *                  in a with a / or \
 *    Filename [I/O] -- This has the default filename in it and returns the
 *                      filename the user selected.  If this is "" then the
 *                      path will be used for the filename and path.  
 *    Filters [I] -- This is a list of filters split by \n chars.  The format
 *                   of the filters is: DisplayName|Filter\n.  For example:
 *                      All Files|*\n
 *                      Image|*.png;*.bmp;*.jpg;*.jpeg\n
 *    SelectedFilter [I] -- The filter selected by default.  This is an index
 *                          into the 'Filters' list.
 *
 * FUNCTION:
 *    This file asks the user to select an existing file for loading.
 *
 * RETURNS:
 *    true -- User selected a file
 *    false -- User canceled or there was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UI_SaveFileReq(const char *Title,std::string &Path,
        std::string &Filename,const char *Filters,int SelectedFilter)
{
    string QTFilter;
    QString RetFileName;
    string StartingPath;
    string SelectFilterStr;
    QFileInfo FileInfo;
    QString SelectFilterStrQTStr;

    QTFilter=FileReq_ConvertFilterIntoQTStyle(Filters,SelectFilterStr,
            SelectedFilter);

    SelectFilterStrQTStr=SelectFilterStr.c_str();

    StartingPath=Path;
    if(Filename!="")
    {
        if(StartingPath!="" && (StartingPath[StartingPath.length()-1]!='/' &&
                StartingPath[StartingPath.length()-1]!='\\'))
        {
            StartingPath+="/";
        }
        StartingPath+=Filename;
    }

    RetFileName=QFileDialog::getSaveFileName(g_MainApp->activeWindow(),Title,
                                                StartingPath.c_str(),
                                                QTFilter.c_str(),
                                                &SelectFilterStrQTStr);

    if(RetFileName=="")
        return false;

    FileInfo.setFile(RetFileName);
    Path=QDir::toNativeSeparators(FileInfo.path()).toUtf8().data();
    Filename=FileInfo.fileName().toUtf8().data();

    return true;
}

static string FileReq_ConvertFilterIntoQTStyle(const char *Filter,
        string &SelectedFilter,int SelectIndex)
{
    string QTFormat;
    string Add;
    const char *p;
    bool InFilter;
    int Index;

    QTFormat="";
    p=Filter;
    InFilter=false;
    Index=0;
    while(*p!=0)
    {
        if(*p=='\n')
        {
            /* End of the current filter */
            Add=");;";
            InFilter=false;
        }
        else if(*p=='|')
        {
            Add=" (";
            InFilter=true;
        }
        else if(*p==';' && InFilter)
        {
            Add=" ";
        }
        else
        {
            Add=*p;
        }
        QTFormat+=Add;
        if(Index==SelectIndex)
            SelectedFilter+=Add;

        if(*p=='\n')
            Index++;

        p++;
    }
    if(InFilter)
    {
        QTFormat+=")";
        if(Index==SelectIndex)
            SelectedFilter+=")";
    }

    return QTFormat;
}

std::string UI_ConcatFile2Path(const std::string &Path,const std::string &File)
{
    string RetStr;

    RetStr=Path;
    if(RetStr!="" && (RetStr[RetStr.length()-1]!='/' &&
            RetStr[RetStr.length()-1]!='\\'))
    {
        RetStr+="/";
    }
    RetStr+=File;
    return QDir::toNativeSeparators(RetStr.c_str()).toUtf8().data();
}

