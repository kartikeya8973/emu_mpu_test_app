#include "statusofips.h"
#include "ui_statusofips.h"
#include "QDebug"
#include "threads.h"
#include "QTimer"
statusOfIPs::statusOfIPs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::statusOfIPs)
{
    ui->setupUi(this);
    ui->label->setText("Please wait...");
    ui->progressBar->setValue(0);
    ui->progressBar->show();
    threads *mthreads=new threads ();
    connect(this,SIGNAL(interruptOccurSignal()),mthreads,SLOT(interruptOccurSlot()));




}

statusOfIPs::~statusOfIPs()
{
    delete ui;
}

void statusOfIPs::totalSuccessfulConnSlot(int pc,int totalNoOfIps){
//    qDebug( ) << pc<<"and total ips "<<totalNoOfIps;

    float total=float(pc)/float(totalNoOfIps);
    total=total*100;
    totalIps=total;
    ui->progressBar->setValue(total);
    ui->progressBar->show();
    QTimer *timer=new QTimer ();
    connect(timer,SIGNAL(timeout()),this,SLOT(closeWindow()));
    timer->start(100);
}



void statusOfIPs::on_pushButton_clicked()
{

    emit interruptOccurSignal();



}

void statusOfIPs::closeWindow(){
    accept();
}

