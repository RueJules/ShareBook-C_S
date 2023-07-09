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
            std::string data(boost::asio::buffers_begin(dataReceived.data()),       boost::asio::buffers_end(dataReceived.data()));
            QByteArray recArray(data.c_str());
            //            dataReceived.commit(bytesFromClient);
            //            std::cout << "server deal the request----" << "size:" << bytesFromClient << "\n";
            //            std::string wel = "Welcome Client\n";
            //            const char* data = boost::asio::buffer_cast<const char*>(dataReceived.data());
            //            QByteArray recArray(data);
            qDebug() << recArray;
            QJsonDocument fstDoc= QJsonDocument::fromJson(recArray);
            QJsonObject fstObj = fstDoc.object();
            qDebug()<<fstObj["function"];
            QByteArray resArray;

            resArray.clear();

            if(fstObj["function"] == "login"){
                qDebug() << "登录\n";
                QJsonDocument jDoc(_control->matchLoginInfo(QByteArray::fromRawData(data.c_str(),bytesFromClient)));
                resArray = jDoc.toJson();
                resArray.append('\r');  //添加分隔符
                dataReceived.consume(bytesFromClient);
                doWrite(resArray);

            }
            if(fstObj["function"] == "view"){
                qDebug() << "得到更多笔记\n";
                resArray = _control->dealRequestRecommendNote(QByteArray::fromRawData(data.c_str(),bytesFromClient));
                dataReceived.consume(bytesFromClient);
                doWrite(resArray);
            }
            //qDebug() << resArray;
        }else{
            std::cout << "没东西\n";
        }

    });

}
void BoostNetwork::doWrite(QByteArray &result)
{
    qDebug() <<"准备发了！！！！" <<result <<'\n';

    dataToSend = boost::asio::buffer(result.data(), result.size());
    auto self(shared_from_this());
    m_socket.async_write_some(dataToSend, [this, self](const boost::system::error_code &e, std::size_t bytes){
        qDebug() << "ok\n";
        qDebug() <<bytes<<'\n';
        sleep(5);
        doRead();
    });
}

