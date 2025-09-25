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
    QMenu *ColorBGSubmenu;

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
    
    void on_BlockSendHex_Clear_pushButton_clicked();
    
    void on_BlockSendHex_Edit_pushButton_clicked();
    
    void on_BlockSend_TextMode_radioButton_clicked();
    
    void on_BlockSend_HexMode_radioButton_clicked();
    
    void on_BlockSend_LineEnd_comboBox_activated(int index);
    
    void on_actionStyleStrike_Through_triggered();
    
    void on_actionStyleUnderline_triggered();
    
    void on_actionStyleItalics_triggered();
    
    void on_actionStyleBold_triggered();
    
    void on_actionStyleBGColor_Black_triggered();

    void on_actionStyleBGColor_Blue_triggered();

    void on_actionStyleBGColor_Green_triggered();

    void on_actionStyleBGColor_Cyan_triggered();

    void on_actionStyleBGColor_Red_triggered();

    void on_actionStyleBGColor_Magenta_triggered();

    void on_actionStyleBGColor_Brown_triggered();

    void on_actionStyleBGColor_White_triggered();

    void on_actionStyleBGColor_Gray_triggered();

    void on_actionStyleBGColor_LightBlue_triggered();

    void on_actionStyleBGColor_LightGreen_triggered();

    void on_actionStyleBGColor_LightCyan_triggered();

    void on_actionStyleBGColor_LightRed_triggered();

    void on_actionStyleBGColor_LightMagenta_triggered();

    void on_actionStyleBGColor_Yellow_triggered();

    void on_actionStyleBGColor_BrightWhite_triggered();
    
    void on_actionFind_CRC_Algorithm_triggered();
    
    void on_actionCalculate_CRC_triggered();
    
    void on_actionCopy_To_Send_Buffer_triggered();
    
private:
    void SendContextMenuEvent(e_UITD_ContextMenuType EventMenu);
};

#endif // FRAME_MAINTEXTAREA_H
