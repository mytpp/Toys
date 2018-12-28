#include "networkutility.h"
#include <QString>

namespace network {

uint16_t port = 9999;

void sockDeleter(QTcpSocket *sock) {
    sock->disconnectFromHost();
    delete sock;
    qDebug()<<"disconnect a tcp socket";
}

std::pair<QString, QString> ParseStatusLine(QByteArray data) {
    QString statusLine = QString::fromUtf8(data).trimmed();
    qDebug()<<"Receive status line: "<<statusLine;
    auto elements = statusLine.split(' ');
    return { elements[0], elements[1] };
}

}
