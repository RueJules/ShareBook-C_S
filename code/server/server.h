#ifndef SERVER_H
#define SERVER_H
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include"boostnetwork.h"

using namespace boost::asio;



class Server
{
public:
    Server();
    void startAccept();
    void startCommunicate();
private:
    tcp::acceptor m_acc;
    static io_service service_2001;
    static io_service service_9999;



    //保证service.run不会结束
    boost::shared_ptr<io_service::work> work;
    boost::thread_group threads;
    //std::vector<std::shared_ptr<BoostNetwork>> sockets;

};

#endif // SERVER_H
