#ifndef FRAME_CUSTOMTEXTWIDGET_H
#define FRAME_CUSTOMTEXTWIDGET_H

#include <QWidget>
#include "ui_Frame_CustomTextWidget.h"
#include <QHBoxLayout>
#include <QMenu>

namespace Ui {
class Frame_CustomTextWidget;
}

class Frame_CustomTextWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Frame_CustomTextWidget(QWidget *parent = nullptr);
    ~Frame_CustomTextWidget();

    Ui::Frame_CustomTextWidget *ui;
    QHBoxLayout *Layout;
    uintptr_t ID;
    bool (*EventHandler)(const struct UICTWEvent *Event);
    QMenu *ContextMenu;

    int WorkingLine;


private slots:
    void on_TextverticalScrollBar_valueChanged(int value);
    
    void on_TexthorizontalScrollBar_valueChanged(int value);
    
    void on_actionCopy_triggered();
    
    void on_actionPaste_triggered();
    
    void on_actionClear_Screen_triggered();
    
    void on_actionZoom_In_triggered();
    
    void on_actionZoom_Out_triggered();
    
    void on_actionEdit_triggered();
    
    void on_actionEndian_Swap_triggered();
    
    void on_actionFind_CRC_Algorithm_triggered();
    
private:
};

#endif // FRAME_CUSTOMTEXTWIDGET_H
