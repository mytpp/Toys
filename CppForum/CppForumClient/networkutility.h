#ifndef NETWORKUTILITY_H
#define NETWORKUTILITY_H

#include <utility>
#include <memory>
#include <QTcpSocket>

class QString;
class QByteArray;

namespace network {
    typedef std::unique_ptr<QTcpSocket, std::function<void(QTcpSocket*)>> SockPtr;

    extern uint16_t port;

    void sockDeleter(QTcpSocket* sock);
    std::pair<QString, QString> ParseStatusLine(QByteArray data);
}

#endif // NETWORKUTILITY_H
