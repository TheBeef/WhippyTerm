#include "ExternPluginMaker.h"
#include "ui_ExternPluginMaker.h"
#include <QFileDialog>
#include <QMessageBox>
#include "TinyCFG.h"
#include "RIFF.h"
#include <stdint.h>
#include <string>
#include <stdlib.h>

using namespace std;

struct ExternPluginData
{
    std::string LastBuildFile;
    std::string LinuxFilename;
    std::string WindowsFilename;
    std::string MacFilename;
    std::string RPI64Filename;
    std::string RPI32Filename;
    std::string PluginName;
    std::string InstallFilename;
    std::string Description;
    std::string Contributors;
    std::string Copyright;
    std::string ReleaseDate;
    std::string Version;
    std::string APIVersion;
    int PluginClass;
    int PluginSubClass;

    public:
        void RegisterAllMembers(class TinyCFG *cfg)
        {
            cfg->Register("LastBuildFile",LastBuildFile);
            cfg->Register("LinuxFilename",LinuxFilename);
            cfg->Register("WindowsFilename",WindowsFilename);
            cfg->Register("MacFilename",MacFilename);
            cfg->Register("RPI64Filename",RPI64Filename);
            cfg->Register("RPI32Filename",RPI32Filename);
            cfg->Register("PluginName",PluginName);
            cfg->Register("InstallFilename",InstallFilename);
            cfg->Register("Description",Description);
            cfg->Register("Contributors",Contributors);
            cfg->Register("Copyright",Copyright);
            cfg->Register("ReleaseDate",ReleaseDate);
            cfg->Register("Version",Version);
            cfg->Register("APIVersion",APIVersion);
            cfg->Register("PluginClass",PluginClass);
            cfg->Register("PluginSubClass",PluginSubClass);
        }
};

ExternPluginMaker::ExternPluginMaker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExternPluginMaker)
{
    ui->setupUi(this);
}

ExternPluginMaker::~ExternPluginMaker()
{
    delete ui;
}

void ExternPluginMaker::on_SelectLinuxFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Shared Objects (*.so);;All Files (*)"));
    ui->Linux_SO->setText(fileName);
}

void ExternPluginMaker::on_SelectWindowsFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("DLL (*.dll);;All Files (*)"));
    ui->Windows_DLL->setText(fileName);
}

void ExternPluginMaker::on_SelectMacFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Shared Objects (*.so);;All Files (*)"));
    ui->Mac_SO->setText(fileName);
}

void ExternPluginMaker::on_BuiltBttn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Whippy Term Plugin"),
                                                    LastBuildFile,
                                                    tr("Whippy Term Plugin (*.wtp);;All Files (*)"));
    Build(fileName.toUtf8().constData());
}

void ExternPluginMaker::on_LoadBttn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Whippy Term Maker (*.wtm);;All Files (*)"));
    Load(fileName.toUtf8().constData());
}

void ExternPluginMaker::on_SaveBttn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    "",
                                                    tr("Whippy Term Maker (*.wtm);;All Files (*)"));
    Save(fileName.toUtf8().constData());
}

void ExternPluginMaker::on_actionBuild_triggered()
{
    on_BuiltBttn_clicked();
}

void ExternPluginMaker::on_actionSave_triggered()
{
    on_SaveBttn_clicked();
}

void ExternPluginMaker::on_actionLoad_triggered()
{
    on_LoadBttn_clicked();
}

void ExternPluginMaker::on_actionQuit_triggered()
{
    on_actionBuild_triggered();
}

void ExternPluginMaker::Save(const char *Filename)
{
    class TinyCFG cfg("ExternPluginMaker");
    struct ExternPluginData Data;

    Data.RegisterAllMembers(&cfg);

    Data.LastBuildFile=LastBuildFile.toStdString();
    Data.LinuxFilename=ui->Linux_SO->text().toStdString();
    Data.WindowsFilename=ui->Windows_DLL->text().toStdString();
    Data.MacFilename=ui->Mac_SO->text().toStdString();
    Data.RPI64Filename=ui->RPI64_SO->text().toStdString();
    Data.RPI32Filename=ui->RPI32_SO->text().toStdString();
    Data.PluginName=ui->PluginName->text().toStdString();
    Data.InstallFilename=ui->InstallFilename->text().toStdString();
    Data.Description=ui->Desc->toPlainText().toStdString();
    Data.Contributors=ui->Contributors->text().toStdString();
    Data.Copyright=ui->Copyright->text().toStdString();
    Data.ReleaseDate=ui->ReleaseDate->text().toStdString();
    Data.Version=ui->Version->text().toStdString();
    Data.APIVersion=ui->APIVersion->text().toStdString();

    Data.PluginClass=ui->PluginType->currentIndex();
    Data.PluginSubClass=ui->SubType->currentIndex();

    cfg.SaveCFGFile(Filename);
}

void ExternPluginMaker::Load(const char *Filename)
{
    class TinyCFG cfg("ExternPluginMaker");
    struct ExternPluginData Data;

    Data.RegisterAllMembers(&cfg);

    cfg.LoadCFGFile(Filename);

    LastBuildFile=Data.LastBuildFile.c_str();
    ui->Linux_SO->setText(Data.LinuxFilename.c_str());
    ui->Windows_DLL->setText(Data.WindowsFilename.c_str());
    ui->Mac_SO->setText(Data.MacFilename.c_str());
    ui->RPI64_SO->setText(Data.RPI64Filename.c_str());
    ui->RPI32_SO->setText(Data.RPI32Filename.c_str());
    ui->PluginName->setText(Data.PluginName.c_str());
    ui->InstallFilename->setText(Data.InstallFilename.c_str());
    ui->Desc->setPlainText(Data.Description.c_str());
    ui->Contributors->setText(Data.Contributors.c_str());
    ui->Copyright->setText(Data.Copyright.c_str());
    ui->ReleaseDate->setText(Data.ReleaseDate.c_str());
    ui->Version->setText(Data.Version.c_str());
    ui->APIVersion->setText(Data.APIVersion.c_str());

    ui->PluginType->setCurrentIndex(Data.PluginClass);
    ui->SubType->setCurrentIndex(Data.PluginSubClass);
}

void ExternPluginMaker::Build(const char *Filename)
{
    struct ExternPluginData Data;
    class RIFF SaveFile(true);
    uint32_t VerNum;
    uint32_t Tmp32;
    const char *VerStr;
    uint8_t Tmp;

    LastBuildFile=Filename;

    Data.LinuxFilename=ui->Linux_SO->text().toStdString();
    Data.WindowsFilename=ui->Windows_DLL->text().toStdString();
    Data.MacFilename=ui->Mac_SO->text().toStdString();
    Data.RPI64Filename=ui->RPI64_SO->text().toStdString();
    Data.RPI32Filename=ui->RPI32_SO->text().toStdString();
    Data.PluginName=ui->PluginName->text().toStdString();
    Data.InstallFilename=ui->InstallFilename->text().toStdString();
    Data.Description=ui->Desc->toPlainText().toStdString();
    Data.Contributors=ui->Contributors->text().toStdString();
    Data.Copyright=ui->Copyright->text().toStdString();
    Data.ReleaseDate=ui->ReleaseDate->text().toStdString();
    Data.Version=ui->Version->text().toStdString();
    Data.APIVersion=ui->APIVersion->text().toStdString();
    Data.PluginClass=ui->PluginType->currentIndex();
    Data.PluginSubClass=ui->SubType->currentIndex();

    Data.PluginClass=ui->PluginType->currentIndex();
    Data.PluginSubClass=ui->SubType->currentIndex();

    SaveFile.Open(Filename,e_RIFFOpen_Write,"WTPI");

    SaveFile.StartDataBlock("FILE");
    SaveFile.Write(Data.InstallFilename.c_str(),Data.InstallFilename.length());
    SaveFile.EndDataBlock();

    SaveFile.StartDataBlock("NAME");
    SaveFile.Write(Data.PluginName.c_str(),Data.PluginName.length());
    SaveFile.EndDataBlock();

    SaveFile.StartDataBlock("DESC");
    SaveFile.Write(Data.Description.c_str(),Data.Description.length());
    SaveFile.EndDataBlock();

    SaveFile.StartDataBlock("CONT");
    SaveFile.Write(Data.Contributors.c_str(),Data.Contributors.length());
    SaveFile.EndDataBlock();

    SaveFile.StartDataBlock("COPY");
    SaveFile.Write(Data.Copyright.c_str(),Data.Copyright.length());
    SaveFile.EndDataBlock();

    SaveFile.StartDataBlock("DATE");
    SaveFile.Write(Data.ReleaseDate.c_str(),Data.ReleaseDate.length());
    SaveFile.EndDataBlock();

    SaveFile.StartDataBlock("TYPE");
    Tmp32=Data.PluginClass;
    SaveFile.Write(&Tmp32,sizeof(Tmp32));
    SaveFile.EndDataBlock();

    SaveFile.StartDataBlock("SUBT");
    Tmp32=Data.PluginSubClass;
    SaveFile.Write(&Tmp32,sizeof(Tmp32));
    SaveFile.EndDataBlock();

    /* Plugin Version */
    SaveFile.StartDataBlock("VER ");
    VerStr=Data.Version.c_str();
    Tmp=strtol(VerStr,(char **)&VerStr,10);
    if(VerStr==NULL)
    {
        QMessageBox::critical(NULL,"Failed","Version must be in the format "
                "0.0.0.0 (major.minor.patch.letter)");
        SaveFile.Close();
        return;
    }
    SaveFile.Write(&Tmp,1);
    VerStr++;

    Tmp=strtol(VerStr,(char **)&VerStr,10);
    if(VerStr==NULL)
    {
        QMessageBox::critical(NULL,"Failed","Version must be in the format "
                "0.0.0.0 (major.minor.patch.letter)");
        SaveFile.Close();
        return;
    }
    SaveFile.Write(&Tmp,1);
    VerStr++;

    Tmp=strtol(VerStr,(char **)&VerStr,10);
    if(VerStr==NULL)
    {
        QMessageBox::critical(NULL,"Failed","Version must be in the format "
                "0.0.0.0 (major.minor.patch.letter)");
        SaveFile.Close();
        return;
    }
    SaveFile.Write(&Tmp,1);
    VerStr++;

    Tmp=strtol(VerStr,(char **)&VerStr,10);
    if(VerStr==NULL)
    {
        QMessageBox::critical(NULL,"Failed","Version must be in the format "
                "0.0.0.0 (major.minor.patch.letter)");
        SaveFile.Close();
        return;
    }
    SaveFile.Write(&Tmp,1);
    SaveFile.EndDataBlock();

    /* API Version */
    SaveFile.StartDataBlock("APIV");
    VerStr=Data.APIVersion.c_str();
    Tmp=strtol(VerStr,(char **)&VerStr,10);
    if(VerStr==NULL)
    {
        QMessageBox::critical(NULL,"Failed","Version must be in the format "
                "0.0.0.0 (major.minor.patch.letter)");
        SaveFile.Close();
        return;
    }
    SaveFile.Write(&Tmp,1);
    VerStr++;

    Tmp=strtol(VerStr,(char **)&VerStr,10);
    if(VerStr==NULL)
    {
        QMessageBox::critical(NULL,"Failed","Version must be in the format "
                "0.0.0.0 (major.minor.patch.letter)");
        SaveFile.Close();
        return;
    }
    SaveFile.Write(&Tmp,1);
    VerStr++;

    Tmp=strtol(VerStr,(char **)&VerStr,10);
    if(VerStr==NULL)
    {
        QMessageBox::critical(NULL,"Failed","Version must be in the format "
                "0.0.0.0 (major.minor.patch.letter)");
        SaveFile.Close();
        return;
    }
    SaveFile.Write(&Tmp,1);
    VerStr++;

    Tmp=strtol(VerStr,(char **)&VerStr,10);
    if(VerStr==NULL)
    {
        QMessageBox::critical(NULL,"Failed","Version must be in the format "
                "0.0.0.0 (major.minor.patch.letter)");
        SaveFile.Close();
        return;
    }
    SaveFile.Write(&Tmp,1);
    SaveFile.EndDataBlock();

    /* Add the Linux binary */
    if(Data.LinuxFilename!="")
    {
        if(!SaveFile2RIFF(&SaveFile,Data.LinuxFilename.c_str(),"LIN6"))
        {
            SaveFile.Close();
            return;
        }
    }

    if(Data.WindowsFilename!="")
    {
        if(!SaveFile2RIFF(&SaveFile,Data.WindowsFilename.c_str(),"WIN6"))
        {
            SaveFile.Close();
            return;
        }
    }

    if(Data.MacFilename!="")
    {
        if(!SaveFile2RIFF(&SaveFile,Data.MacFilename.c_str(),"MACS"))
        {
            SaveFile.Close();
            return;
        }
    }

    if(Data.RPI64Filename!="")
    {
        if(!SaveFile2RIFF(&SaveFile,Data.RPI64Filename.c_str(),"RPI6"))
        {
            SaveFile.Close();
            return;
        }
    }

    if(Data.RPI32Filename!="")
    {
        if(!SaveFile2RIFF(&SaveFile,Data.RPI32Filename.c_str(),"RPI3"))
        {
            SaveFile.Close();
            return;
        }
    }

    SaveFile.Close();
}

bool ExternPluginMaker::SaveFile2RIFF(class RIFF *RIFF,const char *Filename,const char *ChunkName)
{
    FILE *in;
    int size;
    uint8_t *block;

    in=fopen(Filename,"rb");
    if(in==NULL)
    {
        string Msg;
        Msg="Failed to open:";
        Msg+=Filename;
        QMessageBox::critical(NULL,"Failed",Msg.c_str());
        return false;
    }

    fseek(in,0,SEEK_END);
    size=ftell(in);
    fseek(in,0,SEEK_SET);

    block=(uint8_t *)malloc(size);
    if(block==NULL)
    {
        QMessageBox::critical(NULL,"Failed","Out of memory");
        return false;
    }

    if(fread(block,size,1,in)!=1)
    {
        string Msg;
        Msg="Failed to read:";
        Msg+=Filename;
        QMessageBox::critical(NULL,"Failed",Msg.c_str());
        return false;
    }

    RIFF->StartDataBlock(ChunkName);
    RIFF->Write(block,size);
    RIFF->EndDataBlock();

    free(block);

    return true;
}


void ExternPluginMaker::on_SelectRPI64File_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Shared Objects (*.so);;All Files (*)"));
    ui->RPI64_SO->setText(fileName);
}

void ExternPluginMaker::on_SelectRPI32File_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Shared Objects (*.so);;All Files (*)"));
    ui->RPI32_SO->setText(fileName);
}
