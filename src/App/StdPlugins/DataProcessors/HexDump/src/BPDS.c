/*******************************************************************************
 * FILENAME: BPDS.c
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the handling of the Binary Protocol Decoumentation Standard
 *    in it.
 *
 * COPYRIGHT:
 *    Copyright 02 Dec 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (02 Dec 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "BPDS.h"
#include "trycatch.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*** DEFINES                  ***/
#define STRCMP(x,y)  strcasecmp((x),(y))

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef enum
{
    e_State_Searching,
    e_State_Name,
    e_State_MatchAny,
    e_State_Size,
    e_State_Assign,
    e_State_DataType,
    e_State_StringValue,
    e_StateMAX
} e_StateType;

/*** FUNCTION PROTOTYPES      ***/
static void BPDS_FreeFields(struct BPDSField *Start);
static void BPDS_FreeNumValueSet(struct BPDSNumberValueSet *First);
static void BPDS_FreeStrValueSet(struct BPDSStringValueSet *First);
static bool BPDS_GrabName(struct BPDSDef *Def,struct BPDSField *Sym,
        const char *Start,const char *End);
static void BPDS_AddField(struct BPDSField *Sym,struct BPDSField **Head,
        struct BPDSField **Tail);
static struct BPDSNumberValueSet *BPDS_AddValue2Set(struct BPDSDef *Def,
        struct BPDSNumberValueSet *CurVS,const char *Start);
static struct BPDSStringValueSet *BPDS_AddValue2SetStr(struct BPDSDef *Def,
        struct BPDSStringValueSet *CurVS,const char *AddStr,int Len);
static e_BPDSDataFormatType BPDS_Str2DataType(const char *Str);
static void BPDS_SetError(struct BPDSDef *Def,const char *p,
        const char *ErrorMsg);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    BPDS_Parse
 *
 * SYNOPSIS:
 *    struct BPDSDef *BPDS_Parse(const char *BPDSStr);
 *
 * PARAMETERS:
 *    BPDSStr [I] -- The BPDS string to parse.
 *
 * FUNCTION:
 *    This function takes a string with a binary protocol documentaion standard
 *    definition in it and parses it, and builds a list of fields.
 *
 * RETURNS:
 *    The BPDS definition object with the parsed string in it or NULL if the
 *    structure could not be allocated.  It still returns the BPDS definition
 *    if there is an error with 'BPDSStr'.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct BPDSDef *BPDS_Parse(const char *BPDSStr)
{
    struct BPDSDef *NewDef;
    const char *p;
    const char *Start;
    const char *TypeStart;
    const char *SearchStr;
    e_StateType State;
    e_StateType RetState;
    unsigned int MatchAnyCount;
    struct BPDSField *NewField;
    struct BPDSField *FieldListHead;
    struct BPDSField *FieldListTail;
    struct BPDSField *SearchField;
    char SearchName[256];
    unsigned int Len;

#ifdef PRINT
    printf("%s\n",BPDSStr);
#endif

    Len=strlen(BPDSStr);

    NewDef=malloc(sizeof(struct BPDSDef));
    if(NewDef==NULL)
        return NULL;
    NewDef->ErrorStr=NULL;
    NewDef->FieldList=NULL;
    NewDef->ErrorOffset=0;
    NewDef->HadError=false;
    NewDef->DPDSDefStr=malloc(Len+1);
    if(NewDef->DPDSDefStr==NULL)
    {
        free(NewDef);
        return NULL;
    }
    memcpy(NewDef->DPDSDefStr,BPDSStr,Len);
    NewDef->DPDSDefStr[Len]=0;

    FieldListHead=NULL;
    FieldListTail=NULL;
    MatchAnyCount=0;
    Start=NULL;
    State=e_State_Searching;

    ctry()
    {
        p=NewDef->DPDSDefStr;
        while(*p!=0)
        {
            if(*p==' ' || *p=='\t')
            {
                p++;
                continue;
            }
#ifdef PRINT
            printf("%c:",*p);
#endif
            switch(State)
            {
                case e_State_Searching:
                    if(*p=='<')
                    {
                        /* Start of a symbol */
                        MatchAnyCount=0;
#ifdef PRINT
                        printf("Field");
#endif
                        NewField=malloc(sizeof(struct BPDSField));
                        if(NewField==NULL)
                        {
                            BPDS_SetError(NewDef,p,"Out of memory");
                            cthrow();
                        }
                        memset(NewField,0x00,sizeof(struct BPDSField));
                        NewField->Size=1;
                        Start=p+1;
                        while(*Start==' ' || *Start=='\t')
                            Start++;
                        State=e_State_Name;
                    }
                    else if(*p=='@')
                    {
                        BPDS_SetError(NewDef,p,"Groups are not supported");
                        cthrow();
                    }
                    else
                    {
                        /* Unknown char */
                        BPDS_SetError(NewDef,p,"Unknown token");
                        cthrow();
                    }
                break;
                case e_State_Name:
                    if(*p=='>')
                    {
                        /* End of symbol */
#ifdef PRINT
                        printf("Sym end ");
#endif

                        while(*Start==' ' || *Start=='\t')
                            Start++;

                        /* See if it's just a number or a label */
                        if((*Start>='0' && *Start<='9'))
                        {
                            /* We count it as a value */
                            if(NewField->StrValues!=NULL)
                            {
                                BPDS_SetError(NewDef,Start,
                                        "Can't mix numbers and strings");
                                cthrow();
                            }

                            NewField->NumValues=BPDS_AddValue2Set(NewDef,
                                    NewField->NumValues,Start);
                            if(NewField->NumValues==NULL)
                            {
                                cthrow();
                            }
#ifdef PRINT
                            printf("\33[34m%ld\33[m",NewField->Values->Value);
#endif
                        }
                        else
                        {
                            if(*Start=='>')
                            {
                                /* The start is at the end of the field, see if
                                   we collected any values (if so we are good) */
                                if(NewField->NumValues==NULL ||
                                        NewField->StrValues==NULL)
                                {
                                    /* Ok, black field */
                                    BPDS_SetError(NewDef,Start,
                                            "Invalid field name");
                                    cthrow();
                                }
                            }
                            else if(!BPDS_GrabName(NewDef,NewField,Start,p))
                            {
                                cthrow();
                            }

#ifdef PRINT
                            printf("\33[32m%s\33[m",NewField->Name);
#endif
                        }

                        BPDS_AddField(NewField,&FieldListHead,&FieldListTail);
                        State=e_State_Searching;
                    }
                    else if(*p==':')
                    {
                        /* Size */
                        if(!BPDS_GrabName(NewDef,NewField,Start,p))
                        {
                            cthrow();
                        }
#ifdef PRINT
                        printf("\33[32m%s\33[m",NewField->Name);
#endif
                        Start=p+1;
                        State=e_State_Size;
                    }
                    else if(*p=='=')
                    {
                        /* Assign */
                        if(!BPDS_GrabName(NewDef,NewField,Start,p))
                        {
                            cthrow();
                        }
#ifdef PRINT
                        printf("\33[32m%s\33[m",NewField->Name);
#endif
                        Start=p+1;
                        State=e_State_Assign;
                    }
                    else if(*p=='{')
                    {
                        BPDS_SetError(NewDef,p,"Bit fields are not supported");
                        cthrow();
                    }
                    else if(*p=='|')
                    {
                        /* We are doing a set of values */
                        /* Check if the new entry is a number or something else */
                        if(*Start=='|')
                        {
                            BPDS_SetError(NewDef,p,"Missing number");
                            cthrow();
                        }
                        SearchStr=Start;
                        while(isxdigit(*SearchStr) || *SearchStr=='x' ||
                                *SearchStr=='X')
                        {
                            SearchStr++;
                        }
                        if(*SearchStr!='|' && *SearchStr!=' ' && *SearchStr!='\t')
                        {
                            BPDS_SetError(NewDef,SearchStr,
                                    "Value sets can only be numbers");
                            cthrow();
                        }

                        if(NewField->StrValues!=NULL)
                        {
                            BPDS_SetError(NewDef,Start,
                                    "Can't mix numbers and strings");
                            cthrow();
                        }

                        NewField->NumValues=BPDS_AddValue2Set(NewDef,
                                NewField->NumValues,Start);
                        if(NewField->NumValues==NULL)
                        {
                            cthrow();
                        }
#ifdef PRINT
                        printf("\33[34m%ld\33[m",NewField->Values->Value);
#endif
                        Start=p+1;
                    }
                    else if(*p=='\"')
                    {
                        Start=p+1;
                        RetState=State;
                        State=e_State_StringValue;
                    }
                    else if(!isalnum(*p) && ((*p)&0x80)==0 && *p!='_')
                    {
                        BPDS_SetError(NewDef,p,"Invalid token");
                        cthrow();
                    }
                break;
                case e_State_Size:
                    if(*p=='>')
                    {
                        /* End of symbol */
#ifdef PRINT
                        printf("Sym end (size)");
#endif

                        NewField->Size=strtoll(Start,NULL,0);
                        if(NewField->Size<1)
                        {
                            /* See if it's a label */
                            SearchStr=Start;
                            while((*SearchStr>='a' && *SearchStr<='z') ||
                                    (*SearchStr>='A' && *SearchStr<='Z') ||
                                    (*SearchStr>='0' && *SearchStr<='9'))
                            {
                                SearchStr++;
                            }
                            if(SearchStr-Start>sizeof(SearchName)-1)
                            {
                                BPDS_SetError(NewDef,Start,"Label to long");
                                cthrow();
                            }
                            memcpy(SearchName,Start,SearchStr-Start);
                            SearchName[SearchStr-Start]=0;

#ifdef PRINT
                            printf("Search for:%s\n",SearchName);
#endif
                            for(SearchField=FieldListHead;SearchField!=NULL;
                                    SearchField=SearchField->Next)
                            {
                                if(STRCMP(SearchField->Name,SearchName)==0)
                                {
                                    /* Found it */
                                    NewField->SizeLinkedTo=SearchField;
                                    break;
                                }
                            }
                            if(SearchField==NULL)
                            {
                                BPDS_SetError(NewDef,Start,"Invalid size");
                                cthrow();
                            }
                        }
                        BPDS_AddField(NewField,&FieldListHead,&FieldListTail);
                        State=e_State_Searching;
                    }
                    else if(*p==':')
                    {
                        BPDS_SetError(NewDef,p,"Second size token found");
                        cthrow();
                    }
                    else if(*p=='=')
                    {
                        /* Assign */
                        NewField->Size=strtoll(Start,NULL,0);
                        if(NewField->Size<1)
                        {
                            BPDS_SetError(NewDef,Start,"Invalid size");
                            cthrow();
                        }

                        Start=p+1;
                        State=e_State_Assign;
                    }
                    else if(*p=='(')
                    {
                        /* Data type */
                        NewField->Size=strtoll(Start,NULL,0);
                        if(NewField->Size<1)
                        {
                            BPDS_SetError(NewDef,Start,"Invalid size");
                            cthrow();
                        }

                        TypeStart=p+1;
                        State=e_State_DataType;
                    }
                    else if(*p=='.')
                    {
                        /* Match any */
                        MatchAnyCount=1;
                        State=e_State_MatchAny;
                    }
                    else if(!isalnum(*p) && ((*p)&0x80)==0 && *p!='_')
                    {
                        BPDS_SetError(NewDef,p,"Invalid token");
                        cthrow();
                    }
                break;
                case e_State_Assign:
                    if(*p=='>')
                    {
                        /* End of symbol */
#ifdef PRINT
                        printf("Sym end (assign)");
#endif
                        while(*Start==' ' || *Start=='\t')
                            Start++;
                        if(isdigit(*Start))
                        {
                            /* Ok, we only support numbers (no label) */
                            if(NewField->StrValues!=NULL)
                            {
                                BPDS_SetError(NewDef,Start,
                                        "Can't mix numbers and strings");
                                cthrow();
                            }

                            NewField->NumValues=BPDS_AddValue2Set(NewDef,
                                    NewField->NumValues,Start);
                            if(NewField->NumValues==NULL)
                            {
                                cthrow();
                            }
#ifdef PRINT
                            printf("\33[34m%ld\33[m",NewField->Values->Value);
#endif
                        }

                        BPDS_AddField(NewField,&FieldListHead,&FieldListTail);
                        State=e_State_Searching;
                    }
                    else if(*p==':')
                    {
                        BPDS_SetError(NewDef,p,"Size token invalid after assign");
                        cthrow();
                    }
                    else if(*p=='=')
                    {
                        BPDS_SetError(NewDef,p,"Second assign token found");
                        cthrow();
                    }
                    else if(*p=='|')
                    {
                        /* We are doing a set of values */
                        SearchStr=p+1;
                        while(*SearchStr==' ' || *SearchStr=='\t')
                            SearchStr++;

                        /* Check if the new entry is a number or something else */
                        if(*Start=='|')
                        {
                            BPDS_SetError(NewDef,p,"Missing number");
                            cthrow();
                        }
                        SearchStr=Start;
                        while(isxdigit(*SearchStr) || *SearchStr=='x' ||
                                *SearchStr=='X')
                        {
                            SearchStr++;
                        }
                        if(*SearchStr!='|' && *SearchStr!=' ' && *SearchStr!='\t')
                        {
                            BPDS_SetError(NewDef,SearchStr,
                                    "Value sets can only be numbers");
                            cthrow();
                        }

                        if(NewField->StrValues!=NULL)
                        {
                            BPDS_SetError(NewDef,Start,"Can't mix numbers and strings");
                            cthrow();
                        }

                        NewField->NumValues=BPDS_AddValue2Set(NewDef,
                                NewField->NumValues,Start);
                        if(NewField->NumValues==NULL)
                        {
                            cthrow();
                        }
#ifdef PRINT
                        printf("\33[34m%ld\33[m",NewField->Values->Value);
#endif
                        Start=p+1;
                    }
                    else if(*p=='\"')
                    {
                        Start=p+1;
                        RetState=State;
                        State=e_State_StringValue;
                    }
                    else if(!isalnum(*p) && ((*p)&0x80)==0 && *p!='_')
                    {
                        BPDS_SetError(NewDef,p,"Invalid token");
                        cthrow();
                    }
                break;
                case e_State_DataType:
                    if(*p==')')
                    {
                        /* We are at the end of the data type, see if it's valid */
                        while(*TypeStart==' ' || *TypeStart=='\t')
                            TypeStart++;
                        SearchStr=TypeStart;
                        while(isalnum(*SearchStr) || *SearchStr=='_')
                            SearchStr++;
                        if(SearchStr-TypeStart>sizeof(SearchName)-1)
                        {
                            BPDS_SetError(NewDef,TypeStart,"type to long");
                            cthrow();
                        }
                        memcpy(SearchName,TypeStart,SearchStr-TypeStart);
                        SearchName[SearchStr-TypeStart]=0;

                        /* See if it's a valid type */
                        NewField->Format=BPDS_Str2DataType(SearchName);
                        if(NewField->Format==e_BPDSDataFormat_Unknown)
                        {
                            BPDS_SetError(NewDef,TypeStart,"Unknown data type");
                            cthrow();
                        }

#ifdef PRINT
                        printf("\33[33mData Type:%s\33[m",SearchName);
#endif
                        State=e_State_Size;
                    }
                    else if(!isalnum(*p) && ((*p)&0x80)==0 && *p!='_')
                    {
                        BPDS_SetError(NewDef,p,"Invalid token");
                        cthrow();
                    }
                break;
                case e_State_MatchAny:
                    if(*p=='>')
                    {
                        /* End of symbol */
#ifdef PRINT
                        printf("Sym end (match any)");
#endif

                        if(MatchAnyCount!=3)
                        {
                            BPDS_SetError(NewDef,p,"Match any must have 3 dots");
                            cthrow();
                        }

                        NewField->MatchAny=true;
                        BPDS_AddField(NewField,&FieldListHead,&FieldListTail);
                        State=e_State_Searching;
                    }
                    else if(*p=='.')
                    {
                        MatchAnyCount++;
                    }
                    else
                    {
                        BPDS_SetError(NewDef,p,"Invalid token");
                        cthrow();
                    }
                break;
                case e_State_StringValue:
                    if(*p=='\"')
                    {
                        /* We are at the end of the string */
                        if(NewField->NumValues!=NULL)
                        {
                            BPDS_SetError(NewDef,Start-1,"Can't mix numbers and strings");
                            cthrow();
                        }
                        NewField->StrValues=BPDS_AddValue2SetStr(NewDef,
                                NewField->StrValues,Start,p-Start);
                        if(NewField->StrValues==NULL)
                        {
                            cthrow();
                        }
#ifdef PRINT
                        printf("\33[34m%s\33[m",NewField->Values->Str);
#endif
                        /* Ok, see if we are going to be doing a bar */
                        Start=p+1;
                        while(*Start==' ' || *Start=='\t')
                            Start++;
                        if(*Start=='|')
                            Start++;
                        p=Start-1;
                        State=RetState;
                    }
                break;
                default:
                case e_StateMAX:
                break;
            }

#ifdef PRINT
            printf("\n");
#endif
            p++;
        }

        switch(State)
        {
            case e_State_Searching:
            break;
            case e_State_Name:
            case e_State_MatchAny:
            case e_State_Size:
            case e_State_Assign:
                BPDS_SetError(NewDef,p,"Missing end of field marker");
                cthrow();
            return NULL;
            break;
            case e_State_DataType:
                BPDS_SetError(NewDef,p,"Missing end of data type marker");
                cthrow();
            return NULL;
            break;
            case e_State_StringValue:
                BPDS_SetError(NewDef,p,"Missing matching quote");
                cthrow();
            return NULL;
            default:
            case e_StateMAX:
            break;
        }

        NewDef->FieldList=FieldListHead;
    }
    ccatch()
    {
        BPDS_FreeFields(FieldListHead);
    }

    return NewDef;
}

/*******************************************************************************
 * NAME:
 *    BPDS_FreeBPDSDef
 *
 * SYNOPSIS:
 *    void BPDS_FreeBPDSDef(struct BPDSDef *Def);
 *
 * PARAMETERS:
 *    Def [I] -- The BPDS definition to free.
 *
 * FUNCTION:
 *    This funciton frees a DPDS definition.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    BPDS_Parse()
 ******************************************************************************/
void BPDS_FreeBPDSDef(struct BPDSDef *Def)
{
}

/*******************************************************************************
 * NAME:
 *    BPDS_FreeFields
 *
 * SYNOPSIS:
 *    static void BPDS_FreeFields(struct BPDSField *Start);
 *
 * PARAMETERS:
 *    Start [I] -- The first entry in the link list to free
 *
 * FUNCTION:
 *    This function frees a BPDS field link list.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    BPDS_Parse()
 ******************************************************************************/
static void BPDS_FreeFields(struct BPDSField *Start)
{
    struct BPDSField *f;

    while(Start!=NULL)
    {
        f=Start->Next;
        BPDS_FreeNumValueSet(Start->NumValues);
        BPDS_FreeStrValueSet(Start->StrValues);
        free(Start);
        Start=f;
    }
}

/*******************************************************************************
 * NAME:
 *    BPDS_FreeNumValueSet
 *
 * SYNOPSIS:
 *    static void BPDS_FreeNumValueSet(struct BPDSNumberValueSet *First);
 *
 * PARAMETERS:
 *    First [I] -- The first entry in the link list to free
 *
 * FUNCTION:
 *    This function frees a link list of BPDS Numbers values sets.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    BPDS_FreeFields()
 ******************************************************************************/
static void BPDS_FreeNumValueSet(struct BPDSNumberValueSet *First)
{
    struct BPDSNumberValueSet *nvs;
    while(First!=NULL)
    {
        nvs=First->Next;
        free(First);
        First=nvs;
    }
}

/*******************************************************************************
 * NAME:
 *    BPDS_FreeStrValueSet
 *
 * SYNOPSIS:
 *    static void BPDS_FreeStrValueSet(struct BPDSStringValueSet *First);
 *
 * PARAMETERS:
 *    First [I] -- The first entry in the link list to free
 *
 * FUNCTION:
 *    This function frees a link list of BPDS strings values sets.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    BPDS_FreeFields()
 ******************************************************************************/
static void BPDS_FreeStrValueSet(struct BPDSStringValueSet *First)
{
    struct BPDSStringValueSet *svs;

    while(First!=NULL)
    {
        svs=First->Next;
        if(First->Str!=NULL)
            free(First->Str);
        free(First);
        First=svs;
    }
}

/*******************************************************************************
 * NAME:
 *    BPDS_GrabName
 *
 * SYNOPSIS:
 *    static bool BPDS_GrabName(struct BPDSDef *Def,struct BPDSField *Sym,
 *              const char *Start,const char *End);
 *
 * PARAMETERS:
 *    Def [I] -- The BPDS def object
 *    Sym [I] -- The field to fill in the 'Name'
 *    Start [I] -- The start of the name
 *    End [I] -- The end of the name
 *
 * FUNCTION:
 *    This function grabs the name from a string and puts it in the Field info.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false --  There was an error.  Error has been set.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool BPDS_GrabName(struct BPDSDef *Def,struct BPDSField *Sym,
        const char *Start,const char *End)
{
    char *StrEnd;

    if((unsigned)(End-Start)>sizeof(Sym->Name)-1)
    {
        BPDS_SetError(Def,Start,"Name too long");
        BPDS_FreeBPDSDef(Def);
        return false;
    }
    memcpy(Sym->Name,Start,End-Start);
    Sym->Name[End-Start]=0;

    /* Trim off any spaces on the end */
    StrEnd=&Sym->Name[End-Start-1];
    while(StrEnd>Sym->Name && (*StrEnd==' ' || *StrEnd=='\t'))
    {
        *StrEnd=0;
        StrEnd--;
    }

    if(Sym->Name[0]==0)
    {
        BPDS_SetError(Def,Start,"Invalid field name");
        BPDS_FreeBPDSDef(Def);
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    BPDS_AddField
 *
 * SYNOPSIS:
 *    static void BPDS_AddField(struct BPDSDef *Def,struct BPDSField *Sym,
 *          struct BPDSField **Head,struct BPDSField **Tail);

 *
 * PARAMETERS:
 *    Def [I] -- The BPDS def object
 *    Sym [I] -- The new field to add
 *    Head [I/O] -- The head of the list
 *    Tail [I/O] -- The tail of the list
 *
 * FUNCTION:
 *    This function adds a node to the list of fields.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void BPDS_AddField(struct BPDSField *Sym,struct BPDSField **Head,
        struct BPDSField **Tail)
{
    /* Add this symbol */
    if(*Head==NULL)
    {
        *Head=Sym;
        *Tail=Sym;
    }
    else
    {
        (*Tail)->Next=Sym;
        *Tail=Sym;
    }
}

/*******************************************************************************
 * NAME:
 *    BPDS_AddValue2Set
 *
 * SYNOPSIS:
 *    static struct NumberValueSet *BPDS_AddValue2Set(struct BPDSDef *Def,
 *          struct BPDSNumberValueSet *CurVS,const char *Start);
 *
 * PARAMETERS:
 *    Def [I] -- The BPDS def object
 *    CurVS [I/O] -- The current value set to add to
 *    Start [I] -- The start of the string with the value in it.  This will be
 *                 sent into strtoll()
 *
 * FUNCTION:
 *    This function adds a new value to a number value set.
 *
 * RETURNS:
 *    The new value set or NULL if there was an error (error string set)
 *
 * SEE ALSO:
 *    BPDS_AddValue2SetStr()
 ******************************************************************************/
static struct BPDSNumberValueSet *BPDS_AddValue2Set(struct BPDSDef *Def,
        struct BPDSNumberValueSet *CurVS,const char *Start)
{
    struct BPDSNumberValueSet *NewVS;

    NewVS=malloc(sizeof(struct BPDSNumberValueSet));
    if(NewVS==NULL)
    {
        BPDS_SetError(Def,Start,"Out of memory");
        BPDS_FreeNumValueSet(CurVS);
        return NULL;
    }

    NewVS->Value=strtoll(Start,NULL,0);
    NewVS->Next=CurVS;

    NewVS->UserData=NULL;

    return NewVS;
}

/*******************************************************************************
 * NAME:
 *    BPDS_AddValue2SetStr
 *
 * SYNOPSIS:
 *    static struct StringValueSet *BPDS_AddValue2SetStr(struct BPDSDef *Def,
 *          struct StringValueSet *CurVS,const char *AddStr,int Len);
 *
 * PARAMETERS:
 *    Def [I] -- The BPDS def object
 *    CurVS [I/O] -- The current value set to add to
 *    AddStr [I] -- The string to add (this will be copied)
 *    Len [I] -- The length of the string to add
 *
 * FUNCTION:
 *    This function adds a new value to a string value set.
 *
 * RETURNS:
 *    The new value set or NULL if there was an error (error string set)
 *
 * SEE ALSO:
 *    BPDS_AddValue2Set()
 ******************************************************************************/
static struct BPDSStringValueSet *BPDS_AddValue2SetStr(struct BPDSDef *Def,
        struct BPDSStringValueSet *CurVS,const char *AddStr,int Len)
{
    struct BPDSStringValueSet *NewVS;

    NewVS=malloc(sizeof(struct BPDSStringValueSet));
    if(NewVS==NULL)
    {
        BPDS_SetError(Def,(const char *)AddStr,"Out of memory");
        BPDS_FreeStrValueSet(CurVS);
        return NULL;
    }
    NewVS->Str=malloc(Len+1);
    if(NewVS->Str==NULL)
    {
        BPDS_SetError(Def,(const char *)AddStr,"Out of memory");
        free(NewVS);
        BPDS_FreeStrValueSet(CurVS);
        return NULL;
    }
    memcpy(NewVS->Str,AddStr,Len);
    NewVS->Str[Len]=0;
    NewVS->StrLen=Len;

    NewVS->UserData=NULL;
//    memset(&NewVS->ReadData,0x00,sizeof(NewVS->ReadData));

    NewVS->Next=CurVS;

    return NewVS;
}

/*******************************************************************************
 * NAME:
 *    BPDS_Str2DataType
 *
 * SYNOPSIS:
 *    static e_BPDSDataFormatType BPDS_Str2DataType(const char *Str)
 *
 * PARAMETERS:
 *    Str [I] -- The string with the BPDS data type in it.
 *
 * FUNCTION:
 *    This function converts a string with a BPDS data type in it to a
 *    enum of that value.
 *
 * RETURNS:
 *    The BPDS data type as an enum or 'e_DataFormat_Unknown' is the string
 *    does not match any known type.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static e_BPDSDataFormatType BPDS_Str2DataType(const char *Str)
{
    if(STRCMP(Str,"int8_t")==0)
        return e_BPDSDataFormat_Int8;
    if(STRCMP(Str,"uint8_t")==0)
        return e_BPDSDataFormat_UInt8;
    if(STRCMP(Str,"int16_t")==0)
        return e_BPDSDataFormat_Int16;
    if(STRCMP(Str,"uint16_t")==0)
        return e_BPDSDataFormat_UInt16;
    if(STRCMP(Str,"int32_t")==0)
        return e_BPDSDataFormat_Int32;
    if(STRCMP(Str,"uint32_t")==0)
        return e_BPDSDataFormat_UInt32;
    if(STRCMP(Str,"int64_t")==0)
        return e_BPDSDataFormat_Int64;
    if(STRCMP(Str,"uint64_t")==0)
        return e_BPDSDataFormat_UInt64;
    if(STRCMP(Str,"int128_t")==0)
        return e_BPDSDataFormat_Int128;
    if(STRCMP(Str,"uint128_t")==0)
        return e_BPDSDataFormat_UInt128;
    if(STRCMP(Str,"float")==0)
        return e_BPDSDataFormat_Float;
    if(STRCMP(Str,"double")==0)
        return e_BPDSDataFormat_Double;

    return e_BPDSDataFormat_Unknown;
}

/*******************************************************************************
 * NAME:
 *    BPDS_SetError
 *
 * SYNOPSIS:
 *    static void BPDS_SetError(struct BPDSDef *Def,const char *p,
 *          const char *ErrorMsg);
 *
 * PARAMETERS:
 *    Def [I] -- The BPDS def object
 *    p [I] -- The point in the 'DPDSDefStr' string where the error starts
 *    ErrorMsg [I] -- The error message
 *
 * FUNCTION:
 *    This function sets the current error for this BPDS def object
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void BPDS_SetError(struct BPDSDef *Def,const char *p,
        const char *ErrorMsg)
{
    BPDS_FreeFields(Def->FieldList);

    Def->ErrorStr=ErrorMsg;
    Def->ErrorOffset=p-(Def->DPDSDefStr);
    Def->HadError=true;
    Def->FieldList=NULL;
}

