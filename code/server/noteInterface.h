/*Auther:GongYanxian
Date:2023.6.19*/
#ifndef NOTEINTERFACE_H
#define NOTEINTERFACE_H
#include <QString>
#include <QJsonObject>
//#include "materialProxy.h"

class MaterialProxy;
class NoteInterface {

public:
    NoteInterface(QString id);
    virtual void addMaterial(int order, MaterialProxy &&material)=0;//添加新的消息
    QString get_Id() const;
    virtual QJsonObject getNoteAbstract() = 0;
    virtual QJsonObject getNoteDetails() = 0;
private:
    QString m_id;


};

#endif
