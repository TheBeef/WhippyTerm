/*******************************************************************************
 * FILENAME: ContextMenuHelper.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has a helper function to handle setting up a right mouse
 *    context for a widget.  It will take and search the QAction's that
 *    are in a class.  It will search for QAction that start with "Menu_"
 *    followed by the name of the width it goes with.  This is followed
 *    by a "_x" and a number.  This number is the order in which it will
 *    show up in the menu.  This is followed by a "_" and then the name of
 *    the action.
 *
 *    So:
 *      For the button "pushButton" then these are actions:
 *          "Menu_pushButton_x1_First"
 *          "Menu_pushButton_x2_Second"
 *          "Menu_pushButton_x3_Three"
 *          "Menu_pushButton_x4_Four"
 *
 *      If an action has the display text set to "---" then a separator.
 *
 * COPYRIGHT:
 *    Copyright 27 Oct 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Oct 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "ContextMenuHelper.h"
#include <QRegularExpression>
#include <QAction>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

bool ContextMenuSortCmp(QAction * &a,QAction * &b)
{
    int StartA;
    int StartB;
    int EndA;
    int EndB;
    int CmpA;
    int CmpB;

    StartA=a->objectName().indexOf("_x",5);
    StartB=b->objectName().indexOf("_x",5);
    if(StartA<0 || StartB<0)
        return false;

    EndA=a->objectName().indexOf("_",StartA+2);
    EndB=b->objectName().indexOf("_",StartB+2);

    CmpA=a->objectName().mid(StartA+2,EndA-(StartA+2)).toInt();
    CmpB=b->objectName().mid(StartB+2,EndB-(StartB+2)).toInt();

    return CmpA<CmpB;
}

/*******************************************************************************
 * NAME:
 *    AddContextMenu2Widget
 *
 * SYNOPSIS:
 *    QMenu *AddContextMenu2Widget(QWidget *Widgets,QWidget *Target);
 *
 * PARAMETERS:
 *    Widgets [I] -- The class that has the actions and widgets in it.
 *    Target [I] -- The widget to add the context menu to
 *
 * FUNCTION:
 *    This function searchs the actions with the 'Target' names in the format:
 *          Menu_NAME_xPOSITION_NAME
 *    Where:
 *      NAME -- The name of the widget that this action is a menu item for
 *      POSITION -- The order that it will be added menu.  The menu will be
 *                  sent though sort() sorting by this number as an int.
 *      NAME -- The name of the action.
 *
 *    The 'Target' will be set to custom context menu and the menu will be
 *    connected to this width.  Your class ('Widgets') must have a slot with
 *    this in it:
 *      void YOURCLASS::ContextMenu(const QPoint &pos)
 *      {
 *          HandleContextMenuClick(sender(),pos);
 *      }
 *
 * RETURNS:
 *    The QMenu that what built.
 *
 * NOTES:
 *    If the actions text is set to "---" a separator will be added instead.
 *
 * LIMITATIONS:
 *    No sub menus are supported and you must call this function in your
 *    constructor.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
QMenu *AddContextMenu2Widget(QWidget *Widgets,QWidget *Target)
{
    QList<QAction *> Search;
    QList<QAction *>::iterator i;
    QString SearchFor;
    QMenu *ContextMenu;

    SearchFor="^Menu_";
    SearchFor+=Target->objectName();
    SearchFor+="_x.*";

    Search = Widgets->findChildren<QAction *>(QRegularExpression(SearchFor));

    std::sort(Search.begin(),Search.end(),ContextMenuSortCmp);

    ContextMenu = new QMenu;
    for(i=Search.begin();i!=Search.end();i++)
    {
        if((*i)->text()=="---")
            ContextMenu->addSeparator();
        else
            ContextMenu->addAction(*i);
    }

    Target->setContextMenuPolicy(Qt::CustomContextMenu);
    Target->setProperty("ContextMenu",QVariant::fromValue(ContextMenu));

    Widgets->connect(Target,SIGNAL(customContextMenuRequested(const QPoint &)),
            Widgets,SLOT(ContextMenu(const QPoint &)));

    return ContextMenu;
}

void HandleContextMenuClick(QObject *Target,const QPoint &pos)
{
    QMenu *cmenu;
    QVariant var;
    QWidget *TargetWidget;

    TargetWidget=qobject_cast<QWidget*>(Target);
    if(TargetWidget==NULL)
        return;

    var=Target->property("ContextMenu");
    cmenu=var.value<QMenu *>();

    cmenu->exec(TargetWidget->mapToGlobal(pos));
}
