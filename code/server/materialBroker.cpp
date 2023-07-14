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

bool MaterialBroker::createMaterial(QString noteId, QJsonObject materialsObject)
{
    if(!materialsObject.isEmpty()){
        //图片所有uudi的集合
        QStringList keys = materialsObject.keys();

        //遍历每一个图片Json对象
        for(const auto &key:keys){
            //主键是传来的uuid,处理这个素材json对象
            qDebug() << "uuid-----"<<key << '\n';
            QJsonObject materialJ = materialsObject[key].toObject();

            //素材的二进制数据
            QByteArray imageData=QByteArray::fromBase64(materialJ["image"].toString().toUtf8());
            QImage image = QImage::fromData(imageData);
            QString filename = "/root/sharebook/materials/"+key;

            //保存到服务器本地
            image.save(filename, "jpg");

            //继续读素材顺序
            int order=materialJ["order"].toInt();

            //在服务端新建material实例
            Material material(key, filename, noteId,order);

            //加到缓存
            m_cache.addToCache(key,std::move(material));

            //判断是否加入成功
            bool yes_ = m_cache.inCache(key) ? true : false;
            qDebug() << "-------------" << yes_ << Qt::endl;
            return yes_;
        }

    }
    return false;

}

void MaterialBroker::initCache()
{
    //涉及到的表：materail
}

void MaterialBroker::sycn()
{

}




