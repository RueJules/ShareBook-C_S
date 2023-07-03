/*Auther:Huangyihong
Date:2023.6.17*/

#include "materialInterface.h"

MaterialInterface::MaterialInterface(QString new_id):m_id(new_id)
{

}

QString MaterialInterface::get_id()
{
    return m_id;
}


