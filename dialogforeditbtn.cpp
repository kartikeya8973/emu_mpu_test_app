#include "dialogforeditbtn.h"
#include "ui_dialogforeditbtn.h"
#include <QMessageBox>
#include "mainwindow.h"
extern     MainWindow *w;
dialogForEditBtn::dialogForEditBtn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogForEditBtn)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), w, SLOT(refreshDatabase()));
}

dialogForEditBtn::~dialogForEditBtn()
{
    delete ui;
}

void dialogForEditBtn::on_pushButton_clicked()
{
    QString SNoForEdit = QString::number(ui->textEdit->toPlainText().toInt());

    QString SNoUpdated =  QString::number(ui->textEdit_2->toPlainText().toInt());
    QString deviceTypeUpdated = ui->textEdit_3->toPlainText();
    QString IPAddressUpdated =  ui->textEdit_4->toPlainText();
    QString portNoUpdated =  QString::number(ui->textEdit_5->toPlainText().toInt());
    QSqlQuery qry;
    qry.prepare("UPDATE devicesData SET SNo='"+SNoUpdated+"',DeviceType='"+deviceTypeUpdated+"',IPAddress='"+IPAddressUpdated+"',Port='"+portNoUpdated+"' WHERE SNo='"+SNoForEdit+"'  ");
    if(qry.exec()){
        qDebug()<<"Updated successfully";

        QMessageBox msgBox;
        msgBox.setText("UpdatedmyThread"
                       " successfully");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setFixedWidth(500);
        msgBox.setFixedHeight(500);
        msgBox.button(QMessageBox::Ok)->animateClick(5000);
        msgBox.exec();

        accept();
    }
    else{
        qDebug()<<"Not updated";

    }
}

