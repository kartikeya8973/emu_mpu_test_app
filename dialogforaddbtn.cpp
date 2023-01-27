#include "dialogforaddbtn.h"
#include "ui_dialogforaddbtn.h"
#include "QMessageBox"
#include "mainwindow.h"


extern     MainWindow *w;
dialogForAddBtn::dialogForAddBtn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogForAddBtn)
{

    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), w, SLOT(refreshDatabase()));
}

dialogForAddBtn::~dialogForAddBtn()
{
    delete ui;
}

void dialogForAddBtn::on_pushButton_clicked()
{

    QSqlQuery qry;
    QString Sno = QString::number( ui->textEdit->toPlainText().toInt());
    QString DeviceType=ui->comboBox->currentText();
    QString IPAddress=ui->textEdit_3->toPlainText();
    QString Port= QString::number( ui->textEdit_4->toPlainText().toInt());
    qry.prepare("insert into devicesData (SNo,DeviceType,IPAddress,Port) values('"+Sno+"','"+DeviceType+"','"+IPAddress+"','"+Port+"')");
    if(!qry.exec()){
        qDebug() <<"not stored in databse";
    }
    else{
        qDebug() <<"Stored in database";
        QMessageBox msgBox;
        msgBox.setText("Inserted successfully");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setFixedWidth(500);
        msgBox.setFixedHeight(500);
        msgBox.button(QMessageBox::Ok)->animateClick(5000);
        msgBox.exec();
        accept();

    }

}


