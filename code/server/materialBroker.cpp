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
    //在数据库中索取出来的cache内查找
    if(oldClean_cache.inCache(materialId)){

        return &oldClean_cache.getFromCache(materialId);

    }else{
        //新的缓存中查找
        if(new_cache.inCache(materialId))
        {
            return &new_cache.getFromCache(materialId);

        }else{
            //old_dirty_cache中找
            if(oldDirty_cache.inCache(materialId))
            {
                return &oldDirty_cache.getFromCache(materialId);

            }else{
                //从数据库中找
                qDebug() << "material cache 拦截失败\n";
//                std::string cmd="select * from material where id=\""+materialId.toStdString()+"\"";
//                sql::ResultSet *set =query(cmd);
//                if(set->next())
//                {

//                    QString image=QString::fromStdString(set->getString(2).c_str());
//                    int order=set->getInt(3);
//                    QString noteId=QString::fromStdString(set->getString(4).c_str());
//                    Material material(materialId, image,noteId, order);
//                    //加到缓存
//                    oldClean_cache.addToCache(materialId,std::move(material));
//                    return &oldClean_cache.getFromCache(materialId);
//                }
            }
        }
    }

    return nullptr;
}

bool MaterialBroker::createMaterial(QString noteId, QJsonObject materialsObject)
{

    if(!materialsObject.isEmpty()){
        //图片所有uudi的集合
        bool yes_=true;
        QStringList keys = materialsObject.keys();

        //遍历每一个图片Json对象
        for(const auto &key:keys){
            //主键是传来的uuid,处理这个素材json对象
            qDebug() << "uuid-----"<<key << '\n';
            QJsonObject materialJ = materialsObject[key].toObject();

            //素材的二进制数据
            QByteArray imageData=QByteArray::fromBase64(materialJ["image"].toString().toUtf8());
            QImage image = QImage::fromData(imageData);
            QString filename = "/root/sharebook/materials/"+key+".jpg";

            //保存到服务器本地
            image.save(filename, "jpg");

            //继续读素材顺序
            int order=materialJ["order"].toInt();

            //在服务端新建material实例
            Material material(key, filename, noteId,order);

            //加到缓存
            new_cache.addToCache(key,std::move(material));

            //判断是否加入成功
            yes_ &= new_cache.inCache(key) ? true : false;
            qDebug() << "-------------" << yes_ << Qt::endl;

        }
        return yes_;

    }
    return false;

}

void MaterialBroker::initCache()
{
    //涉及到的表：materail
    std::string cmd="select * from material ";
    sql::ResultSet *set =query(cmd);
    while(set->next())
    {
        QString materialId=QString::fromStdString(set->getString(1).c_str());
        QString image=QString::fromStdString(set->getString(2).c_str());
        int order=set->getInt(3);
        QString noteId=QString::fromStdString(set->getString(4).c_str());
        Material material(materialId, image,noteId, order);
        //加到缓存
        oldClean_cache.addToCache(materialId,std::move(material));
    }
    qDebug() << "materials init finish!\n";
}

void MaterialBroker::sycn()
{

    qDebug()<<"MaterialsBroker的同步--------------------------------------------------------------------";
    //将new_cache中新创建的（创建的，创建后没写入数据库就修改的）数据更新到数据库
    std::unordered_map<QString,Material>& nc=new_cache.getCacheConst();
    for(auto it = nc.begin(); it != nc.end(); ++it){
        QString materialId;
        QString image;
        QString note_id;
        int order;
        Material &material=it->second;
        material.getInfo(materialId,image,note_id,order);
        std::string cmd="insert ignore into material(id,image,number,note_id) value(\""+materialId.toStdString()+"\",\""+image.toStdString()+"\","+std::to_string(order)+",\""+note_id.toStdString()+"\")";
        qDebug() << cmd << '\n';
        insert(cmd);
    }
    new_cache.clearCache();
    //将oldDirty_cache中被修改的（数据库中原本存在但现在被修改）数据更新到数据库
    std::unordered_map<QString,Material>& odirtyc=oldDirty_cache.getCacheConst();
    for(auto it = odirtyc.begin(); it != odirtyc.end(); ++it){
        QString materialId;
        QString image;
        QString note_id;
        int order;
        Material material=it->second;
        material.getInfo(materialId,image,note_id,order);
        std::string cmd="update material set image=\""+image.toStdString()+"\",number="+std::to_string(order)+",note_id=\""+note_id.toStdString()+"\"where id=\""+materialId.toStdString()+"\"";
        qDebug() << cmd << '\n';
        update(cmd);
    }
    oldDirty_cache.clearCache();
    //将oldDelete_cache中被删除的（数据库中原本存在但现在被删除）数据更新到数据库
    std::unordered_map<QString,Material>& odeletec=oldDelete_cache.getCacheConst();
    for(auto it = odeletec.begin(); it != odeletec.end(); ++it){
        Material material=it->second;
        QString materialId=material.get_id();
        std::string cmd="delete from material where id=\""+materialId.toStdString()+"\"";
        qDebug() << cmd << '\n';
        drop(cmd);
    }
    oldDelete_cache.clearCache();
    //更新oldClean_cache
    initCache();

}

//void MaterialBroker::start_thread()
//{
//    m_sycn_thread=new std::thread(&MaterialBroker::sycn,this);
//}




