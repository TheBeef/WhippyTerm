#include "Form_DebugStats.h"
#include "ui_Form_DebugStats.h"
#include "DebugUtils.h"
#include <stdio.h>

static const char *m_DBTNames[] =
{
    FOREACH_DBT(GENERATE_STRING)
};

Form_DebugStats::Form_DebugStats(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_DebugStats)
{
    unsigned int r;

    ui->setupUi(this);

    for(r=0;r<e_DBTMAX;r++)
    {
        Labels[r]=new QLabel(ui->widget_2);
        DataValues[r]=new QLabel(ui->widget_2);

        Labels[r]->setText(m_DBTNames[r]);
        DataValues[r]->setText("--");

        ui->formLayout->setWidget(r+1,QFormLayout::LabelRole, Labels[r]);
        ui->formLayout->setWidget(r+1,QFormLayout::FieldRole, DataValues[r]);
    }

    on_Refresh_pushButton_clicked();
}

Form_DebugStats::~Form_DebugStats()
{
    delete ui;
}

void Form_DebugStats::on_Refresh_pushButton_clicked()
{
    unsigned int r;

    for(r=0;r<e_DBTMAX;r++)
        UpdateTime((e_DBTType)r,DataValues[r]);
}

void Form_DebugStats::UpdateTime(e_DBTType item,QLabel *label)
{
    char buff[200];
    uint32_t Avg;

    Avg=0;
    if(g_DBAvgCounts[item]>0)
        Avg=g_DBAvgTimes[item]/g_DBAvgCounts[item];

    sprintf(buff,"%fms,%fms max,%fms avg",g_DBTimes[item]/1000000.0,
            g_DBMaxTimes[item]/1000000.0,Avg/1000000.0);

    label->setText(buff);
}

void Form_DebugStats::on_pushButton_clicked()
{
    int r;
    for(r=0;r<e_DBTMAX;r++)
    {
        g_DBTimers[r]=0;
        g_DBTimes[r]=0;
        g_DBMaxTimes[r]=0;
        g_DBAvgTimes[r]=0;
        g_DBAvgCounts[r]=0;
    }
    on_Refresh_pushButton_clicked();
}
