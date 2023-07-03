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
    boost::asio::async_read_until(m_socket, dataReceived, '\r', [this, self](const boost::system::error_code &e, std::size_t bytesFromClient){
        if(!e){
            dataReceived.commit(bytesFromClient);
            std::cout << "server deal the request----" << "size:" << bytesFromClient << "\n";
           // std::string wel = "Welcome Client\n";
            const char* data = boost::asio::buffer_cast<const char*>(dataReceived.data());
            QByteArray recArray(data);
            qDebug() << recArray;
            QJsonDocument fstDoc= QJsonDocument::fromJson(recArray);
            QJsonObject fstObj = fstDoc.object();
            QByteArray resArray;

            if(fstObj["function"] == "login"){
                qDebug() << "登录\n";
                QJsonDocument jDoc(_control->matchLoginInfo(QByteArray::fromRawData(data,bytesFromClient)));
                resArray = jDoc.toJson();
                resArray.append('\r');  //添加分隔符
            }
            if(fstObj["funtion"] == "view"){
                qDebug() << "得到更多笔记\n";
                QJsonDocument jDoc(_control->dealRequestRecommendNote(QByteArray::fromRawData(data,bytesFromClient)));
                resArray = jDoc.toJson();
                resArray.append('\r');  //添加分隔符
            }
            //qDebug() << resArray;
            dataReceived.consume(bytesFromClient);
            doWrite(resArray);
        }else{
            std::cout << "没东西\n";
        }

    });

}

void BoostNetwork::doWrite(QByteArray &result)
{
    //qDebug() << result;
    dataToSend = boost::asio::buffer(result.data(), result.size());
    auto self(shared_from_this());
    m_socket.async_write_some(dataToSend, [this, self](const boost::system::error_code &e, std::size_t bytes){
        std::cout << "ok\n";
        doRead();
    });
}

