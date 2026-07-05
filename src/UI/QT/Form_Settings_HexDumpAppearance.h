#ifndef FORM_SETTINGS_HEXDUMPAPPEARANCE_H
#define FORM_SETTINGS_HEXDUMPAPPEARANCE_H

#include "UI/UISettingsHexDumpAppearance.h"
#include <QDialog>
#include <stdint.h>

namespace Ui {
class Form_Settings_HexDumpAppearance;
}

class Form_Settings_HexDumpAppearance : public QDialog
{
    Q_OBJECT

public:
    explicit Form_Settings_HexDumpAppearance(QWidget *parent = nullptr);
    ~Form_Settings_HexDumpAppearance();
    Ui::Form_Settings_HexDumpAppearance *ui;

private slots:
    void on_HexDisplay_SelectDivLineColor_pushButton_clicked();
    void on_HexDisplay_DivLineWidth_spinBox_valueChanged(int arg1);
    void on_HexDisplay_BytesPerLine_spinBox_valueChanged(int arg1);

private:
    bool DoingEvent;
    bool SendEvent(e_SHDAEventType EventType,union SHDAEventData *Info,
            uintptr_t ID);
    bool SendEvent(e_SHDAEventType EventType,union SHDAEventData *Info);
    bool SendEvent(e_SHDAEventType EventType);
};

#endif // FORM_SETTINGS_HEXDUMPAPPEARANCE_H
