#include "note.h"
#include <materialBroker.h>
#include "netizen.h"
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
    //需要在这里找到笔记的博主信息，第一张素材的信息和笔记的标题时间


    Netizen *netizen = NetizenBroker::getInstance()->findById(m_bloggerId);
    QJsonObject netizenJson = netizen->getAbstract();
    QJsonObject materialJson = m_materialList.begin()->second.getDetails();

    QString id = get_Id();

    noteAbs = { { get_Id(), QJsonObject{ {"bloggerNickname", "Lana"}, {"bloggerProfile",""}, {"time","2023-7-3"} } } };

    //noteAbs.insert(m_id, netizenJson);
    //noteAbs.insert(m_id, materialJson);

    return noteAbs;

}




