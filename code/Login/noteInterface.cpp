#include "noteInterface.h"

NoteInterface:: NoteInterface(QString id):m_id(id)
{

}

QString NoteInterface::get_Id() {
    return m_id;
}
