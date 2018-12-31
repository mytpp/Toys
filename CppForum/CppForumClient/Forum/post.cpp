#include "post.h"
#include "User/user.h"
#include <QDate>
#include "Forum/forum.h"
#include "networkutility.h"
#include <QJsonDocument>

Post::Post(const QString& id, const QString& author,
           const QString& authorId, const QString& title,
           const QString& content, const QDate birthday,
           const int commentsNum)
    :id(id)
    ,author(author)
    ,authorId(authorId)
    ,title(title)
    ,content(content)
    ,birthday(birthday)
    ,commentsNum(commentsNum)
{

}

const std::list<Comment> Post::Comments() const {
    std::list<Comment> comments;
    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    request += "GET comment\n\n";
    QVariantMap params;
    params["post"] = this->Id();
    request += QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);
    if (sock->write(request) == -1) {
        qDebug()<<"Can't write 'GET comment' request.";
        throw std::runtime_error("Network error: Can't retrive comments.");
    }
    if(!sock->waitForReadyRead()) {
        qDebug()<<"Can't get 'GET comment' response.";
        throw std::runtime_error("Network error: Can't retrive comments.");
    }
    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if(code == "200") {
        sock->readLine(1024);              // read separator
        QByteArray data = sock->readAll(); // get response
        QVariantList rawComments = QJsonDocument::fromJson(data).toVariant().toList();
        for (auto &rawComment: rawComments) {
            auto comment = rawComment.toMap();
            comments.emplace_back(
                           comment["author"].toString(),
                           comment["authorId"].toString(),
                           comment["content"].toString(),
                           QDate::fromString(comment["date"].toString())
                       );
        }
    } else {
        throw std::runtime_error("Network error: Can't retrive comments.");
    }

    return comments;
}

bool Post::AddComment(const QString &content) {
    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    request += "POST comment\n\n";
    QVariantMap params;
    params["post"] = this->Id();
    params["user"] = User::Get()->Id();
    params["board"] = Forum::Get().GetCurBoard().Name();
    params["content"] = content;
    request += QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);

    if (sock->write(request) == -1) {
        qDebug()<<"Can't write 'POST comment' request.";
        return false;
    }
    if(!sock->waitForReadyRead()) {
        qDebug()<<"Can't get 'POST comment' response.";
        return false;
    }
    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if(code == "200") {
        sock->readLine(1024);              // read separator
    } else {
        return false;
    }

    return true;
}
