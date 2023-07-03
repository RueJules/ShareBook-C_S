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
    QJsonDocument matchLoginInfo(QByteArray data);
    QJsonDocument dealRequestRecommendNote(std::string netizenid);
    void dealRequestPublishNote(QByteArray data);

private:


};

#endif
