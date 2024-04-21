#ifndef FORM_PASTEDATA_H
#define FORM_PASTEDATA_H

#include "UI/UIPasteData.h"
#include <QDialog>

namespace Ui {
class Form_PasteData;
}

class Form_PasteData : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_PasteData(QWidget *parent = 0);
    ~Form_PasteData();
    Ui::Form_PasteData *ui;
    e_PasteDataType SelectedDataType;

private slots:
    void on_pushButton_HexDump_clicked();
    
    void on_pushButton_Text_clicked();
    
private:
};

#endif // FORM_PASTEDATA_H
