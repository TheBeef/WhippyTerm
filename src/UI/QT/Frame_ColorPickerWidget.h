#ifndef FRAME_COLORPICKERWIDGET_H
#define FRAME_COLORPICKERWIDGET_H

#include <QWidget>
#include "PluginSDK/PluginUI.h"
#include <stdint.h>

namespace Ui {
class Frame_ColorPickerWidget;
}

class Frame_ColorPickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Frame_ColorPickerWidget(QWidget *parent = nullptr);
    ~Frame_ColorPickerWidget();
    void Init(void (*InitEventCB)(const struct PIColorPickEvent *Event,void *UserData),void *InitUserData,uint32_t InitRGB);
    void SetRGBValue(uint32_t NewValue);
    uint32_t GetRGBValue(void);

private slots:
    void on_Select_pushButton_clicked();

private:
    Ui::Frame_ColorPickerWidget *ui;
    void (*EventCB)(const struct PIColorPickEvent *Event,void *UserData);
    void *UserData;
    uint32_t RGB;
    void UpdatePreviewColor(void);
};

#endif // FRAME_COLORPICKERWIDGET_H
