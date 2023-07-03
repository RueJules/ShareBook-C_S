#include "note.h"
#include <materialBroker.h>
#include "netizen.h"
#include <QJsonValue>
#include <netizenBroker.h>

Note::Note(QString id, QString title, QString content, QDateTime time, int materialCount, QString blogger):NoteInterface{id},m_title{title},m_content{content},m_time{time},m_materials{materialCount},m_bloggerId{blogger}
{}


//void Note::addMaterial(QString materialId, MaterialProxy &&material)
//{
//    m_materialList.emplace(materialId,std::move(material));
//}
void Note::addMaterial(int order, MaterialProxy &&material)
{
    m_materialList.emplace(order,std::move(material));
}

QJsonObject Note::getNoteAbstract()
{
    QJsonObject noteAbs;

    //需要在这里找到笔记的博主信息(头像、昵称)，第一张素材的信息(顺序，图片数据)和笔记的标题时间
    Netizen *netizen = NetizenBroker::getInstance()->findById(m_bloggerId);
    QJsonObject netizenJson = netizen->getAbstract();
    QJsonObject materialJson = m_materialList.begin()->second.getDetails();

    QString id = get_Id();
    noteAbs = {
               { id, QJsonValue(QJsonObject{ {"title", m_title},{"time",m_time.toString("yyyyMMddhhmmss")}})}
        };

    QJsonObject noteJ = noteAbs[id].toObject();
    noteJ.insert("FirstImg", QJsonValue(netizenJson));
    noteJ.insert("Blogger", QJsonValue(materialJson));
    noteAbs.insert(id, noteJ);

    return noteAbs;
}




