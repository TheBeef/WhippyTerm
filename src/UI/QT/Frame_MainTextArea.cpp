#include "Frame_MainTextArea.h"
#include "ui_Frame_MainTextArea.h"
#include "UI/UITextMainArea.h"
#include <string>

using namespace std;

Frame_MainTextArea::Frame_MainTextArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Frame_MainTextArea)
{
    EventHandler=nullptr;
    ui->setupUi(this);

    ContextMenu = new QMenu;
    ContextMenu->addAction(ui->actionSend_Buffer);
    ContextMenu->addAction(ui->actionEdit);
    ContextMenu->addAction(ui->actionFind_CRC_Algorithm);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionCopy);
    ContextMenu->addAction(ui->actionPaste);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionEndian_Swap);
    ContextMenu->addAction(ui->actionClear_Screen);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionStyleBold);
    ContextMenu->addAction(ui->actionStyleItalics);
    ContextMenu->addAction(ui->actionStyleUnderline);
    ContextMenu->addAction(ui->actionStyleStrike_Through);
    ColorBGSubmenu=ContextMenu->addMenu("BG Color");
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_Black);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_Blue);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_Green);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_Cyan);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_Red);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_Magenta);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_Brown);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_White);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_Gray);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_LightBlue);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_LightGreen);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_LightCyan);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_LightRed);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_LightMagenta);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_Yellow);
    ColorBGSubmenu->addAction(ui->actionStyleBGColor_BrightWhite);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionZoom_In);
    ContextMenu->addAction(ui->actionZoom_Out);

    ColorBGSubmenu->menuAction()->setVisible(false);
}

Frame_MainTextArea::~Frame_MainTextArea()
{
    delete ui;
}

void Frame_MainTextArea::on_BlockSendSend_pushButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ButtonPress;
    NewEvent.ID=ID;
    NewEvent.Info.ButtonPress.Bttn=e_UITC_Bttn_Send;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_TextverticalScrollBar_valueChanged(int value)
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_DisplayFrameScrollY;
    NewEvent.ID=ID;
    NewEvent.Info.Scroll.Amount=value;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_TexthorizontalScrollBar_valueChanged(int value)
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_DisplayFrameScrollX;
    NewEvent.ID=ID;
    NewEvent.Info.Scroll.Amount=value;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::SendContextMenuEvent(e_UITD_ContextMenuType EventMenu)
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=EventMenu;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_actionSend_Buffer_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_SendBuffers);
}

void Frame_MainTextArea::on_actionCopy_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_Copy);
}

void Frame_MainTextArea::on_actionPaste_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_Paste);
}

void Frame_MainTextArea::on_actionClear_Screen_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_ClearScreen);
}

void Frame_MainTextArea::on_actionZoom_In_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_ZoomIn);
}

void Frame_MainTextArea::on_actionZoom_Out_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_ZoomOut);
}

void Frame_MainTextArea::on_actionEdit_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_Edit);
}
void Frame_MainTextArea::on_actionEndian_Swap_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_EndianSwap);
}

void Frame_MainTextArea::on_BlockSendHex_Clear_pushButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ButtonPress;
    NewEvent.ID=ID;
    NewEvent.Info.ButtonPress.Bttn=e_UITC_Bttn_Clear;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_BlockSendHex_Edit_pushButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ButtonPress;
    NewEvent.ID=ID;
    NewEvent.Info.ButtonPress.Bttn=e_UITC_Bttn_HexEdit;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_BlockSend_TextMode_radioButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_RadioButtonPress;
    NewEvent.ID=ID;

    NewEvent.Info.RadioButton.BttnID=e_UITC_RadioButton_Text;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_BlockSend_HexMode_radioButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_RadioButtonPress;
    NewEvent.ID=ID;

    NewEvent.Info.RadioButton.BttnID=e_UITC_RadioButton_Hex;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_BlockSend_LineEnd_comboBox_activated(int index)
{
    struct TextDisplayEvent NewEvent;
    uintptr_t ComboxID;   // The ID for this item

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ComboxChange;
    NewEvent.ID=ID;

    ComboxID=ui->BlockSend_LineEnd_comboBox->itemData(index).toULongLong();
    NewEvent.Info.Combox.ID=ComboxID;
    NewEvent.Info.Combox.BoxID=e_UITC_Combox_LineEnd;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_actionStyleStrike_Through_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StrikeThrough);
}

void Frame_MainTextArea::on_actionStyleUnderline_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_Underline);
}

void Frame_MainTextArea::on_actionStyleItalics_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_Italics);
}

void Frame_MainTextArea::on_actionStyleBold_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_Bold);
}

void Frame_MainTextArea::on_actionStyleBGColor_Black_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_Black);
}

void Frame_MainTextArea::on_actionStyleBGColor_Blue_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_Blue);
}

void Frame_MainTextArea::on_actionStyleBGColor_Green_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_Green);
}

void Frame_MainTextArea::on_actionStyleBGColor_Cyan_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_Cyan);
}

void Frame_MainTextArea::on_actionStyleBGColor_Red_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_Red);
}

void Frame_MainTextArea::on_actionStyleBGColor_Magenta_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_Magenta);
}

void Frame_MainTextArea::on_actionStyleBGColor_Brown_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_Brown);
}

void Frame_MainTextArea::on_actionStyleBGColor_White_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_White);
}

void Frame_MainTextArea::on_actionStyleBGColor_Gray_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_Gray);
}

void Frame_MainTextArea::on_actionStyleBGColor_LightBlue_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_LightBlue);
}

void Frame_MainTextArea::on_actionStyleBGColor_LightGreen_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_LightGreen);
}

void Frame_MainTextArea::on_actionStyleBGColor_LightCyan_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_LightCyan);
}

void Frame_MainTextArea::on_actionStyleBGColor_LightRed_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_LightRed);
}

void Frame_MainTextArea::on_actionStyleBGColor_LightMagenta_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_LightMagenta);
}

void Frame_MainTextArea::on_actionStyleBGColor_Yellow_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_Yellow);
}

void Frame_MainTextArea::on_actionStyleBGColor_BrightWhite_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_StyleBGColor_BrightWhite);
}

void Frame_MainTextArea::on_actionFind_CRC_Algorithm_triggered()
{
    SendContextMenuEvent(e_UITD_ContextMenu_FindCRCAlgorithm);
}

