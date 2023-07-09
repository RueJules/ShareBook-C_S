/*
 * datetime:2023-6-17
 * author:chenzexi
*/
#include "netizenProxy.h"
#include "netizen.h"

NetizenProxy::NetizenProxy():NetizenInterface{0}
{

}

NetizenProxy::NetizenProxy(QString id):NetizenInterface{id},m_netizen{nullptr}
{

}

NetizenProxy::NetizenProxy(QString id, std::unique_ptr<Netizen>&&netizen):NetizenInterface{id},m_netizen{std::move(netizen)}
{

}

void NetizenProxy::addNote(QString noteId, NoteProxy &&note)
{
    m_netizen->addNote(noteId,std::move(note));
}

void NetizenProxy::addFan(QString fanId, NetizenProxy &&fan)
{
    m_netizen->addFan(fanId,std::move(fan));
}

void NetizenProxy::addConcern(QString concernId, NetizenProxy &&concern)
{
    m_netizen->addConcern(concernId,std::move(concern));
}
void NetizenProxy::addFootMark(QString checkNoteId, NoteProxy &&checkNote)
{
    m_netizen->addFootMark(checkNoteId, std::move(checkNote));
}

QJsonObject NetizenProxy::getAbstract()
{

}

QJsonObject NetizenProxy::getDetails()
{

}





