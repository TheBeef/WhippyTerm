#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QFontDatabase>
#include <QListWidget>

namespace Ui {
class Form_FontDialog;
}

class Form_FontDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Form_FontDialog(QWidget *parent = 0);
    ~Form_FontDialog();
    void SetOptions(const char *Title,const std::string &FontName,int FontSize,
            long FontStyle,long Flags);
    void GetFontInfo(std::string &FontName,int &FontSize,long &FontStyle);

private slots:
    void on_FontlistWidget_itemSelectionChanged();
    void on_StylelistWidget_itemSelectionChanged();
    void on_FontlineEdit_textChanged(const QString &arg1);
    void on_SizelistWidget_itemSelectionChanged();
    void on_SizelineEdit_textChanged(const QString &arg1);
    void on_StyleItalicCheckBox_clicked();
    void on_StyleBoldCheckBox_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::Form_FontDialog *ui;
    QFontDatabase FontDatabase;
    QString DefaultFontName;
    int LastPointSizeSelected;
    bool DoingEvent;

    void UpdatePreview(void);
    void FillInSizeList();
};

#endif // MAINWINDOW_H
