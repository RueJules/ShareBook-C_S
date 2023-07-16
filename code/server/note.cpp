#include "note.h"
#include <materialBroker.h>
#include "netizen.h"
#include <QJsonValue>
#include <QJsonObject>
#include <netizenBroker.h>

//Note::Note(QString id, QString title, QString content, QDateTime time, int materialCount, QString blogger):NoteInterface{id},m_title{title},m_content{content},m_time{time},m_materials{materialCount},m_bloggerId{blogger},m_commentList{std::move(comments)}
//{}
Note::Note(QString id, QString title, QString content, QDateTime time, int materialCount, QString blogger):NoteInterface{id},m_title{title},m_content{content},m_time{time},m_materials{materialCount},m_bloggerId{blogger}
{}

void Note::addMaterial(int order, MaterialProxy &&material)
{
    m_materialList.emplace(order,std::move(material));
}

QJsonObject Note::getNoteAbstract()
{
    QJsonObject noteAbs;

    //需要在这里找到笔记的博主信息(头像、昵称)，第一张素材的信息(顺序，图片数据)和笔记的标题、时间
    Netizen *netizen = NetizenBroker::getInstance()->findById(m_bloggerId);
    QJsonObject netizenJson = netizen->getAbstract();
    QJsonObject materialJson;
    if(!m_materialList.empty())
    {
        materialJson = m_materialList.begin()->second.getDetails();
    }


    //QString id = get_Id();
    noteAbs = {
               {"title", m_title},{"time",m_time.toString("yyyy-MM-dd hh:mm:ss")}
        };

   // QJsonObject noteJ = noteAbs[id].toObject();
    noteAbs.insert("FirstImg", QJsonValue(materialJson));
    noteAbs.insert("Blogger", QJsonValue(netizenJson));



    //qDebug() << "JsonObject in QJsonObject Note::getNoteAbstract()"<<noteAbs << '\n';

    return noteAbs;
}

QJsonObject Note::getNoteDetails()
{
    QJsonObject noteDetails = {
        {"content", m_content}
    };

    //查询所有的素材，每一个去查询一次，但是不需要第一张了
    QJsonObject materials;
    std::map<int,MaterialProxy>::iterator it = m_materialList.begin();
    for(++it; it != m_materialList.end();it++)
    {
        QJsonObject materialJ = it->second.getDetails();
        QString id = it->second.get_id();
        materials.insert(id, materialJ[id]);
    }
    noteDetails.insert("materials", QJsonValue(materials));
    noteDetails.insert("function", "check");
    return noteDetails;
}

QJsonObject Note::toDB()
{
    QJsonObject note = {
        {"id",get_Id()},
        {"title",m_title},
        {"content",m_content},
        {"materials",m_materials},
        {"time",m_time.toString("yyyyMMddhhmmss")},
        {"blogger",m_bloggerId}
    };
    return note;
}

void Note::addComment(QString commentId)
{
    m_commentList.push_front(commentId);
}




