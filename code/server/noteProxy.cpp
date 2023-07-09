#include "noteProxy.h"

NoteProxy::NoteProxy(QString id):NoteInterface{id},m_note{nullptr}
{

}

NoteProxy::NoteProxy(QString id, std::unique_ptr<Note>&&note):NoteInterface{id},m_note{std::move(note)}
{

}
void NoteProxy::addMaterial(int order, MaterialProxy &&material)
{
    m_note->addMaterial(order,std::move(material));
}

QJsonObject NoteProxy::getNoteAbstract()
{
    return m_note->getNoteAbstract();
}
