#include "UI/UIDebug.h"

#include "Widget_TextCanvas.h"
#include "main.h"
#include "UI/UIDebug.h"
#include "QTKeyMappings.h"
#include <QPainter>
#include <QDebug>
#include <QtGui>

#define FOCUS_BOX_SIZE          1

Widget_TextCanvas::Widget_TextCanvas(QWidget *parent) : QWidget(parent)
{
    CursorX=0;
    CursorY=0;

    CursorSet2BeBlinking=true;
    CursorHiddenByBlink=false;
    DrawFocusBox=false;
    CursorStyle=e_TextCursorStyle_Block;
    HideCursor=false;
    CursorColor=QColor(0xFF,0xFF,0xFF);
    ScrollOffsetX=0;

    DisplayLeftEdgePx=0;
    DisplayTopEdgePx=0;
    DisplayWidth=320;
    DisplayHeight=200;

    TextAreaBackgroundColor.setRgb(0,0,0);

    DisplayBackgroundColor.setRgb(0,0,0);
    FillDisplayBackground=false;

    Setup4Paint();

    CursorTimer=new QTimer(this);
    connect(CursorTimer, SIGNAL(timeout()), this, SLOT(CursorTimerTick()));
    CursorTimer->start(500);

    RethinkCursor();

    OverrideWidget=new Frame_TextCavnasOverrideBox(this);
    OverrideWidget->setVisible(false);
    OverrideActive=false;
}

Widget_TextCanvas::~Widget_TextCanvas()
{
}

void Widget_TextCanvas::SetEventHandler(bool (*EventHandler)(const struct WTCEvent *Event),uintptr_t UserData)
{
    WTCEventHandler=EventHandler;
    WTCEventHandlerUserData=UserData;
}

/*******************************************************************************
 * NAME:
 *    Widget_TextCanvas::SendEvent()
 *
 * SYNOPSIS:
 *    bool Widget_TextCanvas::SendEvent(e_WTCEventType EventType,
 *          union WTCEventData *Info);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *
 * FUNCTION:
 *    This function sends a text canvas event out to it's registered event
 *    handler.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * NOTES:
 *    The return value is normally ignored as most events have no other action.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Widget_TextCanvas::SendEvent(e_WTCEventType EventType,
        union WTCEventData *Info)
{
    struct WTCEvent NewEvent;

    NewEvent.EventType=EventType;
    NewEvent.UserData=WTCEventHandlerUserData;
    NewEvent.Info=Info;

    if(WTCEventHandler!=NULL)
        return WTCEventHandler(&NewEvent);
    return true;
}
bool Widget_TextCanvas::SendEvent(e_WTCEventType EventType)
{
    return SendEvent(EventType,nullptr);
}

void Widget_TextCanvas::Setup4Paint(void)
{
    QFontMetrics fm(RenderFont);

    GUICharHeight=fm.lineSpacing()+1;   // +1 Had some problems with _ not always showing up when using some fonts.
    GUICharWidth=fm.averageCharWidth();
    UnderLinePos=fm.underlinePos();
    OverLinePos=fm.overlinePos();
    StrikeOutPos=fm.strikeOutPos();
    UnderLineHeight=fm.lineWidth();
    BaseLine=fm.ascent();

    dotted << 1 << 1;
    dashed << 4 << 4;
}

/*******************************************************************************
 * NAME:
 *    Widget_TextCanvas::SetFont
 *
 * SYNOPSIS:
 *    void Widget_TextCanvas::SetFont(const char *FontName,int PointSize,
 *          bool Bold,bool Italic);
 *
 * PARAMETERS:
 *    FontName [I] -- The name of font to use
 *    PointSize [I] -- The point size
 *    Bold [I] -- Set this font as the bold version
 *    Italic [I] -- Set this font as the italic version
 *
 * FUNCTION:
 *    This function set the render font to use.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem 
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Widget_TextCanvas::SetFont(const char *FontName,int PointSize,bool Bold,
        bool Italic)
{
    RenderFont.setStyleHint(QFont::Monospace,
            (QFont::StyleStrategy)(QFont::PreferBitmap|QFont::PreferMatch));
    RenderFont.setFamily(FontName);
    RenderFont.setWeight(QFont::Normal);
    RenderFont.setFixedPitch(true);
    RenderFont.setPointSize(PointSize);
    RenderFont.setKerning(false);   // Doesn't actually work
    RenderFont.setLetterSpacing(QFont::AbsoluteSpacing,0);
    RenderFont.setWordSpacing(0);

    RenderFont.setBold(Bold);
    RenderFont.setItalic(Italic);

    Setup4Paint();

    return true;
}

void Widget_TextCanvas::mousePressEvent(QMouseEvent * event)
{
    union WTCEventData EventData;
    QPoint Pos;
    e_WTCEventType EventType;

    if(OverrideActive)
        return;

    Pos=event->pos();

    EventData.Mouse.x=Pos.x();
    EventData.Mouse.y=Pos.y();

    if(event->button()==Qt::LeftButton)
        EventType=e_WTCEvent_MouseDown;
    else if(event->button()==Qt::RightButton)
        EventType=e_WTCEvent_MouseRightDown;
    else if(event->button()==Qt::MiddleButton)
        EventType=e_WTCEvent_MouseMiddleDown;
    else
        return;

    SendEvent(EventType,&EventData);
}

void Widget_TextCanvas::mouseReleaseEvent(QMouseEvent * event)
{
    union WTCEventData EventData;
    QPoint Pos;
    e_WTCEventType EventType;

    if(OverrideActive)
        return;

    Pos=event->pos();

    EventData.Mouse.x=Pos.x();
    EventData.Mouse.y=Pos.y();

    if(event->button()==Qt::LeftButton)
        EventType=e_WTCEvent_MouseUp;
    else if(event->button()==Qt::RightButton)
        EventType=e_WTCEvent_MouseRightUp;
    else if(event->button()==Qt::MiddleButton)
        EventType=e_WTCEvent_MouseMiddleUp;
    else
        return;

    SendEvent(EventType,&EventData);
}

void Widget_TextCanvas::mouseMoveEvent(QMouseEvent *event)
{
    union WTCEventData EventData;
    QPoint Pos;

    if(OverrideActive)
        return;

    Pos=event->pos();

    EventData.Mouse.x=Pos.x();
    EventData.Mouse.y=Pos.y();
    SendEvent(e_WTCEvent_MouseMove,&EventData);
}

void Widget_TextCanvas::focusInEvent(QFocusEvent * event)
{
    SendEvent(e_WTCEvent_GotFocus);

    QWidget::focusInEvent(event);
}

void Widget_TextCanvas::focusOutEvent(QFocusEvent * event)
{
    SendEvent(e_WTCEvent_LostFocus);

    QWidget::focusOutEvent(event);
}

void Widget_TextCanvas::wheelEvent(QWheelEvent * event)
{
    union WTCEventData EventData;
//    int numDegrees = event->delta() / 8;
//    int numSteps = numDegrees / 15;
    QPoint numDegrees;
    QPoint numSteps;
    uint8_t Mods;

    if(OverrideActive)
        return;
    
    numDegrees = event->angleDelta() / 8;
    
    if(!numDegrees.isNull())
    {
        numSteps = numDegrees / 15;
        
        Mods=0;
        if(event->modifiers() & Qt::ShiftModifier)
            Mods|=KEYMOD_SHIFT;
        if(event->modifiers() & Qt::ControlModifier)
            Mods|=KEYMOD_CONTROL;
        if(event->modifiers() & Qt::AltModifier)
            Mods|=KEYMOD_ALT;
        if(event->modifiers() & Qt::MetaModifier)
            Mods|=KEYMOD_LOGO;
        
        EventData.MouseWheel.Steps=numSteps.y();
        EventData.MouseWheel.Mods=Mods;
        SendEvent(e_WTCEvent_MouseWheel,&EventData);
    }

    event->accept();
}

void Widget_TextCanvas::keyPressEvent(QKeyEvent *event)
{
    uint8_t Mods;
    e_UIKeys UIKey;
    union WTCEventData EventData;
    std::string Text;
    char c;

    if(OverrideActive)
        return;

    Mods=0;
    if(event->modifiers() & Qt::ShiftModifier)
        Mods|=KEYMOD_SHIFT;
    if(event->modifiers() & Qt::ControlModifier)
        Mods|=KEYMOD_CONTROL;
    if(event->modifiers() & Qt::AltModifier)
        Mods|=KEYMOD_ALT;
    if(event->modifiers() & Qt::MetaModifier)
        Mods|=KEYMOD_LOGO;

    UIKey=ConvertQTKey2UIKey((Qt::Key)event->key());

    if(UIKey==e_UIKeysMAX)
    {
        /* Convert CTRL-? (AscII<32) to their letter and let 'Mods' say
           it was a letter with the ctrl key held */
        if(event->text().length()==1 && event->text()[0]<' ')
        {
            /* Remove the CTRL part */
            c=event->text()[0].toLatin1();
            c+='@'; // Move up to ABC range
            Text=c;
        }
        else
        {
            Text=event->text().toStdString();
        }
    }
    else
    {
        Text="";
    }

    EventData.Key.Mods=Mods;
    EventData.Key.Key=UIKey;
    EventData.Key.TextPtr=(const uint8_t *)Text.c_str();
    EventData.Key.TextLen=Text.length();

    if(!SendEvent(e_WTCEvent_KeyEvent,&EventData))
    {
        QWidget::keyPressEvent(event);
    }
}

void Widget_TextCanvas::resizeEvent(QResizeEvent *event)
{
    union WTCEventData EventData;

    EventData.NewSize.Width=event->size().width();
    EventData.NewSize.Height=event->size().height();

    GetCorrectedWidgetSize(EventData.NewSize.Width,EventData.NewSize.Height);

    ResizeOverrideWidget();

    SendEvent(e_WTCEvent_Resize,&EventData);
}

void Widget_TextCanvas::RedrawScreen(void)
{
    update();
}

void Widget_TextCanvas::ClearAllLines(void)
{
    Lines.clear();
    RethinkCursor();
}

void Widget_TextCanvas::ClearLine(unsigned int Line,uint32_t BGColor)
{
    struct WTCLine BlankLine;

    BlankLine.BGFillColor=BGColor;
    if(Line>=Lines.size())
    {
        /* We need to add blank lines until we get to the correct number of
           lines */
        while(Line>=Lines.size())
            Lines.push_back(BlankLine);
    }
    Lines[Line]=BlankLine;
}

void Widget_TextCanvas::AppendTextFrag(unsigned int Line,const struct TextCanvasFrag *Frag)
{
    struct WTCFrag NewFrag;
    struct WTCFrag *AddedFrag;

    if(Line>=Lines.size())
        return;

    Lines[Line].Fragments.push_back(NewFrag);
    AddedFrag=&Lines[Line].Fragments.back();

    /* Convert the frag */
    AddedFrag->FragType=Frag->FragType;
    AddedFrag->Text=Frag->Text;
    AddedFrag->Styling=Frag->Styling;
    AddedFrag->Value=Frag->Value;
    AddedFrag->Data=Frag->Data;
}

void Widget_TextCanvas::RedrawLine(unsigned int Line)
{
    if(Line==CursorY)
        RethinkCursor();

    update(DisplayLeftEdgePx,DisplayTopEdgePx+Line*GUICharHeight,
            DisplayWidth,GUICharHeight);
}

void Widget_TextCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    unsigned int line;
    unsigned int ScreenY;
    i_WTCLineFrags Frag;
    QColor FocusColor;
    QPen FocusPen;
    QPalette FocusPal;
    QFontMetrics fm(RenderFont);
    int px;
    QColor TmpColor;
    QRegion OldRegion;
    bool OldClippingOn;
    QRegion NewRegion(DisplayLeftEdgePx,DisplayTopEdgePx,DisplayWidth,
            DisplayHeight);

    RethinkColors();

    if(FillDisplayBackground)
    {
        /* Fill above */
        painter.fillRect(0,                                     // x
                0,                                              // y
                width(),                                        // width
                DisplayTopEdgePx,                               // height
                UseDisplayBackgroundColor);

        /* Fill left */
        painter.fillRect(0,                                     // x
                0,                                              // y
                DisplayLeftEdgePx,                              // width
                height(),                                       // height
                UseDisplayBackgroundColor);

        /* Fill bottom */
        painter.fillRect(0,                                     // x
                DisplayTopEdgePx+DisplayHeight,                 // y
                width(),                                        // width
                height()-(DisplayTopEdgePx+DisplayHeight),      // height
                UseDisplayBackgroundColor);

        /* Fill right */
        painter.fillRect(DisplayLeftEdgePx+DisplayWidth,        // x
                0,                                              // y
                width()-(DisplayLeftEdgePx+DisplayWidth),       // width
                height(),                                       // height
                UseDisplayBackgroundColor);
    }

    if(DrawFocusBox)
    {
        // https://forum.qt.io/topic/58597/solved-get-focus-rectangle-pen-style-and-color/6
        if(hasFocus())
            FocusColor=FocusPal.color(QPalette::Normal,QPalette::Highlight);
        else
            FocusColor=FocusPal.color(QPalette::Normal,QPalette::Window);
        FocusPen.setColor(FocusColor);
        FocusPen.setWidth(FOCUS_BOX_SIZE*2);
        painter.setPen(FocusPen);
        painter.drawRect(0,0,width(),height());
    }

    /* Restrict the update to the display area */
    OldRegion=painter.clipRegion();
    OldClippingOn=painter.hasClipping();
    painter.setClipRegion(NewRegion);
    painter.setClipping(true);

    for(line=0;line<Lines.size();line++)
    {
        ScreenY=line*GUICharHeight;

        px=0;
        for(Frag=Lines[line].Fragments.begin();
                Frag!=Lines[line].Fragments.end();Frag++)
        {
            px+=DrawFrag(&painter,&fm,DisplayLeftEdgePx+px-ScrollOffsetX,
                    DisplayTopEdgePx+ScreenY,&*Frag);
        }

        /* Fill the rest of the line */
        TmpColor=QColor(QRgb(Lines[line].BGFillColor));
        if(OverrideActive)
            TmpColor=TmpColor.darker(200); // 1/2 bright;
        painter.fillRect(DisplayLeftEdgePx+px-ScrollOffsetX,
                DisplayTopEdgePx+ScreenY,width()-(px-ScrollOffsetX),
                GUICharHeight,TmpColor);
    }

    /* Fill the rest with the last bk color */
    ScreenY=line*GUICharHeight;
    painter.fillRect(DisplayLeftEdgePx,DisplayTopEdgePx+ScreenY,
            DisplayWidth,DisplayHeight,UseTextAreaBackgroundColor);

    DrawCursor(&painter,&fm);

    painter.setClipRegion(OldRegion);
    painter.setClipping(OldClippingOn);
}

int Widget_TextCanvas::DrawFrag(QPainter *painter,QFontMetrics *fm,
        int ScreenX,int ScreenY,struct WTCFrag *Frag)
{
    int px;
    struct WTCFrag EOLFrag;

    switch(Frag->FragType)
    {
        case e_TextCanvasFrag_String:
            px=DrawTextFrag(painter,fm,ScreenX,ScreenY,Frag);
        break;
        case e_TextCanvasFrag_NonPrintableChar:
            px=DrawTextFrag(painter,fm,ScreenX,ScreenY,Frag);
        break;
        case e_TextCanvasFrag_SoftRet:
            px=DrawSoftRet(painter,fm,ScreenX,ScreenY,Frag);
        break;
        case e_TextCanvasFrag_HardRet:
            px=DrawHardRet(painter,fm,ScreenX,ScreenY,Frag);
        break;
        case e_TextCanvasFrag_RetText:
            px=DrawTextFrag(painter,fm,ScreenX,ScreenY,Frag);
        break;
        case e_TextCanvasFrag_HR:
            px=DrawHRFrag(painter,fm,ScreenX,ScreenY,Frag);
        break;
        case e_TextCanvasFragMAX:
        default:
            px=0;
        break;
    }
    return px;
}

int Widget_TextCanvas::CalcFragWidth(QFontMetrics *fm,struct WTCFrag *Frag)
{
    int px;

    switch(Frag->FragType)
    {
        case e_TextCanvasFrag_String:
        case e_TextCanvasFrag_NonPrintableChar:
        case e_TextCanvasFrag_RetText:
            px=CalTextFragWidth(fm,Frag,Frag->Text);
        break;
        case e_TextCanvasFrag_SoftRet:
            px=CalSoftRetFragWidth(fm);
        break;
        case e_TextCanvasFrag_HardRet:
            px=CalHardRetFragWidth(fm);
        break;
        case e_TextCanvasFrag_HR:
            px=0;
        break;
        case e_TextCanvasFragMAX:
        default:
            px=0;
        break;
    }
    return px;
}

void Widget_TextCanvas::RethinkCursor(void)
{
    int StrLen;
    int p;
    QString FirstHalf;
    unsigned int xpos;
    QFontMetrics fm(RenderFont);
    i_WTCLineFrags Frag;

    if(CursorY>=Lines.size())
    {
        /* We are in virtual space */
        CursorFrag.FragType=e_TextCanvasFrag_String;
        CursorFrag.Text=" ";
        CursorFrag.Styling.FGColor=0xFFFFFF;
        CursorFrag.Styling.BGColor=UseTextAreaBackgroundColor.rgb();
        CursorFrag.Styling.ULineColor=0xFFFFFF;
        CursorFrag.Styling.Attribs=0;

        /* Move the cursor by the number of virtual char's times the size of a
           space */
        CursorPx=CursorX*CalTextFragWidth(&fm,&CursorFrag,CursorFrag.Text);
        return;
    }

    CursorPx=0;

    xpos=0;
    for(Frag=Lines[CursorY].Fragments.begin();
            Frag!=Lines[CursorY].Fragments.end();Frag++)
    {
        switch(Frag->FragType)
        {
            case e_TextCanvasFrag_String:
                if(Frag->Text.length()<1)
                    break;
                StrLen=Frag->Text.length();
                if(CursorX>=xpos && CursorX<xpos+StrLen)
                {
                    /* Ok, this frag has the cursor in it, we need to split the
                       fragment  */
                    p=CursorX-xpos;
                    FirstHalf=Frag->Text.left(p);
                    CursorPx+=CalTextFragWidth(&fm,&*Frag,FirstHalf);

                    /* Save what's under the cursor */
                    CursorFrag=*Frag;
                    CursorFrag.Text=Frag->Text.mid(p,1);

                    /* Ok, we have what we where looking for.  We are done */
                    return;
                }

                CursorPx+=CalTextFragWidth(&fm,&*Frag,Frag->Text);
                xpos+=StrLen;
            break;
            case e_TextCanvasFrag_NonPrintableChar:
                /* Cursor can't be in the NP char */
                CursorPx+=CalTextFragWidth(&fm,&*Frag,Frag->Text);
            break;
            case e_TextCanvasFrag_SoftRet:
                if(CursorX==xpos)
                {
                    /* Ok, this frag has the cursor in it */

                    /* Save what's under the cursor */
                    CursorFrag=*Frag;

                    /* Ok, we have what we where looking for.  We are done */
                    return;
                }

                CursorPx+=CalSoftRetFragWidth(&fm);
                xpos++;
            break;
            case e_TextCanvasFrag_HardRet:
                if(CursorX==xpos)
                {
                    /* Ok, this frag has the cursor in it */

                    /* Save what's under the cursor */
                    CursorFrag=*Frag;

                    /* Ok, we have what we where looking for.  We are done */
                    return;
                }

                CursorPx+=CalHardRetFragWidth(&fm);
                xpos++;
            break;
            case e_TextCanvasFrag_RetText:
                if(CursorX==xpos)
                {
                    /* Ok, this frag has the cursor in it */

                    /* Save what's under the cursor */
                    CursorFrag=*Frag;

                    /* Ok, we have what we where looking for.  We are done */
                    return;
                }

                CursorPx+=CalTextFragWidth(&fm,&*Frag,Frag->Text);
                xpos+=1;
            break;
            case e_TextCanvasFrag_HR:
                /* The HR can be the only thing on a line so if we see one
                   then we have found the cursor */

                CursorFrag=*Frag;

                /* Ok, we have what we where looking for.  We are done */
                return;
            break;
            case e_TextCanvasFragMAX:
            default:
            break;
        }
    }

    /* Ok, we ran out of text so the cursor is in eol virtual space */
    CursorFrag.FragType=e_TextCanvasFrag_String;
    CursorFrag.Text=" ";
    CursorFrag.Styling.FGColor=0xFFFFFF;
    CursorFrag.Styling.BGColor=Lines[CursorY].BGFillColor;
    CursorFrag.Styling.ULineColor=0xFFFFFF;
    CursorFrag.Styling.Attribs=0;

    /* Move the cursor by the number of virtual char's times the size of a
       space */
    p=CursorX-xpos;
    CursorPx+=p*CalTextFragWidth(&fm,&CursorFrag,CursorFrag.Text);
}

int Widget_TextCanvas::CalTextFragWidth(QFontMetrics *fm,
        struct WTCFrag *Frag,const QString &FragStr)
{
    int TextWidth;
    bool FontWasBold;
    bool FontWasItalic;

    FontWasBold=RenderFont.bold();
    FontWasItalic=RenderFont.italic();

    if(Frag->Styling.Attribs&TXT_ATTRIB_BOLD)
        RenderFont.setBold(true);

    if(Frag->Styling.Attribs&TXT_ATTRIB_ITALIC)
        RenderFont.setItalic(true);

    /* See what the width of the drawen text will be */
#if 0
    TextWidth=fm->width(FragStr);           // Warning: It doc's say this is obsolete and to use horizontalAdvance()
#else
    TextWidth=fm->horizontalAdvance(FragStr);
#endif
    RenderFont.setBold(FontWasBold);
    RenderFont.setItalic(FontWasItalic);

    return TextWidth;
}

int Widget_TextCanvas::DrawTextFrag(QPainter *painter,QFontMetrics *fm,
        int ScreenX,int ScreenY,struct WTCFrag *Frag)
{
    QPen DrawPen;
    int UL_X1;
    int UL_X2;
    int UL_Y1;
    int ScreenBaseLineY;
    int ScreenThickAdjust;
    bool DrawUnderLine;
    int TextWidth;
    int MaxScreenCharY;
    QColor FgColor;
    QColor BgColor;
    QColor UlColor;
    bool FontWasBold;
    bool FontWasItalic;
    int Bottom;

    FgColor=QColor(QRgb(Frag->Styling.FGColor));
    BgColor=QColor(QRgb(Frag->Styling.BGColor));
    UlColor=QColor(QRgb(Frag->Styling.ULineColor));

    if(OverrideActive)
    {
        FgColor=FgColor.darker(200); // 1/2 bright;
        BgColor=BgColor.darker(200); // 1/2 bright;
        UlColor=UlColor.darker(200); // 1/2 bright;
    }

    DrawPen.setWidth(0);    // 1 pixel (don't ask)
    DrawPen.setStyle(Qt::SolidLine);
    DrawPen.setCapStyle(Qt::FlatCap);
    DrawPen.setJoinStyle(Qt::BevelJoin);
    DrawPen.setColor(FgColor);
    painter->setPen(DrawPen);

    painter->setBackground(QBrush(BgColor));

    FontWasBold=RenderFont.bold();
    FontWasItalic=RenderFont.italic();

    if(Frag->Styling.Attribs&TXT_ATTRIB_BOLD)
        RenderFont.setBold(true);

    if(Frag->Styling.Attribs&TXT_ATTRIB_ITALIC)
        RenderFont.setItalic(true);

    painter->setFont(RenderFont);

    /* See what the width of the drawen text will be */
#if 0
    TextWidth=fm->width(Frag->Text);           // Warning: It doc's say this is obsolete and to use horizontalAdvance()
#else
    TextWidth=fm->horizontalAdvance(Frag->Text);
#endif
    /* Clear background (we need to do this because sometimes QT drops the
       Qt::TransparentMode when drawing Unicode chars (like U+2302 on
       Windows)) */
    painter->fillRect(ScreenX,ScreenY,TextWidth,GUICharHeight,BgColor);

    if(Frag->Styling.Attribs&TXT_ATTRIB_OUTLINE)
    {
        painter->setBackgroundMode(Qt::TransparentMode);
        painter->drawText(ScreenX-1,ScreenY,TextWidth,GUICharHeight,
                Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine,Frag->Text);
        painter->drawText(ScreenX+1,ScreenY,TextWidth,GUICharHeight,
                Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine,Frag->Text);
        painter->drawText(ScreenX,ScreenY-1,TextWidth,GUICharHeight,
                Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine,Frag->Text);
        painter->drawText(ScreenX,ScreenY+1,TextWidth,GUICharHeight,
                Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine,Frag->Text);

        /* Draw in background color */
        DrawPen.setColor(BgColor);
        painter->setPen(DrawPen);
    }

    if(Frag->Styling.Attribs&TXT_ATTRIB_ROUNDBOX)
    {
        /* We swap the forground and background colors because this is
           inverted */
        Bottom=GUICharHeight-2;

        /* Fill the background with the forground, but skip the first and
           last px */
        painter->fillRect(ScreenX+1,ScreenY+1,TextWidth-1,Bottom,FgColor);

        DrawPen.setColor(BgColor);
        painter->setPen(DrawPen);
        painter->setBackground(QBrush(FgColor));

        painter->drawText(ScreenX,ScreenY,
                TextWidth,GUICharHeight,
                Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine,Frag->Text);

        /* Remove the corners to make it round */
        painter->drawPoint(ScreenX+1,ScreenY+1);
        painter->drawPoint(ScreenX+TextWidth-1,ScreenY+1);
        painter->drawPoint(ScreenX+TextWidth-1,ScreenY+Bottom);
        painter->drawPoint(ScreenX+1,ScreenY+Bottom);

        DrawPen.setColor(FgColor);
        painter->setPen(DrawPen);
        painter->setBackground(QBrush(BgColor));
    }
    else
    {
        painter->drawText(ScreenX,ScreenY,
                TextWidth,GUICharHeight,
                Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine,Frag->Text);
    }

    if(Frag->Styling.Attribs&TXT_ATTRIB_OUTLINE)
    {
        painter->setBackgroundMode(Qt::OpaqueMode);
        DrawPen.setColor(FgColor);
    }

    DrawPen.setWidth(UnderLineHeight);
    DrawPen.setColor(UlColor);
    painter->setPen(DrawPen);

    /* See if we need to draw a line (under,over,through,etc) */
    if(Frag->Styling.Attribs&(TXT_ATTRIB_LINETHROUGHT|TXT_ATTRIB_OVERLINE|
            TXT_ATTRIB_UNDERLINE|TXT_ATTRIB_UNDERLINE_DOUBLE|
            TXT_ATTRIB_UNDERLINE_DOTTED|TXT_ATTRIB_UNDERLINE_DASHED|
            TXT_ATTRIB_UNDERLINE_WAVY))
    {
        UL_X1=ScreenX;
        UL_X2=ScreenX+TextWidth;
        ScreenBaseLineY=ScreenY+BaseLine;
        ScreenThickAdjust=UnderLineHeight/2;
        MaxScreenCharY=ScreenY+GUICharHeight;

        /* Strike Out */
        if(Frag->Styling.Attribs&TXT_ATTRIB_LINETHROUGHT)
        {
            UL_Y1=ScreenBaseLineY-StrikeOutPos+ScreenThickAdjust;

            painter->drawLine(UL_X1,UL_Y1,UL_X2,UL_Y1);
        }

        /* Over line */
        if(Frag->Styling.Attribs&TXT_ATTRIB_OVERLINE)
        {
            UL_Y1=ScreenBaseLineY-(OverLinePos-1)+ScreenThickAdjust;

            painter->drawLine(UL_X1,UL_Y1,UL_X2,UL_Y1);
        }

        /* Underline */
        DrawUnderLine=false;
        UL_Y1=ScreenBaseLineY+UnderLinePos+ScreenThickAdjust;

        /* Font's suck.  Make sure the underline is still inside the char */
        if(UL_Y1>=MaxScreenCharY)
            UL_Y1=MaxScreenCharY-1;

        if(Frag->Styling.Attribs&TXT_ATTRIB_UNDERLINE)
        {
            DrawUnderLine=true;
        }
        if(Frag->Styling.Attribs&TXT_ATTRIB_UNDERLINE_DOUBLE)
        {
            /* Draw a line 1 above the underline pos and 1 below */
            if(UL_Y1+UnderLineHeight>=MaxScreenCharY)
            {
                /* We will be off the bottom, adjust */
                UL_Y1-=UnderLineHeight;
            }

            UL_Y1=UL_Y1-UnderLineHeight;
            painter->drawLine(
                    UL_X1,
                    UL_Y1,
                    UL_X2,
                    UL_Y1);

            UL_Y1+=UnderLineHeight*2;

            painter->drawLine(
                    UL_X1,
                    UL_Y1,
                    UL_X2,
                    UL_Y1);
        }
        if(Frag->Styling.Attribs&TXT_ATTRIB_UNDERLINE_DOTTED)
        {
            DrawPen.setDashPattern(dotted);
            DrawPen.setStyle(Qt::CustomDashLine);
            painter->setPen(DrawPen);

            DrawUnderLine=true;
        }
        if(Frag->Styling.Attribs&TXT_ATTRIB_UNDERLINE_DASHED)
        {
            DrawPen.setDashPattern(dashed);
            DrawPen.setStyle(Qt::CustomDashLine);
            painter->setPen(DrawPen);

            DrawUnderLine=true;
        }
        if(Frag->Styling.Attribs&TXT_ATTRIB_UNDERLINE_WAVY)
        {
            DrawPen.setDashPattern(dashed);
            DrawPen.setStyle(Qt::CustomDashLine);
            painter->setPen(DrawPen);
            if(UL_Y1+UnderLineHeight>=MaxScreenCharY)
            {
                /* Draw above the under line because we will be off the bottom
                   otherwize */
                painter->drawLine(
                        UL_X1+4,UL_Y1-UnderLineHeight,
                        UL_X2,UL_Y1-UnderLineHeight);
            }
            else
            {
                /* Draw below the underline */
                painter->drawLine(
                        UL_X1+4,UL_Y1+UnderLineHeight,
                        UL_X2,UL_Y1+UnderLineHeight);
            }

            DrawUnderLine=true;
        }

        if(DrawUnderLine)
        {
            painter->drawLine(UL_X1,UL_Y1,UL_X2,UL_Y1);
        }
    }

    RenderFont.setBold(FontWasBold);
    RenderFont.setItalic(FontWasItalic);

    return TextWidth;
}

void Widget_TextCanvas::DrawCursor(QPainter *painter,QFontMetrics *fm)
{
    int x,y;
    QPen DrawPen;
    int offset;
    uint32_t SavedFGColor;
    uint32_t SavedBGColor;
    QBrush DrawBrush;
    int CursorWidth;
    int CursorHeight;
    QColor UseCursorColor;

//    if(CursorFrag.Text=="")
//        return;

    x=CursorPx;
    y=CursorY*GUICharHeight;

    UseCursorColor=CursorColor;
    if(OverrideActive)
        UseCursorColor=UseCursorColor.darker(200); // 1/2 bright;

    DrawPen.setStyle(Qt::SolidLine);
    DrawPen.setCapStyle(Qt::FlatCap);
    DrawPen.setJoinStyle(Qt::BevelJoin);
    DrawPen.setColor(UseCursorColor);

    if(CursorStyle==e_TextCursorStyle_Block && !HideCursor)
    {
        /* We are going to override the colors and draw the char normally */
        SavedFGColor=CursorFrag.Styling.FGColor;
        SavedBGColor=CursorFrag.Styling.BGColor;

        CursorFrag.Styling.FGColor=SavedBGColor;
        CursorFrag.Styling.BGColor=UseCursorColor.rgb();

        DrawFrag(painter,fm,DisplayLeftEdgePx+x-ScrollOffsetX,
                DisplayTopEdgePx+y,&CursorFrag);

        CursorFrag.Styling.FGColor=SavedFGColor;
        CursorFrag.Styling.BGColor=SavedBGColor;
    }
    else
    {
        /* Draw the background char */
        CursorWidth=DrawFrag(painter,fm,DisplayLeftEdgePx+x-ScrollOffsetX,
                DisplayTopEdgePx+y,&CursorFrag);

        if(!HideCursor)
        {
            switch(CursorStyle)
            {
                case e_TextCursorStyle_Block:
                break;
                case e_TextCursorStyle_UnderLine:
                    offset=BaseLine+UnderLinePos-2;
                    CursorHeight=GUICharHeight-BaseLine;

                    /* Make sure we are not off the bottom of the char */
                    if(offset+CursorHeight>=GUICharHeight)
                        CursorHeight=GUICharHeight-offset;

                    painter->fillRect(DisplayLeftEdgePx+x-ScrollOffsetX,
                            DisplayTopEdgePx+y+offset,CursorWidth,
                            CursorHeight,UseCursorColor);
                break;
                case e_TextCursorStyle_Line:
                    painter->fillRect(DisplayLeftEdgePx+x-ScrollOffsetX,
                            DisplayTopEdgePx+y,3,GUICharHeight,UseCursorColor);
                break;
                case e_TextCursorStyle_Box:
                    DrawPen.setWidth(UnderLineHeight);
                    painter->setPen(DrawPen);
                    painter->drawRect(DisplayLeftEdgePx+x-ScrollOffsetX,
                            DisplayTopEdgePx+y,CursorWidth-1,GUICharHeight-1);
                break;
                case e_TextCursorStyle_Dotted:
                    DrawBrush=painter->brush();
                    DrawBrush.setColor(UseCursorColor);
                    DrawBrush.setStyle(Qt::Dense4Pattern);
                    painter->fillRect(DisplayLeftEdgePx+x-ScrollOffsetX,
                            DisplayTopEdgePx+y,CursorWidth,GUICharHeight,
                            DrawBrush);
                break;
                case e_TextCursorStyle_Hidden:
                break;
                case e_TextCursorStyleMAX:
                default:
                break;
            }
        }
    }
}

int Widget_TextCanvas::DrawSoftRet(QPainter *painter,QFontMetrics *fm,
        int ScreenX,int ScreenY,struct WTCFrag *Frag)
{
    // DEBUG PAUL: Someday may want to this out as:
    //    |
    // <--+
    //
    // For now we are just drawing the Uncode char

    QPen DrawPen;
    int TextWidth;
    QColor FgColor;
    QColor BgColor;
    QString TheChar="\u21B5";

    FgColor=QColor(QRgb(Frag->Styling.FGColor));
    BgColor=QColor(QRgb(Frag->Styling.BGColor));

    if(OverrideActive)
    {
        FgColor=FgColor.darker(200); // 1/2 bright;
        BgColor=BgColor.darker(200); // 1/2 bright;
    }

    DrawPen.setWidth(0);    // 1 pixel (don't ask)
    DrawPen.setStyle(Qt::SolidLine);
    DrawPen.setCapStyle(Qt::FlatCap);
    DrawPen.setJoinStyle(Qt::BevelJoin);
    DrawPen.setColor(FgColor);
    painter->setPen(DrawPen);
    painter->setBackground(QBrush(BgColor));

    painter->setFont(RenderFont);

    /* See what the width of the drawen text will be */
#if 0
    TextWidth=fm->width(TheChar);           // Warning: It doc's say this is obsolete and to use horizontalAdvance()
#else
    TextWidth=fm->horizontalAdvance(TheChar);
#endif
    /* Clear background (we need to do this because sometimes QT drops the
       Qt::TransparentMode when drawing Unicode chars (like U+2302 on
       Windows)) */
    painter->fillRect(ScreenX,ScreenY,TextWidth,GUICharHeight,BgColor);

    painter->drawText(ScreenX,ScreenY,TextWidth,GUICharHeight,
            Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine,TheChar);

    return TextWidth;
}

int Widget_TextCanvas::CalSoftRetFragWidth(QFontMetrics *fm)
{
    QString TheChar="\u21B5";

    /* See what the width of the drawen text will be */
#if 0
    return fm->width(TheChar);           // Warning: It doc's say this is obsolete and to use horizontalAdvance()
#else
    return fm->horizontalAdvance(TheChar);
#endif
}

int Widget_TextCanvas::DrawHardRet(QPainter *painter,QFontMetrics *fm,
        int ScreenX,int ScreenY,struct WTCFrag *Frag)
{
    QPen DrawPen;
    QColor FgColor;
    QColor BgColor;
    QPainterPath Arrow;

    FgColor=QColor(QRgb(Frag->Styling.FGColor));
    BgColor=QColor(QRgb(Frag->Styling.BGColor));

    if(OverrideActive)
    {
        FgColor=FgColor.darker(200); // 1/2 bright;
        BgColor=BgColor.darker(200); // 1/2 bright;
    }

    DrawPen.setWidth(0);    // 1 pixel (don't ask)
    DrawPen.setStyle(Qt::SolidLine);
    DrawPen.setCapStyle(Qt::FlatCap);
    DrawPen.setJoinStyle(Qt::BevelJoin);
    DrawPen.setColor(FgColor);
    painter->setPen(DrawPen);
    painter->setBackground(QBrush(BgColor));

    Arrow.moveTo(ScreenX+1,ScreenY+GUICharHeight/2);
    Arrow.lineTo(ScreenX+1+GUICharHeight/4,ScreenY+GUICharHeight/4);
    Arrow.lineTo(ScreenX+1+GUICharHeight/4,ScreenY+GUICharHeight/2+GUICharHeight/4);
    Arrow.lineTo(ScreenX+1,ScreenY+GUICharHeight/2);

    painter->fillRect(ScreenX,ScreenY,GUICharHeight/4+2,GUICharHeight,BgColor);
    painter->fillPath(Arrow,QBrush(FgColor));

    return GUICharHeight/4+2;
}

int Widget_TextCanvas::CalHardRetFragWidth(QFontMetrics *fm)
{
    return GUICharHeight/4+2;
}

int Widget_TextCanvas::DrawHRFrag(QPainter *painter,QFontMetrics *fm,
        int ScreenX,int ScreenY,struct WTCFrag *Frag)
{
    QPen DrawPen;
    QColor FgColor;
    QColor BgColor;
    int DrawWidth;

    FgColor=QColor(QRgb(Frag->Styling.FGColor));
    BgColor=QColor(QRgb(Frag->Styling.BGColor));

    if(OverrideActive)
    {
        FgColor=FgColor.darker(200); // 1/2 bright;
        BgColor=BgColor.darker(200); // 1/2 bright;
    }

    DrawPen.setWidth(0);    // 1 pixel (don't ask)
    DrawPen.setStyle(Qt::DashLine);
    DrawPen.setCapStyle(Qt::FlatCap);
    DrawPen.setJoinStyle(Qt::BevelJoin);
    DrawPen.setColor(FgColor);
    painter->setPen(DrawPen);
    painter->setBackground(QBrush(BgColor));

    painter->setFont(RenderFont);

    DrawWidth=DisplayWidth;

    /* Clear background (we need to do this because sometimes QT drops the
       Qt::TransparentMode when drawing Unicode chars (like U+2302 on
       Windows)) */
    painter->fillRect(ScreenX,ScreenY,DrawWidth,GUICharHeight,BgColor);
    painter->drawLine(ScreenX,ScreenY+GUICharHeight/2,DrawWidth,
            ScreenY+GUICharHeight/2);

    return DrawWidth;
}

int Widget_TextCanvas::CalcCursorWidth(QFontMetrics *fm)
{
    return CalcFragWidth(fm,&CursorFrag);
}

void Widget_TextCanvas::CursorTimerTick()
{
    if(!CursorSet2BeBlinking)
        return;

    CursorHiddenByBlink=!CursorHiddenByBlink;

    HideCursor=CursorHiddenByBlink;

    /* Redraw the cursor */
    RedrawCursor();
}

void Widget_TextCanvas::SetCursorBlinking(bool Blinking)
{
    if(CursorSet2BeBlinking==Blinking)
        return;

    CursorSet2BeBlinking=Blinking;
    if(!Blinking)
    {
        /* Make sure we can see the cursor */
        HideCursor=false;

        RedrawCursor();
    }
}

void Widget_TextCanvas::ChangeCursorStyle(e_TextCursorStyleType Style)
{
    CursorStyle=Style;
    RedrawCursor();
}

void Widget_TextCanvas::RedrawCursor(void)
{
    QFontMetrics fm(RenderFont);

    update(DisplayLeftEdgePx+CursorPx-ScrollOffsetX,
            DisplayTopEdgePx+CursorY*GUICharHeight,CalcCursorWidth(&fm),
            GUICharHeight);
}

void Widget_TextCanvas::SetCursorPos(int NewCursorX,int NewCursorY)
{
    bool Need2EraseOldCursor;

    Need2EraseOldCursor=false;
    if(CursorY!=(unsigned int)NewCursorY)
        Need2EraseOldCursor=true;

    /* Redraw the line with the cursor on it */
    RedrawLine(CursorY);

    /* Move the cursor */
    CursorX=NewCursorX;
    CursorY=NewCursorY;

    /* Make sure it's visible (we always make it visible after moving) */
    HideCursor=false;
    CursorHiddenByBlink=true;   // When the timer goes off this will force it to be shown

    RethinkCursor();
    if(Need2EraseOldCursor)
        RedrawLine(CursorY);
}

int Widget_TextCanvas::GetTextWidth(const struct TextCanvasFrag *Frag)
{
    QFontMetrics fm(RenderFont);
    struct WTCFrag NewFrag;

    /* Convert the frag */
    NewFrag.FragType=Frag->FragType;
    NewFrag.Text=Frag->Text;
    NewFrag.Styling=Frag->Styling;
    NewFrag.Value=Frag->Value;
    NewFrag.Data=Frag->Data;

    switch(Frag->FragType)
    {
        case e_TextCanvasFrag_String:
        case e_TextCanvasFrag_NonPrintableChar:
        case e_TextCanvasFrag_RetText:
            return CalTextFragWidth(&fm,&NewFrag,Frag->Text);
        case e_TextCanvasFrag_SoftRet:
            return CalSoftRetFragWidth(&fm);
        break;
        case e_TextCanvasFrag_HardRet:
            return CalHardRetFragWidth(&fm);
        break;
        case e_TextCanvasFrag_HR:
            return 0;
        break;
        case e_TextCanvasFragMAX:
        default:
        break;
    }
    return 0;
}

int Widget_TextCanvas::GetWidgetWidth(void)
{
    int DrawAreaWidth;
    int DrawAreaHeight;

    DrawAreaWidth=this->width();
    DrawAreaHeight=this->height();

    GetCorrectedWidgetSize(DrawAreaWidth,DrawAreaHeight);

    return DrawAreaWidth;
}

int Widget_TextCanvas::GetWidgetHeight(void)
{
    int DrawAreaWidth;
    int DrawAreaHeight;

    DrawAreaWidth=this->width();
    DrawAreaHeight=this->height();

    GetCorrectedWidgetSize(DrawAreaWidth,DrawAreaHeight);

    return DrawAreaHeight;
}

int Widget_TextCanvas::GetCharPxWidth(void)
{
    return GUICharWidth;
}

int Widget_TextCanvas::GetCharPxHeight(void)
{
    return GUICharHeight;
}

void Widget_TextCanvas::GetCorrectedWidgetSize(int &Width,int &Height)
{
    int DrawAreaWidth;
    int DrawAreaHeight;

    DrawAreaWidth=Width;
    DrawAreaHeight=Height;

    if(DrawFocusBox)
    {
        /* Ok, reserve the space for the focus box */
        DrawAreaWidth-=FOCUS_BOX_SIZE*2;
        DrawAreaHeight-=FOCUS_BOX_SIZE*2;
        if(DrawAreaWidth<0)
            DrawAreaWidth=0;
        if(DrawAreaHeight<0)
            DrawAreaHeight=0;
    }

    Width=DrawAreaWidth;
    Height=DrawAreaHeight;
}

void Widget_TextCanvas::SetXOffsetPx(int XOffsetPx)
{
    ScrollOffsetX=XOffsetPx;
}

void Widget_TextCanvas::SetMaxLines(int MaxLines,uint32_t BGColor)
{
    struct WTCLine NewLine;

    NewLine.BGFillColor=BGColor;
    Lines.resize(MaxLines,NewLine);
}

void Widget_TextCanvas::SetDisplaySize(int LeftEdge,int TopEdge,int Width,int Height)
{
    DisplayLeftEdgePx=LeftEdge;
    DisplayTopEdgePx=TopEdge;
    DisplayWidth=Width;
    DisplayHeight=Height;
}

/*******************************************************************************
 * NAME:
 *    TextCanvas::SetDisplayBackgroundColor
 *
 * SYNOPSIS:
 *    void TextCanvas::SetDisplayBackgroundColor(uint32_t BgColor,bool DrawBackground);
 *
 * PARAMETERS:
 *    BgColor [I] -- The new background fill color
 *    DrawBackground [I] -- Fill the background (true) or just leave it alone
 *                          (false).
 *
 * FUNCTION:
 *    This function sets the background fill color.  The backgroun fill area
 *    is the area OUTSIDE of the text box.  That is:
 *
 *    +-----------------------------------------+
 *    |  Background area                        |
 *    |         +-----------------+             |
 *    |         |    Text Area    |             |
 *    |         |                 |             |
 *    |         +-----------------+             |
 *    |                                         |
 *    +-----------------------------------------+
 *
 *    The text area is fill on a line by line bases.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Widget_TextCanvas::SetDisplayBackgroundColor(uint32_t BgColor,
        bool DrawBackground)
{
    DisplayBackgroundColor=QColor(QRgb(BgColor));
    FillDisplayBackground=DrawBackground;
}

void Widget_TextCanvas::SetTextAreaBackgroundColor(uint32_t BgColor)
{
    TextAreaBackgroundColor=QColor(QRgb(BgColor));
    RethinkColors();
    RethinkCursor();
}

void Widget_TextCanvas::SetOverrideMsg(const char *Msg,bool OnOff)
{
    OverrideWidget->setVisible(OnOff);
    OverrideActive=OnOff;
    if(OnOff)
    {
        OverrideWidget->ui->label->setText(Msg);
        ResizeOverrideWidget();
    }
}

void Widget_TextCanvas::ResizeOverrideWidget(void)
{
    int NewWidth;
    int NewHeight;
    int NewLeft;
    int NewTop;

    NewWidth=width()/4;
    NewHeight=NewWidth*75/100;  // 75% of the width

    if(NewWidth<400)
        NewWidth=400;
    if(NewHeight<300)
        NewHeight=300;

    NewLeft=width()/2-NewWidth/2;
    NewTop=height()/2-NewHeight/2;

    OverrideWidget->setGeometry(QRect(NewLeft,NewTop, NewWidth,NewHeight));
}

void Widget_TextCanvas::RethinkColors(void)
{
    if(OverrideActive)
    {
        UseDisplayBackgroundColor=DisplayBackgroundColor.darker(200); // 1/2 bright;
        UseTextAreaBackgroundColor=TextAreaBackgroundColor.darker(200); // 1/2 bright;
    }
    else
    {
        UseDisplayBackgroundColor=DisplayBackgroundColor;
        UseTextAreaBackgroundColor=TextAreaBackgroundColor;
    }
}
