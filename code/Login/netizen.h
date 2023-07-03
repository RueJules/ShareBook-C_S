/*
 * datetime:2023-6-17
 * author:chenzexi
*/
#ifndef NETIZEN_H
#define NETIZEN_H
#include <iostream>
#include <QPixmap>
#include <unordered_map>
#include "netizenInterface.h"
#include "noteProxy.h"

class NetizenProxy;

class Netizen : public NetizenInterface {

public:
//    //构造函数
    Netizen();
    Netizen(QString id,  QString nickname, QString profile);

    void addNote(QString noteId, NoteProxy &&note) override;//添加新发布的笔记，应该是返回bool类型？？
    void addFan(QString fanId, NetizenProxy &&fan) override;//添加新的分析
    void addConcern(QString concernId, NetizenProxy &&concern) override;//添加新的关注
    void addFootMark(QString checkNoteId, NoteProxy &&checkNote) override;

    QJsonObject getAbstract() override;
    QJsonObject getDetails() override;

private:

    QString m_nickName;
    QString m_profileImage;//头像路径
    std::unordered_map<QString, NoteProxy> m_pulishNoteList;
    std::unordered_map<QString, NoteProxy> m_footMarkList;
    std::unordered_map<QString, NetizenProxy> m_fanList;
    std::unordered_map<QString, NetizenProxy> m_concernList;

};

#endif
