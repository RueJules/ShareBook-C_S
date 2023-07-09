#include "commentinterface.h"

CommentInterface::CommentInterface(QString new_id):m_id{new_id}
{

}
QString CommentInterface::get_id()
{
    return m_id;
}
