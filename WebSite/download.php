<!DOCTYPE html>
<?php
/*******************************************************************************
 * FILENAME: index.php
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
 *    Paul Hutchinson (08 Oct 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
$PageSelected=1;
$PageTitle="Download";
?>
<html>
<head>
    <title>Whippy Term - Downloads</title>
    <link rel="stylesheet" type="text/css" href="/somestyle.css">
    <link rel='stylesheet' href='https://fonts.googleapis.com/css?family=Roboto%3A100%2C100italic%2C200%2C200italic%2C300%2C300italic%2C400%2C400italic%2C500%2C500italic%2C600%2C600italic%2C700%2C700italic%2C800%2C800italic%2C900%2C900italic%7CRoboto+Slab%3A100%2C100italic%2C200%2C200italic%2C300%2C300italic%2C400%2C400italic%2C500%2C500italic%2C600%2C600italic%2C700%2C700italic%2C800%2C800italic%2C900%2C900italic%7CJost%3A100%2C100italic%2C200%2C200italic%2C300%2C300italic%2C400%2C400italic%2C500%2C500italic%2C600%2C600italic%2C700%2C700italic%2C800%2C800italic%2C900%2C900italic&#038;display=auto&#038;ver=6.0.2' media='all' />
</head>
<body>
    <?php include "PageHeader.php" ?>

<?php
    // WhippyTerm_0.9.1.0.tar.gz
    // WhippyTerm_0.9.1.0.deb
    $Files=scandir("Downloads/WhippyTerm/Linux");
    if(count($Files)>2)
    {
        echo "<h2>Linux</h2>";
        foreach($Files as $File)
        {
            if($File=="." || $File=="..")
                continue;

            if(substr($File,-7)==".tar.gz")
                $Version=substr($File,11,-7);
            else if(substr($File,-4)==".deb")
                $Version=substr($File,11,-4);
            else
                $Version=$File;

            echo "<a href='/Downloads/WhippyTerm/Linux/$File' download='$File'>Version $Version</a><br/>";
        }
    }
?>
<?php
    // WhippyTerm_0.9.1.0.tar.gz
    $Files=scandir("Downloads/WhippyTerm/RaspberryPI");
    if(count($Files)>2)
    {
        echo "<h2>Raspberry PI</h2>";
        foreach($Files as $File)
        {
            if($File=="." || $File=="..")
                continue;
            if(substr($File,-7)==".tar.gz")
                $Version=substr($File,11,-7);
            else
                $Version=$File;
            echo "<a href='/Downloads/WhippyTerm/RaspberryPI/$File' download='$File'>Version $Version</a><br/>";
        }
    }
?>

<?php
    // WhippyTerm_0.9.1.0.zip
    $Files=scandir("Downloads/WhippyTerm/Windows");
    if(count($Files)>2)
    {
        echo "<h2>Windows</h2>";
        foreach($Files as $File)
        {
            if($File=="." || $File=="..")
                continue;
            if(substr($File,-4)==".zip")
                $Version=substr($File,11,-4);
            else
                $Version=$File;
            echo "<a href='/Downloads/WhippyTerm/Windows/$File' download='$File'>Version $Version</a><br/>";
        }
    }
?>

</div>
</body>
</html>
