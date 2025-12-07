#ifndef FRAME_STYLEPICKERWIDGET_H
#define FRAME_STYLEPICKERWIDGET_H

#include "PluginSDK/DataProcessors.h"
#include <QWidget>

namespace Ui {
class Frame_StylePickerWidget;
}

class Frame_StylePickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Frame_StylePickerWidget(QWidget *parent = nullptr);
    ~Frame_StylePickerWidget();
    void Init(void (*InitEventCB)(const struct PIStylePickEvent *Event,void *UserData),void *InitUserData,struct StyleData *SD);
    void SetStyleValue(struct StyleData *SD);
    void GetStyleValue(struct StyleData *SD);
    
private slots:
    void on_pushButton_clicked();
    
private:
    Ui::Frame_StylePickerWidget *ui;
    void (*EventCB)(const struct PIStylePickEvent *Event,void *UserData);
    void *UserData;
    struct StyleData CurStyle;
    void UpdatePreviewStyle(void);
};

#endif // FRAME_STYLEPICKERWIDGET_H
