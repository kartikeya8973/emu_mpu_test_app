#ifndef THREADS_H
#define THREADS_H

#include <QThread>
#include <QQueue>
#include "common.h"
#include <QMutex>
#include <QWaitCondition>
#include <QTcpServer>
#include <QTcpSocket>




class threads:public QThread
{
    Q_OBJECT

public:
    int totalNoOfIps;

    int pc=0;
    int c=1;
    threads();
    void run();
    bool stop;
    QList<table_data_now> threadsTableInfoList;
    QMutex mutex1;
    QWaitCondition wait1;
    QList<int> successfulPingPort;
    QList<QString> successfulPingIP;

    QList<int> successfulConnPort;

    #include <QMetaType>
    QTcpServer *server;
    QTcpSocket *socket;





private slots:

   void mainWindowSlot(QList<table_data_now> newData,s_test_task testTask);

   void interruptOccurSlot();
   void checkConnectionSlot(QList<QString> pingIPAddress,QList<int> port,s_test_task testTask);
//   void  pingIPsAndPortSlot(QList<QString> pingIp,QList<int> pingPort);
   void onReadyRead();



signals:
   void pingIPAddressSignal(QString);
   void totalSuccessfulConnSignal(int,int);
//   void pingIPsAndPort(QList<QString> ,QList<int> );
   void successfulConnSignal(QString,QList<int>);

private:
   volatile s_test_task testCases;
   int disableFlag();
   int enableFlag();
//   void onReadyRead();

};




#endif // THREADS_H
