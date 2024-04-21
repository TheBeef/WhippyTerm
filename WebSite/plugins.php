<!DOCTYPE html>
<?php
/*******************************************************************************
 * FILENAME: plugins.php
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
 *    Paul Hutchinson (09 Oct 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
require_once("PluginProcessor.php");

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
$PageSelected=2;
$PageTitle="Plugins";
?>
<html>
<head>
    <title>Whippy Term - Plugins</title>
    <link rel="stylesheet" type="text/css" href="/somestyle.css">
    <link rel='stylesheet' href='https://fonts.googleapis.com/css?family=Roboto%3A100%2C100italic%2C200%2C200italic%2C300%2C300italic%2C400%2C400italic%2C500%2C500italic%2C600%2C600italic%2C700%2C700italic%2C800%2C800italic%2C900%2C900italic%7CRoboto+Slab%3A100%2C100italic%2C200%2C200italic%2C300%2C300italic%2C400%2C400italic%2C500%2C500italic%2C600%2C600italic%2C700%2C700italic%2C800%2C800italic%2C900%2C900italic%7CJost%3A100%2C100italic%2C200%2C200italic%2C300%2C300italic%2C400%2C400italic%2C500%2C500italic%2C600%2C600italic%2C700%2C700italic%2C800%2C800italic%2C900%2C900italic&#038;display=auto&#038;ver=6.0.2' media='all' />
</head>
<body>
    <?php include "PageHeader.php" ?>

    <table class="PluginList">
        <tr>
            <th>Plugin</th>
            <th>Direct Download</th>
            <th>Version</th>
            <th>Description</th>
            <th>Compatible With Whippy</th>
            <th>Platforms</th>
        </tr>
<?php
    /* Ok, process the plugins */
    $Files=scandir("Downloads/Plugins");
    foreach($Files as $File)
    {
        if($File=="." || $File=="..")
            continue;
        $Details=ProcessPlugin("Downloads/Plugins/".$File);
        if($Details===NULL)
            continue;

        $ClippedDesc=substr($Details["Desc"],0,1000);
        if(strlen($ClippedDesc)>9999)
            $ClippedDesc.="...";

        $NewLine=strstr($ClippedDesc,"\n",true);
        if($NewLine!=false)
            $ClippedDesc=$NewLine;

        /* Output table row */
        echo "<tr>\n";
        echo "<td><a href='/ShowPlugin.php?id=$File'>${Details["Name"]}</a></td>\n";
        echo "<td class='center'><a href='/Downloads/Plugins/$File' download='$File'>$File</a></td>\n";
        echo "<td class='center'>${Details["Version"]}</td>\n";
        echo "<td>$ClippedDesc</td>\n";
        echo "<td class='center'>${Details["WhippyTermVer"]}</td>\n";
        echo "<td class='center'>";
        if($Details["WorksWith"]&0x01)
            echo "Linux<br/>";
        if($Details["WorksWith"]&0x02)
            echo "Raspberry PI (64bit)<br/>";
        if($Details["WorksWith"]&0x04)
            echo "Windows<br/>";
        if($Details["WorksWith"]&0x08)
            echo "Mac<br/>";
        if($Details["WorksWith"]&0x10)
            echo "Raspberry PI (32bit)<br/>";
        echo "</td>\n";
        echo "</tr>\n";
    }
?>
    </table>

</div>
</body>
</html>

<?php


?>