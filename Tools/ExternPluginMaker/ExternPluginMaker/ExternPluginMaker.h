#ifndef EXTERNPLUGINMAKER_H
#define EXTERNPLUGINMAKER_H

#include <QMainWindow>
#include <QString>

namespace Ui {
class ExternPluginMaker;
}

class ExternPluginMaker : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ExternPluginMaker(QWidget *parent = 0);
    ~ExternPluginMaker();
    
private slots:
    void on_SelectLinuxFile_clicked();
    
    void on_SelectWindowsFile_clicked();
    
    void on_SelectMacFile_clicked();
    
    void on_BuiltBttn_clicked();
    
    void on_LoadBttn_clicked();
    
    void on_SaveBttn_clicked();
    
    void on_actionBuild_triggered();
    
    void on_actionSave_triggered();
    
    void on_actionLoad_triggered();
    
    void on_actionQuit_triggered();
    
    void on_SelectRPI64File_clicked();
    
    void on_SelectRPI32File_clicked();
    
private:
    Ui::ExternPluginMaker *ui;
    QString LastBuildFile;

    void Save(const char *Filename);
    void Load(const char *Filename);
    void Build(const char *Filename);
    bool SaveFile2RIFF(class RIFF *RIFF,const char *Filename,const char *ChunkName);
};

#endif // EXTERNPLUGINMAKER_H
