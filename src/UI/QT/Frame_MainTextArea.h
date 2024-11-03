#ifndef FRAME_MAINTEXTAREA_H
#define FRAME_MAINTEXTAREA_H

#include <QWidget>
#include "ui_Frame_MainTextArea.h"
#include <QHBoxLayout>
#include <QMenu>

namespace Ui {
class Frame_MainTextArea;
}

class Frame_MainTextArea : public QWidget
{
    Q_OBJECT
    
public:
    explicit Frame_MainTextArea(QWidget *parent = 0);
    ~Frame_MainTextArea();

    Ui::Frame_MainTextArea *ui;
    QHBoxLayout *Layout;
    uintptr_t ID;
    bool (*EventHandler)(const struct TextDisplayEvent *Event);
    QMenu *ContextMenu;

    int WorkingLine;

private slots:
    void on_BlockSendSend_pushButton_clicked();

    void on_TextverticalScrollBar_valueChanged(int value);
    
    void on_TexthorizontalScrollBar_valueChanged(int value);
    
    void on_actionSend_Buffer_triggered();
    
    void on_actionCopy_triggered();
    
    void on_actionPaste_triggered();
    
    void on_actionClear_Screen_triggered();
    
    void on_actionZoom_In_triggered();
    
    void on_actionZoom_Out_triggered();
    
    void on_actionEdit_triggered();
    
    void on_actionEndian_Swap_triggered();
    
private:
};

#endif // FRAME_MAINTEXTAREA_H
