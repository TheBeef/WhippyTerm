#ifndef FRAME_MAINTEXTAREA_H
#define FRAME_MAINTEXTAREA_H

#include <QWidget>
#include "ui_Frame_MainTextArea.h"
#include <QHBoxLayout>

namespace Ui {
class Frame_MainTextArea;
}

class Frame_MainTextArea : public QWidget
{
    Q_OBJECT
    
public:
    explicit Frame_MainTextArea(QWidget *parent = 0);
    ~Frame_MainTextArea();
//    void Init(bool (*EventHandler)(const struct TCEvent *Event),
//            uintptr_t FrameID);
//    void ShutDown(void);

    Ui::Frame_MainTextArea *ui;
    QHBoxLayout *Layout;
//    t_UIDisplayFrameCtrl *DisplayFrame;
    uintptr_t ID;
    bool (*EventHandler)(const struct TextDisplayEvent *Event);

    int WorkingLine;

private slots:
    void on_BlockSendSend_pushButton_clicked();

    void on_TextverticalScrollBar_valueChanged(int value);
    
    void on_TexthorizontalScrollBar_valueChanged(int value);
    
private:
};

#endif // FRAME_MAINTEXTAREA_H
