<?php
/*******************************************************************************
 * FILENAME: PluginProcessor.php
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (09 Oct 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "PluginProcessor.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

function ProcessPlugin($File)
{
    $RetArray=array();
    $RetArray["WorksWith"]=0;

    /* Ok, read each chunk and see if it's the one we are looking for */
    try
    {
        $fp=fopen($File,"rb");
        if(!$fp)
            return NULL;

        $binary=fread($fp,12);
        $PluginData=unpack('C12',$binary);
        $PluginData = array_values($PluginData);

        if($PluginData[0]!=70 || $PluginData[1]!=79 || $PluginData[2]!=82 ||
                $PluginData[3]!=77)
        {
            throw new Exception("Not IFF");
        }

        $TotalSize=$PluginData[4]<<24;
        $TotalSize|=$PluginData[5]<<16;
        $TotalSize|=$PluginData[6]<<8;
        $TotalSize|=$PluginData[7]<<0;

        $FormType="";
        $FormType.=chr($PluginData[8]);
        $FormType.=chr($PluginData[9]);
        $FormType.=chr($PluginData[10]);
        $FormType.=chr($PluginData[11]);

        if($FormType!="WTPI")
            throw new Exception("Not Whippy Term Plugin");

        $BytesLeft=$TotalSize-4;    // 4 for the form time
        while($BytesLeft>0)
        {
            $binary=fread($fp,8);
            if($binary==false)
                break;
            $PluginData=unpack('C8',$binary);
            $PluginData = array_values($PluginData);

            $ChunkID="";
            $ChunkID.=chr($PluginData[0]);
            $ChunkID.=chr($PluginData[1]);
            $ChunkID.=chr($PluginData[2]);
            $ChunkID.=chr($PluginData[3]);
            $ChunkSize=$PluginData[4]<<24;
            $ChunkSize|=$PluginData[5]<<16;
            $ChunkSize|=$PluginData[6]<<8;
            $ChunkSize|=$PluginData[7]<<0;

//            echo "CHUNK:$ChunkID,$ChunkSize (".$BytesLeft.",".($BytesLeft-$ChunkSize).")\n";

            if($ChunkID=="NAME")
            {
                /* The name of this plugin */
                $binary=fread($fp,$ChunkSize);
                if($binary==false)
                    break;
                $PluginData=unpack(sprintf('C%d', $ChunkSize), $binary);
                $PluginData = array_values($PluginData);
                $Name="";
                for($r=0;$r<$ChunkSize;$r++)
                    $Name.=chr($PluginData[$r]);

                $RetArray["Name"]=$Name;

                if($ChunkSize&1)
                    fseek($fp,1,SEEK_CUR);
            }
            else if($ChunkID=="DESC")
            {
                /* The description */
                $binary=fread($fp,$ChunkSize);
                if($binary==false)
                    break;
                $PluginData=unpack(sprintf('C%d', $ChunkSize), $binary);
                $PluginData = array_values($PluginData);
                $Desc="";
                for($r=0;$r<$ChunkSize;$r++)
                    $Desc.=chr($PluginData[$r]);

                $RetArray["Desc"]=$Desc;

                if($ChunkSize&1)
                    fseek($fp,1,SEEK_CUR);
            }
            else if($ChunkID=="VER ")
            {
                /* The Version */
                $binary=fread($fp,$ChunkSize);
                if($binary==false)
                    break;
                $PluginData=unpack(sprintf('C%d', $ChunkSize), $binary);
                $PluginData = array_values($PluginData);
                $VersionStr="";

                $VersionStr.=$PluginData[0];
                $VersionStr.=".".$PluginData[1];
                if($PluginData[2]>0 || $PluginData[3]>0)
                    $VersionStr.=".".$PluginData[2];
                if($PluginData[3]>0)
                    $VersionStr.=".".$PluginData[3];

                $RetArray["Version"]=$VersionStr;

                if($ChunkSize&1)
                    fseek($fp,1,SEEK_CUR);
            }
            else if($ChunkID=="APIV")
            {
                /* The API version */
                $binary=fread($fp,$ChunkSize);
                if($binary==false)
                    break;
                $PluginData=unpack(sprintf('C%d', $ChunkSize), $binary);
                $PluginData = array_values($PluginData);
                $VersionStr="";

                $VersionStr.=$PluginData[0];
                $VersionStr.=".".$PluginData[1];
                if($PluginData[2]>0 || $PluginData[3]>0)
                    $VersionStr.=".".$PluginData[2];
                if($PluginData[3]>0)
                    $VersionStr.=".".$PluginData[3];

                $RetArray["WhippyTermVer"]=$VersionStr;

                if($ChunkSize&1)
                    fseek($fp,1,SEEK_CUR);
            }
            else if($ChunkID=="LIN6")
            {
                $RetArray["WorksWith"]|=0x01;
                /* Make it even */
                $SeekSize=$ChunkSize;
                if($SeekSize&1)
                    $SeekSize+=1;

                /* Skip the data */
                fseek($fp,$SeekSize,SEEK_CUR);
            }
            else if($ChunkID=="WIN6")
            {
                $RetArray["WorksWith"]|=0x04;
                /* Make it even */
                $SeekSize=$ChunkSize;
                if($SeekSize&1)
                    $SeekSize+=1;

                /* Skip the data */
                fseek($fp,$SeekSize,SEEK_CUR);
            }
            else if($ChunkID=="MACS")
            {
                $RetArray["WorksWith"]|=0x08;
                /* Make it even */
                $SeekSize=$ChunkSize;
                if($SeekSize&1)
                    $SeekSize+=1;

                /* Skip the data */
                fseek($fp,$SeekSize,SEEK_CUR);
            }
            else if($ChunkID=="RPI6")
            {
                $RetArray["WorksWith"]|=0x02;
                /* Make it even */
                $SeekSize=$ChunkSize;
                if($SeekSize&1)
                    $SeekSize+=1;

                /* Skip the data */
                fseek($fp,$SeekSize,SEEK_CUR);
            }
            else if($ChunkID=="RPI3")
            {
                $RetArray["WorksWith"]|=0x10;
                /* Make it even */
                $SeekSize=$ChunkSize;
                if($SeekSize&1)
                    $SeekSize+=1;

                /* Skip the data */
                fseek($fp,$SeekSize,SEEK_CUR);
            }
            else
            {
                /* Make it even */
                $SeekSize=$ChunkSize;
                if($SeekSize&1)
                    $SeekSize+=1;

                /* Skip the data */
                fseek($fp,$SeekSize,SEEK_CUR);
            }
            $BytesLeft-=4;  // ChunkID
            $BytesLeft-=4;  // ChunkSize
            $BytesLeft-=$ChunkSize;
            if($ChunkSize&1)
                $BytesLeft--;
        }

        fclose($fp);
    }
    catch(Exception $e)
    {
        fclose($fp);
        return NULL;
    }

    return $RetArray;
}
?>