/*******************************************************************************
 * FILENAME: main.c
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    Build with the release notes and builds output files.
 *
 * COPYRIGHT:
 *    Copyright 01 Mar 2025 Paul Hutchinson.
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
 *
 * CREATED BY:
 *    Paul Hutchinson (01 Mar 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#define RELEASE_NOTES 1
#include "../../src/ReleaseNotes.c"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

int main(void)
{
    int rel;
    int note;
    FILE *outmd;
    FILE *outdeb;
    char *NextPos;
    char datebuff[100];
    struct tm thedate;
    char *Start;
    char *End;
    int step;
    int d;
    int m;
    int y;
    int Major;
    int Minor;
    int Rev;
    int Patch;

    outmd=fopen("ReleaseNotes.md","w");
    if(outmd==NULL)
    {
        printf("Failed to open \"ReleaseNotes.md\" for writing\n");
        return 1;
    }
    outdeb=fopen("changelog.Debian","w");
    if(outdeb==NULL)
    {
        printf("Failed to open \"changelog.Debian\" for writing\n");
        return 1;
    }
    fprintf(outmd,"# Release Notes\n\n");
    for(rel=0;rel<sizeof(m_ReleaseNotes)/sizeof(struct ReleaseInfo);rel++)
    {
        NextPos=(char *)m_ReleaseNotes[rel].Version;

        fprintf(outmd,"## Version %s -- %s\n",m_ReleaseNotes[rel].Version,m_ReleaseNotes[rel].Date);
        if(m_ReleaseNotes[rel].CodeName!=NULL)
        {
            fprintf(outmd,"### %s\n",m_ReleaseNotes[rel].CodeName);
        }

        for(note=0;note<m_ReleaseNotes[rel].NotesCount;note++)
        {
            fprintf(outmd," * %s\n",m_ReleaseNotes[rel].Notes[note].Summary);
            fprintf(outmd,"   - %s\n",m_ReleaseNotes[rel].Notes[note].Details);
        }

        Start=(char *)m_ReleaseNotes[rel].Version;
        Major=strtol(Start,&Start,10);
        Start++;
        Minor=strtol(Start,&Start,10);
        Start++;
        Rev=strtol(Start,&Start,10);
        Start++;
        Patch=strtol(Start,&Start,10);

        fprintf(outdeb,"whippyterm (%d.%d.%d-%d) unstable; urgency=low\n",Major,Minor,Patch,Rev+1);
        fprintf(outdeb,"\n");
        fprintf(outdeb,"  * %s\n",m_ReleaseNotes[rel].CodeName);
        for(note=0;note<m_ReleaseNotes[rel].NotesCount;note++)
            fprintf(outdeb,"    - %s\n",m_ReleaseNotes[rel].Notes[note].Summary);
        fprintf(outdeb,"\n");

        thedate.tm_year=100;
        thedate.tm_mon=0;
        thedate.tm_mday=1;
        thedate.tm_hour=0;
        thedate.tm_min=0;
        thedate.tm_sec=0;
        thedate.tm_isdst=0;
        thedate.tm_wday=0;
        thedate.tm_yday=0;
        strncpy(datebuff,m_ReleaseNotes[rel].Date,sizeof(datebuff)-1);
        datebuff[sizeof(datebuff)-1]=0;
        Start=datebuff;
        End=Start;
        step=0;
        for(;;)
        {
            if(*End==' ' || *End==0)
            {
                *End=0;
                switch(step)
                {
                    case 0: // Day
                        thedate.tm_mday=atoi(Start);
                        step++;
                    break;
                    case 1: // Month text
                        if(strcmp(Start,"Jan")==0)
                            thedate.tm_mon=0;
                        else if(strcmp(Start,"Feb")==0)
                            thedate.tm_mon=1;
                        else if(strcmp(Start,"Mar")==0)
                            thedate.tm_mon=2;
                        else if(strcmp(Start,"Apr")==0)
                            thedate.tm_mon=3;
                        else if(strcmp(Start,"May")==0)
                            thedate.tm_mon=4;
                        else if(strcmp(Start,"Jun")==0)
                            thedate.tm_mon=5;
                        else if(strcmp(Start,"Jul")==0)
                            thedate.tm_mon=6;
                        else if(strcmp(Start,"Aug")==0)
                            thedate.tm_mon=7;
                        else if(strcmp(Start,"Sep")==0)
                            thedate.tm_mon=8;
                        else if(strcmp(Start,"Oct")==0)
                            thedate.tm_mon=9;
                        else if(strcmp(Start,"Nov")==0)
                            thedate.tm_mon=10;
                        else if(strcmp(Start,"Dec")==0)
                            thedate.tm_mon=11;
                        else
                            thedate.tm_mon=0;
                        step++;
                    break;
                    case 2: // Year
                        thedate.tm_year=atoi(Start)-1900;
                        step++;
                    break;
                }
                End++;
                Start=End;
            }
            if(*End==0)
                break;
            End++;
        }

        d=thedate.tm_mday;
        m=thedate.tm_mon+1;
        y=thedate.tm_year+1900;
        thedate.tm_wday=(d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7;

        strftime(datebuff,sizeof(datebuff),"%a, %d %b %Y %H:%M:%S +0000",&thedate);
        fprintf(outdeb," -- Paul Hutchinson <paul@whippyterm.com>  %s\n",datebuff);
    }
    fclose(outmd);
    fclose(outdeb);
    return 0;
}
