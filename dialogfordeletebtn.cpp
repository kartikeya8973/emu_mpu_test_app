#include "dialogfordeletebtn.h"
#include "ui_dialogfordeletebtn.h"
#include <QMessageBox>
#include <QTimer>


dialogForDeleteBtn::dialogForDeleteBtn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogForDeleteBtn)
{
    ui->setupUi(this);
}

dialogForDeleteBtn::~dialogForDeleteBtn()
{
    delete ui;
}

void dialogForDeleteBtn::on_pushButton_clicked()
{


    QString SNo =QString::number( ui->textEdit->toPlainText().toInt());
    QSqlQuery qry;

    qry.prepare("DELETE FROM devicesData WHERE SNo='"+SNo+"'");
    if(qry.exec()){
        ui->textEdit->clear();
        ui->label->clear();
        ui->label->setText("deleted successfully");

        QMessageBox msgBox;
        msgBox.setText("Hello!");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);

        QTimer::singleShot(1000, &msgBox, SLOT(close()));



    }
    else{
        qDebug() << "not deleted";
    }


}


