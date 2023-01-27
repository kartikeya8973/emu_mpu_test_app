#include "routedata.h"

routeData::routeData()
{

}

void routeData::write(QJsonObject &json) const
{

}

void routeData::read(const QJsonObject &json)
{
    if (json.contains("st_list") && json["st_list"].isArray())
    {
        QJsonArray sArray = json["st_list"].toArray();
//        st_list.clear();
//        st_list.reserve(sArray.size());

        for(int sIdx=0;sIdx<sArray.size();sIdx++)
        {
            QJsonObject sObject = sArray[sIdx].toObject();
//            station station;
//            station.read(sObject);
//            st_list.append(station);
        }
    }
}
