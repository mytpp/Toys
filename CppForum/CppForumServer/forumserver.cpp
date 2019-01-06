#include "forumserver.h"
#include "Storage/forumstorage.h"
#include "Forum/forum.h"
#include <algorithm>
//#include <thread>
//#include <iostream>
#include <QJsonDocument>
#include <QThreadPool>
#include <QDebug>

enum Method: uint8_t {
    GET,
    POST,
    PUT,
    DELETE
};

void EchoUserInfoRequest(QTcpSocket *sock, Method method);
void EchoBoardRequest(QTcpSocket *sock, Method method);
void EchoPostRequest(QTcpSocket *sock, Method method);
void EchoCommentRequest(QTcpSocket *sock, Method method);


static inline Method MethodFromString(const QString& method) {
    if(method == "GET")
        return GET;
    if(method == "POST")
        return POST;
    if (method == "PUT")
        return PUT;
    if(method == "DELETE")
        return DELETE;
}

class EchoRequest :public QRunnable
{
public:
    EchoRequest(qintptr socketDescriptor)
        :socketDescriptor(socketDescriptor)
    { }

    virtual void run() override {
        //std::cout<<"this thread id :"<<std::this_thread::get_id()<<std::endl;
        QTcpSocket *sock = new QTcpSocket();
        sock->setSocketDescriptor(socketDescriptor);
        sock->waitForReadyRead();

        QString header = QString::fromUtf8(sock->readLine(1024));
        qInfo()<<"Receive header: "<<header;
        auto headerElements = header.trimmed().split(' ');
        auto sMethod   = headerElements[0];
        auto sResource = headerElements[1];
        Method method = MethodFromString(sMethod);

        if(sResource == "user") {
            EchoUserInfoRequest(sock, method);
        } else if (sResource == "board") {
            EchoBoardRequest(sock, method);
        } else if (sResource == "post") {
            EchoPostRequest(sock, method);
        } else if (sResource == "comment") {
            EchoCommentRequest(sock, method);
        } else {
            qInfo()<<"Unknown Resource Requested";
        }

        sock->waitForBytesWritten();
        delete sock;
    }

private:
    qintptr socketDescriptor;
};


ForumServer::ForumServer(int port, QObject *parent)
    :QTcpServer(parent)
{
    listen(QHostAddress::AnyIPv4, port);
    qInfo()<<"Listening at "<<port<<"...";
    qInfo()<<"max num of thread in thread pool: "
          <<QThreadPool::globalInstance()->maxThreadCount();
    //std::cout<<"this thread id: "<<std::this_thread::get_id()<<std::endl;
}

void ForumServer::incomingConnection(qintptr socketDescriptor) {
    //start a new task in the application's threadpool
    QThreadPool::globalInstance()->start(new EchoRequest(socketDescriptor));
}


static inline QMap<QString, QVariant> ParseParams(QTcpSocket *sock) {
    sock->readLine(1024); //read '\n'
    QByteArray data = sock->readAll();
    QVariant params = QJsonDocument::fromJson(data).toVariant();
    QVariantMap parsedParams = params.toMap();
    qInfo()<<"params: "<<parsedParams;
    return parsedParams;
}

void EchoUserInfoRequest(QTcpSocket *sock, Method method) {
    QString statusLine;
    if (method == GET) {
        // echo login
        auto params = ParseParams(sock);
        QString id = params["id"].toString();
        QString password = params["password"].toString();

        auto result = Forum::Verify(id, password);
        switch (result.error) {
        case infrastructure::SUCCESS:
            statusLine = "200 OK\n\n"; break;
        case infrastructure::ID_NOT_FOUNTD:
            qInfo()<<"Id_Not_Found";
            statusLine = "401 Id_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Id_Not_Found' failed!";
            }
            return;
        case infrastructure::WRONG_PASSWORD:
            qInfo()<<"Wrong_Password";
            statusLine = "401 Wrong_Password\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Wrong_Password' failed!";
            }
            return;
        case infrastructure::ALREADY_ONLINE:
            qInfo()<<"Already_Online";
            statusLine = "401 Already_Online\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Already_Online' failed!";
            }
            return;
        default:
            break;
        }
        qInfo()<<"User "<<id<<" logs in.";

        QVariantMap response;
        response["status"] = static_cast<int>(result.status);
        response["post"] = result.postCount;
        response["name"] = result.name;
        auto data = statusLine.toUtf8() +
                QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        if(sock->write(data) == -1) {
            qInfo()<<"Writing login response failed!";
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
            qInfo()<<"Unknown user status";
        }

        if(success)
            statusLine = "200 OK\n\n";
        else
            statusLine = "404 Not_Found\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qInfo()<<"Writing (assign|dismiss)moderator response failed!";
        }
    } else if (method == DELETE){
        // logout
        auto params = ParseParams(sock);
        QString id = params["id"].toString();

        if(Forum::LogOut(id)){
            statusLine = "200 OK\n\n";
            qInfo()<<"User "<<id<<" logs out.";
        } else {
            statusLine = "401 Logout_Failed\n\n";
        }
        if(sock->write(statusLine.toUtf8()) == -1) {
            qInfo()<<"Writing logout response failed!";
        }
    } else {
        statusLine = "405 Method_Not_Allowed\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qInfo()<<"Writing 'Method_Not_Allowed' failed!";
        }
    }
}

void EchoBoardRequest(QTcpSocket *sock, Method method) {
    QString statusLine;
    if (method == GET) {
        // get boards info
        sock->readAll();//useless data (separator '\n')
        QVariantList response;
        auto& boards = Forum::Get().GetBoards();
        std::for_each(boards.begin(), boards.end(),
                      [&response](const auto& board){
            QVariantMap boardInfo;
            boardInfo["name"] = board.first;
            boardInfo["moderator"] = board.second;
            response.append(boardInfo);
        });

        statusLine = "200 OK\n\n";
        auto data = statusLine.toUtf8() +
                QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        if(sock->write(data) == -1) {
            qInfo()<<"Writing GET board response failed!";
        }
    } else {
        statusLine = "405 Method_Not_Allowed\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qInfo()<<"Writing 'Method_Not_Allowed' failed!";
        }
    }
}

void EchoPostRequest(QTcpSocket *sock, Method method) {
    QString statusLine;
    if (method == GET) {
        // get posts under a board
        auto params = ParseParams(sock);
        QString boardName = params["board"].toString();
        auto board = Forum::Get().GetBoardByName(boardName);
        if (!board) {
            statusLine = "404 Board_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Board_Not_Found' failed!";
            }
            return;
        }

        //pack posts list
        auto posts = board->get().GetPosts();
        QVariantList response;
        for(auto& [id, post]: posts) {
            QVariantMap postInfo;
            postInfo["id"] = post.Id();
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
            qInfo()<<"Writing GET post response failed!";
        }
    } else if (method == POST) {
        //post a post under a board
        auto params = ParseParams(sock);
        QString boardName = params["board"].toString();
        QString userId = params["user"].toString();
        QString title = params["title"].toString();
        QString content = params["content"].toString();
        QString userName = Forum::SelectNameWhereIdEqualTo(userId);

        auto board = Forum::Get().GetBoardByName(boardName);
        if (!board) {
            statusLine = "404 Board_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Board_Not_Found' failed!";
            }
            return;
        }

        auto [success, id] = board->get().AddPost(title, content, userName, userId);
        if(!success) {
            statusLine = "403 Add_Post_Failed\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Add_Post_Failed' failed!";
            }
            return;
        }

        statusLine = "200 OK\n\n";
        QVariantMap response;
        response["id"] = id;
        auto data = statusLine.toUtf8() +
                QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        if(sock->write(data) == -1) {
            qInfo()<<"Writing Post post response failed!";
        }
    } else if (method == DELETE) {
        //delete a post
        auto params = ParseParams(sock);
        QString userId = params["user"].toString();
        QString postId = params["post"].toString();

        bool success = Forum::Get().DeletePost(postId);
        if(!success) {
            statusLine = "404 Delete_Post_Failed\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Delete_Post_Failed' failed!";
            }
            return;
        }

        statusLine = "200 OK\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qInfo()<<"Writing DELETE post response failed!";
        }
    } else {
        statusLine = "405 Method_Not_Allowed\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qInfo()<<"Writing 'Method_Not_Allowed' failed!";
        }
    }
}

void EchoCommentRequest(QTcpSocket *sock, Method method) {
    QString statusLine;
    if (method == GET) {
        // get comments under a post
        auto params = ParseParams(sock);
        QString postId = params["post"].toString();

        auto comments = Forum::Get().GetComments(postId);
        if(!comments) {
            statusLine = "404 Post_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Post_Not_Found' failed!";
            }
            return;
        }

        QVariantList response;
        for(auto &comment: *comments) {
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
            qInfo()<<"Writing GET comment response failed!";
        }
    } else if (method == POST) {
        // publish a comment under a post
        auto params = ParseParams(sock);
        QString userId = params["user"].toString();
        QString postId = params["post"].toString();
        QString boardName = params["board"].toString();
        QString content = params["content"].toString();
        QString userName = Forum::SelectNameWhereIdEqualTo(userId);

        auto board = Forum::Get().GetBoardByName(boardName);
        if (!board) {
            statusLine = "404 Board_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Board_Not_Found' failed!";
            }
            return;
        }

        bool success = board->get().AddComment(postId, content, userName, userId);
        if (!success) {
            statusLine = "404 Post_Not_Found\n\n";
            if(sock->write(statusLine.toUtf8()) == -1) {
                qInfo()<<"Writing 'Post_Not_Found' failed!";
            }
            return;
        }

        statusLine = "200 OK\n\n";
        if(sock->write(statusLine.toUtf8()) == -1) {
            qInfo()<<"Writing POST comment response failed!";
        }
    }
}
