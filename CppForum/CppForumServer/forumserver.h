#ifndef FORUMSERVER_H
#define FORUMSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <cstdint>

class ForumServer: public QTcpServer
{
    Q_OBJECT
public:
    ForumServer(int port = 0, QObject *parent = 0);

protected:
    virtual void incomingConnection(qintptr socketDescriptor) override;
};

#endif // FORUMSERVER_H
