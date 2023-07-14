/*Auther:Huangyihong
Date:2023.6.17*/

#ifndef MATERIALPROXY_H
#define MATERIALPROXY_H

#include"material.h"
#include "materialInterface.h"

class MaterialProxy: public MaterialInterface
{
public:
    MaterialProxy(QString id);
    MaterialProxy(QString id,std::unique_ptr<Material> &&material);
    QJsonObject getDetails()override;

private:
    std::unique_ptr<Material> m_material; //持有一个素材实例
};

#endif // MATERIALPROXY_H
