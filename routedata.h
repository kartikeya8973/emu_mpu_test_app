#ifndef ROUTEDATA_H
#define ROUTEDATA_H


#include <QString>
#include <QTime>
#include <QJsonObject>
#include <QJsonArray>
//#include "station.h"

class routeData
{
public:
    routeData();

//    QList <station> st_list;

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};


#endif // ROUTEDATA_H
