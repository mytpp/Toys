#ifndef FORUMSERVER_H
#define FORUMSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <cstdint>

class ForumServer: public QTcpServer
{
    Q_OBJECT
public:
    enum Method: uint8_t {
        GET,
        POST,
        PUT,
        DELTET
    };

public:
    ForumServer(int port = 0, QObject *parent = 0);

protected:
    virtual void incomingConnection(qintptr socketDescriptor) override;

public slots:
    void SlotDisconnected();
    void EchoRequest();

private:
    void EchoUserInfoRequest(QTcpSocket *sock, Method method);
    void EchoBoardRequest(QTcpSocket *sock, Method method);
    void EchoPostRequest(QTcpSocket *sock, Method method);
    void EchoCommentRequest(QTcpSocket *sock, Method method);

private:
    QList<QTcpSocket*> sockList;
};

#endif // FORUMSERVER_H
