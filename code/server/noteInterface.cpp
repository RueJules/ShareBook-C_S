#include "noteInterface.h"

NoteInterface:: NoteInterface(QString id):m_id(id)
{

}

QString NoteInterface::get_Id() const
{
    return m_id;
}
