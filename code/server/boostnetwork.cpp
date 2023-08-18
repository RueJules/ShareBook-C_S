#include "boostnetwork.h"
#include <iostream>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "server.h"
#include "materialBroker.h"
using namespace boost::asio;

BoostNetwork::BoostNetwork(tcp::socket s):m_socket(std::move(s))
{
    _control = new Control;
}

void BoostNetwork::start()
{
    doRead();
}

void BoostNetwork::doRead()
{
    auto self(shared_from_this());
    boost::asio::async_read_until(m_socket, dataReceived, '\r', [this, self](const boost::system::error_code &e, std::size_t bytes){
        if(!e){

            std::istream is(&dataReceived);
            QByteArray recArray;
            recArray.resize(bytes);
            is.read(recArray.data(), static_cast<std::streamsize>(recArray.size()));

            qDebug() << recArray.size() <<Qt::endl;

            //接收到的数据转换为Json对象
            QJsonDocument receiveDoc= QJsonDocument::fromJson(recArray);
            QJsonObject receiveObj = receiveDoc.object();

            //服务器处理后准备送回去的结果
            QByteArray resultArray;
            resultArray.clear();

            //处理请求
            if(receiveObj["function"] == "login")
            {
                qDebug() << "登录\n";
                QJsonDocument jDoc(_control->matchLoginInfo(recArray));
                resultArray = jDoc.toJson();
                resultArray.append('\r');  //添加分隔符
                doWrite(resultArray);
            }
            if(receiveObj["function"] == "view")
            {
                qDebug() << "得到更多笔记\n";
                resultArray = _control->dealRequestRecommendNote(recArray);
                doWrite(resultArray);
            }
            if(receiveObj["function"] == "check")
            {
                qDebug() << "查看笔记详情\n";
                resultArray = _control->dealRequestNoteDetail(recArray);
                doWrite(resultArray);
            }
            if(receiveObj["function"] == "publish")
            {
                qDebug() << "发布笔记\n";
                if(receiveObj["materialType"] == "video")
                {
                    //把视频数据通过另一个套接字发送给流媒体服务器
                    QStringList keys=receiveObj["materials"].toObject().keys();
                    std::string video=receiveObj["materials"][keys[0]]["material"].toString().toStdString();
                    std::string post = receiveObj["postfix"].toString().toStdString();
                    std::string k = keys[0].toStdString() + post;
                    std::string data = k + video;
                    Server::videoData.push(data);
                    receiveObj["materials"].toObject()[keys[0]].toObject()["material"]="";

                }
                resultArray = _control->dealRequestPublishNote(recArray);
                doWrite(resultArray);
            }
            if(receiveObj["function"] == "publish_comment")
            {
                qDebug() << "发表评论\n";
                resultArray = _control->dealRequestPublishComment(recArray);
                doWrite(resultArray);
            }
            if(receiveObj["function"] == "check_comment")
            {
                qDebug() << "加载一些评论\n";
                resultArray = _control->dealRequestComments(recArray);
                doWrite(resultArray);

             }
            if(receiveObj["function"] == "view_video")
            {
                qDebug() << "查看一些视频\n";
//                resultArray = _control->dealRequestViewVideos(recArray);
//                doWrite(resultArray);
                std::string res;
                QJsonObject resObj;
//                if(Server::path.try_pop(res))
//                {
//                    QString p = QString::fromStdString(res);
//                    resObj = {
//                        {"video", p},
//                        {"function", "view_video"}
//                    };

//                }else{
//                    resObj = {
//                        {"video", "服务器没有视频资源"},
//                        {"function", "view_video"}
//                    };
//                }
                resObj = {
                    {"video", "http://10.252.76.34/hls/c65703585e7145c3b34ddce0a76b3bc6.m3u8"},
                    {"function", "view_video"}
                };
                QJsonDocument jdoc;
                jdoc.setObject(resObj);
                QByteArray rrr = jdoc.toJson();
                rrr.append('\r');
                doWrite(rrr);
            }
        }else{
            std::cout << "没东西\n";
        }

    });

}
void BoostNetwork::doWrite(QByteArray &result)
{
    qDebug() <<"准备发了！！！！" <<result.size() <<'\n';

    dataToSend = boost::asio::buffer(result.data(), result.size());
    auto self(shared_from_this());
    boost::asio::async_write(m_socket, dataToSend, [this, self](const boost::system::error_code &e, std::size_t bytes){
        qDebug() << "ok\n";
        qDebug() <<bytes<<'\n';
        //sleep(5);
        doRead();
    });
}


