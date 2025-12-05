/* Prerelease Version 0.9 */

/*******************************************************************************
 * FILENAME: TinyCFG.cpp
 *
 * PROJECT: TinyCFG
 *
 * FILE DESCRIPTION:
 *    This file is a helper for TinyXML.  This helper supports loads and saving
 *    an xml config file to and from a C struct.
 *
 * COPYRIGHT:
 *    Copyright 2003 Paul Hutchinson
 *
 *    MIT License, See LICENSE file for details.
 *
 * CREATED BY:
 *    PaulHutchinson (19 Jun 2003)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "TinyCFG.h"
#include <ctype.h>
#include <stdarg.h>
#include <exception>
#include <string>
#include <string.h>
#include <list>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** VARIABLE DEFINITIONS     ***/

/*** FUNCTION PROTOTYPES      ***/

/*** Default types            ***/
class TinyCFG_DefaultStringData : public TinyCFGBaseData
{
    public:
        string *Ptr;
        bool LoadData(string &LoadedString)
        {
            *Ptr=LoadedString;
            return true;
        }
        bool SaveData(string &StoreString)
        {
            StoreString=Ptr->c_str();
            return true;
        }
};

class TinyCFG_DefaultIntData : public TinyCFGBaseData
{
    public:
        int *Ptr;
        const char *FormatString;
        bool IsHex;
        TinyCFG_DefaultIntData(bool UnsignedNumber,bool HexOutput)
        {
            IsHex=HexOutput;
            if(IsHex)
                FormatString="%08X";
            else
                FormatString=UnsignedNumber?"%u":"%d";
        }
        bool LoadData(string &LoadedString)
        {
            if(IsHex)
                sscanf(LoadedString.c_str(),"%x",Ptr);
            else
                *Ptr=atoi(LoadedString.c_str());
            return true;
        }
        bool SaveData(string &StoreString)
        {
            char buff[100];
            sprintf(buff,FormatString,*Ptr);
            StoreString=buff;
            return true;
        }
};

class TinyCFG_DefaultShortData : public TinyCFGBaseData
{
    public:
        short *Ptr;
        const char *FormatString;
        TinyCFG_DefaultShortData(bool UnsignedNumber)
        {
            FormatString=UnsignedNumber?"%u":"%d";
        }
        bool LoadData(string &LoadedString)
        {
            *Ptr=atoi(LoadedString.c_str());
            return true;
        }
        bool SaveData(string &StoreString)
        {
            char buff[100];
            sprintf(buff,FormatString,*Ptr);
            StoreString=buff;
            return true;
        }
};
class TinyCFG_DefaultLongData : public TinyCFGBaseData
{
    public:
        long *Ptr;
        const char *FormatString;
        TinyCFG_DefaultLongData(bool UnsignedNumber)
        {
            FormatString=UnsignedNumber?"%u":"%d";
        }
        bool LoadData(string &LoadedString)
        {
            *Ptr=atoi(LoadedString.c_str());
            return true;
        }
        bool SaveData(string &StoreString)
        {
            char buff[100];
            sprintf(buff,FormatString,*Ptr);
            StoreString=buff;
            return true;
        }
};
class TinyCFG_DefaultLongLongData : public TinyCFGBaseData
{
    public:
        long long *Ptr;
        const char *FormatString;
        TinyCFG_DefaultLongLongData(bool UnsignedNumber)
        {
            FormatString=UnsignedNumber?"%llu":"%lld";
        }
        bool LoadData(string &LoadedString)
        {
            *Ptr=strtoll(LoadedString.c_str(),NULL,10);
            return true;
        }
        bool SaveData(string &StoreString)
        {
            char buff[100];
            sprintf(buff,FormatString,*Ptr);
            StoreString=buff;
            return true;
        }
};

class TinyCFG_DefaultBoolData : public TinyCFGBaseData
{
    public:
        bool *Ptr;
        bool LoadData(string &LoadedString);
        bool SaveData(string &StoreString);
};
bool TinyCFG_DefaultBoolData::LoadData(string &LoadedString)
{
    const char *TrueStr="TRUE"; // The true string
    const char *Place;      // Where are we in the string
    const char *TruePlace;  // Where are we in the true string

    /* Do a case insensitive compare */
    Place=LoadedString.c_str();
    TruePlace=TrueStr;
    while(toupper(*Place)==*TruePlace && *Place!=0)
    {
        Place++;
        TruePlace++;
    }
    if(*Place==*TruePlace)
        *Ptr=true;
    else
        *Ptr=false;

    return true;
}
bool TinyCFG_DefaultBoolData::SaveData(string &StoreString)
{
    if(*Ptr)
        StoreString="True";
    else
        StoreString="False";
    return true;
}

class TinyCFG_DefaultDoubleData : public TinyCFGBaseData
{
    public:
        string FormatString;
        double *Ptr;
        bool LoadData(string &LoadedString);
        bool SaveData(string &StoreString);
};
bool TinyCFG_DefaultDoubleData::LoadData(string &LoadedString)
{
    sscanf(LoadedString.c_str(),FormatString.c_str(),Ptr);

    return true;
}
bool TinyCFG_DefaultDoubleData::SaveData(string &StoreString)
{
    char buff[100];

    sprintf(buff,FormatString.c_str(),*Ptr);
    StoreString=buff;

    return true;
}

class TinyCFG_DefaultCharData : public TinyCFGBaseData
{
    private:
        int MaxCharLen;
    public:
        char *Ptr;
        TinyCFG_DefaultCharData(int MaxSize)
        {
            MaxCharLen=MaxSize;
        }
        bool LoadData(string &LoadedString)
        {
            *Ptr=0;
            strncpy(Ptr,LoadedString.c_str(),MaxCharLen-1);
            Ptr[MaxCharLen-1]=0;
            return true;
        }
        bool SaveData(string &StoreString)
        {
            StoreString=Ptr;
            return true;
        }
};
class TinyCFG_EnumData : public TinyCFGBaseData
{
    public:
        int *Ptr;
        int DefaultValue;
        bool LoadData(string &LoadedString);
        bool SaveData(string &StoreString);
        bool AddNewEnum(int Value,const char *String);
    private:
        struct TinyCFGEnumData
        {
            int Value;
            string String;
        };
        list<struct TinyCFGEnumData> EnumValues;
};
bool TinyCFG_EnumData::AddNewEnum(int Value,const char *String)
{
    struct TinyCFGEnumData NewData;
    NewData.Value=Value;
    NewData.String=String;

    EnumValues.push_back(NewData);

    return true;
}
bool TinyCFG_EnumData::LoadData(string &LoadedString)
{
    const char *Place;      // Where are we in the string
    const char *RefPlace;   // Where are we in the ref string
    list<struct TinyCFGEnumData>::iterator Enum;

    for(Enum=EnumValues.begin();Enum!=EnumValues.end();Enum++)
    {
        /* Do a case insensitive compare */
        Place=LoadedString.c_str();
        RefPlace=Enum->String.c_str();
        while(toupper(*Place)==toupper(*RefPlace) && *Place!=0)
        {
            Place++;
            RefPlace++;
        }
        if(toupper(*Place)==toupper(*RefPlace))
        {
            /* Found it */
            *Ptr=Enum->Value;
            return true;
        }
    }
    *Ptr=DefaultValue;

    return true;
}
bool TinyCFG_EnumData::SaveData(string &StoreString)
{
    list<struct TinyCFGEnumData>::iterator Enum;

    /* If we don't find the default value or the current value we use the
      string "Unknown" */
    StoreString="Unknown";

    for(Enum=EnumValues.begin();Enum!=EnumValues.end();Enum++)
    {
        if(*Ptr==Enum->Value)
        {
            /* Found it */
            StoreString=Enum->String;
            return true;
        }
        /* See if this is the default value (in case we don't find this
         value in the enum list */
        if(Enum->Value==DefaultValue)
            StoreString=Enum->String;
    }
    return false;
}

class TinyCFG_DefaultStringListData : public TinyCFGBaseData
{
    public:
        list<string> *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};
bool TinyCFG_DefaultStringListData::LoadElement(class TinyCFG *CFG)
{
    const char *Str;

    Ptr->clear();

    while((Str=CFG->ReadNextDataElement("Data"))!=NULL)
        Ptr->push_back(Str);

    return true;
}
bool TinyCFG_DefaultStringListData::SaveElement(class TinyCFG *CFG)
{
    list<string>::iterator i;

    for(i=Ptr->begin();i!=Ptr->end();i++)
        CFG->WriteDataElement("Data",i->c_str());

    return true;
}
class TinyCFG_DefaultIntListData : public TinyCFGBaseData
{
    public:
        list<int> *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};
bool TinyCFG_DefaultIntListData::LoadElement(class TinyCFG *CFG)
{
    const char *Str;

    Ptr->clear();

    while((Str=CFG->ReadNextDataElement("Data"))!=NULL)
        Ptr->push_back(atoi(Str));

    return true;
}
bool TinyCFG_DefaultIntListData::SaveElement(class TinyCFG *CFG)
{
    list<int>::iterator i;
    char buff[100];

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        sprintf(buff,"%d",*i);
        CFG->WriteDataElement("Data",buff);
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::TinyCFG
 *
 * SYNOPSIS:
 *    TinyCFG::TinyCFG(const char *FirstBlock);
 *
 * PARAMETERS:
 *    FirstBlock [I] -- If this is provided StartBlock() will be called with
 *                      this as it's arg.  It will be the name of the outer
 *                      most block.
 *
 * FUNCTION:
 *    The constructor.  Resets and sets up any needed data.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (22 Jun 2003)
 *       Created
 ******************************************************************************/
TinyCFG::TinyCFG(const char *FirstBlockName)
{
    if(FirstBlockName[0]==0)
        throw("Root block must have a name");
    SavedFirstBlockName=FirstBlockName;
    ResetData();
    StartBlock(SavedFirstBlockName.c_str());
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::ResetData
 *
 * SYNOPSIS:
 *    void TinyCFG::ResetData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function resets all the private and public vars to there just been
 *    constructed state.  It does not free any memory, it just resets pointers.
 *    You should use FreeAllRegisteredTypes() if you want to free up the
 *    resources that this class is using.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::FreeAllRegisteredTypes()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (08 Aug 2003)
 *       Created
 ******************************************************************************/
void TinyCFG::ResetData(void)
{
    Failure=false;

    FirstBlock=NULL;
    CurrentBlock=NULL;
    XMLIndent=0;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::~TinyCFG
 *
 * SYNOPSIS:
 *    TinyCFG::~TinyCFG()
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    The deconstructor for the TinyCFG class.  Frees all memory allocated by
 *    this class.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::FreeAllRegisteredTypes(), TinyCFG::Clear()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (22 Jun 2003)
 *       Created
 ******************************************************************************/
TinyCFG::~TinyCFG()
{
    FreeAllRegisteredTypes(FirstBlock);
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::FreeAllRegisteredTypes
 *
 * SYNOPSIS:
 *    void TinyCFG::FreeAllRegisteredTypes(struct TinyCFG_Entry *List2Free);
 *
 * PARAMETERS:
 *    List2Free [I] -- A pointer to a link list to free.
 *
 * FUNCTION:
 *    This is a recursive function that frees all the memory allocated in a
 *    link list.  This should really only be called from the destructor.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::~TinyCFG()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (22 Jun 2003)
 *       Created
 ******************************************************************************/
void TinyCFG::FreeAllRegisteredTypes(struct TinyCFG_Entry *List2Free)
{
    struct TinyCFG_Entry *NextEntry;    // A temp pointer to be used to free the current node
    struct TinyCFG_RegData *NextData;   // A temp pointer to be used to free the current node

    while(List2Free!=NULL)
    {
        /* If there are sub lists free them */
        if(List2Free->FirstSub!=NULL)
        {
            FreeAllRegisteredTypes(List2Free->FirstSub);
        }
        /* Free the data for this node */
        while(List2Free->FirstData!=NULL)
        {
            NextData=List2Free->FirstData->Next;
            /* Delete the registered type */
            delete List2Free->FirstData->Data;
            /* And now the link list node */
            delete List2Free->FirstData;
            List2Free->FirstData=NextData;
        }
        /* Now free this entry */
        NextEntry=List2Free->Next;
        delete List2Free;
        List2Free=NextEntry;
    }
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::Register
 *
 * SYNOPSIS:
 *    bool TinyCFG::Register(const char *XmlName,string &Data);
 *
 * PARAMETERS:
 *    XmlName [I] -- The name of the xml element to place the data in
 *    Data    [I] -- The data to register
 *
 * FUNCTION:
 *    This function registers a some data for use with TinyCFG.
 *    A pointer to the data will be taken and when save is called a copy of
 *    the data is output (out=*Data).  When load is called 'Data' is set to the
 *    value loaded in (*Data=in).
 *
 * RETURNS:
 *    true -- Every thing worked out
 *    false -- There was an error registering this data member.  See NOTES.
 *
 * NOTES:
 *    On failure the public var 'Failure" is set to true.  You can use this
 *    to register all your data members and then check 'Failure' to see if
 *    everything worked out.
 *
 * SEE ALSO:
 *    TinyCFG::RegisterGeneric()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (22 Jun 2003)
 *       Created
 ******************************************************************************/
bool TinyCFG::Register(const char *XmlName,string &Data)
{
    class TinyCFG_DefaultStringData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultStringData;
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return RegisterGeneric(NewDataClass);
}
bool TinyCFG::Register(const char *XmlName,int &Data)
{
    return RegisterInt(XmlName,(unsigned int *)&Data,false,false);
}
bool TinyCFG::Register(const char *XmlName,unsigned int &Data)
{
    return RegisterInt(XmlName,&Data,false,true);
}
bool TinyCFG::Register(const char *XmlName,short &Data)
{
    return RegisterShort(XmlName,(unsigned short *)&Data,false);
}
bool TinyCFG::Register(const char *XmlName,unsigned short &Data)
{
    return RegisterShort(XmlName,&Data,true);
}
bool TinyCFG::Register(const char *XmlName,long &Data)
{
    return RegisterLong(XmlName,(unsigned long *)&Data,false);
}
bool TinyCFG::Register(const char *XmlName,unsigned long &Data)
{
    return RegisterLong(XmlName,&Data,true);
}
bool TinyCFG::Register(const char *XmlName,long long &Data)
{
    return RegisterLongLong(XmlName,(unsigned long long *)&Data,false);
}
bool TinyCFG::Register(const char *XmlName,unsigned long long &Data)
{
    return RegisterLongLong(XmlName,&Data,true);
}
bool TinyCFG::Register(const char *XmlName,int &Data,bool OutputHex)
{
    return RegisterInt(XmlName,(unsigned int *)&Data,OutputHex,true);
}
bool TinyCFG::Register(const char *XmlName,unsigned int &Data,bool OutputHex)
{
    return RegisterInt(XmlName,&Data,OutputHex,true);
}
bool TinyCFG::RegisterInt(const char *XmlName,unsigned int *Data,bool OutputHex,
        bool IsUnsigned)
{
    class TinyCFG_DefaultIntData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultIntData(IsUnsigned,OutputHex);
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=(int *)Data;
    NewDataClass->XmlName=XmlName;

    return RegisterGeneric(NewDataClass);
}
bool TinyCFG::RegisterShort(const char *XmlName,unsigned short *Data,
        bool IsUnsigned)
{
    class TinyCFG_DefaultShortData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultShortData(IsUnsigned);
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=(short *)Data;
    NewDataClass->XmlName=XmlName;

    return RegisterGeneric(NewDataClass);
}
bool TinyCFG::RegisterLong(const char *XmlName,unsigned long *Data,
        bool IsUnsigned)
{
    class TinyCFG_DefaultLongData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultLongData(IsUnsigned);
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=(long *)Data;
    NewDataClass->XmlName=XmlName;

    return RegisterGeneric(NewDataClass);
}
bool TinyCFG::RegisterLongLong(const char *XmlName,unsigned long long *Data,
        bool IsUnsigned)
{
    class TinyCFG_DefaultLongLongData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultLongLongData(IsUnsigned);
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=(long long *)Data;
    NewDataClass->XmlName=XmlName;

    return RegisterGeneric(NewDataClass);
}

bool TinyCFG::Register(const char *XmlName,char *Data,int MaxSize)
{
    class TinyCFG_DefaultCharData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultCharData(MaxSize);
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=Data;
    NewDataClass->XmlName=XmlName;

    return RegisterGeneric(NewDataClass);
}
bool TinyCFG::Register(const char *XmlName,bool &Data)
{
    class TinyCFG_DefaultBoolData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultBoolData;
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return RegisterGeneric(NewDataClass);
}
bool TinyCFG::Register(const char *XmlName,double &Data,const char *FormatStr)
{
    class TinyCFG_DefaultDoubleData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultDoubleData;
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;
    NewDataClass->FormatString=FormatStr;

    return RegisterGeneric(NewDataClass);
}
bool TinyCFG::Register(const char *XmlName,list<string> &Data)
{
    class TinyCFG_DefaultStringListData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultStringListData;
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return RegisterGeneric(NewDataClass);
}
bool TinyCFG::Register(const char *XmlName,list<int> &Data)
{
    class TinyCFG_DefaultIntListData *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_DefaultIntListData;
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return RegisterGeneric(NewDataClass);
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::RegisterEnum
 *
 * SYNOPSIS:
 *    bool TinyCFG::RegisterEnum(const char *XmlName,int &Data,int DefaultValue,
 *          int NumberOfEntries,int List,...);
 *
 * PARAMETERS:
 *    XmlName        [I] -- The name of the xml element to place the data in
 *    Data           [I] -- The data to register (may have to be cast to int)
 *    DefaultValue   [I] -- When a value is found in the xml (or the Data is
 *                          set to a value that is not listed) does not match
 *                          one of the entries in the list then use this value.
 *    NumberOfEntries[I] -- The number of entries that follow.  The number of
 *                          pairs of data that are sent in must match this
 *                          value.
 *    List           [I] -- This is the first enum value, it is followed by
 *                          a string that repersents this value in the xml.
 *                          For example you might use:
 *                          e_One,"One",
 *                          e_Two,"Two",
 *                          e_Three,"Three".
 *                          There is 2 pieces to a pair.  You must provide both
 *                          parts and you have to have as many pairs as
 *                          'NumberOfEntries' states.  See example.
 *
 * FUNCTION:
 *    This function registers enums for use with TinyCFG.
 *    A pointer to the data will be taken and when save is called a copy of
 *    the data is output (out=*Data).  When load is called 'Data' is set to the
 *    value loaded in (*Data=in).  You may have to cast the enum to (int)
 *    before calling the function.  Also your compiler must have enum's
 *    that are the same size as an int.
 *
 * RETURNS:
 *    true -- Every thing worked out
 *    false -- There was an error registering this data member.  See NOTES.
 *
 * NOTES:
 *    On failure the public var 'Failure" is set to true.  You can use this
 *    to register all your data members and then check 'Failure' to see if
 *    everything worked out.
 *
 * EXAMPLE:
 *    typedef enum
 *    {
 *       e_One    =1,
 *       e_Two    =2,
 *       e_Three  =3,
 *       eMAX     =3
 *    } e_NumbersType;
 *
 *    e_NumbersType nums;
 *
 *    cfg.RegisterEnum("ENums",(int)nums,e_One,
 *          eMAX,e_One,"One",e_Two,"Two",e_Three,"Three");
 *
 * SEE ALSO:
 *    Register();
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (15 Sep 2003)
 *       Created
 ******************************************************************************/
bool TinyCFG::RegisterEnum(const char *XmlName,int &Data,int DefaultValue,
        int NumberOfEntries,int List,...)
{
    class TinyCFG_EnumData *NewDataClass;
    int r; /* Temp var (for loops) */
    va_list vlist; // The variable arg list
    int Value;
    const char *String;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TinyCFG_EnumData;
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    if(!CheckXMLName(XmlName))
    {
        delete NewDataClass;
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->DefaultValue=DefaultValue;
    NewDataClass->XmlName=XmlName;

    va_start(vlist,List);
    Value=List;
    String=va_arg(vlist,const char *);
    for(r=0;r<NumberOfEntries;r++)
    {
        if(!NewDataClass->AddNewEnum(Value,String))
        {
            delete NewDataClass;
            va_end(vlist);
            Failure=true;
            return false;
        }
        if(r<NumberOfEntries-1)
        {
            /* Get the next value */
            Value=va_arg(vlist,int);
            String=va_arg(vlist,const char *);
        }
    }
    va_end(vlist);

    return RegisterGeneric(NewDataClass);
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::RegisterGeneric
 *
 * SYNOPSIS:
 *    bool TinyCFG::RegisterGeneric(class TinyCFGBaseData *Generic);
 *
 * PARAMETERS:
 *    Generic [I] -- This is a pointer to a class derived from 
 *                   'TinyCFGBaseData'.  It will be added to the list of
 *                   registed tags.
 *
 * FUNCTION:
 *    This method provides a way register new data types with TinyCFG.  TinyCFG
 *    uses this method to register the build in types.
 *
 * RETURNS:
 *    true -- Everything worked out
 *    false -- There was an error and this class was not registered.
 *
 * NOTES:
 *    On failure the public var 'Failure" is set to true.  You can use this
 *    to register all your data members and then check 'Failure' to see if
 *    everything worked out.
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (22 Jun 2003)
 *       Created
 ******************************************************************************/
bool TinyCFG::RegisterGeneric(class TinyCFGBaseData *Generic)
{
    struct TinyCFG_RegData *NewData;

    try
    {
        NewData=new struct TinyCFG_RegData;
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    /* Setup the data */
    NewData->Data=Generic;
    NewData->Next=NULL;

    /* Register it */
    if(CurrentBlock==NULL)
    {
        /* We can't add data to an non-existent block */
        Failure=true;
        delete NewData;
        return false;
    }
    if(CurrentBlock->FirstData==NULL)
        CurrentBlock->FirstData=NewData;
    else
        CurrentBlock->CurrentData->Next=NewData;
    CurrentBlock->CurrentData=NewData;
    return true;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::StartBlock
 *
 * SYNOPSIS:
 *    bool TinyCFG::StartBlock(const char *BlockName);
 *    bool TinyCFG::StartBlock(const string &BlockName);
 *
 * PARAMETERS:
 *    BlockName [I] -- The name of the new block to create
 *
 * FUNCTION:
 *    This method starts a new block (a level in the hierarchy).
 *    Any new vars registed after this call will be in the new block
 *    (until EndBlock() is called).
 *
 * RETURNS:
 *    true -- Things worked out.  A new block was started
 *    false -- Error.  A block was not started.
 *
 * NOTES:
 *    On failure the public var 'Failure" is set to true.  You can use this
 *    to register all your data members and then check 'Failure' to see if
 *    everything worked out.
 *
 * SEE ALSO:
 *    TinyCFG::EndBlock()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (24 Jun 2003)
 *       Created
 ******************************************************************************/
bool TinyCFG::StartBlock(const string &BlockName)
{
    return StartBlock(BlockName.c_str());
}
bool TinyCFG::StartBlock(const char *BlockName)
{
    struct TinyCFG_Entry *NewEntry;

    if(!CheckXMLName(BlockName))
        return false;

    try
    {
        NewEntry=new struct TinyCFG_Entry;
    }
    catch(std::bad_alloc)
    {
        Failure=true;
        return false;
    }

    NewEntry->XmlName=BlockName;
    NewEntry->FirstSub=NULL;
    NewEntry->CurrentSub=NULL;
    NewEntry->ParentSubLevel=NULL;
    NewEntry->FirstData=NULL;
    NewEntry->CurrentData=NULL;
    NewEntry->Next=NULL;

    /* Add this to the list of entries */
    if(CurrentBlock==NULL)
    {
        FirstBlock=NewEntry;
    }
    else
    {
        if(CurrentBlock->FirstSub==NULL)
            CurrentBlock->FirstSub=NewEntry;
        else
            CurrentBlock->CurrentSub->Next=NewEntry;
        NewEntry->ParentSubLevel=CurrentBlock;
        CurrentBlock->CurrentSub=NewEntry;
    }
    CurrentBlock=NewEntry;
    return true;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::EndBlock
 *
 * SYNOPSIS:
 *    void TinyCFG::EndBlock(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This method ends a block that was started with StartBlock().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::StartBlock()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (24 Jun 2003)
 *       Created
 ******************************************************************************/
void TinyCFG::EndBlock(void)
{
    if(CurrentBlock!=NULL)
        CurrentBlock=CurrentBlock->ParentSubLevel;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::Clear
 *
 * SYNOPSIS:
 *    void TinyCFG::Clear(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function unregisters all the registered types.  It basicly returns
 *    the TinyCFG class back to the same state it was in when it was
 *    constructed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::TinyCFG()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (08 Aug 2003)
 *       Created
 ******************************************************************************/
void TinyCFG::Clear(void)
{
    FreeAllRegisteredTypes(FirstBlock);
    ResetData();
    StartBlock(SavedFirstBlockName.c_str());
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::SaveCFGFile
 *
 * SYNOPSIS:
 *    bool TinyCFG::SaveCFGFile(const char *FileName);
 *
 * PARAMETERS:
 *    FileName [I] -- What file name to save the xml too.
 *
 * FUNCTION:
 *    This method takes and goes through all the registered types and makes
 *    an XML config file.  The XML is then saved to a file whos name was given
 *    in 'FileName'.
 *
 * RETURNS:
 *    true -- Things worked out.  The file has been saved.
 *    false -- There was an error building the XML or saving the file.
 *
 * NOTES:
 *    On failure the public var 'Failure" is set to true.  You can use this
 *    to register all your data members and then check 'Failure' to see if
 *    everything worked out.
 *
 * SEE ALSO:
 *    TinyCFG::MakeXml(), TinyCFG::LoadCFGFile()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (22 Jun 2003)
 *       Created
 ******************************************************************************/
bool TinyCFG::SaveCFGFile(const char *FileName)
{
    fh=fopen(FileName,"w");
    if(fh==NULL)
        return false;

    fprintf(fh,"<?xml version=\"1.0\" standalone=\"no\" ?>\n");

    if(!WriteXml(FirstBlock))
    {
        Failure=true;
        return false;
    }

    fclose(fh);

    return true;
}

bool TinyCFG::WriteCFGUsingParentCFG(class TinyCFG *OrgCFG)
{
    if(OrgCFG->fh==NULL)
        return false;

    fh=OrgCFG->fh;
    XMLIndent=OrgCFG->XMLIndent;

    if(!WriteXml(FirstBlock))
    {
        Failure=true;
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::WriteXml
 *
 * SYNOPSIS:
 *    bool TinyCFG::WriteXml(struct TinyCFG_Entry *First);
 *
 * PARAMETERS:
 *    First           [I] -- The link list to process
 *
 * FUNCTION:
 *    This method takes and goes through all the registered types and makes
 *    an XML config file.
 *
 * RETURNS:
 *    true -- Things worked out.
 *    false -- There was an error building the XML.
 *
 * NOTES:
 *    On failure the public var 'Failure" is set to true.  You can use this
 *    to register all your data members and then check 'Failure' to see if
 *    everything worked out.
 *
 * SEE ALSO:
 *    TinyCFG::MakeXml(), TinyCFG::SaveCFGFile()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (27 Feb 2020)
 *       Created
 ******************************************************************************/
bool TinyCFG::WriteXml(struct TinyCFG_Entry *First)
{
    struct TinyCFG_Entry *Entry;
    struct TinyCFG_RegData *Data;
    string OutString;

    for(Entry=First;Entry!=NULL;Entry=Entry->Next)
    {
        /* Start a new block */
        WriteXMLOpenElement(Entry->XmlName.c_str());

        /* Process any data */
        for(Data=Entry->FirstData;Data!=NULL;Data=Data->Next)
        {
            if(Data->Data->SaveData(OutString))
            {
                WriteXMLOpenDataElement(Data->Data->XmlName.c_str());
                WriteXMLEscapedString(OutString.c_str());
                WriteXMLCloseDataElement(Data->Data->XmlName.c_str());
            }
            else
            {
                WriteXMLOpenElement(Data->Data->XmlName.c_str());
                Data->Data->SaveElement(this);
                WriteXMLCloseElement(Data->Data->XmlName.c_str());
            }
        }

        /* Process any children */
        if(Entry->FirstSub!=NULL)
            WriteXml(Entry->FirstSub);

        WriteXMLCloseElement(Entry->XmlName.c_str());
     }
     return true;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::WriteXMLOpenElement
 *
 * SYNOPSIS:
 *    void TinyCFG::WriteXMLOpenElement(const char *ElementName);
 *
 * PARAMETERS:
 *    ElementName [I] -- The element name to write.  This must be valid XML
 *
 * FUNCTION:
 *    This function opens a new element.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::WriteXMLCloseElement(), TinyCFG::WriteXMLEscapedString()
 ******************************************************************************/
void TinyCFG::WriteXMLOpenElement(const char *ElementName)
{
    fprintf(fh,"%*s<%s>\n",XMLIndent,"",ElementName);
    XMLIndent+=XMLINDEXSIZE;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::WriteXMLCloseElement
 *
 * SYNOPSIS:
 *    void TinyCFG::WriteXMLCloseElement(const char *ElementName);
 *
 * PARAMETERS:
 *    ElementName [I] -- The element name to write.  This must be the same as
 *          what was sent into WriteXMLOpenElement()
 *
 * FUNCTION:
 *    This function closes an element that was opened with
 *    WriteXMLOpenElement().  The order is important as this just writes the
 *    line to the output file.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::WriteXMLOpenElement()
 ******************************************************************************/
void TinyCFG::WriteXMLCloseElement(const char *ElementName)
{
    XMLIndent-=XMLINDEXSIZE;
    fprintf(fh,"%*s</%s>\n",XMLIndent,"",ElementName);
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::WriteXMLOpenDataElement
 *
 * SYNOPSIS:
 *    void TinyCFG::WriteXMLOpenDataElement(const char *ElementName);
 *
 * PARAMETERS:
 *    ElementName [I] -- The element name to write.  This must be valid XML
 *
 * FUNCTION:
 *    This function opens an xml element for writing of data.  The only
 *    difference between this and WriteXMLOpenElement() is that it does
 *    not indent the same (this doesn't add a new line after the element).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::WriteXMLOpenElement(), TinyCFG::WriteXMLCloseDataElement()
 ******************************************************************************/
void TinyCFG::WriteXMLOpenDataElement(const char *ElementName)
{
    fprintf(fh,"%*s<%s>",XMLIndent,"",ElementName);
    XMLIndent+=XMLINDEXSIZE;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::WriteXMLCloseDataElement
 *
 * SYNOPSIS:
 *    void TinyCFG::WriteXMLCloseDataElement(const char *ElementName);
 *
 * PARAMETERS:
 *    ElementName [I] -- The element name to write.  This must be the same as
 *          what was sent into WriteXMLOpenDataElement()
 *
 * FUNCTION:
 *    This function closes an element that was opened with
 *    WriteXMLOpenDataElement().  The order is important as this just writes the
 *    line to the output file.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::WriteXMLOpenElement()
 ******************************************************************************/
void TinyCFG::WriteXMLCloseDataElement(const char *ElementName)
{
    XMLIndent-=XMLINDEXSIZE;
    fprintf(fh,"</%s>\n",ElementName);
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::WriteXMLEscapedString
 *
 * SYNOPSIS:
 *    void TinyCFG::WriteXMLEscapedString(const char *OutString);
 *
 * PARAMETERS:
 *    OutString [I] -- The string to output
 *
 * FUNCTION:
 *    This function outputs a string for xml data.  It escapes the string
 *    correctly for XML.  It will convert the following:
 *          & -- &amp;
 *          < -- &lt;
 *          > -- &gt;
 *          0-31 -- &#xx;
 *          127-255 -- &#xxx;
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TinyCFG::WriteXMLOpenElement(), UnEscapedString()
 ******************************************************************************/
void TinyCFG::WriteXMLEscapedString(const char *OutString)
{
    char buff[100];
    char tmp[100];
    char *InsertPoint;
    int r;
    int Len;

    InsertPoint=buff;
    while(*OutString!=0)
    {
        if(*OutString=='&')
        {
            *InsertPoint++='&';
            *InsertPoint++='a';
            *InsertPoint++='m';
            *InsertPoint++='p';
            *InsertPoint++=';';
        }
        else if(*OutString=='<')
        {
            *InsertPoint++='&';
            *InsertPoint++='l';
            *InsertPoint++='t';
            *InsertPoint++=';';
        }
        else if(*OutString=='>')
        {
            *InsertPoint++='&';
            *InsertPoint++='g';
            *InsertPoint++='t';
            *InsertPoint++=';';
        }
        else if(*OutString<' ' || *OutString>=127)
        {
            sprintf(tmp,"&#%d;",(unsigned char)*OutString);    // 6 char max
            Len=strlen(tmp);
            for(r=0;r<Len;r++)
                *InsertPoint++=tmp[r];
        }
        else
        {
            *InsertPoint++=*OutString;
        }

        if(InsertPoint>&buff[sizeof(buff)-7])   // 7 because the worst esc is ascII + \0 for end of string
        {
            *InsertPoint++=0;
            fprintf(fh,"%s",buff);
            InsertPoint=buff;
        }
        OutString++;
    }
    if(InsertPoint!=buff)
    {
        *InsertPoint++=0;
        fprintf(fh,"%s",buff);
    }
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::WriteDataElement
 *
 * SYNOPSIS:
 *    void TinyCFG::WriteDataElement(const char *XmlName,const char *Value);
 *
 * PARAMETERS:
 *    XmlName [I] -- The name of the XML tag to wrap the data in
 *    Value [I] -- The string to output for this data.  This is XML escaped.
 *
 * FUNCTION:
 *    This function starts a data element and then writes the data out.
 *
 *    This is ment to be called by anyone adding their on types.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TinyCFG::WriteDataElement(const char *XmlName,const char *Value)
{
    WriteXMLOpenDataElement(XmlName);
    WriteXMLEscapedString(Value);
    WriteXMLCloseDataElement(XmlName);
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::UnEscapedString
 *
 * SYNOPSIS:
 *    std::string TinyCFG::UnEscapedString(std::string &RetStr,char *InString);
 *
 * PARAMETERS:
 *    RetStr [O] -- The string we are returning.  The converted string.
 *    InString [I] -- The string to convert
 *
 * FUNCTION:
 *    This function unconverts a string for xml data.  It unescapes the string
 *    correctly for XML.  It will convert the following:
 *          &amp; -- &
 *          &lt; -- <
 *          &gt; -- >
 *          &#xx; -- 0-31
 *          &#xxx; -- 127-255
 *
 * RETURNS:
 *    A new string that has been unescaped.
 *
 * SEE ALSO:
 *    TinyCFG::WriteXMLEscapedString()
 ******************************************************************************/
void TinyCFG::UnEscapedString(std::string &RetStr,const char *OutString)
{
    int Len;
    const char *Pos;
    char c;

    RetStr="";

    Len=strlen(OutString);
    RetStr.reserve(Len);

    Pos=OutString;
    while(*Pos!=0)
    {
        c=*Pos;
        if(strncmp(Pos,"&amp;",5)==0)
        {
            c='&';
            Pos+=4;
        }
        else if(strncmp(Pos,"&lt;",4)==0)
        {
            c='<';
            Pos+=3;
        }
        else if(strncmp(Pos,"&gt;",4)==0)
        {
            c='>';
            Pos+=3;
        }
        else if(strncmp(Pos,"&#",2)==0)
        {
            Pos+=2;
            c=strtol(Pos,NULL,10);
            while(*Pos!=';' && *Pos!=0)
                Pos++;

            /* If we are at the end of the string then back up by 1 */
            if(*Pos==0)
                Pos--;
        }
        RetStr.push_back(c);
        Pos++;
    }
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::LoadCFGFile
 *
 * SYNOPSIS:
 *    bool TinyCFG::LoadCFGFile(const char *FileName,int MaxFileSize=1000000);
 *
 * PARAMETERS:
 *    FileName [I] -- What file to load
 *    MaxFileSize [I] -- The config file is loaded into ram.  What is the
 *                       biggest it can be (how much ram will we allocate).
 *
 * FUNCTION:
 *    This method loads a CFG File.  It sets all any registed vars that are
 *    found in the cfg file.
 *
 * RETURNS:
 *    true -- The file was loaded correctly and the registered types have had
 *            any data that was found placed in them.
 *    false -- There was an error loading the file or processing the xml.
 *             Not finding all (or any) registed types is not considered a
 *             failure.
 *
 * NOTES:
 *    On failure the public var 'Failure" is set to true.  You can use this
 *    to register all your data members and then check 'Failure' to see if
 *    everything worked out.
 *
 * SEE ALSO:
 *    
 *    TinyCFG::SaveCFGFile()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (28 Feb 2020)
 *       Created
 ******************************************************************************/
bool TinyCFG::LoadCFGFile(const char *FileName,int MaxFileSize)
{
    int ReadFileSize;

    fh=fopen(FileName,"rb");
    if(fh==NULL)
        return false;

    /* NOTE:
        I was going to do this by using a floating read buffer, but decided
        that the XML file is unlikely to be bigger than a meg (and that's
        a big file).  A meg is small these days, so why go to the bother.
        I know that means that there is now an edge case where it will
        be unusable, but that's an edge case that I am ignoring. */

    /* Get the size of the file */
    fseek(fh,0,SEEK_END);
    ReadFileSize=ftell(fh);
    fseek(fh,0,SEEK_SET);

    if(ReadFileSize+1>MaxFileSize)
    {
        fclose(fh);
        return false;
    }

    ReadBuff=(char *)malloc(ReadFileSize+1);
    if(ReadBuff==NULL)
    {
        fclose(fh);
        return false;
    }

    if(fread(ReadBuff,ReadFileSize,1,fh)!=1)
    {
        fclose(fh);
        free(ReadBuff);
        return false;
    }

    fclose(fh);

    ReadBuff[ReadFileSize]=0;   // It's a string
    ReadBuffEnd=ReadBuff+ReadFileSize;
    ReadBuffStartAt=ReadBuff;

    try
    {
        GetAndSetFromXml();
    }
    catch(...)
    {
printf("ERROR\n");
        Failure=true;
    }

    free(ReadBuff);

    return true;
}

void TinyCFG::ConnectToParentCFGForReading(class TinyCFG *OrgCFG)
{
    ReadBuff=OrgCFG->LoadDataDataStart; // This is at the start of data for the current element (<tag>x<--- Here)
    ReadBuffEnd=OrgCFG->ReadPoint;  // This is at the </ in at the end of the current block (----></tag>)
    ReadBuffStartAt=ReadBuff;
}

bool TinyCFG::ReadNextCFG(void)
{
    bool RetValue;
    try
    {
        RetValue=GetAndSetFromXml();

        /* Update the read point so we continue from where we left off */
        ReadBuffStartAt=ReadPoint;
    }
    catch(...)
    {
printf("ERROR\n");
        Failure=true;
        RetValue=false;
    }

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::GetAndSetFromXml
 *
 * SYNOPSIS:
 *    void TinyCFG::GetAndSetFromXml(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This method is a recursive function that process some xml stored in
 *    tiny nodes.  It called any registered types LoadElement() or LoadData()
 *    methods as the matching xml elements are found.
 *
 * RETURNS:
 *    true -- When finished this block
 *    false -- We hit the end.  Part of the data may have been processed (or
 *             not)
 *
 * SEE ALSO:
 *    TinyCFG::LoadCFGFile()
 *******************************************************************************
 * REVISION HISTORY:
 *    PaulHutchinson (24 Jun 2003)
 *       Created
 ******************************************************************************/
bool TinyCFG::GetAndSetFromXml(void)
{
    char c;
    bool InElement;
    bool InComment;
    char *ElementStart;
    struct TinyCFG_RegData *DataEntry;
    char *DataStart;
    string DataString;
    bool DoingData;
    int Level;

    CurrentReadLevel=NULL;
    InComment=false;
    InElement=false;
    DoingData=false;
    ReadPoint=ReadBuffStartAt;
    Level=0;
    while(ReadPoint<ReadBuffEnd)
    {
        c=*ReadPoint;

        if(InComment)
        {
            /* See if this is the end of the comment */
            if(strncmp(ReadPoint,"-->",3)==0)
            {
                ReadPoint+=3;
                InComment=false;
                continue;
            }
            ReadPoint++;
            continue;
        }
        if(strncmp(ReadPoint,"<!--",4)==0)
        {
            /* Start of a comment */
            InComment=true;
            ReadPoint+=4;
            continue;
        }

        if(InElement)
        {
            if(c==' ' || c=='\t' || c=='\n' || c=='>')
            {
                /* End of the element name */
                *ReadPoint=0;
            }
            if(c=='>')
            {
                InElement=false;
                if(strncmp(ElementStart,"?xml",4)==0)
                {
                    /* It's the declaration.  Ignore */
                }
                else
                {
                    if(*ElementStart=='/')
                    {
                        /* End tag */
                        /* If we get below 0 then we have left the current
                           block */
                        Level--;
                        if(Level<=0)
                            return true;

                        if(DoingData)
                        {
                            *(ElementStart-1)=0;    // End the data block (kill the <)

                            DataEntry=FindDataEntry(ElementStart+1);
                            if(DataEntry!=NULL)
                            {
                                LoadDataDataStart=DataStart;
                                LoadDataReadPoint=LoadDataDataStart;
                                if(!DataEntry->Data->LoadElement(this))
                                {
                                    /* Copy to a string (because the old
                                       API used a string instead of a C
                                       String */
//                                    DataString=DataStart;
                                    UnEscapedString(DataString,DataStart);
                                    DataEntry->Data->LoadData(DataString);
                                }
                            }
                            DoingData=false;
                        }
                        else
                        {
                            ExitElementLevel(ElementStart+1);
                        }
                    }
                    else
                    {
                        /* Start tag */
                        Level++;

                        DataStart=ReadPoint+1;

                        /* See if this element matches any data */
                        DataEntry=FindDataEntry(ElementStart);
                        if(DataEntry==NULL)
                        {
                            if(!EnterElementLevel(ElementStart))
                            {
                                SkipToEndTag(ElementStart);
                                continue;
                            }
                        }
                        else
                        {
                            DoingData=true;
                            /* Skip the data (we are going to send it to
                               the handler later) */
                            SkipToEndTag(ElementStart);
                            continue;
                        }
                    }
                }
            }
        }
        else
        {
            if(c=='<')
            {
                ElementStart=ReadPoint+1;
                InElement=true;
            }
        }

        ReadPoint++;
    }

    return false;
}

bool TinyCFG::EnterElementLevel(const char *Name)
{
    struct TinyCFG_Entry *Entry;
    struct TinyCFG_Entry *SearchLevel;

    if(CurrentReadLevel==NULL)
    {
        /* Search the top level */
        SearchLevel=FirstBlock;
    }
    else
    {
        SearchLevel=CurrentReadLevel->FirstSub;
    }

    /* See if we can find this element */
    for(Entry=SearchLevel;Entry!=NULL;Entry=Entry->Next)
    {
        if(Entry->XmlName==Name)
        {
            CurrentReadLevel=Entry;
            return true;
        }
    }
    return false;
}

void TinyCFG::ExitElementLevel(const char *Name)
{
    if(CurrentReadLevel==NULL)
        return;

    if(CurrentReadLevel->XmlName==Name)
        CurrentReadLevel=CurrentReadLevel->ParentSubLevel;
}

struct TinyCFG_RegData *TinyCFG::FindDataEntry(const char *DataName)
{
    struct TinyCFG_RegData *Entry;

    if(CurrentReadLevel==NULL)
        return NULL;

    for(Entry=CurrentReadLevel->FirstData;Entry!=NULL;Entry=Entry->Next)
    {
        if(Entry->Data->XmlName==DataName)
        {
            return Entry;
        }
    }
    return NULL;
}

bool TinyCFG::SkipToEndTag(const char *ElementName)
{
    int Level;
    char c;
    int Len;
    bool InComment;

    Level=0;

    Len=strlen(ElementName);

    InComment=false;
    while(ReadPoint<ReadBuffEnd)
    {
        c=*ReadPoint;
        if(InComment)
        {
            if(c=='-')
            {
                if(strncmp(ReadPoint,"-->",3)==0)
                {
                    InComment=false;
                    continue;
                }
            }
        }
        if(c=='<')
        {
            if(ReadPoint[1]=='/')
            {
                if(Level==0)
                {
                    if(&ReadPoint[2]>=ReadBuffEnd)
                        throw(0);

                    /* Check if this is the end of the block we are trying
                       to end */
                    if(strncmp(&ReadPoint[2],ElementName,Len)==0)
                    {
                        /* Found it.  Done */
                        return true;
                    }
                }
                Level--;
                if(Level<0)
                    throw(0);
            }
            else if(ReadPoint[1]=='!')
            {
                /* Comment */
                InComment=true;
            }
            else
            {
                Level++;
            }
        }
        ReadPoint++;
    }
    throw(0);
}

bool TinyCFG::CheckXMLName(const char *CheckName)
{
    char c;

    /* String must only have A-Z and 0-9 and _*/
    while(*CheckName!=0)
    {
        c=*CheckName;
        if((c<'A' || c>'Z') && (c<'a' || c>'z') && (c<'0' || c>'9') && c!='_')
            return false;
        CheckName++;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::ReadDataElement()
 *
 * SYNOPSIS:
 *    const char *TinyCFG::ReadDataElement(const char *DataElementName);
 *
 * PARAMETERS:
 *    DataElementName [I] -- The name of the element to search for.
 *
 * FUNCTION:
 *    This function starts at the top of the current element block that started
 *    the LoadElement() call.
 *
 *    So for example if you registered a data type of "MyStuff" and added
 *    a LoadElement() callback.  Then for this XML:
 *
 *    <Root>
 *      <MyStuff>
 *          <Element1>Value1</Element1>
 *          <Element2>Value2</Element2>
 *          <Element3>Value3</Element3>
 *          <Element1>Value4</Element1>
 *          <Element2>Value5</Element2>
 *          <Element3>Value6</Element3>
 *      </MyStuff>
 *    </Root>
 *
 *    When LoadElement() is called it will start searching at the top of
 *    the <MyStuff> block.
 *
 *    If you call ReadDataElement("Element2") it will return "Value2".
 *    If you call it again it will return "Value2" again and NOT "Value5".
 *
 * RETURNS:
 *    The data from the 'DataElementName' element or NULL if it was not found.
 *    This is valid until the next call to this function.
 *
 * SEE ALSO:
 *    ReadNextDataElement()
 ******************************************************************************/
const char *TinyCFG::ReadDataElement(const char *DataElementName)
{
    char *StartOfElementData;
    char *p;

    /* Start at the top of the data block again */
    LoadDataReadPoint=LoadDataDataStart;

    return ReadNextDataElement(DataElementName);
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::ReadNextDataElement()
 *
 * SYNOPSIS:
 *    const char *TinyCFG::ReadNextDataElement(const char *DataElementName);
 *
 * PARAMETERS:
 *    DataElementName [I] -- The name of the element to search for.
 *
 * FUNCTION:
 *    This function search from the current position in the element block that
 *    started the LoadElement() call.
 *
 *    This continues searching from the last element found instead of the top
 *    of the element block.
 *
 *    So for example if you registered a data type of "MyStuff" and added
 *    a LoadElement() callback.  Then for this XML:
 *
 *    <Root>
 *      <MyStuff>
 *          <Element1>Value1</Element1>
 *          <Element2>Value2</Element2>
 *          <Element3>Value3</Element3>
 *          <Element1>Value4</Element1>
 *          <Element2>Value5</Element2>
 *          <Element3>Value6</Element3>
 *      </MyStuff>
 *    </Root>
 *
 *    When LoadElement() is called it will start searching at the top of
 *    the <MyStuff> block.
 *
 *    If you call ReadNextDataElement("Element2") it will return "Value2".
 *    If you call it again it search for the NEXT "Element2" and will return
 *    will return "Value5" this time.
 *    If you call it again it will return NULL because that are no more matches.
 *
 * RETURNS:
 *    The data from the 'DataElementName' element or NULL if there where no
 *    more found.  This is only valid until the call to this to this funciton.
 *
 * NOTES:
 *    WARNING: the current position is not reset if the element name changes.
 *    This means that if you search for "Element2" and then search for
 *    "Element1" it will skip "Value1" and instead return "Value4" because
 *    it started it's search from the end of "Element2".
 *
 *    If you want to reset the current position you can use ReadDataElement()
 *    for the first match and then switch to ReadNextDataElement()
 *
 * SEE ALSO:
 *    TinyCFG::ReadDataElement()
 ******************************************************************************/
const char *TinyCFG::ReadNextDataElement(const char *DataElementName)
{
    char *StartOfElementData;
    char *p;
    static string RetStr;

    /* Ok, we search for the next element named 'DataElementName' that is
       inside this levels data.  Start from 'LoadDataReadPoint' */
    StartOfElementData=FindElementAtThisLevel(DataElementName,false);
    if(StartOfElementData==NULL)
        return NULL;

    /* Found, now we need to make it a string */
    p=StartOfElementData;
    while(*p!='<' && *p!=0)
    {
        LoadDataReadPoint++;
        p++;
    }
    *p=0;

    /* Skip the end tag (DEBUG PAUL: This does not handle embedded tags
       (comments or elements) */
    while(LoadDataReadPoint<ReadBuffEnd && *LoadDataReadPoint!='>')
        LoadDataReadPoint++;
    if(*LoadDataReadPoint!='>')
        return NULL;

    LoadDataReadPoint++;    // Move past the >

    UnEscapedString(RetStr,StartOfElementData);

    return RetStr.c_str();
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::ReadFirstTag()
 *
 * SYNOPSIS:
 *    bool ReadFirstTag(std::string &XmlName,std::string &DataElement);
 *
 * PARAMETERS:
 *    XmlName [O] -- The tag that was read
 *    DataElement [O] -- The data in this xml tags element.
 *
 * FUNCTION:
 *    This function starts at the top of the current element block that started
 *    the LoadElement() call.
 *
 *    So for example if you registered a data type of "MyStuff" and added
 *    a LoadElement() callback.  Then for this XML:
 *
 *    <Root>
 *      <MyStuff>
 *          <Element1>Value1</Element1>
 *          <Element2>Value2</Element2>
 *          <Element3>Value3</Element3>
 *          <Element1>Value4</Element1>
 *          <Element2>Value5</Element2>
 *          <Element3>Value6</Element3>
 *      </MyStuff>
 *    </Root>
 *
 *    When LoadElement() is called it will start searching at the top of
 *    the <MyStuff> block.
 *
 *    If you call ReadFirstTag(Tag,Data) then Tag will be "Element1" and
 *    Data will be "Value1".  If you call it again it will return the same
 *    tag and element data.
 *
 * RETURNS:
 *    true -- A tag was found
 *    false -- There where no tags to read
 *
 * SEE ALSO:
 *    ReadNextTag(), ReadNextDataElement()
 ******************************************************************************/
bool TinyCFG::ReadFirstTag(std::string &XmlName,std::string &DataElement)
{
    char *StartOfElementData;
    char *p;

    /* Start at the top of the data block again */
    LoadDataReadPoint=LoadDataDataStart;

    return ReadNextTag(XmlName,DataElement);
}

/*******************************************************************************
 * NAME:
 *    TinyCFG::ReadNextTag()
 *
 * SYNOPSIS:
 *    bool TinyCFG::ReadNextTag(std::string &XmlName,std::string &DataElement);
 *
 * PARAMETERS:
 *    XmlName [O] -- The tag that was read
 *    DataElement [O] -- The data in this xml tags element.
 *
 * FUNCTION:
 *    This function search from the current position in the element block that
 *    started the LoadElement() call.
 *
 *    This continues searching from the last element found instead of the top
 *    of the element block.
 *
 *    So for example if you registered a data type of "MyStuff" and added
 *    a LoadElement() callback.  Then for this XML:
 *
 *    <Root>
 *      <MyStuff>
 *          <Element1>Value1</Element1>
 *          <Element2>Value2</Element2>
 *          <Element3>Value3</Element3>
 *          <Element1>Value4</Element1>
 *          <Element2>Value5</Element2>
 *          <Element3>Value6</Element3>
 *      </MyStuff>
 *    </Root>
 *
 *    When LoadElement() is called it will start searching at the top of
 *    the <MyStuff> block.
 *
 *    If you call ReadNextTag(Tag,Data) then Tag will be set to "Element1" and
 *    Data will be set to "Value1".
 *    If you call it again it will set Tag to "Element2" and Data to "Value2".
 *    The return sequence will be (from the start):
 *          'Tag'       'Data'
 *          Element1    Value1
 *          Element2    Value2
 *          Element3    Value3
 *          Element1    Value4
 *          Element2    Value5
 *          Element3    Value6
 *    The next call will return false and Tag and Data will not be set.
 *
 * RETURNS:
 *    true -- We loaded the next tag
 *    false -- There where no more tags to load.
 *
 * SEE ALSO:
 *    TinyCFG::ReadFirstTag()
 ******************************************************************************/
bool TinyCFG::ReadNextTag(const char **XmlName,const char **DataElement)
{
    char *Tag;
    char *Data;
    char *EndTag;
    static string RetStr;

    if(!FindNextTagStartAndEndAtThisLevel(&Tag,&Data,&EndTag))
        return false;

    UnEscapedString(RetStr,Data);

    *XmlName=Tag;
    *DataElement=RetStr.c_str();

    return true;
}
bool TinyCFG::ReadNextTag(std::string &XmlName,std::string &DataElement)
{
    char *Tag;
    char *Data;
    char *EndTag;

    if(!FindNextTagStartAndEndAtThisLevel(&Tag,&Data,&EndTag))
        return false;

    XmlName=Tag;
    UnEscapedString(DataElement,Data);

    return true;
}

bool TinyCFG::FindNextTagStartAndEndAtThisLevel(char **Start,char **Data,char **End)
{
    int Level;
    char c;
    bool InComment;

    *Start=NULL;
    *Data=NULL;
    *End=NULL;

    Level=0;

    InComment=false;
    while(LoadDataReadPoint<ReadBuffEnd)
    {
        c=*LoadDataReadPoint;
        if(InComment)
        {
            if(c=='-')
            {
                if(strncmp(LoadDataReadPoint,"-->",3)==0)
                {
                    InComment=false;
                    continue;
                }
            }
        }
        if(c=='<' || c==0)  // 0 because when we return a string with convert the '<' into a '\0'.  This ONLY applies to searching user type blocks.  The main parser NEVER goes backward
        {
            if(LoadDataReadPoint[1]=='/')
            {
                if(Level==1)
                {
                    /* Check if this is the element we are looking for */
                    if(&LoadDataReadPoint[2]>=ReadBuffEnd)
                        throw(0);

                    *LoadDataReadPoint=0;   // String it (Data)

                    LoadDataReadPoint+=2;
                    *End=LoadDataReadPoint;

                    /* Find the end of the element tag */
                    while(LoadDataReadPoint<ReadBuffEnd &&
                            (*LoadDataReadPoint!='>' && *LoadDataReadPoint!=0))
                    {
                        LoadDataReadPoint++;
                    }
                    if(LoadDataReadPoint>=ReadBuffEnd)
                        throw(0);

                    *LoadDataReadPoint=0;   // String it (End)
                    LoadDataReadPoint++;

                    /* See if this really was the expected tag (if not
                       then we didn't find this block at all) */
                    if(strcmp(*Start,*End)!=0)
                        return false;

                    return true;
                }
                Level--;
                if(Level<0)
                    return false;
            }
            else if(LoadDataReadPoint[1]=='!')
            {
                /* Comment */
                InComment=true;
            }
            else
            {
                if(Level==0)
                {
                    /* Find the end of the element tag */
                    *Start=LoadDataReadPoint+1;
                    while(LoadDataReadPoint<ReadBuffEnd &&
                            (*LoadDataReadPoint!='>' && *LoadDataReadPoint!=0))
                    {
                        LoadDataReadPoint++;
                    }
                    if(LoadDataReadPoint>=ReadBuffEnd)
                        throw(0);
                    *LoadDataReadPoint=0;   // String it (Start)
                    *Data=LoadDataReadPoint+1;
                }
                Level++;
            }
        }
        LoadDataReadPoint++;
    }
    return false;
}

char *TinyCFG::FindElementAtThisLevel(const char *ElementName,bool EndTag)
{
    int Level;
    char c;
    int Len;
    bool InComment;

    Level=0;

    Len=strlen(ElementName);

    InComment=false;
    while(LoadDataReadPoint<ReadBuffEnd)
    {
        c=*LoadDataReadPoint;
        if(InComment)
        {
            if(c=='-')
            {
                if(strncmp(LoadDataReadPoint,"-->",3)==0)
                {
                    InComment=false;
                    continue;
                }
            }
        }
        if(c=='<' || c==0)  // 0 because when we return a string with convert the '<' into a '\0'.  This ONLY applies to searching user type blocks.  The main parser NEVER goes backward
        {
            if(LoadDataReadPoint[1]=='/')
            {
                if(EndTag && Level==0)
                {
                    /* Check if this is the element we are looking for */
                    if(&LoadDataReadPoint[2]>=ReadBuffEnd)
                        throw(0);

                    if(strncmp(&LoadDataReadPoint[2],ElementName,Len)==0)
                    {
                        /* Found it.  Find the end of the element tag */
                        while(LoadDataReadPoint<ReadBuffEnd &&
                                *LoadDataReadPoint!='>')
                        {
                            LoadDataReadPoint++;
                        }
                        if(*LoadDataReadPoint!='>')
                            throw(0);

                        LoadDataReadPoint++;    // Move past the >

                        return LoadDataReadPoint;
                    }
                }
                Level--;
                if(Level<0)
                    return NULL;
            }
            else if(LoadDataReadPoint[1]=='!')
            {
                /* Comment */
                InComment=true;
            }
            else
            {
                if(!EndTag && Level==0)
                {
                    /* Check if this is the element we are looking for */
                    if(strncmp(&LoadDataReadPoint[1],ElementName,Len)==0)
                    {
                        /* Found it.  Find the end of the element tag */
                        while(LoadDataReadPoint<ReadBuffEnd &&
                                *LoadDataReadPoint!='>')
                        {
                            LoadDataReadPoint++;
                        }
                        if(*LoadDataReadPoint!='>')
                            throw(0);

                        LoadDataReadPoint++;    // Move to the start of data

                        return LoadDataReadPoint;
                    }
                }
                Level++;
            }
        }
        LoadDataReadPoint++;
    }
    throw(0);
}

//////////////////////////////////////////////////////////////////////////////////
#if 0
struct StructData
{
    int a;
    int b;
};

class TestStructList : public TinyCFGBaseData
{
    public:
        list<struct StructData> *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};
bool TestStructList::LoadElement(class TinyCFG *CFG)
{
    const char *Str;
    struct StructData NewData;
    class TinyCFG SubCFG("Data");

    Ptr->clear();

    SubCFG.StartBlock("Second");
    SubCFG.Register("A",NewData.a);
    SubCFG.EndBlock();
    SubCFG.Register("B",NewData.b);

    NewData.a=0;
    NewData.b=0;

    SubCFG.ConnectToParentCFGForReading(CFG);

    while(SubCFG.ReadNextCFG())
        Ptr->push_back(NewData);

    return true;
}
bool TestStructList::SaveElement(class TinyCFG *CFG)
{
    list<struct StructData>::iterator i;
    char buff[100];
    class TinyCFG SubCFG("Data");
    struct StructData NewData;

    SubCFG.StartBlock("Second");
    SubCFG.Register("A",NewData.a);
    SubCFG.EndBlock();
    SubCFG.Register("B",NewData.b);

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        NewData=*i;
        SubCFG.WriteCFGUsingParentCFG(CFG);
    }
    return true;
}

bool RegisterStructDataList(class TinyCFG &cfg,const char *XmlName,
        list<struct StructData> &Data)
{
    class TestStructList *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new TestStructList;
    }
    catch(std::bad_alloc)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}

class TestStruct
{
    public:
        string GreetingString;
        string ExitString;
        char ACharBuff[100];
        list<string> AStringList;
        list<int> AIntList;
        list<struct StructData> AStructDataList;
        struct _SubStruct
        {
            string Menu1;
            string CloseHelp;
            int Number;
            unsigned int UnNumber;
        } SubStruct;
        void RegisterAllMembers(class TinyCFG &cfg)
        {
            cfg.StartBlock("TestStruct");
            cfg.Register("Greeting",GreetingString);
            cfg.Register("Goodbye",ExitString);
            cfg.Register("List",AStringList);
            cfg.Register("Int",AIntList);
            cfg.Register("CharBuff",ACharBuff,sizeof(ACharBuff));
            cfg.StartBlock("SubStruct");
            cfg.Register("MainMenuBar",SubStruct.Menu1);
            cfg.Register("CloseHint",SubStruct.CloseHelp);
            cfg.Register("Number",SubStruct.Number);
            cfg.Register("UnNumber",SubStruct.UnNumber);
            cfg.EndBlock();
            RegisterStructDataList(cfg,"StructData",AStructDataList);
            cfg.EndBlock();
        }
};

int main(void)
{
    class TestStruct ts;
    class TinyCFG o("Root");
    string str;
    int vari;
    struct StructData sd;
    list<string>::iterator it;
    list<int>::iterator it_i;
    list<struct StructData>::iterator it_sd;

    ts.RegisterAllMembers(o);

    ts.GreetingString="Hi there";
    ts.ExitString="Bye Bye";
    strcpy(ts.ACharBuff,"   Test\nstring   ");
    ts.SubStruct.Menu1="File";
    ts.SubStruct.CloseHelp="Click here to exit the hell out";
    ts.SubStruct.Number=-99;
    ts.SubStruct.UnNumber=-1;
    str="Test1";ts.AStringList.push_back(str);
    str="Test2";ts.AStringList.push_back(str);
    str="Test3";ts.AStringList.push_back(str);
    vari=100;ts.AIntList.push_back(vari);
    vari=111;ts.AIntList.push_back(vari);
    vari=666;ts.AIntList.push_back(vari);
    sd.a=1;sd.b=2;ts.AStructDataList.push_back(sd);
    sd.a=3;sd.b=4;ts.AStructDataList.push_back(sd);
    sd.a=5;sd.b=6;ts.AStructDataList.push_back(sd);

    o.SaveCFGFile("test2.CFG");

    system("cat test2.CFG");

    /* Load */
    class TinyCFG i("Root");
    class TestStruct newts;

    newts.RegisterAllMembers(i);
    i.LoadCFGFile("test2.CFG");

    printf("GreetingString=%s\n",newts.GreetingString.c_str());
    printf("ExitString=%s\n",newts.ExitString.c_str());
    printf("ACharBuff=%s\n",newts.ACharBuff);
    printf("SubStruct.Menu1=%s\n",newts.SubStruct.Menu1.c_str());
    printf("SubStruct.CloseHelp=%s\n",newts.SubStruct.CloseHelp.c_str());
    printf("SubStruct.Number=%d\n",newts.SubStruct.Number);
    printf("SubStruct.UnNumber=%X\n",newts.SubStruct.UnNumber);

    printf("AStringList:\n");
    for(it=newts.AStringList.begin();it!=newts.AStringList.end();it++)
        printf("   %s\n",it->c_str());

    printf("AIntList:\n");
    for(it_i=newts.AIntList.begin();it_i!=newts.AIntList.end();it_i++)
        printf("   %d\n",*it_i);

    printf("StructData:\n");
    for(it_sd=newts.AStructDataList.begin();it_sd!=newts.AStructDataList.end();it_sd++)
        printf("   a=%d,b=%d\n",it_sd->a,it_sd->b);
}
#endif

/* DEBUG PAUL: Convert to something like this when TinyCFG has been updated to support it */
//    char buff[100];
//    char TagName[100];
//    while(CFG->ReadNextXMLTag(TagName,sizeof(TagName),buff,sizeof(buff)))
//    {
//        for(cmd=0;cmd<e_CmdMAX;cmd++)
//        {
//            if(strcmp(TagName,GetCmdName((e_CmdType)cmd))==0)
//            {
//                /* Found it */
//                ConvertString2KeySeq(&(*Ptr)[cmd],buff);
//                break;
//            }
//        }
//    }
//ReadData <--- Already exists
//CFG->FindXMLBlock("Name")             <- Searchs from the start of the call back block.  (moves cursor to just after the block start)
//CFG->FindNextXMLBlock("Name")         <- Searchs from the cursor in the call back block (only returns blocks at the curent indent). (moves cursor to just after the block start)
//CFG->ReadNextXMLTag(buff,size);       <- Reads the next XML tag from the current cursor (only returns blocks at the curent indent). (moves cursor to the end of tag)


/* DEBUG PAUL: Convert to something like this when TinyCFG has been updated to support it */
//    char buff[100];
//    for(cmd=0;cmd<e_CmdMAX;cmd++)
//    {
//        strcpy(buff[cmd],ConvertKeySeq2String(&(*Ptr)[cmd]));
//        CFG->WriteXMLTag(GetCmdName((e_CmdType)cmd),buff);
//    }
////CFG->WriteXMLStartBlock("Name");
////CFG->WriteXMLEndBlock();
////WriteData()  <--- Already exists

/* DEBUG PAUL: Things to look at:
    * Do we need LoadDataDataStart or can we just use the normal read pos?
    * Add headers to everything
*/
