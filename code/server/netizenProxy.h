/*
 * datetime:2023-6-17
 * author:chenzexi
*/
#ifndef NETIZENPROXY_H
#define NETIZENPROXY_H
#include "netizenInterface.h"
#include <memory>
class Netizen;
class NetizenProxy : public NetizenInterface {

public:
    NetizenProxy();
    NetizenProxy(QString id);
    NetizenProxy(QString id,std::unique_ptr<Netizen> &&netizen);

//    //netizen中是简单的添加，proxy中可能有一些处理
    void addNote(QString noteId, NoteProxy &&note) override;//添加新发布的笔记，应该是返回bool类型？？
    void addFan(QString fanId, NetizenProxy && fan) override;//添加新的分析
    void addConcern(QString concernId, NetizenProxy &&concern) override;//添加新的关注
    void addFootMark(QString checkNoteId, NoteProxy &&checkNote) override; //将查看了详情的笔记加入历史记录

    QJsonObject getAbstract() override;
    QJsonObject getDetails() override;

private:
    std::unique_ptr<Netizen> m_netizen;
};

#endif
