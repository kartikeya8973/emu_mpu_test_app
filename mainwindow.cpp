#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form1.h"
#include "QList"
#include "QtDebug"
#include "QTableView"

#include "ui_dialogforaddbtn.h"
//#include "ui_dialogfordeletebtn.h"
#include "ui_dialogforeditbtn.h"
#include "ui_statusofips.h"

#include <QCheckBox>
#include <QRadioButton>
#include "common.h"
#include "threads.h"
#include "statusofips.h"
#include "QDir"
#include <QtPrintSupport/QPrinter>
#include <QTextTableCell>
#include <QBoxLayout>
#include <QFileDialog>
#include <QPainter>
#include "QScreen"
#include "QDataStream"
#include "QDesktopWidget"
#include "QStackedWidget"
#include <QMediaPlayer>
#include <QVideoWidget>

#include <gst/gst.h>








extern threads *mthread;




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{

    ui->setupUi(this);
    player=new QMediaPlayer () ;
    video=new QVideoWidget (ui->videoTab) ;


    connect(ui->DDBSLTextEdit,SIGNAL(textChanged()),this,SLOT(textChanged()));
//    qDebug()<< pac1->emergency_button;

//    read json files
    read_rinfo();



//    make connection button disabled
    ui->connectionBtn->setEnabled(false);

    connect(ui->deleteBtn, SIGNAL(clicked()), this, SLOT(refreshDatabase()));


    mthread =new threads ();

//    statusOfIPs *statusOfIps =new statusOfIPs();
    connect(this,SIGNAL(ping_IPs(QList<table_data_now>,s_test_task)),mthread,SLOT(mainWindowSlot(QList<table_data_now>,s_test_task)));
    connect(this,SIGNAL(checkConnectionSignal(QList<QString>,QList<int>,s_test_task)),mthread,SLOT(checkConnectionSlot(QList<QString>,QList<int>,s_test_task)));
    connect(mthread,SIGNAL(pingIPAddressSignal(QString)),this,SLOT(pingIPAddressSlot(QString)));
    connect(mthread,SIGNAL(successfulConnSignal(QString,QList<int>)),this,SLOT(successfulConnSlot(QString,QList<int>)));



    mthread->setObjectName("first thread");
    mthread->start(QThread::HighestPriority);

    deviceInfoData=QSqlDatabase::addDatabase("QSQLITE");
    deviceInfoData.setDatabaseName(("/home/csemi/projectDatabase/deviceData.sqlite"));
    QSqlQuery qry;
    if(deviceInfoData.open()){
        qDebug()<<"database open";
    }
    else{
        qDebug()<<"database not open";
    }
    qry.exec("select * from devicesData");


    QList<QString> Sno;
    QList<QString> Port;
    QList<QString> IPAddress;
    QList<QString> deviceType;

    //storing all the values of database in a local array
    while(qry.next()){

        Sno.append((qry.value(0).toString()));
        deviceType.append((qry.value(1).toString()));
        IPAddress.append((qry.value(2).toString()));
        Port.append((qry.value(3).toString()));

    }
    int index=0;
    while(index<Sno.size()){

//        QTableWidgetItem *checkBoxItem = new QTableWidgetItem();
//        checkBoxItem->setCheckState(Qt::Unchecked);

        QWidget *pWidget = new QWidget();
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setCheckState(Qt::Unchecked);
        QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(checkBox);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0,0,0,0);
        pWidget->setLayout(pLayout);



        ui->deviceTable->insertRow(index);
        QTableWidgetItem *item0=new QTableWidgetItem(Sno.value(index));
        item0->setTextAlignment( Qt::AlignCenter );
        ui->deviceTable->setItem(index,0,item0);
        QTableWidgetItem *item1=new QTableWidgetItem(deviceType.value(index));
        item1->setTextAlignment( Qt::AlignCenter );
        ui->deviceTable->setItem(index,1,item1);
        QTableWidgetItem *item2=new QTableWidgetItem(IPAddress.value(index));
        item2->setTextAlignment( Qt::AlignCenter );
        ui->deviceTable->setItem(index,2,item2);
        QTableWidgetItem *item3=new QTableWidgetItem(Port.value(index));
        item3->setTextAlignment( Qt::AlignCenter );
        ui->deviceTable->setItem(index,3,item3);
        ui->deviceTable->setCellWidget(index,4,pWidget);
//        checkBoxItem->setTextAlignment(Qt::AlignHCenter);
//         ui->deviceTable->setCellWidget(index,4,pWidget);

//        ui->deviceTable->setItem(index,4,checkBoxItem);
//        ui->deviceTable->item(index,4)->setTextAlignment(Qt::AlignCenter);
        index=index+1;
    }
    ui->deviceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->deviceTable->resizeColumnToContents(4);
    ui->deviceTable->verticalHeader()->setVisible(false);

//    create csv file for status check
    QDir *dir=new QDir ();

    QString Snos="Serial No";
    QString DeviceType="Device Type";
    QString IPAdd="IP Address";
    QString port="Port";
    QString ping="Ping Status";
    QString conn="Connection Status";
    QString columnNameOfFile= Snos + "," + DeviceType +  "," + IPAdd +  "," + port+  "," +ping+  "," + conn;
    QFile csvFile(QString(dir->absolutePath() + "/" + "test.csv"));
//     | QIODevice::Truncate
    if(csvFile.open(QFile::WriteOnly| QFile::Append| QIODevice::Truncate)){
        qDebug()<<"file created successfully";
        QTextStream out(&csvFile);
        out << columnNameOfFile<<'\n';
        csvFile.close();
    }
    else{
        qDebug()<<"file not created";
    }






}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_addBtn_clicked()
{

    dialogForAddBtn *dialog = new dialogForAddBtn(this);
    dialog->show();
}

void MainWindow::on_deleteBtn_clicked()
{
    qDebug( ) <<"and total ips "<<totalIps;
    QList<QString> serialNo;
    int row=ui->deviceTable->rowCount();

    for(int i=0;i<row;i++){
       QWidget *widget=new QWidget ();
       widget=ui->deviceTable->cellWidget(i,4);
       auto hbox=widget->layout();
       auto    checkBtn = qobject_cast<QCheckBox *>(hbox->itemAt(0)->widget());

       if(checkBtn->checkState()){
           QString SNo = QString::number(ui->deviceTable->item(i,0)->text().toInt());
           serialNo.append(SNo);
       }


    }
    int sizeOfCheckedBoxes=serialNo.size();
    int counter=0;
    QSqlQuery qry;
    for(int index=0;index<serialNo.size();index++){
        QString SNo=QString::number(serialNo.value(index).toInt());
        qry.prepare("DELETE FROM devicesData WHERE SNo='"+SNo+"'");

        if(qry.exec()){
            counter +=1;
        }
        else{
            qDebug() << "not deleted";
        }
    }
    if(counter==0){
        QMessageBox msgBox;
        msgBox.setText("you did't select any row, Please select a row");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setFixedWidth(500);
        msgBox.setFixedHeight(500);
        msgBox.button(QMessageBox::Ok)->animateClick(5000);
        msgBox.exec();
    }
    else{
        if(counter==sizeOfCheckedBoxes){

            QMessageBox msgBox;
            msgBox.setText("Deleted successfully");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setFixedWidth(500);
            msgBox.setFixedHeight(500);
            msgBox.button(QMessageBox::Ok)->animateClick(5000);
            msgBox.exec();

        }
        else{
            QMessageBox msgBox;
            msgBox.setText("Not Deleted");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
    }


}


void MainWindow::on_editBtn_clicked()
{
    dialogForEditBtn *dialog =new dialogForEditBtn(this);
    dialog->show();
}






void MainWindow::refreshDatabase(){
    ui->deviceTable->setRowCount(0);
     ui->connectionBtn->setEnabled(false);
     ui->pingBtn->setEnabled(true);

    QSqlQuery qry;
    qry.exec("select * from devicesData");

    QList<QString> Sno;
    QList<QString> Port;
    QList<QString> IPAddress;
    QList<QString> deviceType;


    //clear my QList
    tableInfoQueue.clear();

    //storing all the values of database in a local array
    while(qry.next()){

        Sno.append((qry.value(0).toString()));
        deviceType.append((qry.value(1).toString()));
        IPAddress.append((qry.value(2).toString()));
        Port.append((qry.value(3).toString()));

    }
    int index=0;
    while(index<Sno.size()){


//        QTableWidgetItem *checkBoxItem = new QTableWidgetItem();
//        checkBoxItem->setCheckState(Qt::Unchecked);


        ui->deviceTable->insertRow(index);
        ui->deviceTable->setItem(index,0,new QTableWidgetItem(Sno.value(index)));
        ui->deviceTable->setItem(index,1,new QTableWidgetItem(deviceType.value(index)));
        ui->deviceTable->setItem(index,2,new QTableWidgetItem(IPAddress.value(index)));
        ui->deviceTable->setItem(index,3,new QTableWidgetItem(Port.value(index)));
        QWidget *pWidget = new QWidget();
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setCheckState(Qt::Unchecked);
        QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(checkBox);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0,0,0,0);
        pWidget->setLayout(pLayout);

        ui->deviceTable->setCellWidget(index,4,pWidget);

//        ui->deviceTable->setItem(index,4,checkBoxItem);


        index=index+1;
    }


}


void MainWindow::on_pingBtn_clicked()
{

    refreshDatabase();
    ui->pingBtn->setEnabled(false);
    ui->connectionBtn->setEnabled(false);
    statusOfIPs *dialogIPs = new statusOfIPs (this) ;
    connect(mthread,SIGNAL(totalSuccessfulConnSignal(int,int)),dialogIPs,SLOT(totalSuccessfulConnSlot(int,int)));
    dialogIPs->show();


    int totalEntriesInTable=ui->deviceTable->rowCount();
    int index=0;
    tableInfoQueue.clear();
    while(index<totalEntriesInTable){
        int SNo=ui->deviceTable->item(index,0)->text().toInt();
        QString IP=ui->deviceTable->item(index,2)->text();
        int port=ui->deviceTable->item(index,3)->text().toInt();

        table_data_now tb;
        tb.serialNo=SNo;
        tb.IPAddress=IP;
        tb.port=port;
        tb.checkStatus=false;
        tableInfoQueue.append(tb);
        index++;
    }
    emit ping_IPs(tableInfoQueue,PING_TEST);
}

void MainWindow:: pingIPAddressSlot(QString checkedIPList){

//    QList<QString> dupList;
    int index=0;
    QString word="";
    dupList.clear();
    dupPort.clear();
    while(index<checkedIPList.size()){
        if(checkedIPList.at(index)=="/"){
            pingIPList.append(word);
            word="";
        }
        else{
            word=word+checkedIPList.at(index);
        }
        index++;
    }
//    mke connection button enabled

    int lengthOfDupList=pingIPList.size();
    index=0;
    ui->connectionBtn->setEnabled(true);
    for(int i=0;i<ui->deviceTable->rowCount();i++){

        if(index<lengthOfDupList){

            if(pingIPList[index]==ui->deviceTable->item(i,2)->text()){

//                ui->deviceTable->item(i,4)->setCheckState(Qt::Checked);
                QWidget *pWidget = new QWidget();
                QCheckBox *checkBox = new QCheckBox();
                checkBox->setCheckState(Qt::Checked);
                QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
                pLayout->addWidget(checkBox);
                pLayout->setAlignment(Qt::AlignCenter);
                pLayout->setContentsMargins(0,0,0,0);
                pWidget->setLayout(pLayout);
                pWidget->setStyleSheet("Background-color:rgb(150, 200, 50);");
                ui->deviceTable->setCellWidget(i,4,pWidget);

                for(int j=0;j<ui->deviceTable->columnCount()-1;j++){
                    ui->deviceTable->item(i,j)->setBackground(QColor(150, 200, 50)); /*green*/

                }
                index++;
            }
            else{

                QWidget *pWidget = new QWidget();
                QCheckBox *checkBox = new QCheckBox();
                checkBox->setCheckState(Qt::Unchecked);
                QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
                pLayout->addWidget(checkBox);
                pLayout->setAlignment(Qt::AlignCenter);
                pLayout->setContentsMargins(0,0,0,0);
                pWidget->setLayout(pLayout);
                pWidget->setStyleSheet("Background-color:rgb(200, 41, 41);");

                ui->deviceTable->setCellWidget(i,4,pWidget);
                for(int j=0;j<ui->deviceTable->columnCount()-1;j++){
                    ui->deviceTable->item(i,j)->setBackground(QColor(200, 41, 41));/* red*/
                }
            }

        }

     }


    ui->pingBtn->setEnabled(true);

}

void MainWindow::on_clearBtn_clicked()
{

    refreshDatabase();

}


// create csv files

void MainWindow::on_actioncsvFile_triggered()
{
    dupList.clear();
    dupPort.clear();
    successfulConnIPsAndPort();
    QDir *dir=new QDir ();
    for(int i=0;i<ui->deviceTable->rowCount();i++){
         QString Sno=ui->deviceTable->item(i,0)->text();
         QString DeviceType=ui->deviceTable->item(i,1)->text();
          QString IPAdd=ui->deviceTable->item(i,2)->text();
          QString port=ui->deviceTable->item(i,3)->text();
          QString ping;
          QString conn;
          if(dupList.contains(ui->deviceTable->item(i,2)->text())){
              ping="Ok!";
          }
          else{
              ping="Not Ok!";
          }
          if(dupList.contains(ui->deviceTable->item(i,2)->text())){
              conn="Start";
          }
          else{
              conn="Not Start";

          }
          QString columnNameOfFile= Sno + "," + DeviceType +  "," + IPAdd +  "," + port+  "," +ping+  "," + conn;
          QFile csvFile(QString(dir->absolutePath() + "/" + "test.csv"));
          if(csvFile.open(QFile::WriteOnly| QFile::Append)){
              QTextStream out(&csvFile);
              out << columnNameOfFile<<'\n';
              csvFile.close();
          }
          else{
              qDebug()<<"not added data";
          }




    }

}

//

void MainWindow::on_actionPDF_Format_triggered()
{
    dupList.clear();
    dupPort.clear();
    successfulConnIPsAndPort();
    qDebug()<<dupPort;
    const int columns = ui->deviceTable->columnCount();
    const int rows = ui->deviceTable->rowCount();
    QTextDocument doc;
    QTextCursor cursor(&doc);
    QTextTableFormat tableFormat;
    tableFormat.setHeaderRowCount(1);
    tableFormat.setAlignment(Qt::AlignHCenter);
    tableFormat.setCellPadding(0);
    tableFormat.setCellSpacing(0);
    tableFormat.setBorder(1);

    tableFormat.clearColumnWidthConstraints();
    QTextTable *textTable = cursor.insertTable(rows + 1, columns+1, tableFormat);
    QTextCharFormat tableHeaderFormat;
    tableHeaderFormat.setBackground(QColor("#DADADA"));
    int i;
    QList<QString> tableCol;
    tableCol<<"Serial No" <<"Device Type" << "IP Address" <<  "Port" << "Ping Status" << "Connection";
    for (i = 0; i < tableCol.size(); i++) {
        QTextTableCell cell = textTable->cellAt(0, i);
        cell.setFormat(tableHeaderFormat);
        QTextCursor cellCursor = cell.firstCursorPosition();
        QString value=tableCol.value(i);
        cellCursor.insertText(value);

    }


    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns-1; j++) {


            QTextTableCell cell = textTable->cellAt(i+1, j);
            QTextCursor cellCursor = cell.firstCursorPosition();
            cellCursor.insertText(ui->deviceTable->item(i, j)->text());
        }
    }
    for(int i=0;i<rows;i++){
        QString ping;

        if(dupList.contains(ui->deviceTable->item(i,2)->text())){
            ping="Ok!";
        }
        else{
            ping="Not Ok!";
        }
        QTextTableCell cell = textTable->cellAt(i+1, 4);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(ping);

    }
    for(int i=0;i<rows;i++){

        QString conn;
        if(dupList.contains(ui->deviceTable->item(i,2)->text())){
            conn="Start";
        }
        else{
            conn="Not Start";

        }
        QTextTableCell cell = textTable->cellAt(i+1, 5);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(conn);

    }


    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);

    printer.setOutputFileName("/home/csemi/Desktop/dtest.pdf");
    doc.setDocumentMargin(0);
    doc.setTextWidth(5);
    doc.print(&printer);
}



void MainWindow::on_connectionBtn_clicked()
{

    successfulConnIPsAndPort();
    ui->connectionBtn->setEnabled(false);

    emit checkConnectionSignal(dupList,dupPort,CONN_TEST);



}



void MainWindow::successfulConnSlot(QString checkConnIp,QList<int> checkConnPort){
    int r=ui->deviceTable->rowCount();
    ui->connectionBtn->setEnabled(true);
    int index=0;
    QString word="";
    QList<QString> checkConnIPList;
    while(index<checkConnIp.size()){
        if(checkConnIp.at(index)=="/"){
            checkConnIPList.append(word);
            word="";
        }
        else{
            word=word+checkConnIp.at(index);
        }
        index++;
    }
    qDebug()<<checkConnIPList;
    for(int i=0;i<r;i++){
        QString IPAddress = ui->deviceTable->item(i,2)->text();
        int port= ui->deviceTable->item(i,3)->text().toInt();
        if(checkConnIPList.contains(IPAddress) && checkConnPort.contains(port)){
            ui->deviceTable->item(i,0)->setBackground(QColor(150, 200, 50));
            ui->deviceTable->item(i,1)->setBackground(QColor(150, 200, 50));
            ui->deviceTable->item(i,2)->setBackground(QColor(150, 200, 50));
            ui->deviceTable->item(i,3)->setBackground(QColor(150, 200, 50));
            QWidget *pWidget = new QWidget();
            QCheckBox *checkBox = new QCheckBox();
            checkBox->setCheckState(Qt::Checked);
            QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
            pLayout->addWidget(checkBox);
            pLayout->setAlignment(Qt::AlignCenter);
            pLayout->setContentsMargins(0,0,0,0);
            pWidget->setLayout(pLayout);
            pWidget->setStyleSheet("Background-color:rgb(150, 200, 50);");
            ui->deviceTable->setCellWidget(i,4,pWidget);


        }
        else{
            dupList.clear();
            dupPort.clear();
            successfulConnIPsAndPort();

            if(dupList.contains(IPAddress))
            {


                ui->deviceTable->item(i,0)->setBackground(QColor(114, 159, 207));
                ui->deviceTable->item(i,1)->setBackground(QColor(114, 159, 207));
                ui->deviceTable->item(i,2)->setBackground(QColor(114, 159, 207));
                ui->deviceTable->item(i,3)->setBackground(QColor(114, 159, 207));
                QWidget *pWidget = new QWidget();
                QCheckBox *checkBox = new QCheckBox();
                checkBox->setCheckState(Qt::Unchecked);
                QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
                pLayout->addWidget(checkBox);
                pLayout->setAlignment(Qt::AlignCenter);
                pLayout->setContentsMargins(0,0,0,0);
                pWidget->setLayout(pLayout);
                pWidget->setStyleSheet("Background-color:rgb(114, 159, 207);");
                ui->deviceTable->setCellWidget(i,4,pWidget);
            }
        }
    }

    checkConnIPList.clear();
}

void MainWindow::on_sendMsgBtn_2_clicked()
{
    QString msg=ui->msgTextEdit_2->toPlainText();
    QString selectedLang=ui->comboBox_2->currentText();



    successfulConnIPsAndPort();
    qDebug()<<"IP are "<<dupList<<"POrt are "<< dupPort;
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if((deviceType=="LCD-PIS-18"|deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port)){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);

            QJsonObject msgObj;
            msgObj["msg"]=msg;
            msgObj["lang"]=selectedLang;
            QJsonDocument doc(msgObj);
            QString temp(doc.toJson(QJsonDocument::Compact));
            temp="msg=" + temp;
            socket->write(temp.toLocal8Bit());


            socket->disconnect();
            socket->close();
        }
    }


//    for(int index=0;index<dupList.size();index++){
//        socket=new QTcpSocket ();
//        QString IPAddress=dupList.value(index);
//        int port=dupPort.value(index);
//        socket->connectToHost(QHostAddress(IPAddress),port);

//        QJsonObject msgObj;
//        msgObj["msg"]=msg;
//        msgObj["lang"]=selectedLang;
//        QJsonDocument doc(msgObj);
//        QString temp(doc.toJson(QJsonDocument::Compact));
//        temp="msg=" + temp;
//        socket->write(temp.toLocal8Bit());


//        socket->disconnect();
//        socket->close();


//    }
    ui->msgTextEdit->clear();
}


void MainWindow::on_browseBtn_clicked()
{

    ui->pathOfRouteTextEdit->clear();
    QFileDialog *file=new QFileDialog ();
    QStringList fileName;
    if(file->exec()){

        fileName=file->selectedFiles();
    }
    pathOFJson=fileName.join("");
    ui->pathOfRouteTextEdit->setText(pathOFJson);

}


void MainWindow::on_loadBtn_clicked()
{
    ui->horizontalSlider->setRange(0,r_info.totalDistance);
    qDebug()<<"value of slider is -> "<<r_info.totalDistance;
     unstableSlider();
     connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(valueOfSlider(int)));


}

void MainWindow::valueOfSlider(int val){
    dupList.clear();
    dupPort.clear();
    successfulConnIPsAndPort();
//    qDebug()<<val;
    for(int index=0;index<dupList.size();index++){
        socket=new QTcpSocket ();

        QString IPAddress=dupList.value(index);
        int port=dupPort.value(index);
        socket->connectToHost(QHostAddress(IPAddress),port);
        QString msgFromTextEdit="dist="+QString::number( ui->horizontalSlider->value());
         socket->write(msgFromTextEdit.toLocal8Bit());


         socket->disconnect();
         socket->close();


    }


}

void MainWindow::read_rinfo(){



}


void MainWindow::unstableSlider(){



    int totalNoOfStations=r_info.stationName.size();

    int totalDistance;
    if(totalNoOfStations<=10){
        totalDistance=300;
    }
    else if(totalNoOfStations>10 & totalNoOfStations<=20){
        totalDistance=600;
    }
    else{
        totalDistance=900;
    }
    double s=0;
    double totalPixel=(double)(totalDistance)/r_info.totalDistance;
    double valueOfX=0;


    for(int index=0;index<r_info.stationDistanceFromSource.size();index++){
        valueOfX=totalPixel * r_info.totalDistanceList[index];

        if(valueOfX<=300){
            if(index%2==0){
                ui->stackedWidget->setCurrentIndex(0);
                QLabel *label=new QLabel (ui->routePage1);
                QLabel *label2 =new QLabel(ui->routePage1);
                label->setStyleSheet("background-color:rgb(0,0,0);");
                label->setGeometry(valueOfX+5,90,3,10);
                label2->setStyleSheet("background-color:rgb(200,50,50);");
                label2->setGeometry(valueOfX-10,60,40,25);
                label2->setStyleSheet("font-size:8px");
                label2->setAlignment(Qt::AlignCenter);
                label2->setText( r_info.stationName[index]);
                label2->setWordWrap(true);
                label2->show();
                label->show();
//                qDebug()<<label->geometry().x();

            }
            else{
                ui->stackedWidget->setCurrentIndex(0);
                QLabel *label=new QLabel (ui->routePage1);
                QLabel *label2 =new QLabel(ui->routePage1);
                label2->setStyleSheet("background-color:rgb(200,50,50);");
                label2->setGeometry(valueOfX-10,160,40,25);
                label2->setAlignment(Qt::AlignCenter);
                label2->setText( r_info.stationName[index]);
                label2->setWordWrap(true);
                label2->setStyleSheet("font-size:8px");
                label2->show();
                label->setStyleSheet("background-color:rgb(0,0,0);");
                label->setGeometry(valueOfX+5,130,3,10);
                label->show();
            }


        }
        else if(valueOfX>300 & valueOfX<=600){
            if(index%2==0){

                ui->stackedWidget->setCurrentIndex(1);
                QLabel *label=new QLabel (ui->routePage2);
                label->setStyleSheet("background-color:rgb(0,0,0);");
                label->setGeometry(valueOfX-5-300,90,3,10);
                QLabel *label2 =new QLabel(ui->routePage2);
                label2->setStyleSheet("background-color:rgb(200,60,50);");
                label2->setGeometry(valueOfX-15-300,60,40,25);
                label2->setAlignment(Qt::AlignCenter);
                label2->setText( r_info.stationName[index]);
                label2->setWordWrap(true);
                label2->setStyleSheet("font-size:8px");
                label2->show();
                label->show();
            }
            else{
                ui->stackedWidget->setCurrentIndex(1);
                QLabel *label=new QLabel (ui->routePage2);
                label->setStyleSheet("background-color:rgb(0,0,0);");
                label->setGeometry(valueOfX+5-300,130,3,10);
                QLabel *label2 =new QLabel(ui->routePage2);
                label2->setStyleSheet("background-color:rgb(200,60,50);");
                label2->setGeometry(valueOfX-15-300,150,40,25);
                label2->setAlignment(Qt::AlignCenter);
                label2->setText( r_info.stationName[index]);
                label2->setWordWrap(true);
                label2->setStyleSheet("font-size:8px");
                label2->show();
                label->show();
            }
        }
        else if(valueOfX>600 & valueOfX<=900){
            if(index%2==0){

                ui->stackedWidget->setCurrentIndex(1);
                QLabel *label=new QLabel (ui->routePage2);
                label->setStyleSheet("background-color:rgb(0,0,0);");
                label->setGeometry(valueOfX-5-600,90,3,10);
                QLabel *label2 =new QLabel(ui->routePage2);
                label2->setStyleSheet("background-color:rgb(200,60,50);");
                label2->setGeometry(valueOfX-15-600,60,40,25);
                label2->setAlignment(Qt::AlignCenter);
                label2->setText( r_info.stationName[index]);
                label2->setWordWrap(true);
                label2->setStyleSheet("font-size:8px");
                label2->show();
                label->show();
            }
            else{
                ui->stackedWidget->setCurrentIndex(1);
                QLabel *label=new QLabel (ui->routePage2);
                label->setStyleSheet("background-color:rgb(0,0,0);");
                label->setGeometry(valueOfX+5-600,130,3,10);
                QLabel *label2 =new QLabel(ui->routePage2);
                label2->setStyleSheet("background-color:rgb(200,60,50);");
                label2->setGeometry(valueOfX-15-600,150,40,25);
                label2->setAlignment(Qt::AlignCenter);
                label2->setText( r_info.stationName[index]);
                label2->setWordWrap(true);
                label2->setStyleSheet("font-size:8px");
                label2->show();
                label->show();
            }
        }
    }
    ui->stackedWidget->setCurrentIndex(0);

    r_info.stationDistanceFromSource.clear();






}
void MainWindow::slideShow(int v){

}


void MainWindow::on_sendJsonBtn_clicked()
{

    QFile loadFile(pathOFJson);
    QByteArray jsonFile;
    r_info.stationDistanceFromSource.clear();
    r_info.stationName.clear();
//    ui->pathOfRouteTextEdit->clear();
    r_info.totalDistance=0;
    if (!loadFile.open(QIODevice::ReadOnly)) {

        return;
    }
    else
    {

        QByteArray saveData = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        QJsonObject json = loadDoc.object();
        jsonFile=loadDoc.toJson();

        r_info.read(json);

        ui->horizontalSlider->setMaximum(r_info.totalDistance);
        ui->horizontalSlider->setMinimum(0);

        double s=0;
        double totalPixel=(double)(340-40)/r_info.totalDistance;

        for(int index=0;index<r_info.stationDistanceFromSource.size();index++){

            QFont fontSB;

            if(index%2==0){
                QFrame *frame=new QFrame ();
                QLabel *label =new QLabel();

                frame->setParent(ui->routeTab);
                frame->setStyleSheet("background-color:black;");
                s +=r_info.stationDistanceFromSource[index];
                frame->setGeometry((20+(s*totalPixel)),217,3,30);
                label->setGeometry((20+(s*totalPixel)-30),240,70,50);

                label->setAlignment(Qt::AlignCenter);
                label->setText( r_info.stationName[index]);
                label->setParent(ui->routeTab);


                fontSB.setPointSize(7);

                label->setFont(fontSB);
                label->setWordWrap(true);
                label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
                label->setStyleSheet("font-size:8");
                label->show();
                frame->show();

            }
            else{
                QFrame *frame=new QFrame ();
                QLabel *label =new QLabel();
                fontSB.setPointSize(7);

                label->setFont(fontSB);
                frame->setParent(ui->routeTab);
                frame->setStyleSheet("background-color:black;");
                s +=r_info.stationDistanceFromSource[index];
                frame->setGeometry((20+(s*totalPixel)),184,3,30);

                label->setGeometry((20+(s*totalPixel)-20),140,100,50);
                label->setText(r_info.stationName[index]);
                label->setWordWrap(true);
                label->setParent(ui->routeTab);
                label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
                label->show();
                frame->show();

            }




        }



    }

    // for sending route to the server
    dupList.clear();
    dupPort.clear();
    successfulConnIPsAndPort();

    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port)){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
//            QString msgFromTextEdit="msg="+pathOFJson;
            socket->write(QString("rinfo=" + jsonFile).toLocal8Bit());


            socket->disconnect();
            socket->close();
        }
    }

//    for(int index=0;index<dupList.size();index++){
//        socket=new QTcpSocket ();

//        QString IPAddress=dupList.value(index);
//        int port=dupPort.value(index);
//        socket->connectToHost(QHostAddress(IPAddress),port);
//        QString msgFromTextEdit="msg="+pathOFJson;
//         socket->write(msgFromTextEdit.toLocal8Bit());


//         socket->disconnect();
//         socket->close();


//    }
    ui->pathOfRouteTextEdit->clear();
}

void MainWindow::successfulConnIPsAndPort(){
    dupList.clear();
    dupPort.clear();
    for(int index=0;index<ui->deviceTable->rowCount();index++){
        QWidget *widget=new QWidget ();
        widget=ui->deviceTable->cellWidget(index,4);
        auto hbox=widget->layout();
        auto    checkBtn = qobject_cast<QCheckBox *>(hbox->itemAt(0)->widget());
        if(checkBtn->checkState()){
            dupList.append(ui->deviceTable->item(index,2)->text());
            dupPort.append(ui->deviceTable->item(index,3)->text().toInt());
        }
    }
}






void MainWindow::on_DDBSLBtn_clicked()
{

//    QFileDialog *file=new QFileDialog ();
//    file->setFileMode(QFileDialog::Directory);
//    file->show();
//    connect(file,SIGNAL(fileSelected(const QString)),this,SLOT(findPath(const QString)));

    QRect rect(462,141,192,16);
    QPixmap pixmap=this->grab(rect);
    QImage image=pixmap.toImage().convertToFormat(QImage::Format_Mono,Qt::MonoOnly);

    ushort arr[512]={0};
    int idx=0,idx_ctr=15;
    int temp=0;
    for(int y=0;y<16;y++){
        for(int x=0;x<512;x++){
            if(image.pixel(x,y)==0xFF000000){
                temp=1;
            }
            else{
                temp=0;
            }
            arr[idx] |= (ushort)(temp << idx_ctr);
            idx_ctr--;
            if(idx_ctr<0){
                idx_ctr=15;
                idx++;
            }

        }
    }
    const char* ptr_led_pis;
    ptr_led_pis = (char*)arr;
    socket=new QTcpSocket ();
    socket->connectToHost(QHostAddress("192.168.1.131"),1000);

    socket->write(ptr_led_pis,1024);

    socket->disconnect();
    socket->close();
//    successfulConnIPsAndPort();
//    qDebug()<<dupList;
    for(int j=0;j<ui->deviceTable->rowCount();j++){
//        QString deviceType=ui->deviceTable->item(j,1)->text();
//        QString ip=ui->deviceTable->item(j,2)->text();
//        int port=ui->deviceTable->item(j,3)->text().toInt();

//        if(((deviceType=="LED-PIS")&&dupList.contains(ip)&&dupPort.contains(port))){
//            socket=new QTcpSocket ();
//            socket->connectToHost(QHostAddress(ip),port);

//            socket->write(ptr_led_pis,1024);

//            socket->disconnect();
//            socket->close();
//        }
    }



}

void MainWindow::textChanged(){
    ui->label_3->setText(ui->DDBSLTextEdit->toPlainText());

}






void MainWindow::on_ledPushButton_2_clicked()
{

}


void MainWindow::on_pushButton_10_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="alert=1";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_alertBtn2_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="alert=2";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_pushButton_12_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="alert=3";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_alertBtn4_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="alert=4";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_clearbtn1_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="clear=1";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_clearBtn2_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="clear=2";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_clearBtn3_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="clear=3";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_pushButton_11_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="clear=4";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_nightModeBtn_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="mode=n";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_DayModeBtn_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="mode=d";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_returnBtn_clicked()
{
    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="return";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}


void MainWindow::on_errorBtn_clicked()
{

    successfulConnIPsAndPort();
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18"||deviceType=="LCD-PIS-10")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="err";
            socket->write(msg.toLocal8Bit());

            socket->disconnect();
            socket->close();
        }
    }
}



//get data for the toilet tab
void MainWindow::on_pushButton_13_clicked()
{
    QTimer *timerForToilet=new QTimer;
    connect(timerForToilet,SIGNAL(timeout()),this,SLOT(inTimer()));
    timerForToilet->start(2000);






}
void MainWindow::inTimer(){
    _socket.connectToHost(QHostAddress("192.168.1.129"),24);
    if(_socket.waitForConnected(3000)){
        qDebug()<<"server connected";
        QString msg="data";
        _socket.write(msg.toLocal8Bit());
    }
    else{
        qDebug()<<"not connected";
    }
    connect(&_socket,SIGNAL(readyRead()),this,SLOT(serverReadyRead()));

    _socket.close();


}


void MainWindow::serverReadyRead(){
    countNumberOfTCPReciveByToilet += 1;
    qDebug()<<"number of receive by toilet management unit -> "<<countNumberOfTCPReciveByToilet;
    QByteArray datas=_socket.readAll();
    pac1=*(packet_t*)datas.data();

    int x=pac1.t_packet2.occupancy;
    qDebug()<<x;
    toiletDevices();

}
void MainWindow::toiletDevices(){


    QList<QString> sensedValues;

    sensedValues.append(QString::number(pac1.t_packet1.clogged_sensor));
    sensedValues.append(QString::number( pac1.t_packet1.stationary_water));
    sensedValues.append(QString::number( pac1.t_packet1.carten_open_close));
    sensedValues.append(QString::number( pac1.t_packet1.occupancy));
    QString v1;
    if(pac1.t_packet1.water_leval_high==1){
        v1="high";
    }
    else if(pac1.t_packet1.water_leval_low==1){
        v1="low";
    }
    else {
        v1="medium";
    }
    sensedValues.append(v1);
    sensedValues.append(QString::number( pac1.t_packet1.emergency_button));

    //second values
    sensedValues.append(QString::number( pac1.t_packet1.clogged_sensor));
    sensedValues.append(QString::number( pac1.t_packet1.stationary_water));
    sensedValues.append(QString::number( pac1.t_packet1.carten_open_close));
    sensedValues.append(QString::number( pac1.t_packet1.occupancy));
    QString v2;
    if(pac1.t_packet1.water_leval_high==1){
        v2="high";
    }
    else if(pac1.t_packet1.water_leval_low==1){
        v2="low";
    }
    else {
        v2="medium";
    }
    sensedValues.append(v2);
    sensedValues.append(QString::number( pac1.t_packet1.emergency_button));

    qDebug()<<sensedValues;
    //    create table in toilet tab

        QList<QString> colNameForToiletTable;
        colNameForToiletTable<<"Sensors"<<"TMU1"<<"TMU2"<<"TMU3"<<"TMU4";
        ui->toiletTableWidget->setColumnCount(colNameForToiletTable.size());
        ui->toiletTableWidget->setHorizontalHeaderLabels(colNameForToiletTable);
        ui->toiletTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->toiletTableWidget->resizeColumnToContents(4);
        QList<QString>  firstColName;


        int j=0;
        firstColName<<"clogged sensor1"<<"stationary water1"<<"curtain Open1"<<"occupancy1"<<"water label1"<<"emergency1"<<"clogged sensor2"<<"stationary water2"<<"curtain Open2"<<"occupancy2"<<"water label2"<<"emergency2";
        for(int index=0;index<firstColName.size();index++){
            ui->toiletTableWidget->insertRow(index);
            QTableWidgetItem *item0=new QTableWidgetItem(firstColName.value(index));
            QFont *font=new QFont();
            font->setPointSize(8);
            item0->setTextAlignment( Qt::AlignCenter );
            item0->setFont(*font);
            ui->toiletTableWidget->setItem(index,0,item0);
            if(index<6){
                QTableWidgetItem *item1=new QTableWidgetItem(sensedValues[j]);
                item1->setTextAlignment( Qt::AlignCenter );
                ui->toiletTableWidget->setItem(index,1,item1);
                QTableWidgetItem *item2=new QTableWidgetItem(sensedValues[j]);
                item2->setTextAlignment( Qt::AlignCenter );
                ui->toiletTableWidget->setItem(index,2,item2);
                QTableWidgetItem *item3=new QTableWidgetItem("0");
                item3->setTextAlignment( Qt::AlignCenter );
                ui->toiletTableWidget->setItem(index,3,item3);
                QTableWidgetItem *item4=new QTableWidgetItem("0");
                item4->setTextAlignment( Qt::AlignCenter );
                ui->toiletTableWidget->setItem(index,4,item4);
            }
            else{
                QTableWidgetItem *item1=new QTableWidgetItem("0");
                item1->setTextAlignment( Qt::AlignCenter );
                ui->toiletTableWidget->setItem(index,1,item1);
                QTableWidgetItem *item2=new QTableWidgetItem("0");
                item2->setTextAlignment( Qt::AlignCenter );
                ui->toiletTableWidget->setItem(index,2,item2);
                QTableWidgetItem *item3=new QTableWidgetItem(sensedValues[j]);
                item3->setTextAlignment( Qt::AlignCenter );
                ui->toiletTableWidget->setItem(index,3,item3);
                QTableWidgetItem *item4=new QTableWidgetItem(sensedValues[j]);
                item4->setTextAlignment( Qt::AlignCenter );
                ui->toiletTableWidget->setItem(index,4,item4);
            }
            j=j+1;

            }


        ui->toiletTableWidget->verticalHeader()->setVisible(false);

}


void MainWindow::on_pushButton_14_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_15_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}







QString MainWindow::fileDialogOpen(){
    QFileDialog *file=new QFileDialog ();
    QStringList fileName;
    if(file->exec()){

        fileName=file->selectedFiles();
    }
    return fileName.join("");
}

void MainWindow::sendImages(QByteArray imageData){
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            socket->write(imageData);
            socket->disconnect();
            socket->close();
        }
    }
}

void MainWindow::on_imageOneBtn_clicked()
{

    QPixmap selectPic1(fileDialogOpen());
    ui->imageLabel->setPixmap(selectPic1);
    int i=0;
    QImage image1=selectPic1.toImage();

    img1.append("msg1=");
    QByteArray ba1;
    QBuffer buffer1(&ba1);
    buffer1.open(QIODevice::WriteOnly);
    image1.save(&buffer1,"PNG");

    while(i<ba1.size()){
        img1.append(ba1.at(i));


        i=i+1;
    }
    qDebug()<<" value of i is -> "<<i;
    sendImages(img1);

}


void MainWindow::on_imageTwoBtn_clicked()
{

    QPixmap selectPic2(fileDialogOpen());
    ui->imageLabel->setPixmap(selectPic2);
    int i=0;
    QImage image2=selectPic2.toImage();

    img2.append("msg2=");
    QByteArray ba2;
    QBuffer buffer2(&ba2);
    buffer2.open(QIODevice::WriteOnly);
    image2.save(&buffer2,"PNG");
    i=0;
    while(i<ba2.size()){
        img2.append(ba2.at(i));

        i=i+1;
    }
    sendImages(img2);
}



void MainWindow::on_imageBtnThree_clicked()
{

    QPixmap selectPic3(fileDialogOpen());

    ui->imageLabel->setPixmap(selectPic3);

    QImage image3=selectPic3.toImage();
    int i=0;
    img3.append("msg3=");
    QByteArray ba3;
    QBuffer buffer3(&ba3);
    buffer3.open(QIODevice::WriteOnly);
    image3.save(&buffer3,"PNG");
    i=0;
    while(i<ba3.size()){
        img3.append(ba3.at(i));

        i=i+1;
    }

    sendImages(img3);

}


void MainWindow::on_imageFourBtn_clicked()
{

    QPixmap selectPic4(fileDialogOpen());
    ui->imageLabel->setPixmap(selectPic4);
    int i=0;
    QImage image4=selectPic4.toImage();

    img4.append("msg4=");
    QByteArray ba4;
    QBuffer buffer4(&ba4);
    buffer4.open(QIODevice::WriteOnly);
    image4.save(&buffer4,"PNG");
    i=0;
    while(i<ba4.size()){
        img4.append(ba4.at(i));

        i=i+1;
    }


    sendImages(img4);
}


void MainWindow::on_imageFiveBtn_clicked()
{

     QPixmap selectPic5(fileDialogOpen());
     ui->imageLabel->setPixmap(selectPic5);
     int i=0;




     QImage image5=selectPic5.toImage();

     img5.append("msg5=");
     QByteArray ba5;

     QBuffer buffer5(&ba5);
     buffer5.open(QIODevice::WriteOnly);
     image5.save(&buffer5,"PNG");
     i=0;
     while(i<ba5.size()){
         img5.append(ba5.at(i));

         i=i+1;
     }


     sendImages(img5);
}




void MainWindow::on_videoPlayBtn_clicked()
{
    player=new QMediaPlayer ();
    video=new QVideoWidget (ui->videoTab) ;
    QString pathOFMediaFile=fileDialogOpen();
    qDebug()<<"path of the file is "<<pathOFMediaFile;
    player->setVideoOutput(video);
    player->setMedia(QUrl::fromLocalFile(pathOFMediaFile));
    video->setGeometry(10,10,330,235);
    video->show();
    player->play();

//    free(player);
//    free(video);

}


void MainWindow::on_videoPaused_clicked()
{

    player->pause();
}


void MainWindow::on_videoResumeBtn_clicked()
{

    player->play();
}




void MainWindow::on_sendVideoBtn_clicked()
{
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="play=";
            socket->write(msg.toLocal8Bit());
            socket->disconnect();
            socket->close();
        }
    }
    QTimer::singleShot(2000, this, SLOT(play()));

}
void MainWindow::play(){
    system("gst-launch-1.0 filesrc location= ~/Downloads/test.mp4 ! qtdemux ! queue ! h264parse ! rtph264pay config-interval=10 ! udpsink host=127.0.0.1 port=9999 -v &");


}


void MainWindow::on_pushButton_16_clicked()
{
    for(int j=0;j<ui->deviceTable->rowCount();j++){
        QString deviceType=ui->deviceTable->item(j,1)->text();
        QString ip=ui->deviceTable->item(j,2)->text();
        int port=ui->deviceTable->item(j,3)->text().toInt();
        if(((deviceType=="LCD-PIS-18")&&dupList.contains(ip)&&dupPort.contains(port))){
            socket=new QTcpSocket ();
            socket->connectToHost(QHostAddress(ip),port);
            QString msg="show=";
            socket->write(msg.toLocal8Bit());
            socket->disconnect();
            socket->close();
        }
    }
}

