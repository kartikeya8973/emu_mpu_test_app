#ifndef RINFO_H
#define RINFO_H
#include <QString>
#include <QJsonObject>
#include <QJsonObject>
#include <QJsonArray>


class rinfo
{
public:
    rinfo();

    int     sno;

    QList<double> totalDistanceList;
    double totalDistance=0;
    QString sourceStation;
    QString lastStation;
    QList<double> stationDistanceFromSource;
    QList<QString> stationName;
    QString tr_no_en="";
    QString tr_nm_en="";
    QString tr_route_en="";
    QString tr_via_en="";

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

public slots:
//    void totalDistanceBwtStations(int);
};

#endif // RINFO_H
