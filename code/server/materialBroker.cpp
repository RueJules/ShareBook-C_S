#include "materialBroker.h"
#include "materialProxy.h"
#include <QByteArray>
#include <QBuffer>
#include <QDebug>
#include <QJsonDocument>
#include <mutex>
#include <memory>

std::shared_ptr<MaterialBroker> MaterialBroker::s_materialBroker=nullptr;
std::mutex MaterialBroker::materialBrokerMutex;

MaterialBroker::~MaterialBroker()
{

}
MaterialBroker::MaterialBroker()
{

}
std::shared_ptr<MaterialBroker> MaterialBroker::getInstance()
{
    //可以用call_once和once_flag替换
    if (s_materialBroker == nullptr) {
        std::unique_lock<std::mutex> lock(materialBrokerMutex);
        if (s_materialBroker == nullptr) {
            auto temp = std::shared_ptr<MaterialBroker>(new MaterialBroker());
            s_materialBroker = temp;
        }
    }
    return s_materialBroker;
}

Material *MaterialBroker::findById(QString materialId)
{
    if(m_cache.inCache(materialId)){
        return &m_cache.getFromCache(materialId);
    }
    else{
        //从数据库中找
        std::string cmd="select * from material where id=\""+materialId.toStdString()+"\"";
        sql::ResultSet *set =query(cmd);
        if(set->next())
        {

            QString image=QString::fromStdString(set->getString(2).c_str());
            int order=set->getInt(3);
            QString noteId=QString::fromStdString(set->getString(4).c_str());
            Material material(materialId, image,noteId, order);
            //加到缓存
            m_cache.addToCache(materialId,std::move(material));
            return &m_cache.getFromCache(materialId);
        }

    }
    return nullptr;
}

void MaterialBroker::createMaterial(QJsonObject materialObject)
{
    if(materialObject.isEmpty()){
        QStringList keys = materialObject.keys();
        for(const auto &key:keys){
            //主键是传来的uuid
            QString materialId=key;
            //图片是二进制传过来的，先存在本地生成路径再生成实例
            QJsonDocument doc(materialObject["image"].toObject());
            QByteArray imageData=doc.toJson();
            QImage image = QImage::fromData(imageData);
            QString filename = "../"+materialId+".png";
            image.save(filename);
            //继续读其他的
            QString noteId=materialObject["noteId"].toString();
            int order=materialObject["order"].toInt();
            //在服务端新建material实例
            Material material(materialId, materialId, noteId,order);
            //加到缓存
            m_cache.addToCache(materialId,std::move(material));
        }
    }
}

void MaterialBroker::initCache()
{
    //涉及到的表：materail
}

void MaterialBroker::sycn()
{

}




