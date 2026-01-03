#ifndef FRAME_MAINTEXTCANVAS_H
#define FRAME_MAINTEXTCANVAS_H

#include <QWidget>
#include "ui_Frame_MainTextCanvas.h"

typedef enum
{
    e_FMTCEvent_ScrollX,
    e_FMTCEvent_ContextMenu,
    e_FMTCEventMAX
} e_FMTCEventType;

struct FMTCEventFrameScroll
{
    int Amount;
};

struct FMTCEventFrameContextMenu
{
    e_UITD_ContextMenuType Menu;
};

union FMTCEventData
{
    struct FMTCEventFrameScroll Scroll;
    struct FMTCEventFrameContextMenu Context;
};

struct FMTCEvent
{
    e_FMTCEventType EventType;
    uintptr_t UserData;
    union FMTCEventData *Info;
    class Frame_MainTextCanvas *Source;
};

namespace Ui {
class Frame_MainTextCanvas;
}

class Frame_MainTextCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit Frame_MainTextCanvas(QWidget *parent = nullptr);
    ~Frame_MainTextCanvas();
    void SetEventHandler(bool (*EventHandler)(const struct FMTCEvent *Event),uintptr_t UserData);
    void HeaderVisible(bool Visible);
    void SetParentSplitter(QSplitter *Splitter);
    void SetHeaderLabel(const char *Name);
    int WorkingLine;

public:
    Ui::Frame_MainTextCanvas *ui;
    
private slots:
    void on_TexthorizontalScrollBar_valueChanged(int value);
    void ContextMenu(const QPoint &pos);
    
    void on_Menu_HeaderLabel_x1_Hide_triggered();
    
private:
    bool SendEvent(e_FMTCEventType EventType,union FMTCEventData *Info);
    bool SendEvent(e_FMTCEventType EventType);
    bool (*FMTCEventHandler)(const struct FMTCEvent *Event);
    uintptr_t FMTCEventHandlerUserData;

};

#endif // FRAME_MAINTEXTCANVAS_H
