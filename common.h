#ifndef COMMON_H
#define COMMON_H
#include <QString>
#include <QQueue>
#include <gst/gst.h>




typedef enum s_test_task{

    IDLE,
    PING_TEST,
    CONN_TEST

} s_test_task_t;

//QList<QString> testTaskList;


typedef struct table_data_now{
  int serialNo;
  QString IPAddress;
  int port;
  bool checkStatus;


}table_data_now;

typedef struct t_packet_s
{
    uint8_t clogged_sensor:1;
    uint8_t stationary_water:1;
    uint8_t carten_open_close:1;
    uint8_t occupancy:1;
    uint8_t water_leval_low:1;
    uint8_t water_leval_medium:1;
    uint8_t water_leval_high:1;
    uint8_t emergency_button:1;

}t_packet_t;


typedef struct packet_s
{
    t_packet_t t_packet1;
    t_packet_t t_packet2;
}packet_t;

//packet_t pac1;




#endif // COMMON_H

