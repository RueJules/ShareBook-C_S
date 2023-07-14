/*
 * datetime:2023-6-17
 * author:chenzexi
*/
#ifndef NETIZENINTERFACE_H
#define NETIZENINTERFACE_H
#include <QString>
#include <QJsonDocument>
#include "noteProxy.h"

class NetizenProxy;
class NetizenInterface {

public:
    NetizenInterface(QString id);
    virtual void addNote(QString noteId, NoteProxy &&note)=0;//添加新发布的笔记，应该是bool类型？？
    virtual void addFan(QString fanId, NetizenProxy &&fan)=0;//添加新的粉丝
    virtual void addConcern(QString concernId, NetizenProxy &&concern)=0;//添加新的关注
    virtual void addFootMark(QString checkNoteId, NoteProxy &&checkNote) = 0;
    virtual QJsonObject getAbstract()=0;
    virtual QJsonObject getDetails()=0;

    QString id() const;

private:
    QString m_id;

};

#endif
