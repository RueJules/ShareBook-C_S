#include "noteProxy.h"
#include "noteBroker.h"


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
    if(m_note == nullptr){
        Note *note = NoteBroker::getInstance()->findById(get_Id());
        m_note = std::make_unique<Note>(std::move(*note)); //因为note里边维护了素材代理的列表，而每个代理中有素材实例的指针，所以这里不能直接使用*note生成智能指针
    }
    return m_note->getNoteAbstract();
}

QJsonObject NoteProxy::getNoteDetails()
{
    if(!m_note){
        Note *note = NoteBroker::getInstance()->findById(get_Id());
        m_note = std::make_unique<Note>(std::move(*note));
    }
    return m_note->getNoteDetails();
}
