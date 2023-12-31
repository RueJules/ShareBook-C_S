 /*
 * datetime:2023-6-17
 * author:chenzexi
*/
#ifndef CONTROL_H
#define CONTROL_H
#include <vector>
#include <QObject>
#include <QJsonDocument>
class Control:public QObject{
    Q_OBJECT

public:
    QJsonDocument matchLoginInfo(QByteArray &data);
    QByteArray dealRequestRecommendNote(QByteArray &data);
    QByteArray dealRequestNoteDetail(QByteArray &data);
    QByteArray dealRequestPublishNote(QByteArray &data);
    QByteArray dealRequestPublishComment(QByteArray &data);
    QByteArray dealRequestComments(QByteArray &data);
    QByteArray dealRequestViewVideos(QByteArray &data);

private:


};

#endif
