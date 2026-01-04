#ifndef FRAME_MAINTEXTCOLUMN_H
#define FRAME_MAINTEXTCOLUMN_H

#include <QWidget>
#include "ui_Frame_MainTextColumn.h"

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
    class Frame_MainTextColumn *Source;
};

namespace Ui {
class Frame_MainTextColumn;
}

class Frame_MainTextColumn : public QWidget
{
    Q_OBJECT

public:
    explicit Frame_MainTextColumn(QWidget *parent = nullptr);
    ~Frame_MainTextColumn();
    void SetEventHandler(bool (*EventHandler)(const struct FMTCEvent *Event),uintptr_t UserData);
    void HeaderVisible(bool Visible);
    void SetParentSplitter(QSplitter *Splitter);
    void SetHeaderLabel(const char *Name);
    int WorkingLine;

public:
    Ui::Frame_MainTextColumn *ui;
    
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
