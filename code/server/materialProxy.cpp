/*Auther:Huangyihong
Date:2023.6.17*/

#include "materialProxy.h"
#include "materialBroker.h"


MaterialProxy::MaterialProxy(QString id):MaterialInterface{id},m_material{nullptr}
{

}
MaterialProxy::MaterialProxy(QString id, std::unique_ptr<Material>&&material):MaterialInterface{id},m_material{std::move(material)}
{

}

QJsonObject MaterialProxy::getDetails()
{
    if(m_material == nullptr){
        Material *material =  MaterialBroker::getInstance()->findById(get_id());
        m_material = std::make_unique<Material>(*material);
    }
    return m_material->getDetails();
}


