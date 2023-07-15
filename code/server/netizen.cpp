/*
 * datetime:2023-6-17
 * author:chenzexi
*/
#include<QDebug>
#include "netizen.h"
#include "netizenProxy.h"
#include <QPixmap>
#include <QBuffer>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonValue>

Netizen::Netizen():NetizenInterface{0}{}

Netizen::Netizen(QString id, QString psw,  QString nickname, QString profile):NetizenInterface{id},m_password(psw),m_nickName{nickname},m_profileImage{profile}
{

}

void Netizen::addNote(QString noteId, NoteProxy &&note)
{
    m_pulishNoteList.emplace(noteId,std::move(note));
}
void Netizen::addFan(QString fanId, NetizenProxy &&fan)
{
    m_fanList.emplace(fanId,std::move(fan));
}

void Netizen::addConcern(QString concernId, NetizenProxy &&concern)
{
    m_concernList.emplace(concernId,std::move(concern));
}
void Netizen::addFootMark(QString checkNoteId, NoteProxy &&checkNote)
{
    m_footMarkList.emplace(checkNoteId, std::move(checkNote));
}

void Netizen::getNoteList(QList<QString> &notesId)
{
    for(std::unordered_map<QString, NoteProxy>::iterator note=m_pulishNoteList.begin();note != m_pulishNoteList.end();note++)
    {
        notesId.push_back(note->first);
    }
}

void Netizen::getFootmarkList(QList<QString> &notesId)
{
    for(std::unordered_map<QString, NoteProxy>::iterator note=m_footMarkList.begin();note != m_footMarkList.end();note++)
    {
        notesId.push_back(note->first);
    }
}

void Netizen::getConcernList(QList<QString> &netizensId)
{
    for(std::unordered_map<QString, NetizenProxy>::iterator netizen=m_concernList.begin();netizen != m_concernList.end();netizen++)
    {
        netizensId.push_back(netizen->first);
    }
}

void Netizen::getFanList(QList<QString> &netizensId)
{
    for(std::unordered_map<QString, NetizenProxy>::iterator netizen=m_fanList.begin();netizen != m_fanList.end();netizen++)
    {
        netizensId.push_back(netizen->first);
    }
}

void Netizen::getInfo(QString &netizenid, QString &password, QString &nickname, QString &profile) const
{
    netizenid=id();
    password=m_password;
    nickname=m_nickName;
    profile=m_profileImage;
}

//笔记缩略图需要的网民信息
QJsonObject Netizen::getAbstract()
{
    QPixmap pixMap;
    pixMap.load(m_profileImage); //根据id查找图片路径
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixMap.save(&buffer, "jpg", 1);
    buffer.close();

    QJsonObject netizenJson;
    netizenJson={
        {"netizenId",id()},
        {"nickname",m_nickName},
        {"profile",QJsonValue::fromVariant(bytes.toBase64())}

    };
    return netizenJson;
}

//个人主页信息详情
QJsonObject Netizen::getDetails()
{

}




