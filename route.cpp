#include "route.h"

route::route()
{

}

void route::write(QJsonObject &json) const
{
    json["sno"]=sno;
    json["tr_no_en"]=tr_no_en;
    json["tr_nm_en"]=tr_nm_en;
    json["tr_route_en"]=tr_route_en;
    json["tr_via_en"]=tr_via_en;
}

void route::read(const QJsonObject &json)
{
    if (json.contains("sno") && json["sno"].isDouble())
        sno = json["sno"].toInt();

    if (json.contains("tr_no_en") && json["tr_no_en"].isString())
        tr_no_en = json["tr_no_en"].toString();

    if (json.contains("tr_nm_en") && json["tr_nm_en"].isString())
        tr_nm_en = json["tr_nm_en"].toString();

    if (json.contains("tr_route_en") && json["tr_route_en"].isString())
        tr_route_en = json["tr_route_en"].toString();

    if (json.contains("tr_via_en") && json["tr_via_en"].isString())
        tr_via_en = json["tr_via_en"].toString();
}
