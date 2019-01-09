#include "forum.h"
#include "networkutility.h"
#include "User/user.h"
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <QDate>
#include <QVector>
#include <QJsonDocument>
#include <QDebug>

Forum::Forum()
{
    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    request += "GET board\n\n";
    if (sock->write(request) == -1) {
        qDebug()<<"Can't write 'GET board' request.";
        throw std::runtime_error("Network error: Can't retrive board names.");
    }
    if(!sock->waitForReadyRead()) {
        qDebug()<<"Can't get 'GET board' response.";
        throw std::runtime_error("Network error: Can't retrive board names.");
    }

    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if(code == "200") {
        sock->readLine(1024);              // read separator
        QByteArray data = sock->readAll(); // get response
        QVariantList boards = QJsonDocument::fromJson(data).toVariant().toList();
        for(auto board: boards) {
            boardNames.push_back(board.toMap()["name"].toString());
        }
    } else {
        throw std::runtime_error("Network error: Can't retrive board names.");
    }
}

Forum::~Forum() {}

void Forum::Show() {
    SetCurBoard(boardNames[0]);//navigate to the first board
    emit ReadyToDraw();
}

void Forum::Hide() {
    emit HideForum();
}

void Forum::SetCurBoard(const QString& name) {
    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    //get current board moderator
    request = "GET board\n\n";
    if (sock->write(request) == -1) {
        qDebug()<<"Can't write 'GET board' request.";
        throw std::runtime_error("Network error: Can't retrive board names.");
    }
    if(!sock->waitForReadyRead()) {
        qDebug()<<"Can't get 'GET board' response.";
        throw std::runtime_error("Network error: Can't retrive board names.");
    }
    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if(code == "200") {
        sock->readLine(1024);              // read separator
        QByteArray data = sock->readAll(); // get response
        QVariantList boards = QJsonDocument::fromJson(data).toVariant().toList();
        for(auto rawBoard: boards) {
            auto board = rawBoard.toMap();
            if(board["name"] == name){
                curBoard.reset(new Board(name, board["moderator"].toString()));
            }
        }
    } else {
        throw std::runtime_error("Network error: Can't retrive board names.");
    }
    if(!curBoard) {
        throw std::runtime_error("No board detected.");
    }

    //get post
    sock.reset(new QTcpSocket);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    request = "GET post\n\n";
    QVariantMap params;
    params["board"] = name;
    request += QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);
    if (sock->write(request) == -1) {
        qDebug()<<"Can't write 'GET post' request.";
        throw std::runtime_error("Network error: Can't retrive posts.");
    }
    if(!sock->waitForReadyRead()) {
        qDebug()<<"Can't get 'GET post' response.";
        throw std::runtime_error("Network error: Can't retrive posts.");
    }
    auto [_code, _message] = network::ParseStatusLine(sock->readLine(1024));
    if(_code == "200") {
        sock->readLine(1024);              // read separator
        QByteArray data = sock->readAll(); // get response
        QVariantList posts = QJsonDocument::fromJson(data).toVariant().toList();
        for (auto& rawPost: posts) {
            auto post = rawPost.toMap();
            qDebug()<<QDate::fromString(post["date"].toString());
            curBoard->AddInitialPost({
                             post["id"].toString(),
                             post["author"].toString(),
                             post["authorId"].toString(),
                             post["title"].toString(),
                             post["content"].toString(),
                             QDate::fromString(post["date"].toString()),
                             post["comment"].toInt()
                     });
        }
    } else {
        throw std::runtime_error("Network error: Can't retrive posts.");
    }
}

bool Forum::AssignModerator(const QString& id) {
    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    request = "PUT user\n\n";
    QVariantMap params;
    params["admin"] = User::Get()->Id();
    params["id"] = id;
    params["status"] = 2;
    params["board"] = this->GetCurBoard().Name();
    request += QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);

    if (sock->write(request) == -1) {
        qDebug()<<"Writing AssignModerator request failed.";
        return false;
    }
    if(!sock->waitForReadyRead()){
        qDebug()<<"Can't get AssignModerator response.";
        return false;
    }

    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if (code == "200") {
        sock->readLine(1024);              // read separator
    } else {
        return false;
    }

    this->GetCurBoard().SetModerator(id);
    return true;
}

bool Forum::DismissModerator() {
    auto id = this->GetCurBoard().ModeratorId();
    if(id.isEmpty())
        return true;

    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    request = "PUT user\n\n";
    QVariantMap params;
    params["admin"] = User::Get()->Id();
    params["id"] = id;
    params["status"] = 1;
    params["board"] = this->GetCurBoard().Name();
    request += QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);

    if (sock->write(request) == -1) {
        qDebug()<<"Writing DismissModerator request failed.";
        return false;
    }
    if(!sock->waitForReadyRead()){
        qDebug()<<"Can't get DismissModerator response.";
        return false;
    }
    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if (code == "200") {
        sock->readLine(1024);              // read separator
    } else {
        return false;
    }

    this->GetCurBoard().SetModerator(""); //empty str means no moderator
    return true;
}


Forum& Forum::Get() {
    static Forum _forum;
    return _forum;
}

infrastructure::Response Forum::Verify(QString id, QString password){
    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    request = "GET user\n\n";
    QVariantMap params;
    params["id"] = id;
    params["password"] = password;
    request += QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);

    if (sock->write(request) == -1) {
        qDebug()<<"Writing login request failed.";
        return {infrastructure::NETWORK_ERROR};
    }
    if(!sock->waitForReadyRead()){
        qDebug()<<"Can't get login response.";
        return {infrastructure::NETWORK_ERROR};
    }

    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if (code == "200") {
        sock->readLine(1024);              // read separator
        QByteArray data = sock->readAll(); // get response
        QVariantMap userInfo = QJsonDocument::fromJson(data).toVariant().toMap();
        return {
            infrastructure::SUCCESS,
            static_cast<infrastructure::Status>(userInfo["status"].toInt()),
            static_cast<uint32_t>(userInfo["post"].toInt()),
            userInfo["name"].toString()
        };
    } else if (message == "Id_Not_Found") {
        return { infrastructure::ID_NOT_FOUNTD };
    } else if (message == "Wrong_Password") {
        return { infrastructure::WRONG_PASSWORD };
    } else if (message == "Already_Online") {
        qDebug()<<"??";
        return { infrastructure::ALREADY_ONLINE };
    }
    return { infrastructure::UNKNOWN_ERROR };
}

bool Forum::LogOut() {
    network::SockPtr sock(new QTcpSocket, &network::sockDeleter);
    sock->connectToHost("127.0.0.1", network::port);
    sock->waitForConnected();

    QByteArray request;
    request = "DELETE user\n\n";
    QVariantMap params;
    params["id"] = User::Get()->Id();
    request += QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);

    if (sock->write(request) == -1) {
        qDebug()<<"Writing logout request failed.";
        return false;
    }
    if(!sock->waitForReadyRead()){
        qDebug()<<"Can't get logout response.";
        return false;
    }

    auto [code, message] = network::ParseStatusLine(sock->readLine(1024));
    if (code == "200") {
        sock->readLine(1024);              // read separator
        return true;
    }
    return false;
}

