#ifndef ZMQSUBSCRIBER_H
#define ZMQSUBSCRIBER_H

#include "3pp/cppzmq/zmq.hpp"
#include <QString>
#include <QThread>

class ZmqSubscriber : public QThread
{
    Q_OBJECT

public:
    ZmqSubscriber(const QString &addr, const QString &room) :
        address(addr), topic(room) {}

signals:
    void dataReceived(const QByteArray &data);

private:
    const QString address;
    const QString topic;

    void run() Q_DECL_OVERRIDE {
        zmq::context_t context (1);

        //  Socket to talk to server
        zmq::socket_t subscriber (context, ZMQ_SUB);
        subscriber.connect(address.toLocal8Bit().toStdString());
        subscriber.setsockopt(ZMQ_SUBSCRIBE,
                              topic.toLocal8Bit(),
                              topic.toLocal8Bit().size());

        while (true) {
            zmq::message_t msg;
            subscriber.recv(&msg);
            QByteArray data((const char *)msg.data(), msg.size());
            emit dataReceived(data);
        }
    }
};

#endif // ZMQSUBSCRIBER_H
