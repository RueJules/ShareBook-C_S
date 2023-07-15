/*Auther:Huangyihong
Date:2023.6.17*/

#ifndef MATERIALBROKER_H
#define MATERIALBROKER_H
#include "relationalBroker.h"
#include <QPixmap>
#include "materialProxy.h"

class MaterialBroker : public RelationalBroker
{
public:
    ~MaterialBroker();
    static  std::shared_ptr<MaterialBroker> getInstance();
    Material *findById(QString materialId);
    bool createMaterial(QString noteId, QJsonObject materialObject);
    void initCache();//同步数据库，把数据库的内容写到cache里
    void sycn();//同步数据库，把cache的内容写回数据库

private:
    MaterialBroker();//私有构造函数，单例模式

    static std::shared_ptr<MaterialBroker> s_materialBroker;  //代管者实例
    static std::mutex materialBrokerMutex;


    Cache<Material> new_cache;
    Cache<Material> oldClean_cache;
    Cache<Material> oldDirty_cache;
    Cache<Material> oldDelete_cache;
};

#endif // MATERIALBROKER_H
