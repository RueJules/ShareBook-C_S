/*Auther:GongYanxian
Date:2023.6.19*/
#ifndef NOTEPROXY_H
#define NOTEPROXY_H
#include "note.h"

class NoteProxy : public NoteInterface {

public:

    NoteProxy(QString id);
    NoteProxy(QString id,std::unique_ptr<Note>&& note);
    void addMaterial(int order,MaterialProxy &&material)override;
    QJsonObject getNoteAbstract() override;
    QJsonObject getNoteDetails() override;

private:
     std::unique_ptr<Note> m_note;
};

#endif
