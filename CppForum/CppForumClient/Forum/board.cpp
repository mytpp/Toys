#include "board.h"
#include "User/user.h"
#include "networkutility.h"
#include <QJsonDocument>
#include <QDate>
#include <iterator>
#include <algorithm>


Board::Board(const QString &name, const QString &moderatorId)
    :name(name)
    ,moderatorId(moderatorId)
{ }


Post& Board::AddPost(const QString &title, const QString &content){
    auto& userId   = User::Get()->Id();
    auto& userName = User::Get()->Name();
    auto  now      = QDate::currentDate();
    QString postId;

    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    request = "POST post\n\n";
    QVariantMap params;
    params["board"] = name;
    params["user"] = userId;
    params["title"] = title;
    params["content"] = content;
    request += QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);
    if (sock->write(request) == -1) {
        qDebug()<<"Can't write 'POST post' request.";
        throw std::runtime_error("Network error: Can't post a post.");
    }
    if(!sock->waitForReadyRead()) {
        qDebug()<<"Can't get 'POST post' response.";
        throw std::runtime_error("Network error: Can't post a post.");
    }
    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if(code == "200") {
        sock->readLine(1024);              // read separator
        QByteArray data = sock->readAll(); // get response
        postId = QJsonDocument::fromJson(data).toVariant().toMap()["id"].toString();
    } else {
        throw std::runtime_error("Network error: Can't post a post.");
    }

    User::Get()->AddPost();
    auto retVal = posts.emplace(postId, Post{
                    postId,
                    userName,
                    userId,
                    title,
                    content,
                    now
                }
            );
    return retVal.first->second;
}

bool Board::DeletePost(const QString id) {
    auto it = posts.find(id);
    if(it == posts.end())
        return false;

    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    request = "DELETE post\n\n";
    QVariantMap params;
    params["user"] = User::Get()->Id();
    params["post"] = id;
    request += QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);
    if (sock->write(request) == -1) {
        qDebug()<<"Can't write 'DELETE post' request.";
        return false;
    }
    if(!sock->waitForReadyRead()) {
        qDebug()<<"Can't get 'DELETE post' response.";
        return false;
    }
    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if(code == "200") {
        sock->readLine(1024);              // read separator
    } else {
        return false;
    }

    if(it->second.AuthorId() == User::Get()->Id()) {
        User::Get()->DeletePost();
    }
    posts.erase(it);
    return true;
}
