#include "netizenInterface.h"

NetizenInterface::NetizenInterface(QString id):m_id(id)
{

}

QString NetizenInterface::id() const
{
    return m_id;
}
