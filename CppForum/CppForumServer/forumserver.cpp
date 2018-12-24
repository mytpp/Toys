#include "forumserver.h"
#include "Storage/forumstorage.h"
#include "Forum/forum.h"
#include <algorithm>
#include <QJsonDocument>
#include <QDebug>

ForumServer::ForumServer(int port, QObject *parent)
    :QTcpServer(parent)
{
    listen(QHostAddress::AnyIPv4, port);
    qDebug()<<"Listening at "<<port<<"...";
}

void ForumServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *sock = new QTcpSocket(this);
    connect(sock, &QTcpSocket::disconnected,
            this, &ForumServer::SlotDisconnected);
    connect(sock, &QTcpSocket::readyRead,
            this, &ForumServer::EchoRequest);

    sock->setSocketDescriptor(socketDescriptor);
    sockList.append(sock);
}

void ForumServer::SlotDisconnected() {
    auto closingsock = qobject_cast<QTcpSocket*>(sender());
    std::remove_if(sockList.begin(), sockList.end(),
                   [closingsock](const QTcpSocket* sock) {
        return sock == closingsock;
    });
    //may cause EchoRequest() access violation
//    delete closingsock;
}

static inline ForumServer::Method MethodFromString(const QString& method) {
    if(method == "GET")
        return ForumServer::GET;
    if(method == "POST")
        return ForumServer::POST;
    if (method == "PUT")
        return ForumServer::PUT;
    if(method == "DELETE")
        return ForumServer::DELTET;
}

void ForumServer::EchoRequest() {
    auto sendingsock = qobject_cast<QTcpSocket*>(sender());
    QString header = QString::fromUtf8(sendingsock->readLine(1024));
    qDebug()<<"Receive header: "<<header;
    auto headerElements = header.trimmed().split(' ');
    auto sMethod   = headerElements[0];
    auto sResource = headerElements[1];
    Method method = MethodFromString(sMethod);

    if(sResource == "user") {
        EchoUserInfoRequest(sendingsock, method);
    } else if (sResource == "board") {
        EchoBoardRequest(sendingsock, method);
    } else if (sResource == "post") {
        EchoPostRequest(sendingsock, method);
    } else if (sResource == "comment") {
        EchoCommentRequest(sendingsock, method);
    } else {
        qDebug()<<"Unknown Resource Requested";
    }
}

static inline QMap<QString, QVariant> ParseParams(QTcpSocket *sock) {
    sock->readLine(1024); //read '\n'
    QByteArray data = sock->readAll();
    QVariant params = QJsonDocument::fromJson(data).toVariant();
    return params.toMap();
}

void ForumServer::EchoUserInfoRequest(QTcpSocket *sock, Method method) {
    QString statusLine;
    if (method == GET) {
        // echo login
        auto params = ParseParams(sock);
        qDebug()<<"params: "<<params;
        QString id = params["id"].toString();
        QString password = params["password"].toString();

        auto result = Forum::Verify(id, password);
        switch (result.error) {
        case infrastructure::SUCCESS:
            statusLine = "200 OK\n\n"; break;
        case infrastructure::ID_NOT_FOUNTD:
            statusLine = "401 Id_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Id_Not_Found' failed!";
            }
            return;
        case infrastructure::WRONG_PASSWORD:
            statusLine = "401 Wrong_Password\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Wrong_Password' failed!";
            }
            return;
        default:
            break;
        }

        QVariantMap response;
        response["status"] = static_cast<int>(result.status);
        response["post"] = result.postCount;
        response["name"] = result.name;
        auto data = statusLine.toUtf8() +
                QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        if(sock->write(data) == -1) {
            qDebug()<<"Writing login response failed!";
        }
    } else if (method == PUT) {
        // set or dismiss moderator
        auto params = ParseParams(sock);
        QString admin = params["admin"].toString();
        QString candidate = params["id"].toString();//empty if 'status'==1
        QString board = params["board"].toString();
        auto status = static_cast<infrastructure::Status>(
                    params["status"].toInt());

        bool success = false;
        if(status == infrastructure::MODERATOR) {
            success = Forum::Get().AssignModerator(candidate, board);
        } else if (status == infrastructure::COMMON_USER) {
            success = Forum::Get().DismissModerator(board);
        } else {
            qDebug()<<"Unknown user status";
        }

        if(success)
            statusLine = "200 OK\n\n";
        else
            statusLine = "404 Not_Found\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qDebug()<<"Writing (assign|dismiss)moderator response failed!";
        }
    } else {
        statusLine = "405 Method_Not_Allowed\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qDebug()<<"Writing 'Method_Not_Allowed' failed!";
        }
    }
}

void ForumServer::EchoBoardRequest(QTcpSocket *sock, Method method) {
    QString statusLine;
    if (method == GET) {
        // get boards info
        sock->readAll();//useless data (separator '\n')
        QVariantList response;
        auto& boards = Forum::Get().GetBoards();
        std::for_each(boards.begin(), boards.end(),
                      [&response](const auto& board){
            QVariantMap boardInfo;
            boardInfo["name"] = board.Name();
            boardInfo["moderator"] = board.ModeratorId();
            response.append(boardInfo);
        });

        statusLine = "200 OK\n\n";
        auto data = statusLine.toUtf8() +
                QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        if(sock->write(data) == -1) {
            qDebug()<<"Writing GET board response failed!";
        }
    } else {
        statusLine = "405 Method_Not_Allowed\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qDebug()<<"Writing 'Method_Not_Allowed' failed!";
        }
    }
}

void ForumServer::EchoPostRequest(QTcpSocket *sock, Method method) {
    QString statusLine;
    if (method == GET) {
        // get posts under a board
        auto params = ParseParams(sock);
        qDebug()<<"params: "<<params;
        QString boardName = params["board"].toString();
        auto board = Forum::Get().GetBoardByName(boardName);
        if (!board) {
            statusLine = "404 Board_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Board_Not_Found' failed!";
            }
            return;
        }

        //pack posts list
        auto& posts = board->get().GetPosts();
        QVariantList response;
        for(auto& [id, post]: posts) {
            QVariantMap postInfo;
            postInfo["id"] = id;
            postInfo["author"] = post.Poster();
            postInfo["authorId"] = post.AuthorId();
            postInfo["title"] = post.Title();
            postInfo["content"] = post.Content();
            postInfo["data"] = post.Date().toString();
            postInfo["comment"] = post.Comments().size();
            response.append(postInfo);
        }

        statusLine = "200 OK\n\n";
        auto data = statusLine.toUtf8() +
                QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        if(sock->write(data) == -1) {
            qDebug()<<"Writing GET post response failed!";
        }
    } else if (method == POST) {
        //post a post under a board
        auto params = ParseParams(sock);
        qDebug()<<"params: "<<params;
        QString boardName = params["board"].toString();
        QString userId = params["user"].toString();
        QString title = params["title"].toString();
        QString content = params["content"].toString();
        QString userName = Forum::SelectNameWhereIdEqualTo(userId);

        auto board = Forum::Get().GetBoardByName(boardName);
        if (!board) {
            statusLine = "404 Board_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Board_Not_Found' failed!";
            }
            return;
        }

        auto [success, id] = board->get().AddPost(title, content, userName, userId);
        if(!success) {
            statusLine = "403 Add_Post_Failed\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Add_Post_Failed' failed!";
            }
            return;
        }

        statusLine = "200 OK\n\n";
        QVariantMap response;
        response["id"] = id;
        auto data = statusLine.toUtf8() +
                QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        if(sock->write(data) == -1) {
            qDebug()<<"Writing Post post response failed!";
        }
    } else if (method == DELTET) {
        //delete a post
        auto params = ParseParams(sock);
        qDebug()<<"params: "<<params;
        QString userId = params["user"].toString();
        QString postId = params["post"].toString();

        bool success = Forum::Get().DeletePost(postId);
        if(!success) {
            statusLine = "404 Delete_Post_Failed\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Delete_Post_Failed' failed!";
            }
            return;
        }

        statusLine = "200 OK\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qDebug()<<"Writing DELETE post response failed!";
        }
    } else {
        statusLine = "405 Method_Not_Allowed\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qDebug()<<"Writing 'Method_Not_Allowed' failed!";
        }
    }
}

void ForumServer::EchoCommentRequest(QTcpSocket *sock, Method method) {
    QString statusLine;
    if (method == GET) {
        // get comments under a post
        auto params = ParseParams(sock);
        qDebug()<<"params: "<<params;
        QString postId = params["post"].toString();

        auto comments = Forum::Get().GetComments(postId);
        if(!comments) {
            statusLine = "404 Post_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Post_Not_Found' failed!";
            }
            return;
        }

        QVariantList response;
        for(auto &comment: comments->get()) {
            QVariantMap commentInfo;
            commentInfo["author"] = comment.Author();
            commentInfo["authorId"] = comment.AuthorId();
            commentInfo["content"] = comment.Content();
            commentInfo["date"] = comment.PublishDate().toString();
            response.append(commentInfo);
        }

        statusLine = "200 OK\n\n";
        auto data = statusLine.toUtf8() +
                QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        if(sock->write(data) == -1) {
            qDebug()<<"Writing GET comment response failed!";
        }
    } else if (method == POST) {
        // publish a comment under a post
        auto params = ParseParams(sock);
        qDebug()<<"params: "<<params;
        QString userId = params["user"].toString();
        QString postId = params["post"].toString();
        QString boardName = params["board"].toString();
        QString content = params["content"].toString();
        QString userName = Forum::SelectNameWhereIdEqualTo(userId);

        auto board = Forum::Get().GetBoardByName(boardName);
        if (!board) {
            statusLine = "404 Board_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Board_Not_Found' failed!";
            }
            return;
        }

        auto &posts = board->get().GetPosts();
        auto it = posts.find(postId);
        if (it == posts.end()) {
            statusLine = "404 Post_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Post_Not_Found' failed!";
            }
            return;
        }

        bool success = it->second.AddComment(content, userName, userId);
        if(!success) {
            statusLine = "403 Add_Comment_Failed\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qDebug()<<"Writing 'Add_Comment_Failed' failed!";
            }
            return;
        }

        statusLine = "200 OK\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qDebug()<<"Writing POST comment response failed!";
        }
    }
}
