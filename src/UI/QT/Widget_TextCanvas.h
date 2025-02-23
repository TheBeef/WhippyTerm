#ifndef WIDGET_TEXTCAVNAS_H
#define WIDGET_TEXTCAVNAS_H

#include <QWidget>
#include <QString>
#include <QTimer>
#include "PluginSDK/KeyDefines.h"
#include "UI/UITextDefs.h"
#include "UI/UITextDisplay.h"
#include "Frame_TextCavnasOverrideBox.h"
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <list>
#include <vector>

typedef enum
{
    e_WTCEvent_MouseDown,
    e_WTCEvent_MouseUp,
    e_WTCEvent_MouseRightDown,
    e_WTCEvent_MouseRightUp,
    e_WTCEvent_MouseMiddleDown,
    e_WTCEvent_MouseMiddleUp,
    e_WTCEvent_MouseWheel,
    e_WTCEvent_MouseMove,
    e_WTCEvent_Resize,
    e_WTCEvent_LostFocus,
    e_WTCEvent_GotFocus,
    e_WTCEvent_KeyEvent,
    e_WTCEventMAX
} e_WTCEventType;

struct WTCEventFrameScroll
{
    int Amount;
};

struct WTCEventDataXY
{
    int x;
    int y;
};

struct WTCEventDataSize
{
    int Width;
    int Height;
};

struct WTCEventDataWheel
{
    int Steps;
    int Mods;
};

struct WTCEventKeyPress
{
    uint8_t Mods;
    e_UIKeys Key;
    const uint8_t *TextPtr;
    unsigned int TextLen;
};

union WTCEventData
{
    struct WTCEventFrameScroll Scroll;
    struct WTCEventDataXY Mouse;
    struct WTCEventDataWheel MouseWheel;
    struct WTCEventDataSize NewSize;
    struct WTCEventKeyPress Key;
};

struct WTCEvent
{
    e_WTCEventType EventType;
    uintptr_t UserData;
    union WTCEventData *Info;
};

struct WTCFrag
{
    e_TextCanvasFragType FragType;
    QString Text;
    struct CharStyling Styling;
    int Value;
    void *Data;
    bool CursorFrag;
};

typedef std::list<struct WTCFrag> t_WTCLineFrags;
typedef t_WTCLineFrags::iterator i_WTCLineFrags;

struct WTCLine
{
    uint32_t BGFillColor;   // Selected when we first bring a new line into existance (scroll off bottom)
    t_WTCLineFrags Fragments;
};

typedef std::vector<struct WTCLine> t_WTCLines;

class Widget_TextCanvas : public QWidget
{
    Q_OBJECT

public:
    Widget_TextCanvas(QWidget *parent = 0);
    ~Widget_TextCanvas();
    void SetEventHandler(bool (*EventHandler)(const struct WTCEvent *Event),uintptr_t UserData);
    void keyPressEvent(QKeyEvent *event);
    bool event(QEvent *event);
    void RedrawScreen(void);
    bool SetFont(const char *FontName,int PointSize,bool Bold,bool Italic);
    void SetCursorBlinking(bool Blinking);
    void SetDrawMask(uint16_t Mask);
    void ShowBell(void);
    void SetMouseCursor(e_UITD_MouseCursorType Cursor);

    void ClearLine(unsigned int Line,uint32_t BGColor);
    void AppendTextFrag(unsigned int Line,const struct TextCanvasFrag *Frag);
    void RedrawLine(unsigned int Line);
    void ClearAllLines(void);
//    int AddAtCursorAndAdvance(const char *Str);

    int GetTextWidth(const struct TextCanvasFrag *Frag);

    void ChangeCursorStyle(e_TextCursorStyleType Style);
    void SetCursorPos(int NewCursorX,int NewCursorY);

    void SetXOffsetPx(int XOffsetPx);
    void SetMaxLines(int MaxLines,uint32_t BGColor);
    void SetDisplaySize(int LeftEdge,int TopEdge,int Width,int Height);
    void SetDisplayBackgroundColor(uint32_t BgColor,bool DrawBackground);

    void SetTextAreaBackgroundColor(uint32_t BgColor);
    void SetTextDefaultColor(uint32_t FgColor);
    void SetOverrideMsg(const char *Msg,bool OnOff);

    int GetWidgetWidth(void);
    int GetWidgetHeight(void);
    int GetCharPxWidth(void);
    int GetCharPxHeight(void);

    QColor CursorColor;

public slots:
        void CursorTimerTick();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void focusInEvent(QFocusEvent * event);
    void focusOutEvent(QFocusEvent * event);
    void wheelEvent(QWheelEvent * event);
    void resizeEvent(QResizeEvent *event);

private:
    t_WTCLines Lines;

    int GUICharHeight;
    int GUICharWidth;
    int UnderLinePos;
    int OverLinePos;
    int StrikeOutPos;
    int UnderLineHeight;
    int BaseLine;
    QVector<qreal> dotted;
    QVector<qreal> dashed;

    unsigned int CursorX;
    unsigned int CursorY;
    unsigned int CursorPx;   // Pixel pos of the cursor
    struct WTCFrag CursorFrag;
    e_TextCursorStyleType CursorStyle;
    bool HideCursor;
    bool CursorSet2BeVisble;
    bool CursorSet2BeBlinking;
    bool CursorHiddenByBlink;
    QTimer *CursorTimer;
    QLabel *BellLabel;
    Frame_TextCavnasOverrideBox *OverrideWidget;
    bool OverrideActive;

    QFont RenderFont;
    QColor TextAreaDefaultColor;
    QColor UseTextAreaDefaultColor;
    QColor TextAreaBackgroundColor;
    QColor UseTextAreaBackgroundColor;
    bool (*WTCEventHandler)(const struct WTCEvent *Event);
    uintptr_t WTCEventHandlerUserData;
    bool DrawFocusBox;
    int ScrollOffsetX;

    int DisplayLeftEdgePx;        /// Used when the text canvas is smaller than the screen (to center the text area)
    int DisplayTopEdgePx;
    int DisplayWidth;
    int DisplayHeight;
    QColor DisplayBackgroundColor;
    QColor UseDisplayBackgroundColor;
    bool FillDisplayBackground;
    uint16_t DrawAttribMask;

    QGraphicsOpacityEffect *BellAnimEffect;
    QPropertyAnimation *BellAnim;

    bool SendEvent(e_WTCEventType EventType,union WTCEventData *Info);
    bool SendEvent(e_WTCEventType EventType);
    void Setup4Paint(void);
    void RethinkCursor(void);
    int DrawTextFrag(QPainter *painter,QFontMetrics *fm,int ScreenX,int ScreenY,struct WTCFrag *Frag);
    int DrawSoftRet(QPainter *painter,QFontMetrics *fm,int ScreenX,int ScreenY,struct WTCFrag *Frag);
    int CalTextFragWidth(QFontMetrics *fm,struct WTCFrag *Frag,const QString &FragStr);
    int CalSoftRetFragWidth(QFontMetrics *fm);
    int DrawHardRet(QPainter *painter,QFontMetrics *fm,int ScreenX,int ScreenY,struct WTCFrag *Frag);
    int CalHardRetFragWidth(QFontMetrics *fm);
    int DrawHRFrag(QPainter *painter,QFontMetrics *fm,int ScreenX,int ScreenY,struct WTCFrag *Frag);
    void DrawCursor(QPainter *painter,QFontMetrics *fm);
    void RedrawCursor(void);
    int DrawFrag(QPainter *painter,QFontMetrics *fm,int ScreenX,int ScreenY,struct WTCFrag *Frag);
    int CalcFragWidth(QFontMetrics *fm,struct WTCFrag *Frag);
    int CalcCursorWidth(QFontMetrics *fm);
    void GetCorrectedWidgetSize(int &Width,int &Height);
    void ResizeOverrideWidget(void);
    void RethinkColors(void);
};

#endif // MYCANVAS_H
