#include "forum.h"
#include <algorithm>
#include <iterator>
#include <QUuid>
#include <QDate>
#include <QVector>
#include <functional>
#include <QException>
#include <QDebug>
#include "Storage/userinfostorage.h"
#include "Storage/boardsstorage.h"
#include "Storage/postsstorage.h"
#include "Storage/commentsstorage.h"


namespace std {
    //make QString hashable
    template<> struct hash<QString> {
        std::size_t operator()(const QString& s) const {
            return qHash(s);
        }
    };
}

std::unordered_map<QString, Forum::UserInfo> Forum::users;

Forum::Forum()
{
    //they must appear in order
    SetBoards();
    SetPosts();
    SetComments();
}

Forum::~Forum() {}

const std::vector<std::pair<QString, QString>>
Forum::GetBoards() const {
    std::vector<std::pair<QString, QString>> boardInfo;
    for(auto &board: boards) {
        boardInfo.emplace_back(board->Name(), board->ModeratorId());
    }
    return boardInfo;
}

std::optional<std::reference_wrapper<Board>>
Forum::GetBoardByName(const QString& name) {
    auto it = std::find_if(boards.begin(), boards.end(),
                 [&name](const auto& board){
        return board->Name() == name;
    });
    if(it == boards.end())
        qInfo()<<"Unknown board name.";

    return it == boards.end() ?
           std::nullopt :
           std::optional<std::reference_wrapper<Board>>(**it);
}

bool Forum::AssignModerator(const QString& id, const QString& board) {
    auto it = users.find(id);
    if(it == users.end() ||
       it->second.status == infrastructure::ADMINISTRATOR)
        return false;

    //update userinfo storage
    auto& userStorage = ForumStorage::GetStorage(ForumStorage::USERINFO);
    if(!userStorage.UpdateRecord(id, 1, QString().setNum(2)))
        return false;
    //update boards storage
    auto& boardStorage = ForumStorage::GetStorage(ForumStorage::BOARDS);
    if(!boardStorage.UpdateRecord(board, 1, id))
        return false;

    it->second.status = infrastructure::MODERATOR;
    auto targetBoard = std::find_if(boards.begin(), boards.end(),
                       [&name=board](auto& board){ return name==board->Name();});
    if(targetBoard == boards.end()) {
        qInfo()<<"Board not in memory!";
        return false;
    }
    (*targetBoard)->SetModerator(id);
    return true;
}

bool Forum::DismissModerator(const QString& board) {
    auto targetBoard = std::find_if(boards.begin(), boards.end(),
                       [&name=board](auto& board){ return name==board->Name();});
    if(targetBoard == boards.end()) {
        qInfo()<<"Board not in memory!";
        return false;
    }
    auto id = (*targetBoard)->ModeratorId();
    if(id.isEmpty())
        return false;
    auto it = users.find(id);
    if(it == users.end()) //necessary?
        return false;

    //update userinfo storage
    auto& userStorage = ForumStorage::GetStorage(ForumStorage::USERINFO);
    if(!userStorage.UpdateRecord(id, 1, QString().setNum(1)))
        return false;
    auto& boardStorage = ForumStorage::GetStorage(ForumStorage::BOARDS);
    if(!boardStorage.UpdateRecord(board, 1, ""))
        return false;

    it->second.status = infrastructure::COMMON_USER;
    (*targetBoard)->SetModerator(""); //empty str means no moderator
    return true;
}

bool Forum::IncPostCountOf(const QString &id) {
    auto it = users.find(id);
    if(it == users.end())
        return false;

    auto& userStorage = ForumStorage::GetStorage(ForumStorage::USERINFO);
    if(!userStorage.UpdateRecord(id, 2, QString().setNum(it->second.postCount+1)))
        return false;

    it->second.postCount++;
    return true;
}

bool Forum::DecPostCountOf(const QString &id) {
    auto it = users.find(id);
    if(it == users.end())
        return false;

    auto& userStorage = ForumStorage::GetStorage(ForumStorage::USERINFO);
    if(!userStorage.UpdateRecord(id, 2, QString().setNum(it->second.postCount-1)))
        return false;

    it->second.postCount--;
    return true;
}

bool Forum::DeletePost(const QString& id) {
    bool success = false;
    for(auto &board: boards) {
        if(board->DeletePost(id)) {
            success = true;
            break;
        }
    }
    return success;
}

std::optional<const std::list<Comment>>
Forum::GetComments(const QString &id) {
    for(auto &board: boards) {
        auto posts = board->GetPosts();
        auto it = posts.find(id);
        if(it != posts.end()) {
            return it->second.Comments();
        }
    }
    qInfo()<<"file:"<<__FILE__<<" line:"<<__LINE__<<"post not found";
    return std::nullopt;
}


Forum& Forum::Get() {
    static Forum _forum;
    return _forum;
}

infrastructure::Response Forum::Verify(QString id, QString password){
    auto it = users.find(id);

    if(it == users.end()) {                       //id not found
        return { infrastructure::ID_NOT_FOUNTD };
    } else if(it->second.password != password) {  //wrong password
        return { infrastructure::WRONG_PASSWORD };
    } else if(it->second.status != infrastructure::ANONYMOUS
              && it->second.online == true) {
        return {infrastructure::ALREADY_ONLINE };
    } else {                                      //passed verification
        auto& info = it->second;
        it->second.online = true;
        return {
            infrastructure::SUCCESS,
            static_cast<infrastructure::Status>(info.status.load()),
            info.postCount,
            info.name
        };
    } //end else
}

bool Forum::LogOut(QString id) {
    auto it = users.find(id);

    if(it == users.end()) {
        return false;
    } else if (it->second.status == infrastructure::ANONYMOUS) {
        return true;
    } else if (it->second.online == false) {
        return false;
    } else {
        it->second.online = false;
        return true;
    }
}


bool Forum::SetBoards() {
    ForumStorage& storage = ForumStorage::GetStorage(ForumStorage::BOARDS);
    QVector<QString> record;
    int i = 0;
    while (storage>>record) {
        //QString name      = record[0];
        //QString moderator = record[1];
        boards.emplace_back(std::make_unique<Board>(record[0], record[1]));
        record.clear();
    }
    return true;
}

bool Forum::SetPosts() {
    ForumStorage& storage = ForumStorage::GetStorage(ForumStorage::POSTS);
    QVector<QString> record;
    while (storage>>record) {
//        QString id       = record[0];
//        QString board    = record[1];
//        QString author   = record[2];
//        QString authorId = record[3];
//        QString title    = record[4];
//        QString content  = record[5];
        QDate birthday = QDate::fromString(record[6]);
        auto p = std::find_if(boards.begin(), boards.end(),
                     [&val = record[1]](auto& board) {return val==board->Name();});
        if(p != boards.end())
            (*p)->AddInitialPost(
                { record[0], record[2], record[3], record[4],
                  record[5], birthday, record[1] }
            );
        else
            qInfo()<<"Adding initial post failed";

        record.clear();
    }
    return true;
}

bool Forum::SetComments() {
    ForumStorage& storage = ForumStorage::GetStorage(ForumStorage::COMMENTS);
    QVector<QString> record;
    while (storage>>record) {
//        QString id       = record[0];
//        QString board    = record[1];
//        QString postId   = record[2];
//        QString author   = record[3];
//        QString authorId = record[4];
//        QString content  = record[5];
        QDate birthday = QDate::fromString(record[6]);
        auto p = std::find_if(boards.begin(), boards.end(),
                     [&val = record[1]](auto& board) {return val==board->Name();});
        if(p != boards.end())
            (*p)->AddInitialComment(
                { record[3], record[4], record[5], birthday},
                record[2]
            );

        record.clear();
    }
    return true;
}


QString Forum::SelectNameWhereIdEqualTo(QString id) {

    auto it = users.find(id);
    if(it == users.end()) {
        return "Unknown";
    } else {
        return it->second.name;
    }
}

void Forum::SetExistUsers(){
    ForumStorage& storage = ForumStorage::GetStorage(ForumStorage::USERINFO);
    QVector<QString> record;
    while (storage>>record) {
        QString id = record[0];
        uint8_t  status    = record[1].toInt();
        uint32_t postCount = record[2].toInt();
        QString  name      = record[3];
        QString  password  = record[4];

        users.emplace(std::piecewise_construct,
                    std::forward_as_tuple(id),
                    std::forward_as_tuple(
                          status,
                          postCount,
                          name,
                          password,
                          false)
                     );
        record.clear();
    }
    if(users.empty())
        throw QException();
}

