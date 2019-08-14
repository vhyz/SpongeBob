#ifndef SPONGEBOB_TCPCLIENT_H
#define SPONGEBOB_TCPCLIENT_H

#include "CallBack.h"
#include "Connector.h"
#include "EventLoop.h"
#include "TcpConnection.h"

class TcpClient {
   public:
    TcpClient(EventLoop* loop, const InetAddress& peer);

    ~TcpClient() = default;

    void start() { connector_.start(); }

    void setMessageCallBack(MessageCallBack cb) {
        messageCallBack_ = std::move(cb);
    }

    void setConnectionCallBack(ConnectionCallBack cb) {
        connectionCallBack_ = std::move(cb);
    }

    void setWriteCompleteCallBack(ConnectionCallBack cb) {
        writeCompleteCallBack_ = std::move(cb);
    }

   private:
    void newConnection(int fd);

   private:
    EventLoop* loop_;

    const InetAddress peer_;

    Connector connector_;

    spTcpConnection conn_;

    MessageCallBack messageCallBack_;
    ConnectionCallBack connectionCallBack_;
    ConnectionCallBack writeCompleteCallBack_;
};

#endif