#include "boostnetwork.h"
#include <iostream>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
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

            qDebug() << recArray << recArray.size() <<Qt::endl;

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
            if(receiveObj["function"] == "check_reply")
            {

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


