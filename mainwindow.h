#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QtSql>
//#include <dialogfordeletebtn.h>
#include <dialogforaddbtn.h>
#include <dialogforeditbtn.h>
#include <statusofips.h>
#include <QList>
#include "common.h"
#include "threads.h"
#include <QtPrintSupport/QPrinter>
//#include "QTcpServer"
//#include "QTcpSocket"
#include "rinfo.h"

#include "route.h"
#include "routedata.h"
#include <QMediaPlayer>
#include <QVideoWidget>




Q_DECLARE_METATYPE(QList<QString>)


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_ENUM(s_test_task)
    Q_ENUMS(s_test_task)

public:

    int count=0;
    s_test_task test;
    QSqlDatabase deviceInfoData;
//    QTcpServer  _server;
//    QTcpSocket  _socket;
    QString ipAddressForCheck;

    void read_rinfo();

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


signals:

    void ping_IPs(QList<table_data_now>,s_test_task);

    void test_signal(int);
    void totalIPsSignal(int,int);
    void checkConnectionSignal(QList<QString>,QList<int>,s_test_task);


private slots:
    void inTimer();
    void play();
    void on_addBtn_clicked();



    void on_deleteBtn_clicked();

    void on_editBtn_clicked();

    void on_pingBtn_clicked();

    void on_clearBtn_clicked();

    void on_actioncsvFile_triggered();

    void on_actionPDF_Format_triggered();

    void on_connectionBtn_clicked();



    void on_sendMsgBtn_2_clicked();

    void on_browseBtn_clicked();

    void on_loadBtn_clicked();




    void on_sendJsonBtn_clicked();
    void textChanged();




    void on_DDBSLBtn_clicked();



    void on_ledPushButton_2_clicked();

    void on_pushButton_10_clicked();

    void on_alertBtn2_clicked();

    void on_pushButton_12_clicked();

    void on_alertBtn4_clicked();

    void on_clearbtn1_clicked();

    void on_clearBtn2_clicked();

    void on_clearBtn3_clicked();

    void on_pushButton_11_clicked();

    void on_nightModeBtn_clicked();

    void on_DayModeBtn_clicked();

    void on_returnBtn_clicked();

    void on_errorBtn_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();




    void on_imageOneBtn_clicked();

    void on_imageTwoBtn_clicked();

    void on_imageBtnThree_clicked();

    void on_imageFourBtn_clicked();

    void on_imageFiveBtn_clicked();


    void on_videoPlayBtn_clicked();

    void on_videoPaused_clicked();

    void on_videoResumeBtn_clicked();


    void on_sendVideoBtn_clicked();

    void on_pushButton_16_clicked();

private:
    int countNumberOfTCPReciveByToilet;
    QMediaPlayer *player;
    QVideoWidget *video;
    QByteArray img1;
    QByteArray img2;
    QByteArray img3;
    QByteArray img4;
    QByteArray img5;

    QString receivedDataForToilet;
    int currentSlide=0;
    int widthOfSlider=0;
    QSlider *hSlider;
    QTcpServer *server;
    QTcpSocket *socket;
    QTcpSocket  _socket;
    packet_t pac1;


    int totalIps;
    QList<QString> dupList;
    QList<QString> pingIPList;
    QList<int>dupPort;
    QList<table_data_now> tableInfoQueue;
    Ui::MainWindow *ui;
//    route variables
    QString pathOFJson;
    rinfo r_info;
    QString family;
    int id;
    QList<QString> lcdPisIPs;
    QList<int> lcdPisPort;
    void lcdPisTypeIPs();
    void successfulConnIPsAndPort();
    void toiletDevices();
    void unstableSlider();
    QString fileDialogOpen();
    void sendImages(QByteArray);


public slots:
//    void onReadyRead();
    void refreshDatabase();
    void pingIPAddressSlot(QString checkedIPList);
    void successfulConnSlot(QString checkConnIp,QList<int> checkConnPort);
    void valueOfSlider(int val);
    void slideShow(int);
//    void clientConnected();
    void serverReadyRead();
//    void totalDistanceBwtStations(int totalDistance);
//    void totalSuccessfulConnSlot(int pc,int totalNoOfIps);


};
#endif // MAINWINDOW_H
