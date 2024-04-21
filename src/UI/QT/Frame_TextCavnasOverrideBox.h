#ifndef FRAME_TEXTCAVNASOVERRIDEBOX_H
#define FRAME_TEXTCAVNASOVERRIDEBOX_H

#include <QWidget>
#include "ui_Frame_TextCavnasOverrideBox.h"

namespace Ui {
class Frame_TextCavnasOverrideBox;
}

class Frame_TextCavnasOverrideBox : public QWidget
{
    Q_OBJECT
    
public:
    explicit Frame_TextCavnasOverrideBox(QWidget *parent = nullptr);
    ~Frame_TextCavnasOverrideBox();
    Ui::Frame_TextCavnasOverrideBox *ui;
    
private:
};

#endif // FRAME_TEXTCAVNASOVERRIDEBOX_H
