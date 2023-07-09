/*Auther:Huangyihong
Date:2023.6.17*/

#ifndef MATERIALINTERFACE_H
#define MATERIALINTERFACE_H
#include <QString>
#include <QJsonObject>
class MaterialInterface
{
public:
    MaterialInterface(QString new_id);
    QString get_id();
    virtual QJsonObject getDetails()=0;
private:
    QString m_id; //素材id
};

#endif // MATERIALINTERFACE_H
