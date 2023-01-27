#include "threads.h"
#include "QDebug"
#include "common.h"


#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>
#include "statusofips.h"




//statusOfIPs *statusCheck = new statusOfIPs ();

threads::threads()
{
    //    connect(this,SIGNAL(pingIPsAndPort(QList<QString>,QList<int> )),this,SLOT(pingIPsAndPort(QList<QString>,QList<int> )));



}
void threads::run(){


    testCases=IDLE;
    //        threads *mthread=new threads ();
    forever {
        switch (testCases) {
        case IDLE:
        {

            if(threadsTableInfoList.empty()){
                this->msleep(100);
            }
            else{
                testCases=PING_TEST;
            }

        }
            break;

        case PING_TEST:
        {
            QString IPAddressList="";



            totalNoOfIps=threadsTableInfoList.size();
            if(threadsTableInfoList.empty()){
                this->msleep(100);
            }
            else{
                int index=0;
                pc=0;
//                QList<QString> pingIp;
//                QList<int>pingPort;
                while(index<threadsTableInfoList.size()){
                    QString IPAddress=threadsTableInfoList.value(index).IPAddress;


                    QString ping_req ="ping -w 1 "  + IPAddress;
                    pc++;
                    c=enableFlag();
                    c=disableFlag();

                    if(c==0){
                        c=enableFlag();
                        testCases=IDLE;

                        break;
                    }

                    if(!system(ping_req.toLocal8Bit())){

                        IPAddress=IPAddress+"/";
                        IPAddressList.append(IPAddress);

                    }
                    emit totalSuccessfulConnSignal(pc,totalNoOfIps);


                    index++;

                }



            }

            emit pingIPAddressSignal(IPAddressList);

            IPAddressList.clear();

            testCases=IDLE;

            threadsTableInfoList.clear();

        }
            break;
            //        check for connection
        case CONN_TEST:
        {
                QString successfulConnIP="";
                for(int index=0;index<successfulPingIP.size();index++){

                    socket=new QTcpSocket ();

                    QString IPAddress=successfulPingIP.value(index);
                    int port=successfulPingPort.value(index);
                    socket->connectToHost(QHostAddress(IPAddress),port);

                     if(socket->waitForConnected(100)){


                         socket->write(QByteArray("id?"));
                         socket->waitForBytesWritten(10000);
                         if(socket->waitForReadyRead(10000)){
//                             connect(socket,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
                             QByteArray datas =socket->readAll();
                             qDebug()<<datas;
                             IPAddress=IPAddress+'/';
                             successfulConnIP.append(IPAddress);
                             successfulConnPort.append(port);
                         }
                         socket->disconnect();
                         socket->close();
                         this->msleep(1000);


                     }




                }


            emit successfulConnSignal(successfulConnIP,successfulConnPort);

            successfulConnPort.clear();



            testCases=IDLE;

        }
            break;
        default:{
            if(threadsTableInfoList.empty()){
                this->msleep(100);
            }
            else{
                testCases=PING_TEST;
            }
        }

        }
    }

}


void threads::onReadyRead(){
     QByteArray datas =socket->readAll();

}


void threads::mainWindowSlot(QList<table_data_now> newData,s_test_task testTask){

    int index=0;
    if(testCases!=IDLE){
        threadsTableInfoList.clear();

    }
    testCases=testTask;
    successfulPingIP.clear();
    successfulPingPort.clear();


    while(index<newData.size()){
        table_data_now tb;
        QString IPAddress= newData.value(index).IPAddress;
        int port = newData.value(index).port;
        int SNo = newData.value(index).serialNo;
        tb.IPAddress=IPAddress;
        tb.port=port;
        tb.serialNo=SNo;
        threadsTableInfoList.append(tb);
        index++;

    }

}


void threads::checkConnectionSlot(QList<QString> IPAddress,QList<int> port, s_test_task testTask){


    testCases=testTask;

    successfulConnPort.clear();
    successfulPingIP.clear();
    successfulPingPort.clear();

    for(int index=0;index<IPAddress.size();index++)    {
        successfulPingIP.append(IPAddress.value(index));
        successfulPingPort.append(port.value(index));
    }


}



int x=1;
void threads::interruptOccurSlot()
{
    //    threadsTableInfoList.clear();
    testCases=IDLE;

    x=0;
    disableFlag();

}
int threads::disableFlag(){

    return x;
}
int threads::enableFlag(){
    x=1;
    return x;
}


