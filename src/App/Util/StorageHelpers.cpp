/*******************************************************************************
 * FILENAME: StorageHelpers.cpp
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
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Util/StorageHelpers.h"
#include "App/Util/KeyValue.h"

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

//class ConnectionsOptions_TinyCFG : public TinyCFGBaseData
//{
//    public:
//        t_ConnectionsOptions *Ptr;
//        bool LoadElement(TiXmlElement *Element);
//        bool SaveElement(TiXmlElement *Element);
//};
//bool ConnectionsOptions_TinyCFG::LoadElement(TiXmlElement *Element)
//{
//    TiXmlElement *BlockElement;
//    TiXmlElement *ConnectionOptionsBlock;
//    TiXmlText *Text;
//    TiXmlNode *Node;
//    struct ConOptions ConOptions;
//    const char *Age;
//
//    Ptr->clear();
//
//    ConnectionOptionsBlock=Element->FirstChildElement();
//    while(ConnectionOptionsBlock!=NULL)
//    {
//        Age=ConnectionOptionsBlock->Attribute("Age");
//        if(Age!=NULL)
//            ConOptions.Age=strtoll(Age,NULL,10);
//        else
//            ConOptions.Age=0;
//        ConOptions.Options.clear();
//        BlockElement=ConnectionOptionsBlock->FirstChildElement();
//        while(BlockElement!=NULL)
//        {
//            Text=NULL;
//            Node=BlockElement->FirstChild();
//            while(Node!=NULL)
//            {
//                Text=Node->ToText();
//                if(Text!=NULL)
//                    break;
//                Node=Node->NextSibling();
//            }
//            if(Text!=NULL)
//            {
//                ConOptions.Options.insert(make_pair(BlockElement->Value(),
//                        Text->Value()));
//            }
//
//            BlockElement=BlockElement->NextSiblingElement();
//        }
//
//        Ptr->insert(make_pair(ConnectionOptionsBlock->Value(),ConOptions));
//
//        ConnectionOptionsBlock=ConnectionOptionsBlock->NextSiblingElement();
//    }
//
//   return true;
//}
//bool ConnectionsOptions_TinyCFG::SaveElement(TiXmlElement *Element)
//{
//    i_ConnectionsOptions i;
//    i_KVList kv;
//    TiXmlElement BlockElement("");
//    TiXmlElement ConnectionOptionsBlock("");
//    TiXmlText NewText("");
//
//    for(i=Ptr->begin();i!=Ptr->end();i++)
//    {
//        ConnectionOptionsBlock.Clear();
//        ConnectionOptionsBlock.SetAttribute("Age",i->second.Age);
//        ConnectionOptionsBlock.SetValue(i->first.c_str());
//
//        for(kv=i->second.Options.begin();kv!=i->second.Options.end();kv++)
//        {
//            BlockElement.Clear();
//            BlockElement.SetValue(kv->first.c_str());
//            NewText.SetValue(kv->second.c_str());
//            BlockElement.InsertEndChild(NewText);
//            ConnectionOptionsBlock.InsertEndChild(BlockElement);
//        }
//
//        Element->InsertEndChild(ConnectionOptionsBlock);
//    }
//
//    return true;
//}
//
//bool RegisterConnectionOptions_TinyCFG(class TinyCFG &cfg,const char *XmlName,
//      t_ConnectionsOptions &Data)
//{
//    class ConnectionsOptions_TinyCFG *NewDataClass;
//
//    /* Make a new class to handle this new piece of data */
//    try
//    {
//        NewDataClass=new ConnectionsOptions_TinyCFG;
//    }
//    catch(std::bad_alloc const &)
//    {
//        return false;
//    }
//
//    /* Setup the data */
//    NewDataClass->Ptr=&Data;
//    NewDataClass->XmlName=XmlName;
//
//    return cfg.RegisterGeneric(NewDataClass);
//}

/////////////////////////////////
class KVList_TinyCFG : public TinyCFGBaseData
{
    public:
        t_KVList *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};
bool KVList_TinyCFG::LoadElement(class TinyCFG *CFG)
{
    class TinyCFG SubCFG("Option");
    string NewValue;
    string NewKey;

    Ptr->clear();

    SubCFG.Register("Key",NewKey);
    SubCFG.Register("Value",NewValue);

    NewKey="";
    NewValue="";

    SubCFG.ConnectToParentCFGForReading(CFG);

    while(SubCFG.ReadNextCFG())
    {
        if(NewKey=="")
            return false;
        Ptr->insert(make_pair(NewKey,NewValue));
    }

    return true;
}

bool KVList_TinyCFG::SaveElement(class TinyCFG *CFG)
{
    i_KVList i;
    class TinyCFG SubCFG("Option");
    string NewValue;
    string NewKey;

    SubCFG.Register("Key",NewKey);
    SubCFG.Register("Value",NewValue);

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        NewKey=i->first;
        NewValue=i->second;
        SubCFG.WriteCFGUsingParentCFG(CFG);
    }
    return true;
}

bool RegisterKVList_TinyCFG(class TinyCFG &cfg,const char *XmlName,
        t_KVList &Data)
{
    class KVList_TinyCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new KVList_TinyCFG;
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
/////////////////////////////////
class ConnectionsOptions_TinyCFG : public TinyCFGBaseData
{
    public:
        t_ConnectionsOptions *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};
bool ConnectionsOptions_TinyCFG::LoadElement(class TinyCFG *CFG)
{
    class TinyCFG SubCFG("Connection");
    struct ConOptions NewData;
    string ConnectionUniqueID;

    Ptr->clear();

    SubCFG.Register("UniqueID",ConnectionUniqueID);
    SubCFG.Register("Age",NewData.Age);

    RegisterKVList_TinyCFG(SubCFG,"Options",NewData.Options);

    NewData.Age=0;
    ConnectionUniqueID="";

    SubCFG.ConnectToParentCFGForReading(CFG);

    while(SubCFG.ReadNextCFG())
    {
        if(ConnectionUniqueID=="")
            return false;
        Ptr->insert(make_pair(ConnectionUniqueID,NewData));
    }

    return true;
}

bool ConnectionsOptions_TinyCFG::SaveElement(class TinyCFG *CFG)
{
    i_ConnectionsOptions i;
    class TinyCFG SubCFG("Connection");
    struct ConOptions NewData;
    string ConnectionUniqueID;
    i_KVList kvi;

    SubCFG.Register("UniqueID",ConnectionUniqueID);
    SubCFG.Register("Age",NewData.Age);

    RegisterKVList_TinyCFG(SubCFG,"Options",NewData.Options);

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        ConnectionUniqueID=i->first;
        NewData=i->second;
        SubCFG.WriteCFGUsingParentCFG(CFG);
    }
    return true;
}

bool RegisterConnectionOptions_TinyCFG(class TinyCFG &cfg,const char *XmlName,
        t_ConnectionsOptions &Data)
{
    class ConnectionsOptions_TinyCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new ConnectionsOptions_TinyCFG;
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

/////////////////////////////////
class KVListMap_TinyCFG : public TinyCFGBaseData
{
    public:
        t_KVListMap *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};
bool KVListMap_TinyCFG::LoadElement(class TinyCFG *CFG)
{
    class TinyCFG SubCFG("List");
    t_KVList NewData;
    string UniqueID;

    Ptr->clear();

    SubCFG.Register("UniqueID",UniqueID);

    RegisterKVList_TinyCFG(SubCFG,"KeyValues",NewData);

    UniqueID="";

    SubCFG.ConnectToParentCFGForReading(CFG);

    while(SubCFG.ReadNextCFG())
    {
        if(UniqueID=="")
            return false;
        Ptr->insert(make_pair(UniqueID,NewData));
    }

    return true;
}

bool KVListMap_TinyCFG::SaveElement(class TinyCFG *CFG)
{
    i_KVListMap i;
    class TinyCFG SubCFG("List");
    t_KVList NewData;
    string UniqueID;
    i_KVList kvi;

    RegisterKVList_TinyCFG(SubCFG,"KeyValues",NewData);

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        UniqueID=i->first;
        NewData=i->second;
        SubCFG.WriteCFGUsingParentCFG(CFG);
    }
    return true;
}

bool RegisterKVListMap_TinyCFG(class TinyCFG &cfg,const char *XmlName,
        t_KVListMap &Data)
{
    class KVListMap_TinyCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new KVListMap_TinyCFG;
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

/////////////////////////////////
class ConUploadOptions_TinyCFG : public TinyCFGBaseData
{
    public:
        t_ConUploadOptions *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};
bool ConUploadOptions_TinyCFG::LoadElement(class TinyCFG *CFG)
{
    class TinyCFG SubCFG("Connection");
    struct ConUploadOptions NewData;
    string ConnectionUniqueID;

    Ptr->clear();

    SubCFG.Register("UniqueID",ConnectionUniqueID);
    SubCFG.Register("Age",NewData.Age);
    SubCFG.Register("Filename",NewData.Filename);
    SubCFG.Register("Protocol",NewData.Protocol);

    RegisterKVList_TinyCFG(SubCFG,"Options",NewData.Options);

    NewData.Age=0;
    NewData.Filename="";
    NewData.Protocol="";
    ConnectionUniqueID="";

    SubCFG.ConnectToParentCFGForReading(CFG);

    while(SubCFG.ReadNextCFG())
    {
        if(ConnectionUniqueID=="")
            return false;
        Ptr->insert(make_pair(ConnectionUniqueID,NewData));
    }

    return true;
}

bool ConUploadOptions_TinyCFG::SaveElement(class TinyCFG *CFG)
{
    i_ConUploadOptions i;
    class TinyCFG SubCFG("Connection");
    struct ConUploadOptions NewData;
    string ConnectionUniqueID;
    i_KVList kvi;

    SubCFG.Register("UniqueID",ConnectionUniqueID);
    SubCFG.Register("Age",NewData.Age);
    SubCFG.Register("Filename",NewData.Filename);
    SubCFG.Register("Protocol",NewData.Protocol);

    RegisterKVList_TinyCFG(SubCFG,"Options",NewData.Options);

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        ConnectionUniqueID=i->first;
        NewData=i->second;
        SubCFG.WriteCFGUsingParentCFG(CFG);
    }
    return true;
}

bool RegisterConUploadOptions_TinyCFG(class TinyCFG &cfg,const char *XmlName,
        t_ConUploadOptions &Data)
{
    class ConUploadOptions_TinyCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new ConUploadOptions_TinyCFG;
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
/////////////////////////////////
class ConDownloadOptions_TinyCFG : public TinyCFGBaseData
{
    public:
        t_ConDownloadOptions *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};
bool ConDownloadOptions_TinyCFG::LoadElement(class TinyCFG *CFG)
{
    class TinyCFG SubCFG("Connection");
    struct ConDownloadOptions NewData;
    string ConnectionUniqueID;

    Ptr->clear();

    SubCFG.Register("UniqueID",ConnectionUniqueID);
    SubCFG.Register("Age",NewData.Age);
    SubCFG.Register("Protocol",NewData.Protocol);

    RegisterKVList_TinyCFG(SubCFG,"Options",NewData.Options);

    NewData.Age=0;
    NewData.Protocol="";
    ConnectionUniqueID="";

    SubCFG.ConnectToParentCFGForReading(CFG);

    while(SubCFG.ReadNextCFG())
    {
        if(ConnectionUniqueID=="")
            return false;
        Ptr->insert(make_pair(ConnectionUniqueID,NewData));
    }

    return true;
}

bool ConDownloadOptions_TinyCFG::SaveElement(class TinyCFG *CFG)
{
    i_ConDownloadOptions i;
    class TinyCFG SubCFG("Connection");
    struct ConDownloadOptions NewData;
    string ConnectionUniqueID;
    i_KVList kvi;

    SubCFG.Register("UniqueID",ConnectionUniqueID);
    SubCFG.Register("Age",NewData.Age);
    SubCFG.Register("Protocol",NewData.Protocol);

    RegisterKVList_TinyCFG(SubCFG,"Options",NewData.Options);

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        ConnectionUniqueID=i->first;
        NewData=i->second;
        SubCFG.WriteCFGUsingParentCFG(CFG);
    }
    return true;
}

bool RegisterConDownloadOptions_TinyCFG(class TinyCFG &cfg,const char *XmlName,
        t_ConDownloadOptions &Data)
{
    class ConDownloadOptions_TinyCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new ConDownloadOptions_TinyCFG;
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
