<!DOCTYPE html>
<?php
/*******************************************************************************
 * FILENAME: Links.php
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

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
$PageSelected=6;
$PageTitle="Links";
?>
<html>
<head>
    <title>Whippy Term - Links</title>
    <link rel="stylesheet" type="text/css" href="/somestyle.css">
    <link rel='stylesheet' href='https://fonts.googleapis.com/css?family=Roboto%3A100%2C100italic%2C200%2C200italic%2C300%2C300italic%2C400%2C400italic%2C500%2C500italic%2C600%2C600italic%2C700%2C700italic%2C800%2C800italic%2C900%2C900italic%7CRoboto+Slab%3A100%2C100italic%2C200%2C200italic%2C300%2C300italic%2C400%2C400italic%2C500%2C500italic%2C600%2C600italic%2C700%2C700italic%2C800%2C800italic%2C900%2C900italic%7CJost%3A100%2C100italic%2C200%2C200italic%2C300%2C300italic%2C400%2C400italic%2C500%2C500italic%2C600%2C600italic%2C700%2C700italic%2C800%2C800italic%2C900%2C900italic&#038;display=auto&#038;ver=6.0.2' media='all' />
</head>
<body>
    <?php include "PageHeader.php" ?>

    Other projects:

    <table>
        <tr>
            <td><a href="http://bittyhttp.com/">BittyHTTP</a></td>
            <td>A simple C web server</td>
        </tr>
        <tr>
            <td><a href="http://webcprecompiler.com/">WebC</a></td>
            <td>This is another project for making web pages in C. It uses php like escape tags to let you write C code and HTML in the same source. It uses Bitty HTTP as an example web server.</td>
        </tr>
        <tr>
            <td><a href="http://makemymakefile.com/">Make My Makefile</a></td>
            <td>A web site for generate a makefile for you. You answer a few questions hit the generate button and a good functional makefile. You then can just add new files directly to the makefile without needing regenerate it.</td>
        </tr>
    </table>

</div>
</body>
</html>
