/* Prerelease Version 0.6 */

/*******************************************************************************
 * FILENAME: TinyCFG.h
 * 
 * PROJECT: Europa
 *
 * FILE DESCRIPTION:
 *    This is the .h file for the TinyCFG.cpp file.  It does .h file stuff.
 *
 * COPYRIGHT:
 *    Copyright 2003 Paul Hutchinson
 *
 *    MIT License
 *
 *    See LICENSE file.
 *
 * HISTORY:
 *    PaulHutchinson (19 Jun 2003)
 *       Created
 *
 *******************************************************************************/
#ifndef __TINYCFG_H_
#define __TINYCFG_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <string>
#include <list>
#include <stdio.h>

/***  DEFINES                          ***/
#define XMLINDEXSIZE 4

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef char * t_TinyCFGReadHandle;

class TinyCFGBaseData
{
    public:
        /* Public vars */
        std::string XmlName;

        /* Public methods */
        virtual bool LoadData(std::string &LoadedString) {return false;};
        virtual bool SaveData(std::string &StoreString) {return false;};
        virtual bool LoadElement(class TinyCFG *CFG) {return false;};
        virtual bool SaveElement(class TinyCFG *CFG) {return false;};

        virtual ~TinyCFGBaseData() {}
};

struct TinyCFG_RegData
{
    class TinyCFGBaseData *Data;
    struct TinyCFG_RegData *Next;
};

struct TinyCFG_Entry
{
    std::string XmlName;
    struct TinyCFG_Entry *FirstSub;
    struct TinyCFG_Entry *CurrentSub;
    struct TinyCFG_Entry *ParentSubLevel;
    struct TinyCFG_RegData *FirstData;
    struct TinyCFG_RegData *CurrentData;
    struct TinyCFG_Entry *Next;
};

class TinyCFG
{
    public:
        /* Public Vars */
        bool Failure;

        /* Public Methods */
        TinyCFG(const char *FirstBlockName);
        ~TinyCFG();
        bool Register(const char *XmlName,std::string &Data);
        bool Register(const char *XmlName,int &Data);
        bool Register(const char *XmlName,int &Data,bool OutputHex);
        bool Register(const char *XmlName,unsigned int &Data);
        bool Register(const char *XmlName,unsigned int &Data,bool OutputHex);
        bool Register(const char *XmlName,short &Data);
        bool Register(const char *XmlName,unsigned short &Data);
        bool Register(const char *XmlName,long &Data);
        bool Register(const char *XmlName,unsigned long &Data);
        bool Register(const char *XmlName,long long &Data);
        bool Register(const char *XmlName,unsigned long long &Data);
        bool Register(const char *XmlName,char *Data,int MaxSize);
        bool Register(const char *XmlName,bool &Data);
        bool Register(const char *XmlName,std::list<std::string> &Data);
        bool Register(const char *XmlName,std::list<int> &Data);
        bool Register(const char *XmlName,double &Data,const char *FormatStr);
        bool RegisterEnum(const char *XmlName,int &Data,int DefaultValue,
                int NumberOfEntries,int List,...);
        bool RegisterGeneric(class TinyCFGBaseData *Generic);
        bool StartBlock(const char *BlockName);
        bool StartBlock(const std::string &BlockName);
        void EndBlock(void);
        bool SaveCFGFile(const char *FileName);
        bool LoadCFGFile(const char *FileName,int MaxFileSize=1000000);
        void Clear(void);

        /* For custom types */
        void WriteDataElement(const char *XmlName,const char *Value);
        const char *ReadDataElement(const char *DataElementName);
        const char *ReadNextDataElement(const char *DataElementName);
        bool ReadFirstTag(std::string &XmlName,std::string &DataElement);
        bool ReadNextTag(const char **XmlName,const char **DataElement);
        bool ReadNextTag(std::string &XmlName,std::string &DataElement);
        /* For advanced data types */
        void ConnectToParentCFGForReading(class TinyCFG *OrgCFG);
        bool WriteCFGUsingParentCFG(class TinyCFG *OrgCFG);
        bool ReadNextCFG(void);

    private:
        /* Private Vars */
        struct TinyCFG_Entry *FirstBlock;   // The list of blocks
        struct TinyCFG_Entry *CurrentBlock; // The current block we are adding to
        std::string SavedFirstBlockName;   // If we where constructed with a block name we need to save it incase Clear() needs it later
        FILE *fh;
        int XMLIndent;
        char *ReadBuff;
        char *ReadBuffEnd;
        char *ReadPoint;
        char *ReadBuffStartAt;      // The point we start reading at (normally a copy of ReadBuff, but changed when chaining)
        char *LoadDataDataStart;
        char *LoadDataReadPoint;    // A copy of 'ReadPoint' for the LoadData() functions from 'TinyCFGBaseData' to use
        struct TinyCFG_Entry *CurrentReadLevel;

        /* Private Methods */
        void FreeAllRegisteredTypes(struct TinyCFG_Entry *List2Free);
        bool WriteXml(struct TinyCFG_Entry *First);
        bool GetAndSetFromXml(void);
        bool RegisterInt(const char *XmlName,unsigned int *Data,bool OutputHex,
                bool IsUnsigned);
        bool RegisterShort(const char *XmlName,unsigned short *Data,
                bool IsUnsigned);
        bool RegisterLong(const char *XmlName,unsigned long *Data,
                bool IsUnsigned);
        bool RegisterLongLong(const char *XmlName,unsigned long long *Data,
                bool IsUnsigned);
        void ResetData(void);
        bool CheckXMLName(const char *CheckName);
        void WriteXMLOpenElement(const char *ElementName);
        void WriteXMLCloseElement(const char *ElementName);
        void WriteXMLOpenDataElement(const char *ElementName);
        void WriteXMLCloseDataElement(const char *ElementName);
        void WriteXMLEscapedString(const char *OutString);
        void UnEscapedString(std::string &RetStr,const char *OutString);
        bool EnterElementLevel(const char *Name);
        void ExitElementLevel(const char *Name);
        bool SkipToEndTag(const char *ElementName);
        struct TinyCFG_RegData *FindDataEntry(const char *DataName);
        char *FindElementAtThisLevel(const char *ElementName,bool EndTag);
        bool FindNextTagStartAndEndAtThisLevel(char **Start,char **Data,char **End);
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/


#endif  /* end of "#ifndef __TINYCFG_H_" */
