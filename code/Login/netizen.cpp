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

Netizen::Netizen(QString id,  QString nickname, QString profile):NetizenInterface{id},m_nickName{nickname},m_profileImage{profile}
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

QJsonObject Netizen::getAbstract()
{
    QPixmap pixMap(""); //根据id查找图片路径
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixMap.save(&buffer, "JPG");

    QJsonObject netizenJson;
    netizenJson={
        {"nickname",m_nickName},
        {"profile",QJsonValue::fromVariant(bytes.toBase64())}

        };
    return netizenJson;
}

QJsonObject Netizen::getDetails()
{

}




